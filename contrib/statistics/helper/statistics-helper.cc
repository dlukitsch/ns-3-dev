/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "statistics-helper.h"
#include "ns3/node-list.h"
#include "ns3/names.h"
#include "ns3/node-container.h"
#include "ns3/statistics.h"
#include "ns3/lr-wpan-net-device.h"
#include "ns3/net-device-container.h"

namespace ns3 {

StatisticsHelper::StatisticsHelper()
{
  m_agentFactory.SetTypeId("ns3::Statistics");
}


StatisticsHelper::~StatisticsHelper()
{

}

void StatisticsHelper::Install(Ptr<Node> node, Ptr<LrWpanNetDevice> netDev)
{
  Ptr<Statistics> agent = m_agentFactory.Create<Statistics>();
  node->AggregateObject(agent);

  Ptr<LrWpanPhy> phy = netDev->GetPhy();
  NS_ASSERT_MSG(phy, "ERROR: No LrWPAN Phy found!");

  // Set the trace-sinks for the nodes
  agent->InstallTraces(phy);
}


std::string StatisticsHelper::PrintResults(NodeContainer nodes)
{
  std::ostringstream oss;
  oss << std::endl;

  // iterate over all the nodes, get the aggregated Statistics object and print its result values
  for(uint i = 0; i < nodes.GetN(); i++)
  {
    Ptr<Statistics> stat = nodes.Get(i)->GetObject<Statistics>();
    if(stat)
    {
      oss << "Node: " << i << std::endl;
      oss << stat->GetResultString() << std::endl;
    }
  }

  return oss.str();
}


void StatisticsHelper::Set (std::string name, const AttributeValue &value)
{
  m_agentFactory.Set(name, value);
}


}
