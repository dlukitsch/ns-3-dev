/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "statistics.h"
#include "ns3/packet.h"
#include "ns3/lr-wpan-net-device.h"
#include "ns3/simulator.h"
#include "ns3/lr-wpan-mac-header.h"
#include "ns3/lr-wpan-radio-energy-model.h"
#include <algorithm>

NS_LOG_COMPONENT_DEFINE ("Statistics");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Statistics);

TypeId Statistics::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::Statistics")
    .SetParent<Object> ()
    .SetGroupName ("Statistics")
    .AddConstructor<Statistics> ()
    .AddAttribute ("SimEndTime",
                       "The end time of the simulation in seconds.",
                       DoubleValue (3800),
                       MakeDoubleAccessor (&Statistics::m_simEndTime),
                       MakeDoubleChecker<double> ());
  return tid;
}

Statistics::Statistics()
{
  m_nodeId = 0;

  m_numSentPackets = 0;
  m_sentBytes = 0;
  m_sentDataPackets = 0;
  m_sentDataBytes = 0;
  m_sentControlPackets = 0;
  m_sentControlBytes = 0;

  m_numSentDropPackets = 0;
  m_sentDropBytes = 0;
  m_sentDropDataPackets = 0;
  m_sentDropDataBytes = 0;
  m_sentDropControlPackets = 0;
  m_sentDropControlBytes = 0;

  m_numRecPackets = 0;
  m_recBytes = 0;
  m_recDataPackets = 0;
  m_recDataBytes = 0;
  m_recControlPackets = 0;
  m_recControlBytes = 0;

  m_numRxDropPackets = 0;
  m_rxDropBytes = 0;
  m_rxDropDataPackets = 0;
  m_rxDropDataBytes = 0;
  m_rxDropControlPackets = 0;
  m_rxDropControlBytes = 0;

  m_doubleSentPackets = 0;
  m_doubleSentBytes = 0;

  m_doubleSentDataPackets = 0;
  m_doubleSentDataBytes = 0;
  m_doubleSentControlPackets = 0;
  m_doubleSentControlBytes = 0;

  m_doubleRecPackets = 0;
  m_doubleRecBytes = 0;

  m_doubleRecDataPackets = 0;
  m_doubleRecDataBytes = 0;
  m_doubleRecControlPackets = 0;
  m_doubleRecControlBytes = 0;

  m_newCompletedDataPackets = 0;
  m_minTransmissionDelay = 0;
  m_maxTransmissionDelay = 0;
  m_varianceTransmissionDelay = 0;
  m_medianTransmissionDelay = 0;

  m_lastCall = Seconds(0);
  m_energyUsage = 0.0;

  m_RxOn = MicroSeconds(0);
  m_RxOff = MicroSeconds(0);
  m_RxBusy = MicroSeconds(0);

  m_TxOn = MicroSeconds(0);
  m_TxOff = MicroSeconds(0);
  m_TxBusy = MicroSeconds(0);

  m_Idle = MicroSeconds(0);

  m_TxRxOff = MicroSeconds(0);
  m_Busy = MicroSeconds(0);

  m_nodeLifeTime = Seconds(0);
  m_simEndTime = 0.0;
}

Statistics::~Statistics()
{

}

void Statistics::SetNodeId(uint32_t nodeId)
{
  m_nodeId = nodeId;
}

void Statistics::InstallTraces(Ptr<LrWpanPhy> phy, bool withEnergyModel)
{
  phy->TraceConnectWithoutContext("TrxStateValue", MakeCallback(&Statistics::TransceiverStateTraceSink , this));
  phy->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&Statistics::TxEndDropTraceSink, this));
  phy->TraceConnectWithoutContext("PhyTxEnd", MakeCallback(&Statistics::TxEndTraceSink, this));
  phy->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&Statistics::RxEndTraceSink, this));
  phy->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&Statistics::RxEndDropTraceSink, this));
  if(withEnergyModel)
  {
    phy->GetLrWpanRadioEnergyModel()->TraceConnectWithoutContext("TotalEnergyDepleted", MakeCallback(&Statistics::NodeEnergyDepleted, this));
    phy->GetLrWpanRadioEnergyModel()->TraceConnectWithoutContext("TotalEnergyConsumption", MakeCallback(&Statistics::EnergyUsage, this));
  }
}

void Statistics::NodeEnergyDepleted(bool oldState, bool newState)
{
  if(newState)
    m_nodeLifeTime = Now();
}

void Statistics::EnergyUsage(double oldEnergy, double newEnergy)
{
  m_energyUsage = newEnergy;
  //std::cout << "energy first: " <<  oldEnergy  <<  " new energy: " << newEnergy << std::endl;
}

void Statistics::TransceiverStateTraceSink(LrWpanPhyEnumeration oldState, LrWpanPhyEnumeration newState)
{
  Time vTimeDiff = Now() - m_lastCall;

  switch(oldState)
  {
    case IEEE_802_15_4_PHY_BUSY:
      m_Busy += vTimeDiff;
      m_TxOn += vTimeDiff;
      m_RxOn += vTimeDiff;
      break;
    case IEEE_802_15_4_PHY_BUSY_RX:
      m_RxBusy += vTimeDiff;
      m_RxOn += vTimeDiff;
      m_TxOff += vTimeDiff;
      break;
    case IEEE_802_15_4_PHY_BUSY_TX:
      m_TxBusy += vTimeDiff;
      m_TxOn += vTimeDiff;
      m_RxOff += vTimeDiff;
      break;
    case IEEE_802_15_4_PHY_IDLE:
      m_Idle += vTimeDiff;
      break;
    case IEEE_802_15_4_PHY_RX_ON:
      m_RxOn += vTimeDiff;
      m_TxOff += vTimeDiff;
      break;
    case IEEE_802_15_4_PHY_TRX_OFF:
    case IEEE_802_15_4_PHY_FORCE_TRX_OFF:
      m_TxRxOff += vTimeDiff;
      m_RxOff += vTimeDiff;
      m_TxOff += vTimeDiff;
      break;
    case IEEE_802_15_4_PHY_TX_ON:
      m_RxOff += vTimeDiff;
      m_TxOn += vTimeDiff;
      break;
    default:
      NS_LOG_INFO("Strange OldState " << oldState  << " reached!");
  }
  m_lastCall = Now();
}


void Statistics::TxEndDropTraceSink (Ptr<const Packet> packet)
{
  Ptr<Packet> copyPacket = packet->Copy();

  if(!CheckIfInterferenceMessage(copyPacket))
    return;

  m_numSentDropPackets++;
  m_sentDropBytes += copyPacket->GetSize();

  if(IsDataMessage(copyPacket))
  {
    m_sentDropDataBytes += copyPacket->GetSize();
    m_sentDropDataPackets++;
  }
  else
  {
    m_sentDropControlBytes += copyPacket->GetSize();
    m_sentDropControlPackets++;
  }
}

void Statistics::TxEndTraceSink(Ptr<const Packet> packet)
{
  Ptr<Packet> copyPacket = packet->Copy();

  if(!CheckIfInterferenceMessage(copyPacket))
    return;

  m_numSentPackets++;
  m_sentBytes += copyPacket->GetSize();

  if(IsDataMessage(copyPacket))
  {
    m_sentDataBytes += copyPacket->GetSize();
    m_sentDataPackets++;
    if(!TryToInsertPacket(copyPacket))
    {
      m_doubleSentBytes += copyPacket->GetSize();
      m_doubleSentDataBytes += copyPacket->GetSize();
      m_doubleSentDataPackets++;
      m_doubleSentPackets++;
    }
  }
  else
  {
    m_sentControlBytes += copyPacket->GetSize();
    m_sentControlPackets++;
    if(!TryToInsertPacket(copyPacket))
    {
      m_doubleSentBytes += copyPacket->GetSize();
      m_doubleSentControlBytes += copyPacket->GetSize();
      m_doubleSentControlPackets++;
      m_doubleSentPackets++;
    }
  }
}

void Statistics::RxEndTraceSink (Ptr<const Packet> packet, double SINR)
{
  Ptr<Packet> copyPacket = packet->Copy();

  if(!CheckIfInterferenceMessage(copyPacket))
    return;

  m_numRecPackets++;
  m_recBytes += copyPacket->GetSize();
  m_delayTimes.push_back(Simulator::Now().GetMilliSeconds() - packet->GetSendTimeStamp().GetMilliSeconds());

  if(IsDataMessage(copyPacket))
  {
    m_recDataBytes += copyPacket->GetSize();
    m_recDataPackets++;
    if(!TryToInsertPacket(copyPacket))
    {
      m_doubleRecBytes += copyPacket->GetSize();
      m_doubleRecDataBytes += copyPacket->GetSize();
      m_doubleRecDataPackets++;
      m_doubleRecPackets++;
    }
    else
      m_newCompletedDataPackets++;
  }
  else
  {
    m_recControlBytes += copyPacket->GetSize();
    m_recControlPackets++;
    if(!TryToInsertPacket(copyPacket))
    {
      m_doubleRecBytes += copyPacket->GetSize();
      m_doubleRecControlBytes += copyPacket->GetSize();
      m_doubleRecControlPackets++;
      m_doubleRecPackets++;
    }
  }
}

void Statistics::RxEndDropTraceSink (Ptr<const Packet> packet)
{
  Ptr<Packet> copyPacket = packet->Copy();

  if(!CheckIfInterferenceMessage(copyPacket))
    return;

  m_numRxDropPackets++;
  m_rxDropBytes += copyPacket->GetSize();

  if(IsDataMessage(copyPacket))
  {
    m_rxDropDataBytes += copyPacket->GetSize();
    m_rxDropDataPackets++;
  }
  else
  {
    m_rxDropControlBytes += copyPacket->GetSize();
    m_rxDropControlPackets++;
  }
}

bool Statistics::TryToInsertPacket(Ptr<Packet> packet)
{
    if(m_messages.find(packet) == m_messages.end())
    {
      m_messages.insert(packet);
      return true;
    }

  return false;
}

// Determines if a given Message is a Data-Message or not (currently only MPL and Flooding supported)
bool Statistics::IsDataMessage(Ptr<Packet> packet)
{
  std::ostringstream oss;
  packet->Print(oss); // Get the general structure of the packet

  if(oss.str().find("ns3::Icmpv6Header") != std::string::npos)
  {
    return false;
  }

  return true;
}

// check the destination and source PAN-IDs to distinguish between the interference messages and valid messages
bool Statistics::CheckIfInterferenceMessage(Ptr<Packet> packet)
{
  std::ostringstream oss;
  packet->Print(oss); // Get the general structure of the packet

  if(oss.str().find("LrWpanMacHeader") == std::string::npos)
    return false;

  LrWpanMacHeader lrWpanHeader;
  packet->PeekHeader(lrWpanHeader);

  // The interference Networks have a PAN-ID != 10
  if(lrWpanHeader.GetDstPanId() != 10 || lrWpanHeader.GetSrcPanId() != 10)
    return false;

  return true;
}

void Statistics::CalculateDelayTimes()
{
  double var = 0;
  double mean = 0;


  for (auto& n : m_delayTimes)
      mean += n;
  mean /= m_delayTimes.size();

  for(auto& n : m_delayTimes)
    var += (n - mean) * (n - mean);

   m_varianceTransmissionDelay = var / m_delayTimes.size();

  std::sort(m_delayTimes.begin(), m_delayTimes.end());
  if(m_delayTimes.size() != 0)
  {
    m_minTransmissionDelay = m_delayTimes.front();
    m_maxTransmissionDelay = m_delayTimes.back();

    if(m_delayTimes.size()%2)
      m_medianTransmissionDelay = m_delayTimes[m_delayTimes.size()/2];
    else
      m_medianTransmissionDelay = (m_delayTimes[m_delayTimes.size()/2] + m_delayTimes[m_delayTimes.size()/2-1])/2;
  }
}


std::string Statistics::GetResultString()
{
  std::ostringstream res;

  res << "NumSentPackets: " << m_numSentPackets << std::endl;
  res << "NumRecPackets: " << m_numRecPackets << std::endl;
  res << "NumRxDropPackets: " << m_numRxDropPackets << std::endl;

  res << "Time TxOff: " << m_TxOff.GetMilliSeconds() << "ms" << std::endl;
  res << "Time TxOn: " << m_TxOn.GetMilliSeconds() << "ms" << std::endl;
  res << "Time TxBusy: " << m_TxBusy.GetMilliSeconds() << "ms" << std::endl;
  res << "Time RxOff: " << m_RxOff.GetMilliSeconds() << "ms" << std::endl;
  res << "Time RxOn: " << m_RxOn.GetMilliSeconds() << "ms"<< std::endl;
  res << "Time RxBusy: " << m_RxBusy.GetMilliSeconds() << "ms" << std::endl;
  res << "Time Idle: " << m_Idle.GetMilliSeconds() << "ms" << std::endl;

  return res.str();
}

std::string Statistics::GetCsvHeaderString()
{
  std::ostringstream res;

  res << "Node ID" << ",";
  res << "Successfully Sent Packets" << ",";
  res << "Successfully Sent Bytes" << ",";
  res << "Successfully Sent Data-Packets" << ",";
  res << "Successfully Sent Data-Bytes" << ",";
  res << "Successfully Sent Control-Packets" << ",";
  res << "Successfully Sent Control-Bytes" << ",";

  res << "Dropped Sent Packets" << ",";
  res << "Dropped Sent Bytes" << ",";
  res << "Dropped Sent Data-Packets" << ",";
  res << "Dropped Sent Data-Bytes" << ",";
  res << "Dropped Sent Control-Packets" << ",";
  res << "Dropped Sent Control-Bytes" << ",";

  res << "Successfully Received Packets" << ",";
  res << "Successfully Received Bytes" << ",";
  res << "Successfully Received Data-Packets" << ",";
  res << "Successfully Received Data-Bytes" << ",";
  res << "Successfully Received Control-Packets" << ",";
  res << "Successfully Received Control-Bytes" << ",";

  res << "Dropped Received Packets" << ",";
  res << "Dropped Received Bytes" << ",";
  res << "Dropped Received Data-Packets" << ",";
  res << "Dropped Received Data-Bytes" << ",";
  res << "Dropped Received Control-Packets" << ",";
  res << "Dropped Received Control-Bytes" << ",";

  res << "Double Sent Packets" << ",";
  res << "Double Sent Bytes" << ",";

  res << "Double Sent Data-Packets" << ",";
  res << "Double Sent Data-Bytes" << ",";
  res << "Double Sent Control-Packets" << ",";
  res << "Double Sent Control-Bytes" << ",";

  res << "Double Received Packets" << ",";
  res << "Double Received Bytes" << ",";

  res << "Double Received Data-Packets" << ",";
  res << "Double Received Data-Bytes" << ",";
  res << "Double Received Control-Packets" << ",";
  res << "Double Received Control-Bytes" << ",";

  res << "Different Received Data Packets" << ",";
  res << "Min Transmission Delay [ms]" << ",";
  res << "Max Transmission Delay [ms]" << ",";
  res << "Median Transmission Delay [ms]" << ",";
  res << "Variance Transmission Delay [ms]" << ",";

  res << "Node LifeTime [s]" << ",";
  res << "Used Energy [J]" << ",";

  res << "Tx Off [ms]" << ",";
  res << "Tx On [ms]" << ",";
  res << "Tx Busy [ms]" << ",";
  res << "Rx Off [ms]" << ",";
  res << "Rx On [ms]" << ",";
  res << "Rx Busy [ms]" << ",";
  res << "Idle [ms]" << ",";
  res << "TxRx Off [ms]" << ",";
  res << "Busy [ms]";

  return res.str();
}

std::string Statistics::GetCsvStyleString()
{
  std::ostringstream res;

  CalculateDelayTimes();

  res << m_nodeId << ",";
  res << m_numSentPackets << ",";
  res << m_sentBytes << ",";
  res << m_sentDataPackets << ",";
  res << m_sentDataBytes << ",";
  res << m_sentControlPackets << ",";
  res << m_sentControlBytes << ",";

  res << m_numSentDropPackets << ",";
  res << m_sentDropBytes << ",";
  res << m_sentDropDataPackets << ",";
  res << m_sentDropDataBytes << ",";
  res << m_sentDropControlPackets << ",";
  res << m_sentDropControlBytes << ",";

  res << m_numRecPackets << ",";
  res << m_recBytes << ",";
  res << m_recDataPackets << ",";
  res << m_recDataBytes << ",";
  res << m_recControlPackets << ",";
  res << m_recControlBytes << ",";

  res << m_numRxDropPackets << ",";
  res << m_rxDropBytes << ",";
  res << m_rxDropDataPackets << ",";
  res << m_rxDropDataBytes << ",";
  res << m_rxDropControlPackets << ",";
  res << m_rxDropControlBytes << ",";

  res << m_doubleSentPackets << ",";
  res << m_doubleSentBytes << ",";

  res << m_doubleSentDataPackets << ",";
  res << m_doubleSentDataBytes << ",";
  res << m_doubleSentControlPackets << ",";
  res << m_doubleSentControlBytes << ",";

  res << m_doubleRecPackets << ",";
  res << m_doubleRecBytes << ",";

  res << m_doubleRecDataPackets << ",";
  res << m_doubleRecDataBytes << ",";
  res << m_doubleRecControlPackets << ",";
  res << m_doubleRecControlBytes << ",";

  res << m_newCompletedDataPackets << ",";

  res << m_minTransmissionDelay << ",";
  res << m_maxTransmissionDelay << ",";
  res << m_medianTransmissionDelay << ",";
  res << m_varianceTransmissionDelay << ",";

  if(m_nodeLifeTime.GetSeconds() == 0.0)
    res << m_simEndTime << ",";
  else
    res << m_nodeLifeTime.GetSeconds() << ",";

  res << m_energyUsage << ",";

  res << m_TxOff.GetMilliSeconds() << ",";
  res << m_TxOn.GetMilliSeconds() << ",";
  res << m_TxBusy.GetMilliSeconds() << ",";
  res << m_RxOff.GetMilliSeconds() << ",";
  res << m_RxOn.GetMilliSeconds() << ",";
  res << m_RxBusy.GetMilliSeconds() << ",";
  res << m_Idle.GetMilliSeconds() << ",";
  res << m_TxRxOff.GetMilliSeconds() << ",";
  res << m_Busy.GetMilliSeconds();

  return res.str();
}


}

