/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
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
#include "ns3/statistics-helper.h"
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/multi-model-spectrum-channel.h>
#include "ns3/lr-wpan-radio-energy-model-helper.h"
#include "ns3/energy-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/udp-client-server-helper.h"


#define IPV6_PREFIX "2001::"

using namespace ns3;

static const std::string filename = "aodv-lrwpan";

int main (int argc, char **argv)
{
 // LogComponentEnable ("AodvIpv6RoutingProtocol", LOG_LEVEL_ALL);
 // LogComponentEnable ("AodvIpv6RoutingTable", LOG_LEVEL_ALL);

 // LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);

  std::string simName = filename;
  std::string path = "./output/" + simName + "/";

  NodeContainer nodes;

  uint32_t size = 10;
  /// Distance between nodes, meters
  double step = 75;
  /// Simulation time, seconds
  double totalTime = 3800;
  /// Write per-device PCAP traces if true
  bool pcap = true;
  /// Print routes if true
  bool printRoutes = false;

  SeedManager::SetSeed (12345);
  CommandLine cmd (__FILE__);

  cmd.AddValue ("pcap", "Write PCAP traces.", pcap);
  cmd.AddValue ("printRoutes", "Print routing table dumps.", printRoutes);
  cmd.AddValue ("size", "Number of nodes.", size);
  cmd.AddValue ("time", "Simulation time, s.", totalTime);
  cmd.AddValue ("step", "Grid step, m", step);

  cmd.Parse (argc, argv);

  LrWpanHelper lrWpanHelper;
  StatisticsHelper statHelper;

  auto folderName = "results/";
  std::system(("mkdir -p " + path + folderName).c_str());

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
                                 "GridWidth", UintegerValue (10),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  LrWpanHelper lrwpanHelper;
  NetDeviceContainer netDevices = lrwpanHelper.Install(nodes);

  LrWpanSpectrumValueHelper svh;
  Ptr<SpectrumValue> psd = svh.CreateTxPowerSpectralDensity (0, 11); // 0dBm and channel 11

  for(int it = 0; it < nodes.GetN(); it++)
  {
    Ptr<LrWpanPhy> phy = netDevices.Get(it)->GetObject<LrWpanNetDevice>()->GetPhy();
    phy->SetAttribute("TxPower", DoubleValue(0)); // 0dBm
    phy->SetTxPowerSpectralDensity(psd);
  }

  BasicEnergySourceHelper basicSourceHelper;
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (100));
  basicSourceHelper.Set ("PeriodicEnergyUpdateInterval", TimeValue (Simulator::GetMaximumSimulationTime())); // do not reload the battery
  EnergySourceContainer sources = basicSourceHelper.Install(nodes);
  LrWpanRadioEnergyModelHelper radioEnergyHelper;
  DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (netDevices, sources);

  lrwpanHelper.AssociateToPan(netDevices, 10);
  lrwpanHelper.EnablePcapAll(path + simName, false);

  AodvIpv6Helper aodv;
  // you can configure AODV attributes here using aodv.Set(name, value)
  InternetStackHelper stack;
  stack.SetIpv4StackInstall(false);
  stack.SetRoutingHelper (aodv); // has effect on the next Install ()
  stack.Install (nodes);

  if (printRoutes)
  {
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (path + "aodv.routes", std::ios::out | std::ios::trunc);
    aodv.PrintRoutingTableAllAt (Seconds (4), routingStream);
  }

  SixLowPanHelper sixlowpan;
  //sixlowpan.SetDeviceAttribute("Rfc6282", BooleanValue(false));
  NetDeviceContainer six1 = sixlowpan.Install (netDevices);
  sixlowpan.AddContext (six1, 0, Ipv6Prefix ("2001::", 64), Time (Hours (2)));

  Ipv6AddressHelper ipv6;
  ipv6.SetBase(Ipv6Address(IPV6_PREFIX), Ipv6Prefix(64));

  Ipv6InterfaceContainer interfaces = ipv6.Assign(six1);
  for(uint i = 0; i < nodes.GetN(); i++)
  {
    nodes.Get(i)->GetObject<Ipv6L3Protocol>()->SetAttribute ("SendIcmpv6Redirect", BooleanValue (false));
    interfaces.SetForwarding(i, true);
  }

//  ApplicationContainer p = ApplicationContainer();
//  UdpClientHelper clientHelper;
//  UdpServerHelper serverHelper;

  for(int i = 0; i < interfaces.GetN(); i++)
    for(int j = 0; j < 2; j++)
      std::cout << "Node " << i <<  " has IP: "  << interfaces.GetAddress(i, j) << " on interface " << j << std::endl;

  /*
  clientHelper.SetAttribute("RemotePort", UintegerValue(10));
  clientHelper.SetAttribute("PacketSize", UintegerValue(20));
  clientHelper.SetAttribute("MaxPackets", UintegerValue(60));
  clientHelper.SetAttribute("Interval", TimeValue(Minutes(1)));

  serverHelper.SetAttribute ("Port", UintegerValue (10));

//  for(auto i = 1; i < nodes.GetN(); i++ )
//  {
    clientHelper.SetAttribute("RemoteAddress", AddressValue (interfaces.GetAddress(9, 1)));
    p.Add(clientHelper.Install(nodes.Get(0)));

    p.Add(serverHelper.Install(nodes.Get(9)));
 // }

*/
  Ping6Helper ping;
  ping.SetIfIndex(1);
  ping.SetRemote(interfaces.GetAddress(size-1, 1));
  ping.SetAttribute("PacketSize", UintegerValue(20));
 // ping.SetAttribute("MaxPackets", UintegerValue(60));
 // ping.SetAttribute("Interval", TimeValue(Minutes(1)));
  ApplicationContainer p = ping.Install (nodes.Get (0));

  p.Start (Seconds (0));
  p.Stop (Seconds (totalTime) - Seconds (0.001));

  AnimationInterface anim (path + "lrwpan-aodv.xml");
  anim.EnablePacketMetadata (); // Optional

  std::cout << "Starting simulation for " << totalTime << " s ...\n";

  Simulator::Stop (Seconds (totalTime));
  Simulator::Run ();
  Simulator::Destroy ();

  statHelper.PrintResultsCsvStyle(nodes, path + folderName + simName + ".csv", true, true);

  return 0;
}
