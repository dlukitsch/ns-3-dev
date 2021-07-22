/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "mpl-helper.h"
#include "ns3/mpl.h"
#include "ns3/node-list.h"
#include "ns3/names.h"

namespace ns3 {

MplHelper::MplHelper() : Ipv6RoutingHelper()
{
  m_agentFactory.SetTypeId("ns3::mpl::RoutingProtocol");
}

MplHelper::~MplHelper()
{

}

MplHelper* MplHelper::Copy() const
{
  return new MplHelper(*this);
}

Ptr<Ipv6RoutingProtocol> MplHelper::Create(Ptr<Node> node) const
{
  Ptr<mpl::RoutingProtocol> agent = m_agentFactory.Create<mpl::RoutingProtocol>();
  node->AggregateObject(agent);
  return agent;
}

void MplHelper::Set(std::string name, const AttributeValue &value)
{
  m_agentFactory.Set(name, value);
}

}
