#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/netanim-module.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/address.h"
#include "ns3/global-value.h"
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include "ns3/energy-module.h"
#include "ns3/wifi-radio-energy-model-helper.h"

using namespace ns3;

static const std::string filename = "Flood_6LoWPAN_5_Nodes_Line_Structure_Example_with_interference";
static const std::string path = "./output/" + filename + "/" + filename;

NS_LOG_COMPONENT_DEFINE (filename);

int main (int argc, char **argv)
{
  bool verbose = true;
  uint8_t numInterferer = 2;
  uint32_t numNodes = 5+numInterferer;
  double distance = 75.0;
  uint32_t packetSize = 50;
  uint32_t numPackets = 50;
  uint32_t numHops = 50;

  std::string animFile = path + ".xml";

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("animFile",  "File Name for Animation Output", animFile);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnable (filename.c_str(), LOG_LEVEL_ALL);
    }

  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (numNodes);

  // Install mobility
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // distribute the nodes
  Ptr<ListPositionAllocator> nodesPositionAlloc = CreateObject<ListPositionAllocator> ();
  for(uint16_t i = 0; i < nodes.GetN()-numInterferer; i++)
    nodesPositionAlloc->Add (Vector (distance*i, 0.0, 0.0));

  for(uint16_t i = 0; i < numInterferer; i++)
    nodesPositionAlloc->Add (Vector (distance*i, 50.0, 0.0));

  mobility.SetPositionAllocator (nodesPositionAlloc);
  mobility.Install (nodes);

  NS_LOG_INFO ("Create channels.");
  Ptr<SpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
  Ptr<LogDistancePropagationLossModel> lossModelLogDist = CreateObject<LogDistancePropagationLossModel> ();
  Ptr<RangePropagationLossModel> lossModelRange = CreateObject<RangePropagationLossModel> ();
  lossModelRange->SetAttribute("MaxRange", DoubleValue(distance));
  channel->AddPropagationLossModel (lossModelLogDist);
  channel->AddPropagationLossModel(lossModelRange);

  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  channel->SetPropagationDelayModel (delayModel);

  LrWpanHelper lrWpanHelper;
  lrWpanHelper.SetChannel(channel);
  NetDeviceContainer devContainer = lrWpanHelper.Install(nodes);
  lrWpanHelper.AssociateToPan (devContainer, 10);

  //double txPower = 0;
  //uint32_t channelNumber = 11;

  //LrWpanSpectrumValueHelper svh;
  //Ptr<SpectrumValue> psd = svh.CreateTxPowerSpectralDensity (txPower, channelNumber);
  //nodes.Get(0)->GetDevice(0)->GetObject<LrWpanNetDevice>()->GetPhy()->SetTxPowerSpectralDensity(psd);

  std::cout << "Created " << devContainer.GetN() << " devices" << std::endl;
  std::cout << "There are " << nodes.GetN() << " nodes" << std::endl;

  /* Install IPv4/IPv6 stack */
  NS_LOG_INFO ("Install Internet stack.");

  InternetStackHelper internetv6;
  internetv6.SetIpv4StackInstall (false);
  internetv6.SetIpv6StackInstall(true);
  internetv6.Install (nodes);

  //TODO disable neighbor solitication

  // Install 6LowPan layer
  NS_LOG_INFO ("Install 6LoWPAN.");
  SixLowPanHelper sixlowpan;
  sixlowpan.SetDeviceAttribute("Rfc6282", BooleanValue(false));
  NetDeviceContainer six1 = sixlowpan.Install (devContainer);

  for (uint32_t i = 0; i < six1.GetN (); i++)
  {
    Ptr<NetDevice> dev = six1.Get (i);
    dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
    dev->SetAttribute ("MeshUnderRadius", UintegerValue (numHops)); // number of hops
  }

  NS_LOG_INFO ("Assign addresses.");
  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer interfaces = ipv6.AssignWithoutOnLink(six1);
//  interfaces.SetDefaultRouteInAllNodes (0);

  for(uint32_t i = 0; i < nodes.GetN(); i++)
  {
    interfaces.SetForwarding (i, true);
    nodes.Get(i)->GetObject<Icmpv6L4Protocol>()->SetAttribute("DAD", BooleanValue(false));
    nodes.Get(i)->GetObject<Icmpv6L4Protocol>()->SetAttribute("MaxUnicastSolicit", IntegerValue(0));
    nodes.Get(i)->GetObject<Icmpv6L4Protocol>()->SetAttribute("MaxMulticastSolicit", IntegerValue(0));

    nodes.Get(i)->GetObject<Ipv6L3Protocol>()->SetAttribute ("SendIcmpv6Redirect", BooleanValue (false));
    std::cout << "Flood: Interface " << i << " Local-Link-IPv6: " << interfaces.GetAddress(i, 0) << std::endl;
    std::cout << "Flood: Interface " << i << " Global-IPv6: " << interfaces.GetAddress(i, 1) << std::endl;
  }

/*
  // energy Source setup
  BasicEnergySourceHelper basicSourceHelper;
  // configure energy source
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (0.1));
  // install source
  EnergySourceContainer sources;
  sources.Add(basicSourceHelper.Install (nodes));

  // Energy using device Setup
  EnergyModel radioEnergyHelper;
  // configure radio energy model
  radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174));
  // install device model
  DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (six1, sources);
*/


  NS_LOG_INFO ("Create Applications.");
  UdpClientHelper clientHelper(Ipv6Address("fe80::ff:fe00:5"), 9);
  UdpClientHelper interferenceClientHelper(Ipv6Address("fe80::ff:fe00:7"), 10); // something just that we do create some traffic
  UdpServerHelper serverHelper(9);
  UdpServerHelper interferenceServerHelper(10);

  interferenceClientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
  interferenceClientHelper.SetAttribute("MaxPackets", UintegerValue(UINT32_MAX));
  interferenceClientHelper.SetAttribute("Interval", TimeValue(MilliSeconds(100)));

  clientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
  clientHelper.SetAttribute("MaxPackets", UintegerValue(numPackets));
  //echoClientHelper.SetAttribute("Interval", TimeValue(Minutes(10)));

  ApplicationContainer apps = clientHelper.Install (nodes.Get(0));
  apps.Add(serverHelper.Install(nodes.Get(numNodes-1-numInterferer)));

  apps.Add(interferenceClientHelper.Install(nodes.Get(numNodes-2)));
  apps.Add(interferenceServerHelper.Install(nodes.Get(numNodes-1)));

  apps.Start (Seconds (0.0));
  apps.Stop (Minutes (5.0));

  AsciiTraceHelper ascii;
  lrWpanHelper.EnableAsciiAll (ascii.CreateFileStream (path + ".tr"));
  lrWpanHelper.EnablePcapAll (path + ".xml", true);

  AnimationInterface anim (animFile);
  anim.EnablePacketMetadata (); // Optional

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
