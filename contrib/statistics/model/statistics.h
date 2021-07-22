/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef STATISTICS_H
#define STATISTICS_H

#include <ns3/object.h>
#include "ns3/packet.h"
#include "ns3/lr-wpan-net-device.h"

namespace ns3 {


class Statistics :  public Object
{
public:
  static TypeId GetTypeId(void);

  Statistics();
  virtual ~Statistics();

  void InstallTraces(Ptr<LrWpanPhy> phy);

  std::string GetResultString();

private:
  uint m_numSentPackets;
  uint m_numRecPackets;
  uint m_numRxDropPackets;

  void TxEndTraceSink (Ptr<const Packet> packet);
  void RxEndTraceSink (Ptr<const Packet> packet, double SINR);
  void RxEndDropTraceSink (Ptr<const Packet> packet);
};

}

#endif /* STATISTICS_H */

