/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef AODV_IPV6_HELPER_H
#define AODV_IPV6_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/ipv6-routing-helper.h"

namespace ns3 {

class AodvIpv6Helper : public Ipv6RoutingHelper
{
public:
  AodvIpv6Helper ();

  AodvIpv6Helper* Copy (void) const;

  virtual Ptr<Ipv6RoutingProtocol> Create (Ptr<Node> node) const;

  void Set (std::string name, const AttributeValue &value);

private:
  ObjectFactory m_agentFactory;
};

}

#endif /* AODV_IPV6_HELPER_H */
