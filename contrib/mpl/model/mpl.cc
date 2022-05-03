/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "mpl.h"
#include "ns3/ipv6-interface-container.h"
#include "ns3/ipv6-extension-demux.h"
#include "ns3/ipv6-extension.h"
#include "ns3/ipv6-extension-header.h"
#include "ns3/ipv6-option-demux.h"
#include "ns3/ipv6-option.h"
#include "ns3/ipv6-option-header.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/icmpv6-l4-protocol.h"

#include "seed-set-entry.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MplRoutingProtocol");

namespace mpl {

NS_OBJECT_ENSURE_REGISTERED (RoutingProtocol);

const uint8_t RoutingProtocol::ICMP_TYPE = 159;
const uint8_t RoutingProtocol::ICMP_CODE = 0;
const uint8_t RoutingProtocol::ICMP_TTL = 255;
const uint8_t RoutingProtocol::MPL_DATA_DOMAIN = 3;
const uint8_t RoutingProtocol::MPL_CONTROL_DOMAIN = 2;

TypeId RoutingProtocol::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::mpl::RoutingProtocol")
    .SetParent<Ipv6RoutingProtocol> ()
    .SetGroupName ("Mpl")
    .AddConstructor<RoutingProtocol> ()
    .AddAttribute ("ProactiveForwarding","Enables the MPL Proactive-Forwarding mode.",
                   BooleanValue (true),
                   MakeBooleanAccessor(&RoutingProtocol::m_EnableProactiveForwarding),
                   MakeBooleanChecker ())
    .AddAttribute ("ReactiveForwarding","Enables the MPL Reactive-Forwarding mode.",
                   BooleanValue (true),
                   MakeBooleanAccessor(&RoutingProtocol::m_EnableReactiveForwarding),
                   MakeBooleanChecker ())
    .AddAttribute ("SeedSetEntryLifetime", "The minimum lifetime for an entry in the Seed Set.",
                   TimeValue (Minutes (30)),
                   MakeTimeAccessor (&RoutingProtocol::m_SeedSetEntryLifetime),
                   MakeTimeChecker ())
    .AddAttribute ("DataMessageIMin", "The minimum Trickle timer interval for MPL Data Message transmissions.",
                   TimeValue (MilliSeconds(1000)),
                   MakeTimeAccessor(&RoutingProtocol::m_TrickleIMinData),
                   MakeTimeChecker ())
    .AddAttribute ("DataMessageIMax", "The maximum Trickle timer interval, in doublings of the DataMessageIMin, for MPL Data Message transmissions.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&RoutingProtocol::m_TrickleIMaxData),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DataMessageK","The redundancy constant for MPL Data Message transmissions.",
                   UintegerValue(1),
                   MakeUintegerAccessor(&RoutingProtocol::m_TrickleKData),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DataMessageTimerExpirations","The number of Trickle timer expirations that occur before terminating the Trickle algorithm's retransmission of a given MPL Data Message.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&RoutingProtocol::m_TrickleTimerExpirationsData),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("ControlMessageIMin", "The minimum Trickle timer interval for MPL Control Message transmissions.",
                   TimeValue (MilliSeconds(1000)),
                   MakeTimeAccessor(&RoutingProtocol::m_TrickleIMinControl),
                   MakeTimeChecker ())
    .AddAttribute ("ControlMessageIMax", "The maximum Trickle timer interval for MPL Control Message transmissions.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&RoutingProtocol::m_TrickleIMaxControl),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("ControlMessageK","The redundancy constant for MPL Control Message transmissions.",
                   UintegerValue(1),
                   MakeUintegerAccessor(&RoutingProtocol::m_TrickleKControl),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("ControlMessageTimerExpirations","The number of Trickle timer expirations that occur before terminating the Trickle algorithm's retransmission of a given MPL Control Message.",
                   UintegerValue (10),
                   MakeUintegerAccessor (&RoutingProtocol::m_TrickleTimerExpirationsControl),
                   MakeUintegerChecker<uint8_t> ());
  return tid;
}

RoutingProtocol::RoutingProtocol ()
{
  static uint16_t m_seedCounter = 1;

  m_seedID = m_seedCounter++;
  m_sequenceNr = 0;
}

RoutingProtocol::~RoutingProtocol ()
{
  for(auto control_it = m_controlTimers.begin(); control_it != m_controlTimers.end(); control_it++)
    delete control_it->second.first;

  m_controlTimers.clear();

  for(auto domain_it = m_BufferedMessageSet.begin(); domain_it != m_BufferedMessageSet.end(); domain_it++)
  {
    for(auto seed_it = domain_it->second.begin(); seed_it != domain_it->second.end(); seed_it++)
    {
      for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); message_it++)
      {
        delete message_it->second;
      }
      seed_it->second.clear();
    }
    domain_it->second.clear();
  }
  m_BufferedMessageSet.clear();

  for(auto domain_it = m_SeedSet.begin(); domain_it != m_SeedSet.end(); domain_it++)
  {
    domain_it->second.clear();
  }
  m_SeedSet.clear();

}


Ptr<Ipv6Route> RoutingProtocol::RouteOutput (Ptr<Packet> p, const Ipv6Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
  Ipv6Address dst = header.GetDestinationAddress();
  Ptr<Ipv6Route> route;
  uint8_t bytes[16];

  dst.GetBytes(bytes);

  if(!p)
  {
    NS_LOG_DEBUG ("Packet is == 0");
    return (Ptr<Ipv6Route>)0;
  }

  Ptr<Ipv6L3Protocol> l3protocol = m_myAddress->GetObject<Ipv6L3Protocol>();
  if(!l3protocol)
  {
    NS_FATAL_ERROR("Error: No Ipv6 Layer3 Protocol available!");
    return (Ptr<Ipv6Route>)0;
  }

  if(!header.GetDestinationAddress().IsMulticast())
  {
    NS_LOG_DEBUG ("Mpl cannot handle unicast messages use another routing protocol!");
    return (Ptr<Ipv6Route>)0;
  }

  route = FindRoute(dst, Ipv6Address::GetZero());

  // process a control messages
  if(bytes[1] == MPL_CONTROL_DOMAIN)
  {
    if(m_EnableReactiveForwarding && route && route->GetSource() != Ipv6Address::GetZero())
    {
      NS_LOG_DEBUG("Send a Subscribed Control Message!");
      return route;
    }
    else
    {
      NS_LOG_DEBUG("Send a Subscribed Control Message but Proactive mode is not enabled!");
      return (Ptr<Ipv6Route>)0;
    }
  }

  //process the data messages
  Ipv6ExtensionHopByHopHeader ipv6HopByHopHeader = Ipv6ExtensionHopByHopHeader();
  MplOptionHeader mplOptionHeader = MplOptionHeader();
  mplOptionHeader.SetParams(MplOptionHeader::SEED_16_BIT_ADDRESSING, true, m_sequenceNr, m_seedID);
  ipv6HopByHopHeader.AddOption(mplOptionHeader);
  ipv6HopByHopHeader.SetNextHeader(header.GetNextHeader());
  const_cast<Ipv6Header&>(header).SetNextHeader(Ipv6ExtensionHopByHop::EXT_NUMBER);
  p->AddHeader(ipv6HopByHopHeader);

  mpl::SeedSetEntry* seedEntry = GetSeedSetEntry(dst, m_seedID);

  MplDataMessage* pDataMessage = CreateDataMessage(p->Copy(), route, m_sequenceNr, dst, m_seedID);

  if(!seedEntry)
    InsertSeedSetEntry(dst, m_seedID, m_sequenceNr);

  InsertMessage(pDataMessage, dst, m_seedID);


  return (Ptr<Ipv6Route>)0; // sending of data-messages is handled by trickle-timers
}


bool RoutingProtocol::RouteInput  (Ptr<const Packet> p, const Ipv6Header &header, Ptr<const NetDevice> idev,
                          UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                          LocalDeliverCallback lcb, ErrorCallback ecb)
{
  Ptr<Packet> pCopy = p->Copy();
  Ipv6Address dst = header.GetDestinationAddress();
  Ipv6Address src = header.GetSourceAddress();
  uint8_t bytes[16];
  uint8_t srcBytes[16];

  dst.GetBytes(bytes);
  src.GetBytes(srcBytes);

  //Correct the bug with the wrong received IPv6-Address, where bytes[2]-[5] are set completely randomly to any value without any reason
  memset(&srcBytes[2], 0, 4); // reset them back to zero as they should be
  const_cast<Ipv6Header&>(header).SetSourceAddress(Ipv6Address(srcBytes));

  if(!dst.IsMulticast())
  {
    NS_LOG_DEBUG("MPL cannot handle Unicast Messages!");
    return false;
  }

  Ptr<Ipv6Route> route = FindRoute(dst, header.GetSourceAddress());

  if(!route)
  {
    NS_LOG_DEBUG("No registered Mpl domain found!");
    return false;
  }

  // implement steps defined in 10.3 of RFC7731
  if(m_EnableReactiveForwarding && bytes[1] == MPL_CONTROL_DOMAIN)
  {
    Icmpv6Header controlHeader;
    pCopy->RemoveHeader(controlHeader);
    if(controlHeader.GetType() != ICMP_TYPE || controlHeader.GetCode() != ICMP_CODE)
    {
      NS_LOG_DEBUG("MPL Received unknown ICMPv6 Message!");
      return false;
    }

    bytes[1] = MPL_DATA_DOMAIN;
    auto controlTimer_it = m_controlTimers.find(dst);
    if(controlTimer_it == m_controlTimers.end())
    {
      NS_LOG_DEBUG("No assigned control Trickle Timer found!");
      HandleControlMessage(pCopy, Ipv6Address(bytes));
      return false;
    }
    TrickleTimer* pTrickle = controlTimer_it->second.first;

    if(HandleControlMessage(pCopy, Ipv6Address(bytes)))
    {
      controlTimer_it->second.second = 0;
      pTrickle->Reset();
     // pTrickle->InconsistentEvent();  // do we need this if we already reset it??
      NS_LOG_DEBUG("Inconsistent Control Event happened!");
    }
    else
    {
      pTrickle->ConsistentEvent();
      NS_LOG_DEBUG("Consistent Control Event happened!");
    }

    return true; // we can stop now was we have done all necessary steps
  }

  // implement steps defined in 9.3 of RFC7731
  Ptr<Ipv6ExtensionDemux> ipv6ExtensionDemux = idev->GetNode()->GetObject<Ipv6ExtensionDemux> ();
  Ptr<Ipv6Extension> ipv6Extension = 0;
  MplOptionHeader mplOptionHeader;

  ipv6Extension = ipv6ExtensionDemux->GetExtension (Ipv6Header::IPV6_EXT_HOP_BY_HOP);

  if (!ipv6Extension)
  {
    NS_LOG_DEBUG("No Ipv6 Extension found!");
    return false;
  }

  Ptr<Ipv6OptionDemux> ipv6OptionDemux = idev->GetNode()->GetObject<Ipv6OptionDemux> ();
  Ptr<Ipv6Option> ipv6Option = 0;

  ipv6Option = ipv6OptionDemux->GetOption(MplOption::OPT_NUMBER);
  if(!ipv6Option)
  {
    NS_LOG_DEBUG("No Ipv6 Mpl-Option found!");
    return false;
  }

  Ptr<MplOption> mplOption = DynamicCast<MplOption>(ipv6Option);
  mplOptionHeader = mplOption->GetOptionHeader();

  if(mplOptionHeader.GetFlags() & (1 << 4))
  {
    NS_LOG_DEBUG("V-Flag is set --> drop message!");
    return false;
  }

  MplDataMessage* message = GetMessage(header.GetDestinationAddress(), mplOptionHeader.GetSeedID(), mplOptionHeader.GetSequence());
  mpl::SeedSetEntry* seedSetEntry = GetSeedSetEntry(header.GetDestinationAddress(), mplOptionHeader.GetSeedID());

  if(message)
  {
    NS_LOG_DEBUG("Received Message is already known!");

    if(mplOptionHeader.GetM() && CompareSequenceNumbersSmaller(mplOptionHeader.GetSequence(), message->GetSequenceNr()))
    {
      NS_LOG_DEBUG("Performed an Inconsistent Data-Transmission Operation!");
      message->ResetTrickle();
     // message->GetTrickle()->InconsistentEvent();
    }
    else
    {
      NS_LOG_DEBUG("Performed an Consistent Data-Transmission Operation!");
      message->GetTrickle()->ConsistentEvent();
    }
    return false;
  }
  if(seedSetEntry && (CompareSequenceNumbersSmaller(mplOptionHeader.GetSequence(), seedSetEntry->GetMinSequenceNr())))
  {
    NS_LOG_DEBUG("Received Message is too old!");
    return false;
  }

  mplOptionHeader.SetM(AcquireMFlagValue(header.GetDestinationAddress(), mplOptionHeader.GetSeedID(), mplOptionHeader.GetSequence()));
  MplDataMessage* pDataMessage = CreateDataMessage(pCopy, route, mplOptionHeader.GetSequence(), dst, mplOptionHeader.GetSeedID());

  if(seedSetEntry)
    seedSetEntry->GetWatchdog()->Ping(m_SeedSetEntryLifetime);
  else
    InsertSeedSetEntry(dst, mplOptionHeader.GetSeedID(), 0); // initialize Min_SeqNr but in case of a late first message and we start wit a seqNr bigger than the late one, we would ignore the late one --> start with 0

  InsertMessage(pDataMessage, header.GetDestinationAddress(), mplOptionHeader.GetSeedID());

  // TODO currently not working as NS-3 not supporting UDP-Multicast properly
  lcb(pCopy, header, m_myAddress->GetInterfaceForDevice(idev));

  /*
  Ptr<Ipv6MulticastRoute> multiRoute = Create<Ipv6MulticastRoute>();
  multiRoute->SetGroup(dst);
  multiRoute->SetOrigin(route->GetSource());
  multiRoute->SetParent(m_myAddress->GetInterfaceForDevice(idev));
  // use TTL = 254, as 255 disables the interface
  multiRoute->SetOutputTtl(m_myAddress->GetInterfaceForDevice(route->GetOutputDevice()), 254);

  mcb(route->GetOutputDevice(), multiRoute, pCopy, header); // forward the currently received message
  */
  return true;
}

Ptr<Ipv6Route> RoutingProtocol::FindRoute(Ipv6Address mplDomain, Ipv6Address sourceAddress)
{
  Ptr<Ipv6Route> route = 0;
  Ptr<Ipv6L3Protocol> l3protocol = m_myAddress->GetObject<Ipv6L3Protocol>();
  if(!l3protocol)
  {
    NS_FATAL_ERROR("Error: No Ipv6 Layer3 Protocol available!");
    return route;
  }

  uint8_t bytes[16];
  mplDomain.GetBytes(bytes);

  if(m_EnableReactiveForwarding && bytes[1] == MPL_CONTROL_DOMAIN)
  {
    bytes[1] = MPL_DATA_DOMAIN;
    mplDomain = Ipv6Address(bytes);
  }

  bool mplDomainAndOifDefined = false;
  uint32_t definedInterface = 1;

  bool mplDomainWithoutOif = l3protocol->IsRegisteredMulticastAddress(mplDomain);

  for(uint32_t i = 1; i < l3protocol->GetNInterfaces(); i++)
  {
    if(l3protocol->IsRegisteredMulticastAddress(mplDomain, i))
    {
      mplDomainAndOifDefined = true;
      definedInterface = i;
      break;
    }
  }

  if(mplDomainAndOifDefined || mplDomainWithoutOif)
  {
    route = Create<Ipv6Route>();
    route->SetDestination(mplDomain);

    Ipv6InterfaceAddress x(m_myAddress->GetAddress(definedInterface, 0)); // get the link-local address of the interface

    if(sourceAddress == Ipv6Address::GetZero())
      route->SetSource(x.GetAddress());
    else
      route->SetSource(sourceAddress);

    route->SetOutputDevice(l3protocol->GetNetDevice(definedInterface));
    return route;
  }
  else // use IP-to-IP tunneling to to deliver and preserve the message
  {
    NS_LOG_DEBUG ("TODO: Add MPL-Tunneling to forward packets from not subscribed multicast-domains");
    return route;
  }
}

MplDataMessage* RoutingProtocol::CreateDataMessage(Ptr<Packet> p, Ptr<Ipv6Route> route, uint8_t sequenceNr, Ipv6Address mplDomain, uint64_t seedId)
{
  MplDataMessage* message =  new MplDataMessage(m_TrickleIMinData, m_TrickleIMaxData, m_TrickleKData, p, route, sequenceNr);
  message->GetTrickle()->SetFunction(&RoutingProtocol::RetransmitMessage, this);
  message->GetTrickle()->SetArguments(mplDomain, seedId, message);
  if(m_EnableProactiveForwarding)
    message->GetTrickle()->Enable();

  return message;
}

SeedSetEntry* RoutingProtocol::GetSeedSetEntry(Ipv6Address mplDomain, uint64_t seedId)
{
  auto domain_it = m_SeedSet.find(mplDomain);
  if(domain_it != m_SeedSet.end())
  {
    auto seed_it = domain_it->second.find(seedId);

    if(seed_it != domain_it->second.end())
      return seed_it->second;
  }
  return NULL;
}

void RoutingProtocol::InsertSeedSetEntry(Ipv6Address mplDomain, uint64_t seedId, uint8_t minSequence)
{
  auto domain_it = m_SeedSet.find(mplDomain);
  if(domain_it != m_SeedSet.end())
  {
    NS_LOG_DEBUG("Seed-ID already known --> nothing to do");
    return;
  }

  mpl::SeedSetEntry* data = new mpl::SeedSetEntry;

  data->SetDomain(mplDomain);
  data->SetMinSequenceNr(minSequence);
  data->SetSeedId(seedId);
  data->GetWatchdog()->SetFunction(&mpl::RoutingProtocol::DeleteSeedSetEntry, this);
  data->GetWatchdog()->SetArguments(mplDomain, seedId);
  data->GetWatchdog()->Ping(m_SeedSetEntryLifetime);

  m_SeedSet.insert(std::make_pair(mplDomain, std::map<uint64_t, mpl::SeedSetEntry*> {std::make_pair(seedId, data)}));

}

void RoutingProtocol::DeleteSeedSetEntry(Ipv6Address domain, uint64_t seedId)
{
  auto domain_it = m_SeedSet.find(domain);
  if(domain_it != m_SeedSet.end())
  {
    auto seed_it = domain_it->second.find(seedId);
    if(seed_it != domain_it->second.end())
    {
      domain_it->second.erase(seedId);
      delete seed_it->second;
    }
  }

  if(domain_it->second.empty())
    m_SeedSet.erase(domain);
}

void RoutingProtocol::ClearMessagesForSeedId(Ipv6Address mplDomain, uint64_t seedId)
{
  auto domain_it = m_BufferedMessageSet.find(mplDomain);

  if(domain_it != m_BufferedMessageSet.end())
  {
    auto seed_it = domain_it->second.find(seedId);
    if(seed_it != domain_it->second.end())
    {
      for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); )
      {
        delete message_it->second;
        message_it = seed_it->second.erase(message_it);
      }
      domain_it->second.erase(seedId);
    }
    if(domain_it->second.empty())
      m_BufferedMessageSet.erase(mplDomain);
  }
}

void RoutingProtocol::InsertMessage(MplDataMessage* message, Ipv6Address mplDomain, uint64_t seedId)
{
  auto domain_it = m_BufferedMessageSet.find(mplDomain);

  if(domain_it == m_BufferedMessageSet.end())
  {
    m_BufferedMessageSet.insert(std::make_pair(mplDomain, std::map<uint64_t, std::map<uint8_t, MplDataMessage*>>{std::make_pair(seedId, std::map<uint8_t, MplDataMessage*> {std::make_pair(message->GetSequenceNr(), message)})}));
    m_sequenceNr++;
  }
  else
  {
    auto seed_it = domain_it->second.find(seedId);
    if(seed_it == domain_it->second.end())
    {
      domain_it->second.insert(std::make_pair(seedId, std::map<uint8_t, MplDataMessage*>{std::make_pair(message->GetSequenceNr(), message)}));
      m_sequenceNr++;
    }
    else
    {
      auto message_it = seed_it->second.find(message->GetSequenceNr());
      if(message_it == seed_it->second.end())
      {
        seed_it->second.insert(std::make_pair(message->GetSequenceNr(), message));
        m_sequenceNr++;
      }
    }
  }
  ResetTrickleControl(mplDomain);
}

MplDataMessage* RoutingProtocol::GetMessage(Ipv6Address mplDomain, uint64_t seedId, uint8_t sequenceNr)
{
  auto domain_it = m_BufferedMessageSet.find(mplDomain);

  if(domain_it != m_BufferedMessageSet.end())
  {
    auto seed_it = domain_it->second.find(seedId);
    if(seed_it != domain_it->second.end())
    {
      for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); message_it++)
      {
        if(message_it->second->GetSequenceNr() == sequenceNr)
          return message_it->second;
      }
    }
  }

  return NULL;
}

void RoutingProtocol::DeleteOldMessages(Ipv6Address mplDomain, uint8_t minSequenceNr, uint64_t seedId)
{
  mpl::SeedSetEntry* pSeedSetEntry = GetSeedSetEntry(mplDomain, seedId);
  auto domain_it = m_BufferedMessageSet.find(mplDomain);

  if(domain_it == m_BufferedMessageSet.end())
  {
    NS_LOG_DEBUG("No suitable Domain-Address available");
    return;
  }

  auto seed_it = domain_it->second.find(seedId);
  if(seed_it == domain_it->second.end())
  {
    NS_LOG_DEBUG("No suitable SeedId available");
    return;
  }

  if(pSeedSetEntry)
  {
    for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); )
    {
      if(CompareSequenceNumbersSmaller(message_it->second->GetSequenceNr(), minSequenceNr))
      {
        delete message_it->second;
        message_it = seed_it->second.erase(message_it);
      }
      else
        ++message_it;
    }
  }
  else
  {
    for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); )
    {
        delete message_it->second;
        message_it = seed_it->second.erase(message_it);
    }
    domain_it->second.erase(seedId);
  }

}

bool RoutingProtocol::AcquireMFlagValue(Ipv6Address mplDomain, uint64_t seedId, uint8_t currentSequenceNr)
{
  auto domain_it = m_BufferedMessageSet.find(mplDomain);
  if(domain_it != m_BufferedMessageSet.end())
  {
    auto seed_it = domain_it->second.find(seedId);
    if(seed_it != domain_it->second.end())
    {
      for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); message_it++)
      {
        if(!CompareSequenceNumbersSmaller(message_it->second->GetSequenceNr(), currentSequenceNr))
        {
           return false;
        }
      }
    }
  }
  return true;
}

void RoutingProtocol::RetransmitMessage(Ipv6Address mplDomain, uint64_t seedId, MplDataMessage* dataMessage)
{
    if(dataMessage->GetRetransmissions() < m_TrickleTimerExpirationsData)
    {
      dataMessage->IncreaseRetransmission();
      Ptr<Ipv6L3Protocol> l3Protocol = m_myAddress->GetObject<Ipv6L3Protocol>();

      if(l3Protocol)
      {
        Ptr<Packet> packet = dataMessage->GetPacket();
        Ptr<Ipv6Route> route = dataMessage->GetRoute();

        std::vector<uint8_t> buffer(packet->GetSize(), 0);
        // Iterate over the paket to find the M-flag
        packet->CopyData(&buffer[0], packet->GetSize());

        if(AcquireMFlagValue(mplDomain, seedId, dataMessage->GetSequenceNr()))
          buffer[4] |= (1 << 5);
        else
          buffer[4] &= ~(1 << 5);

        Ptr<Packet> newPacket = Create<Packet> (&buffer[0], packet->GetSize());
        newPacket->SetUid(packet->GetUid());

        l3Protocol->Send(newPacket, route->GetSource(), route->GetDestination(), Ipv6ExtensionHopByHop::EXT_NUMBER, route);
      }
    }
    else // disable the trickle timer and increase the MinSequenceId
    {
      dataMessage->GetTrickle()->Stop();
      mpl::SeedSetEntry* seedEntry = GetSeedSetEntry(mplDomain, seedId);
      if(seedEntry)
      {
        uint8_t minSeq = seedEntry->GetMinSequenceNr()+1;
        seedEntry->SetMinSequenceNr(minSeq);
        DeleteOldMessages(mplDomain, minSeq, seedId);

        ResetTrickleControl(mplDomain);
      }
    }
}

bool RoutingProtocol::CompareSequenceNumbersSmaller(uint16_t x, uint16_t y) const
{
  if((x < y && ((y - x) < 255)) || (x > y && ((x - y) > 255)))
    return true;

  return false;
}

void RoutingProtocol::SendControlMessage(Ipv6Address controlDomain)
{
  auto it = m_controlTimers.find(controlDomain);

  if(it == m_controlTimers.end())
  {
    NS_LOG_DEBUG("No Control Trickle Timer found");
    return;
  }

  if(it->second.second < m_TrickleTimerExpirationsControl)
  {
    NS_LOG_DEBUG("No need to send control message increase counter");
    it->second.second++;
    return;
  }

  if(!m_TrickleTimerExpirationsControl)
  {
    NS_LOG_DEBUG("No need to send control message Retransmissions set to 0");
    return;
  }

  Ptr<Packet> controlMessage = Create<Packet>();
  Ptr<Icmpv6L4Protocol> icmpL4 = m_myAddress->GetObject<Icmpv6L4Protocol>();
  Icmpv6Header icmpHeader;

  if(!icmpL4)
  {
    NS_LOG_ERROR ("No ICMPL4 Object found!");
    return;
  }
  auto control_it = m_controlTimers.find(controlDomain);
  if(control_it != m_controlTimers.end())
  {
    NS_LOG_DEBUG("Control Expirations reached --> disabling Control Trickle");
    control_it->second.first->Stop();
    control_it->second.second = 0;
  }
  else
    NS_LOG_DEBUG("No Trickle Timer found --> cannot stop");

  icmpHeader.SetCode(ICMP_CODE);
  icmpHeader.SetType(ICMP_TYPE);

  //INFO: currently only 16-bit seed-ID implemented

  uint8_t bytes[16];
  controlDomain.GetBytes(bytes);
  bytes[1] = MPL_DATA_DOMAIN; // switch to the corresponding data domain
  Ipv6Address dataDomain(bytes);

  auto seedDomain = m_SeedSet.find(dataDomain);
  if(seedDomain == m_SeedSet.end())
  {
    NS_LOG_DEBUG("No Domain-Address found --> send empty Control-Message");
    icmpL4->SendMessage(controlMessage, controlDomain, icmpHeader, ICMP_TTL);
    return;
  }


  auto domain_it = m_BufferedMessageSet.find(dataDomain);
  if(domain_it == m_BufferedMessageSet.end())
    return;

  for(auto seed_it = seedDomain->second.begin(); seed_it != seedDomain->second.end(); seed_it++)
  {
    auto bufferedSeed_it = domain_it->second.find(seed_it->second->GetSeedId());
    if(bufferedSeed_it == domain_it->second.end())
      continue;

    std::vector<uint8_t> bits(0);

    for(auto message_it = bufferedSeed_it->second.begin(); message_it != bufferedSeed_it->second.end(); message_it++)
    {
      uint8_t difference = message_it->second->GetSequenceNr()-seed_it->second->GetMinSequenceNr();
      uint8_t byteIndex = difference/8;
      uint8_t bitIndex = difference%8;

      if((uint8_t)bits.size() < byteIndex+1)
        bits.resize(byteIndex+1, false);
      bits[byteIndex] |= 1 << (7-bitIndex); // transmit in network byte order --> for correct display in wireshark
    }

    std::vector<uint8_t> mplInfo;
    mplInfo.insert(mplInfo.end() ,bits.begin(), bits.end());

    // create and add the seed-info header
    uint32_t mplSeedInfoHeader = 0;
    mplSeedInfoHeader = (seed_it->second->GetMinSequenceNr() << 24) | ((mplInfo.size() & 0x3F) << 18) | (MplOptionHeader::SEED_16_BIT_ADDRESSING << 16) | ((uint16_t)seed_it->second->GetSeedId());

    for(uint8_t i = 0; i< 4; i++)
      mplInfo.insert(mplInfo.begin(),mplSeedInfoHeader >> (i<<3));

    controlMessage->AddAtEnd(Create<Packet> (&mplInfo[0], mplInfo.size()));
  }
  // checksum is calculated here
  icmpL4->SendMessage(controlMessage, controlDomain, icmpHeader, ICMP_TTL);
}

void RoutingProtocol::CreateControlTimer(Ipv6Address mplDomain)
{
  uint8_t bytes[16];
  mplDomain.GetBytes(bytes);
  Ipv6Address controlDomain;

  if(bytes[1] == MPL_DATA_DOMAIN || bytes[1] == MPL_CONTROL_DOMAIN)
  {
    bytes[1] = MPL_CONTROL_DOMAIN;
    controlDomain = Ipv6Address(bytes);
  }
  else
  {
    NS_LOG_DEBUG("Received a non MPL-Domain!");
    return;
  }

  if(m_controlTimers.find(controlDomain) == m_controlTimers.end())
  {
    TrickleTimer* timer = new TrickleTimer(m_TrickleIMinControl, m_TrickleIMaxControl, m_TrickleKControl);
    timer->SetFunction(&RoutingProtocol::SendControlMessage, this);
    timer->SetArguments(controlDomain);
    timer->Enable();
    m_controlTimers.insert(std::make_pair(controlDomain, std::pair<TrickleTimer*, uint8_t> {std::make_pair(timer, 0)}));
  }
  else
    NS_LOG_DEBUG("Tried to create another Control-Trickle!");
}

void RoutingProtocol::RemoveControlTimer(Ipv6Address mplDomain)
{
  uint8_t bytes[16];
  mplDomain.GetBytes(bytes);
  Ipv6Address controlDomain;

  if( bytes[0] == 0xFF && (bytes[1] == MPL_DATA_DOMAIN || bytes[1] == MPL_CONTROL_DOMAIN))
  {
    bytes[1] = MPL_CONTROL_DOMAIN;
    controlDomain = Ipv6Address(bytes);
  }
  else
  {
    NS_LOG_DEBUG("Received a non MPL-Domain!");
    return;
  }

  auto iter = m_controlTimers.find(controlDomain);
  if(iter != m_controlTimers.end())
  {
    delete iter->second.first;
    m_controlTimers.erase(iter);
  }
}

void RoutingProtocol::ResetTrickleControl(Ipv6Address dataDomain)
{
  uint8_t bytes[16];
  dataDomain.GetBytes(bytes);
  bytes[1] = MPL_CONTROL_DOMAIN; // switch to the corresponding data domain
  Ipv6Address controlDomain(bytes);

  if(m_EnableReactiveForwarding)
  {
    auto it = m_controlTimers.find(controlDomain);
    if(it != m_controlTimers.end())
    {
      if(it->second.first)
      {
        it->second.first->Reset();
        it->second.second = 0;
      }
      else
        CreateControlTimer(dataDomain);
    }
    else
      NS_LOG_DEBUG("No Trickle Timer found");
  }
}

bool RoutingProtocol::HandleControlMessage(Ptr<Packet> p, Ipv6Address mplDataDomain)
{
  std::vector<uint8_t> packetContent;
  packetContent.resize(p->GetSize());
  p->CopyData(&packetContent[0], p->GetSize());
  uint16_t recSeedId;
  uint8_t recMinSeq;
  uint8_t seedInfoLength;
  bool inconsistentEvent = false;

  std::map<uint64_t, std::map<uint8_t ,MplDataMessage*>> copyBufferedMessages;
  auto copyBufferedMessages_it = m_BufferedMessageSet.find(mplDataDomain);

  if(copyBufferedMessages_it == m_BufferedMessageSet.end())
  {
    NS_LOG_DEBUG("Received from DataDomain we currently do not know!");
    return true;
  }
  copyBufferedMessages = copyBufferedMessages_it->second; // copy the current messages in a second map

  if(packetContent.empty()) // handle a control message with no seed-info blocks
  {
    // iterate over all all the remaining messages to reset their trickle timer if the message is relevant for the neighbour node
    auto domain_it = m_BufferedMessageSet.find(mplDataDomain);

    if(domain_it != m_BufferedMessageSet.end())
    {
      for(auto seed_it = domain_it->second.begin(); seed_it != domain_it->second.end(); seed_it++)
      {
        for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); message_it++)
        {
          inconsistentEvent = true;
          message_it->second->ResetTrickle();
        }
      }
    }
  }

  while(!packetContent.empty())
  {
    recMinSeq = packetContent[0];
    seedInfoLength = packetContent[1] >> 2;
    std::vector<uint8_t> seedInfo;

    switch(packetContent[1] & 0x03)
    {
      case MplOptionHeader::SEED_16_BIT_ADDRESSING:
        recSeedId = packetContent[2] << 8 | packetContent[3];
        seedInfo.insert(seedInfo.begin(), packetContent.begin()+4, packetContent.begin()+4+seedInfoLength);
        packetContent.erase(packetContent.begin(), packetContent.begin()+4+seedInfoLength); // erase the current seedInfo
        break;
      case MplOptionHeader::IPV6_ADDRESSING:
      case MplOptionHeader::SEED_64_BIT_ADDRESSING:
      case MplOptionHeader::SEED_128_BIT_ADDRESSING:
        // TODO implement missing addressing schemes
        break;
    }

    //check if there are differences in the seed-Entries
    mpl::SeedSetEntry* seedEntry = GetSeedSetEntry(mplDataDomain, recSeedId);
    if(!seedEntry)
    {
      NS_LOG_DEBUG("Received an MPL-Seed that we currently do not know!");
      inconsistentEvent = true;
    }
    else
    {
      auto seed_it = copyBufferedMessages.find(recSeedId);
      uint8_t realSeqNr;
      for(uint8_t i = 0; i < seedInfo.size(); i++)
      {
        for(uint8_t bit = 0; bit < 8; bit++)
        {
          realSeqNr = recMinSeq+i*8+bit;
          if(CompareSequenceNumbersSmaller(realSeqNr, seedEntry->GetMinSequenceNr()))
          {
            NS_LOG_DEBUG("Message to check is already to old --> continue with the next");
            // no need to remove anything, as we have already deleted these messages
            continue;
          }

          if(seedInfo[i] & (1 << (7-bit))) // transmit in network byte order --> for correct display in wireshark
          {
            MplDataMessage* message = GetMessage(mplDataDomain, recSeedId, realSeqNr);
            if(!message)
            {
              NS_LOG_DEBUG("Found a message we currently do not have!");
              inconsistentEvent = true;
            }
            else
            {
              //Both know the message so we can remove it from our copied list
              seed_it->second.erase(realSeqNr);
            }
          }
        }
      }

      // iterate over all all the remaining messages to reset their trickle timer if the message is relevant for the neighbour node
      for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); message_it++)
      {
        if(!CompareSequenceNumbersSmaller(message_it->first, recMinSeq))
        {
          inconsistentEvent = true;
          message_it->second->ResetTrickle();
        }
      }
    }
  }

  return inconsistentEvent;
}

void RoutingProtocol::SubscribeToDataDomain(Ipv6Address dataDomain)
{
  m_subscribedDataDomains.insert(dataDomain);
  if(m_EnableReactiveForwarding)
    CreateControlTimer(dataDomain);

  m_BufferedMessageSet.insert(std::make_pair(dataDomain, std::map<uint64_t, std::map<uint8_t, MplDataMessage*>>()));

  Ptr<Ipv6L3Protocol> l3prot = m_myAddress->GetObject<Ipv6L3Protocol>();
  if(!l3prot)
    NS_FATAL_ERROR("L3-Protocol not available!");

  l3prot->AddMulticastAddress(dataDomain); // subscribe to multicast domain
}

void RoutingProtocol::UnsubscribeFromDataDomain(Ipv6Address dataDomain)
{
  m_subscribedDataDomains.erase(dataDomain);
  if(m_EnableReactiveForwarding)
    RemoveControlTimer(dataDomain);

  auto domain_it = m_BufferedMessageSet.begin();
  if(domain_it != m_BufferedMessageSet.end())
  {
    {
      for(auto seed_it = domain_it->second.begin(); seed_it != domain_it->second.end(); seed_it++)
      {
        for(auto message_it = seed_it->second.begin(); message_it != seed_it->second.end(); message_it++)
        {
          delete message_it->second;
        }
        seed_it->second.clear();
      }
      domain_it->second.clear();
    }
    m_BufferedMessageSet.erase(dataDomain);
  }

  Ptr<Ipv6L3Protocol> l3prot = m_myAddress->GetObject<Ipv6L3Protocol>();
  if(!l3prot)
    NS_FATAL_ERROR("L3-Protocol not available!");

  l3prot->RemoveMulticastAddress(dataDomain); // unsubscribe from multicast domain
}


void RoutingProtocol::NotifyInterfaceUp (uint32_t interface)
{

}


void RoutingProtocol::NotifyInterfaceDown (uint32_t interface)
{

}


void RoutingProtocol::NotifyAddAddress (uint32_t interface, Ipv6InterfaceAddress address)
{

}


void RoutingProtocol::NotifyRemoveAddress (uint32_t interface, Ipv6InterfaceAddress address)
{

}


void RoutingProtocol::NotifyAddRoute (Ipv6Address dst, Ipv6Prefix mask, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse)
{

}


void RoutingProtocol::NotifyRemoveRoute (Ipv6Address dst, Ipv6Prefix mask, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse)
{

}


void RoutingProtocol::SetIpv6 (Ptr<Ipv6> ipv6)
{
  m_myAddress = ipv6;
  m_myAddress->SetAttribute("IpForward", BooleanValue(true));
}


void RoutingProtocol::PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit) const
{

}

}
}
