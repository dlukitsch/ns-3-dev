/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef STATISTICS_HELPER_H
#define STATISTICS_HELPER_H

#include "ns3/statistics.h"
#include "ns3/object-factory.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/lr-wpan-net-device.h"

namespace ns3 {

class StatisticsHelper
{
  public:
    StatisticsHelper();
    ~StatisticsHelper();

    void Install(Ptr<Node> node, Ptr<LrWpanNetDevice> netDev);

    std::string PrintResults(NodeContainer nodes);

    void Set (std::string name, const AttributeValue &value);

  private:
    ObjectFactory m_agentFactory;
  };

}

#endif /* STATISTICS_HELPER_H */

