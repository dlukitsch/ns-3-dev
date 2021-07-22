/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "statistics.h"
#include "ns3/packet.h"
#include "ns3/lr-wpan-net-device.h"

NS_LOG_COMPONENT_DEFINE ("Statistics");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Statistics);

TypeId Statistics::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::Statistics")
    .SetParent<Object> ()
    .SetGroupName ("Statistics")
    .AddConstructor<Statistics> ();
  return tid;
}

Statistics::Statistics()
{
  m_numSentPackets = 0;
  m_numRecPackets = 0;
  m_numRxDropPackets = 0;
}

Statistics::~Statistics()
{

}

void Statistics::InstallTraces(Ptr<LrWpanPhy> phy)
{
  phy->TraceConnectWithoutContext("PhyTxEnd", MakeCallback(&Statistics::TxEndTraceSink, this));
  phy->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&Statistics::RxEndTraceSink, this));
  phy->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&Statistics::RxEndDropTraceSink, this));
}


void Statistics::TxEndTraceSink (Ptr<const Packet> packet)
{
  m_numSentPackets++;

}

void Statistics::RxEndTraceSink (Ptr<const Packet> packet, double SINR)
{
  m_numRecPackets++;
  //std::cout << "SINR: " << std::to_string(SINR) << std::endl;
}

void Statistics::RxEndDropTraceSink (Ptr<const Packet> packet)
{
  m_numRxDropPackets++;
}


std::string Statistics::GetResultString()
{
  std::ostringstream res;

  res << "NumSentPackets: " << m_numSentPackets << std::endl;
  res << "NumRecPackets: " << m_numRecPackets << std::endl;
  res << "NumRxDropPackets: " << m_numRxDropPackets << std::endl;

  return res.str();
}


}

