/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <fstream>
#include <utility>
#include <cstdlib>
#include <sstream>
#include "ns3/node-container.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "ns3/log.h"

#include "custom-topology-reader.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CustomTopologyReader");

NS_OBJECT_ENSURE_REGISTERED (CustomTopologyReader);

TypeId CustomTopologyReader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CustomTopologyReader")
    .SetParent<TopologyReader> ()
    .SetGroupName ("TopologyReader")
    .AddConstructor<CustomTopologyReader> ()
  ;
  return tid;
}

CustomTopologyReader::CustomTopologyReader ()
{
  //NS_LOG_FUNCTION (this);
}

CustomTopologyReader::~CustomTopologyReader ()
{
  //NS_LOG_FUNCTION (this);
}

NodeContainer CustomTopologyReader::Read()
{
  NodeContainer nodes;
  return nodes;
}

std::map<uint , std::tuple<Vector, int, int, int, int > >
CustomTopologyReader::ReadTopology (void)
{
  std::ifstream topgen;
  topgen.open (GetFileName ().c_str ());

  std::map<uint , std::tuple<Vector, int, int, int, int > > nodeMap;

  if ( !topgen.is_open () )
  {
    NS_LOG_WARN ("Custom topology file object is not open, check file name, path and permissions for " << GetFileName());
    return nodeMap;
  }
  NS_LOG_INFO("Successfully opened" << GetFileName());

  std::istringstream lineBuffer;
  std::string line;

  // get rid of first line
  getline (topgen,line);

  while (getline (topgen,line))
  {
    std::vector<int> fields;
    fields.clear();
    lineBuffer.clear ();
    lineBuffer.str (line);

    while(lineBuffer.good())
    {
      std::string field;
      getline(lineBuffer, field, ',');

      try
      {
        fields.push_back(stoi(field));
      }
      catch (const std::invalid_argument& ia) {
        fields.push_back(-1);
      }
    }
    nodeMap.insert(std::make_pair(fields[0], std::make_tuple(Vector(fields[1], fields[2], fields[3]), fields[4], fields[5], fields[6], fields[7])));
  }
  topgen.close ();

  return nodeMap;
}

} /* namespace ns3 */
