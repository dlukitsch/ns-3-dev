#include "mpl-data-message.h"

namespace ns3 {
namespace mpl {

MplDataMessage::MplDataMessage(Time minInterval, uint8_t doublings, uint16_t redundancy, Ptr<Packet> packet, Ptr<Ipv6Route> route, uint8_t sequenceNr)
{
  m_retransmission = 0;
  m_trickle.SetParameters(minInterval, doublings, redundancy);
  m_packet = packet;
  m_sequenceNr = sequenceNr;
  m_route = route;
}

MplDataMessage::~MplDataMessage()
{

}


uint8_t MplDataMessage::GetRetransmissions() const
{
  return m_retransmission;
}

void MplDataMessage::IncreaseRetransmission()
{
  m_retransmission++;
}

Ptr<Packet> MplDataMessage::GetPacket() const
{
  return m_packet;
}

TrickleTimer* MplDataMessage::GetTrickle()
{
  return &m_trickle;
}

uint8_t MplDataMessage::GetSequenceNr()
{
  return m_sequenceNr;
}

Ptr<Ipv6Route> MplDataMessage::GetRoute()
{
  return m_route;
}

void MplDataMessage::ResetTrickle()
{
  m_trickle.Reset();
  m_retransmission = 0;
}


}
}
