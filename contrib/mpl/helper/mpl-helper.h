/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MPL_HELPER_H
#define MPL_HELPER_H

#include "ns3/ipv6-routing-helper.h"
#include "ns3/object-factory.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/mpl.h"


namespace ns3 {

class MplHelper : public Ipv6RoutingHelper
{
public:
  MplHelper();
  ~MplHelper();

  MplHelper* Copy(void) const;

  virtual Ptr<Ipv6RoutingProtocol> Create(Ptr<Node> node) const;

  void Set (std::string name, const AttributeValue &value);

  void UnsubscribeFromMplDomain(Ptr<NetDevice>& netDevice, Ipv6Address multicastAddress);
  void SubscribeToMplDomain(Ptr<NetDevice>& netDevice, Ipv6Address deviceIp,Ipv6Address multicastAddress);

private:
  ObjectFactory m_agentFactory;
};

}

#endif /* MPL_HELPER_H */

