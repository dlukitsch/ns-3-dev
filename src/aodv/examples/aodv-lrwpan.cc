/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <cmath>
#include "ns3/aodv-ipv6-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ping6-helper.h"
#include "ns3/lr-wpan-helper.h"
#include "ns3/sixlowpan-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main (int argc, char **argv)
{
  LogComponentEnable ("AodvIpv6RoutingProtocol", LOG_LEVEL_ALL);
 // LogComponentEnable ("AodvIpv6RoutingTable", LOG_LEVEL_ALL);

  LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);


  NodeContainer nodes;

  uint32_t size = 15;
  /// Distance between nodes, meters
  double step = 50;
  /// Simulation time, seconds
  double totalTime = 20;
  /// Write per-device PCAP traces if true
  bool pcap = true;
  /// Print routes if true
  bool printRoutes = true;

  SeedManager::SetSeed (12345);
  CommandLine cmd (__FILE__);

  cmd.AddValue ("pcap", "Write PCAP traces.", pcap);
  cmd.AddValue ("printRoutes", "Print routing table dumps.", printRoutes);
  cmd.AddValue ("size", "Number of nodes.", size);
  cmd.AddValue ("time", "Simulation time, s.", totalTime);
  cmd.AddValue ("step", "Grid step, m", step);

  cmd.Parse (argc, argv);


  std::cout << "Creating " << (unsigned)size << " nodes " << step << " m apart.\n";
  nodes.Create (size);
  // Name nodes
  for (uint32_t i = 0; i < size; ++i)
    {
      std::ostringstream os;
      os << "node-" << i;
      Names::Add (os.str (), nodes.Get (i));
    }
  // Create static grid
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (step),
                                 "DeltaY", DoubleValue (step),
                                 "GridWidth", UintegerValue (5),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  LrWpanHelper lrwpanHelper;
  NetDeviceContainer netDevices = lrwpanHelper.Install(nodes);
  lrwpanHelper.AssociateToPan(netDevices, 10);
  lrwpanHelper.EnablePcapAll("aodv_lrwpan", true);

  AodvIpv6Helper aodv;
  // you can configure AODV attributes here using aodv.Set(name, value)
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv); // has effect on the next Install ()
  stack.Install (nodes);

  if (printRoutes)
  {
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("aodv.routes", std::ios::out | std::ios::trunc);
    aodv.PrintRoutingTableAllAt (Seconds (4), routingStream);
  }

  SixLowPanHelper sixlowpan;
  sixlowpan.SetDeviceAttribute("Rfc6282", BooleanValue(false));
  NetDeviceContainer six1 = sixlowpan.Install (netDevices);

  Ipv6AddressHelper ipv6;
  Ipv6InterfaceContainer interfaces = ipv6.AssignWithoutAddress(six1);
  for(uint i = 0; i < interfaces.GetN(); i++)
    interfaces.SetForwarding(i, true);

  Ping6Helper ping;
  ping.SetRemote(interfaces.GetAddress(size-1, 0));
  ping.SetLocal(interfaces.GetAddress(0, 0));
  ping.SetAttribute("PacketSize", UintegerValue(20));
  ping.SetIfIndex(0);

  ApplicationContainer p = ping.Install (nodes.Get (0));
  p.Start (Seconds (0));
  p.Stop (Seconds (totalTime) - Seconds (0.001));

  AnimationInterface anim ("lrwpan-aodv.xml");
  anim.EnablePacketMetadata (); // Optional

  std::cout << "Starting simulation for " << totalTime << " s ...\n";

  Simulator::Stop (Seconds (totalTime));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
