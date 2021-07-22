/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef CUSTOM_TOPOLOGY_READER_H
#define CUSTOM_TOPOLOGY_READER_H

#include "topology-reader.h"
#include "ns3/core-module.h"

namespace ns3 {


class CustomTopologyReader : public TopologyReader
{
public:

  static TypeId GetTypeId (void);

  CustomTopologyReader();
  virtual ~CustomTopologyReader ();

  virtual NodeContainer Read(void);

  std::map<uint , std::tuple<Vector, int, int, int, int > > ReadTopology (void);

private:

  CustomTopologyReader (const CustomTopologyReader&);

  CustomTopologyReader& operator= (const CustomTopologyReader&);

};

// end namespace ns3
};


#endif
