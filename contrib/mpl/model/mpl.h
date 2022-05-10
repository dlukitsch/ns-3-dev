/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MPL_H
#define MPL_H

#include "ns3/node.h"
#include "ns3/ipv6-routing-protocol.h"
#include "ns3/ipv6-interface.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/ipv6-extension-header.h"
#include "ns3/ipv6-route.h"
#include "mpl-data-message.h"
#include "ns3/watchdog.h"
#include "ns3/icmpv6-header.h"

#include "seed-set-entry.h"


namespace ns3 {
namespace mpl {

class RoutingProtocol : public Ipv6RoutingProtocol
{
public:

  static const uint8_t ICMP_TYPE;
  static const uint8_t ICMP_CODE;
  static const uint8_t ICMP_TTL;
  static const uint8_t MPL_DATA_DOMAIN;
  static const uint8_t MPL_CONTROL_DOMAIN;

  static TypeId GetTypeId(void);

  RoutingProtocol();
  virtual ~RoutingProtocol();

  virtual Ptr<Ipv6Route> RouteOutput (Ptr<Packet> p, const Ipv6Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);

  /**
   * \brief Route an input packet (to be forwarded or locally delivered)
   *
   * This lookup is used in the forwarding process.  The packet is
   * handed over to the Ipv6RoutingProtocol, and will get forwarded onward
   * by one of the callbacks.  The Linux equivalent is ip_route_input ().
   * There are four valid outcomes, and a matching callbacks to handle each.
   *
   * \param p received packet
   * \param header input parameter used to form a search key for a route
   * \param idev Pointer to ingress network device
   * \param ucb Callback for the case in which the packet is to be forwarded
   *            as unicast
   * \param mcb Callback for the case in which the packet is to be forwarded
   *            as multicast
   * \param lcb Callback for the case in which the packet is to be locally
   *            delivered
   * \param ecb Callback to call if there is an error in forwarding
   * \returns true if the Ipv6RoutingProtocol takes responsibility for
   *          forwarding or delivering the packet, false otherwise
   */
  virtual bool RouteInput  (Ptr<const Packet> p, const Ipv6Header &header, Ptr<const NetDevice> idev,
                            UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                            LocalDeliverCallback lcb, ErrorCallback ecb);

  /**
   * \brief Notify when specified interface goes UP.
   *
   * Protocols are expected to implement this method to be notified of the state change of
   * an interface in a node.
   * \param interface the index of the interface we are being notified about
   */
  virtual void NotifyInterfaceUp (uint32_t interface);

  /**
   * \brief Notify when specified interface goes DOWN.
   *
   * Protocols are expected to implement this method to be notified of the state change of
   * an interface in a node.
   * \param interface the index of the interface we are being notified about
   */
  virtual void NotifyInterfaceDown (uint32_t interface);

  /**
   * \brief Notify when specified interface add an address.
   *
   * Protocols are expected to implement this method to be notified whenever
   * a new address is added to an interface. Typically used to add a 'network route' on an
   * interface. Can be invoked on an up or down interface.
   * \param interface the index of the interface we are being notified about
   * \param address a new address being added to an interface
   */
  virtual void NotifyAddAddress (uint32_t interface, Ipv6InterfaceAddress address);

  /**
   * \brief Notify when specified interface add an address.
   *
   * Protocols are expected to implement this method to be notified whenever
   * a new address is removed from an interface. Typically used to remove the 'network route' of an
   * interface. Can be invoked on an up or down interface.
   * \param interface the index of the interface we are being notified about
   * \param address a new address being added to an interface
   */
  virtual void NotifyRemoveAddress (uint32_t interface, Ipv6InterfaceAddress address);

  /**
   * \brief Notify a new route.
   *
   * Typically this is used to add another route from IPv6 stack (i.e. ICMPv6
   * redirect case, ...).
   * \param dst destination address
   * \param mask destination mask
   * \param nextHop nextHop for this destination
   * \param interface output interface
   * \param prefixToUse prefix to use as source with this route
   */
  virtual void NotifyAddRoute (Ipv6Address dst, Ipv6Prefix mask, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse = Ipv6Address::GetZero ());

  /**
   * \brief Notify route removing.
   * \param dst destination address
   * \param mask destination mask
   * \param nextHop nextHop for this destination
   * \param interface output interface
   * \param prefixToUse prefix to use as source with this route
   */
  virtual void NotifyRemoveRoute (Ipv6Address dst, Ipv6Prefix mask, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse = Ipv6Address::GetZero ());

  /**
   * \brief Typically, invoked directly or indirectly from ns3::Ipv6::SetRoutingProtocol
   * \param ipv6 the ipv6 object this routing protocol is being associated with
   */
  virtual void SetIpv6 (Ptr<Ipv6> ipv6);

  /**
   * \brief Print the Routing Table entries
   *
   * \param stream The ostream the Routing table is printed to
   * \param unit The time unit to be used in the report
   */
  virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit = Time::S) const;

  void SubscribeToDataDomain(Ipv6Address dataDomain);

  void UnsubscribeFromDataDomain(Ipv6Address dataDomain);

private:

  Ptr<Ipv6Route> FindRoute(Ipv6Address mplDomain, Ipv6Address sourceAddress);

  void RetransmitMessage(Ipv6Address mplDomain, uint64_t seedId, MplDataMessage* dataMessage);

  void InsertMessage(MplDataMessage* message, Ipv6Address domain, uint64_t seedId);
  MplDataMessage* GetMessage(Ipv6Address mplDomain, uint64_t seedId, uint8_t sequenceNr);

  void InsertSeedSetEntry(Ipv6Address mplDomain, uint64_t seedId, uint8_t minSequence);
  SeedSetEntry* GetSeedSetEntry(Ipv6Address mplDomain, uint64_t seedId);

  MplDataMessage* CreateDataMessage(Ptr<Packet> p, Ptr<Ipv6Route> route, uint8_t sequenceNr, Ipv6Address mplDomain, uint64_t seedId);

  void ClearMessagesForSeedId(Ipv6Address mplDomain, uint64_t seedId);

  void DeleteOldMessages(Ipv6Address mplDomain, uint8_t minSequenceNr, uint64_t seedId);
  void DeleteSeedSetEntry(Ipv6Address domain, uint64_t seedId);

  bool CompareSequenceNumbersSmaller(uint16_t x, uint16_t y) const;

  bool AcquireMFlagValue(Ipv6Address mplDomain, uint64_t seedId, uint8_t currentSequenceNr);

  void SendControlMessage(Ipv6Address controlDomain);

  void ResetTrickleControl(Ipv6Address dataDomain);

  void CreateControlTimer(Ipv6Address mplDomain);
  void RemoveControlTimer(Ipv6Address mplDomain);

  bool HandleControlMessage(Ptr<Packet> p, Ipv6Address mplDataDomain);

  void DeleteMessagesForDomain(Ipv6Address domain);
  bool CheckIfSubscriptedToDomain(Ipv6Address dataDomain);

  struct cmpIpv6Address {
      bool operator()(const Ipv6Address& a, const Ipv6Address& b) const {
          return a < b;
      }
  };

  Ptr<Ipv6> m_myAddress;

  bool m_EnableProactiveForwarding;
  bool m_EnableReactiveForwarding;
  Time m_SeedSetEntryLifetime;

  Time m_TrickleIMinData;
  uint8_t m_TrickleIMaxData;
  uint16_t m_TrickleKData;
  uint8_t m_TrickleTimerExpirationsData;

  Time m_TrickleIMinControl;
  uint8_t m_TrickleIMaxControl;
  uint16_t m_TrickleKControl;
  uint8_t m_TrickleTimerExpirationsControl;

  uint16_t m_seedID;
  uint8_t m_sequenceNr;

  std::map<Ipv6Address, std::pair<TrickleTimer*, uint8_t>, cmpIpv6Address> m_controlTimers;
  std::set<Ipv6Address, cmpIpv6Address> m_subscribedDataDomains;

  std::map<Ipv6Address, std::map<uint64_t, SeedSetEntry*>, cmpIpv6Address> m_SeedSet; // subscribed multicast addresses --> Seed --> minimum sequence Number
  std::map<Ipv6Address, std::map<uint64_t, std::map<uint8_t ,MplDataMessage*>>, cmpIpv6Address> m_BufferedMessageSet; // subscribed multicast addresses --> seed --> Set of received data messages
};

}
}

#endif /* MPL_H */

