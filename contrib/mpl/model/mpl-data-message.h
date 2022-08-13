#ifndef MPL_DATA_MESSAGE_H
#define MPL_DATA_MESSAGE_H

#include "ns3/trickle-timer.h"
#include "ns3/packet.h"
#include "ns3/ipv6-route.h"

namespace ns3
{
namespace mpl
{

class MplDataMessage
{
public:

  MplDataMessage(Time minInterval, uint8_t doublings, uint16_t redundancy, Ptr<Packet> packet, Ptr<Ipv6Route> route, uint8_t sequenceNr);
  ~MplDataMessage();

  uint8_t GetRetransmissions() const;
  Ptr<Packet> GetPacket() const;
  TrickleTimer* GetTrickle();
  void IncreaseRetransmission();
  void ResetTrickle();
  void InconsistentTrickleEvent();
  uint8_t GetSequenceNr();
  Ptr<Ipv6Route> GetRoute();

private:
  TrickleTimer m_trickle;
  Ptr<Packet> m_packet;
  uint8_t m_retransmission;
  uint8_t m_sequenceNr;

  Ptr<Ipv6Route> m_route;
};

}
}


#endif
