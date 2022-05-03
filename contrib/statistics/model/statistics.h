/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef STATISTICS_H
#define STATISTICS_H

#include <ns3/object.h>
#include "ns3/packet.h"
#include "ns3/lr-wpan-net-device.h"
#include "ns3/lr-wpan-mac-header.h"

namespace ns3 {

class Statistics :  public Object
{
public:
  static TypeId GetTypeId(void);

  Statistics();
  virtual ~Statistics();

  void InstallTraces(Ptr<LrWpanPhy> phy, bool withEnergyModel);

  std::string GetResultString();
  std::string GetCsvStyleString();
  std::string GetCsvHeaderString();

  void SetNodeId(uint32_t nodeId);

private:
  uint32_t m_nodeId;

  uint m_numSentPackets;
  uint m_sentBytes;
  uint m_sentDataPackets;
  uint m_sentDataBytes;
  uint m_sentControlPackets;
  uint m_sentControlBytes;

  uint m_numSentDropPackets;
  uint m_sentDropBytes;
  uint m_sentDropDataPackets;
  uint m_sentDropDataBytes;
  uint m_sentDropControlPackets;
  uint m_sentDropControlBytes;

  uint m_numRecPackets;
  uint m_recBytes;
  uint m_recDataPackets;
  uint m_recDataBytes;
  uint m_recControlPackets;
  uint m_recControlBytes;

  uint m_numRxDropPackets;
  uint m_rxDropBytes;
  uint m_rxDropDataPackets;
  uint m_rxDropDataBytes;
  uint m_rxDropControlPackets;
  uint m_rxDropControlBytes;

  uint m_doubleSentPackets;
  uint m_doubleSentBytes;

  uint m_doubleSentDataPackets;
  uint m_doubleSentDataBytes;
  uint m_doubleSentControlPackets;
  uint m_doubleSentControlBytes;

  uint m_doubleRecPackets;
  uint m_doubleRecBytes;

  uint m_doubleRecDataPackets;
  uint m_doubleRecDataBytes;
  uint m_doubleRecControlPackets;
  uint m_doubleRecControlBytes;

  uint m_newCompletedDataPackets;

  uint m_minTransmissionDelay;
  uint m_maxTransmissionDelay;
  uint m_medianTransmissionDelay;
  double m_varianceTransmissionDelay;

  Time m_nodeLifeTime;
  double m_energyUsage;
  double m_simEndTime;

  Time m_lastCall;

  Time m_RxOn;
  Time m_RxOff;
  Time m_RxBusy;

  Time m_TxOn;
  Time m_TxOff;
  Time m_TxBusy;

  Time m_TxRxOff;

  Time m_Idle;
  Time m_Busy;

  struct cmpPacket {
      bool operator()(const Ptr<Packet>& a, const Ptr<Packet>& b) const
      {
        return a->GetUid() < b->GetUid();
      }
  };

  // store all the messages with their data content once, so that we have an history of previously handled messages
  std::set<Ptr<Packet>, cmpPacket> m_messages;
  std::vector<uint64_t> m_delayTimes; // store the delays of every successfull transmitted packet in ms


  void CalculateDelayTimes();
  bool TryToInsertPacket(Ptr<Packet> packet);
  bool IsDataMessage(Ptr<Packet> packet);
  bool CheckIfInterferenceMessage(Ptr<Packet> packet);

  void TransceiverStateTraceSink(LrWpanPhyEnumeration oldState, LrWpanPhyEnumeration newState);

  void NodeEnergyDepleted(bool oldState, bool newState);
  void EnergyUsage(double oldEnergy, double newEnergy);

  void TxEndTraceSink (Ptr<const Packet> packet);
  void TxEndDropTraceSink (Ptr<const Packet> packet);

  void RxEndTraceSink (Ptr<const Packet> packet, double SINR);
  void RxEndDropTraceSink (Ptr<const Packet> packet);
};

}

#endif /* STATISTICS_H */

