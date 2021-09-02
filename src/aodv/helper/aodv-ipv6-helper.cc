/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "aodv-ipv6-helper.h"
#include "ns3/aodv-ipv6-routing-protocol.h"
#include "ns3/node-list.h"
#include "ns3/names.h"
#include "ns3/ptr.h"
#include "ns3/ipv6-list-routing.h"

namespace ns3
{

AodvIpv6Helper::AodvIpv6Helper() : Ipv6RoutingHelper()
{
  m_agentFactory.SetTypeId("ns3::aodvIpv6::RoutingProtocol");
}

AodvIpv6Helper* AodvIpv6Helper::Copy() const
{
  return new AodvIpv6Helper(*this);
}

Ptr<Ipv6RoutingProtocol> AodvIpv6Helper::Create(Ptr<Node> node) const
{
  Ptr<aodvIpv6::RoutingProtocol> agent = m_agentFactory.Create<aodvIpv6::RoutingProtocol>();
  node->AggregateObject(agent);
  return agent;
}

void AodvIpv6Helper::Set(std::string name, const AttributeValue &value)
{
  m_agentFactory.Set(name, value);
}

}
