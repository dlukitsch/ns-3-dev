/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "statistics-helper.h"
#include "ns3/node-list.h"
#include "ns3/names.h"
#include "ns3/node-container.h"
#include "ns3/statistics.h"
#include "ns3/lr-wpan-net-device.h"
#include "ns3/net-device-container.h"
#include <fstream>

namespace ns3 {

StatisticsHelper::StatisticsHelper()
{
  m_agentFactory.SetTypeId("ns3::Statistics");
}


StatisticsHelper::~StatisticsHelper()
{

}

void StatisticsHelper::Install(Ptr<Node> node, Ptr<LrWpanNetDevice> netDev, bool withEnergyModel, uint32_t simEndTime)
{
  Ptr<Statistics> agent = m_agentFactory.Create<Statistics>();
  agent->SetAttribute("SimEndTime", DoubleValue (simEndTime));

  node->AggregateObject(agent);

  agent->SetNodeId(node->GetId());

  Ptr<LrWpanPhy> phy = netDev->GetPhy();
  NS_ASSERT_MSG(phy, "ERROR: No LrWPAN Phy found!");

  // Set the trace-sinks for the nodes
  agent->InstallTraces(phy, withEnergyModel);
}


std::string StatisticsHelper::PrintResults(Ptr<Node> node)
{
  Ptr<Statistics> stat = node->GetObject<Statistics>();
  std::ostringstream oss;

  if(stat)
  {
    oss << "Node: " << node->GetId() << std::endl;
    oss << stat->GetResultString() << std::endl;
  }

  return oss.str();
}

std::string StatisticsHelper::PrintResults(NodeContainer nodes)
{
  std::ostringstream oss;
  oss << std::endl;

  for(uint i = 0; i < nodes.GetN(); i++)
  {
    oss << PrintResults(nodes.Get(i));
  }

  return oss.str();
}

void StatisticsHelper::PrintResultsCsvStyle(Ptr<Node> node, std::string filename, bool printHeader, bool newFile)
{
  std::ofstream fileStream;
  Ptr<Statistics> stat = node->GetObject<Statistics>();

  if(newFile)
  {
    std::remove(filename.c_str());
    fileStream.open(filename, std::ofstream::out);
  }
  else
    fileStream.open(filename, std::ofstream::out | std::ofstream::app);

  if(stat && fileStream.is_open())
  {
    if(printHeader)
      fileStream << stat->GetCsvHeaderString() << std::endl;

    fileStream << stat->GetCsvStyleString() << std::endl;
  }

  fileStream.close();
}

void StatisticsHelper::PrintResultsCsvStyle(NodeContainer nodes, std::string filename, bool printHeader, bool newFile)
{
  for(uint i = 0; i < nodes.GetN(); i++)
  {
    PrintResultsCsvStyle(nodes.Get(i), filename, printHeader, newFile);
    newFile = false;
    printHeader = false;
  }
}


void StatisticsHelper::Set (std::string name, const AttributeValue &value)
{
  m_agentFactory.Set(name, value);
}


}
