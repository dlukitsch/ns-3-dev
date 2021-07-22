#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mpl-helper.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/address.h"
#include "ns3/global-value.h"
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>

using namespace ns3;

static const std::string filename = "Mpl_6LoWPAN_5_Nodes_Line_Structure_Example";
static const std::string path = "./output/" + filename + "/" + filename;

NS_LOG_COMPONENT_DEFINE (filename);

int main (int argc, char **argv)
{
  bool verbose = true;
  uint32_t numNodes = 3;
  double distance = 500.0;
  uint32_t packetSize = 50;
  uint32_t numPackets = 50;

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
  for(uint16_t i = 0; i < nodes.GetN(); i++)
    nodesPositionAlloc->Add (Vector (distance*i, 0.0, 0.0));

  mobility.SetPositionAllocator (nodesPositionAlloc);
  mobility.Install (nodes);

  NS_LOG_INFO ("Create channels.");
  Ptr<SpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
  Ptr<LogDistancePropagationLossModel> lossModelLogDist = CreateObject<LogDistancePropagationLossModel> ();
  //Ptr<RangePropagationLossModel> lossModelRange = CreateObject<RangePropagationLossModel> ();
  //lossModelRange->SetAttribute("MaxRange", DoubleValue(distance));
  channel->AddPropagationLossModel (lossModelLogDist);
  //channel->AddPropagationLossModel(lossModelRange);

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

  MplHelper mplHelper;
  Ipv6ListRoutingHelper routingHelper;

  routingHelper.Add(mplHelper, 0);
  internetv6.SetRoutingHelper(routingHelper);
  internetv6.Install (nodes);


  // Install 6LowPan layer
  NS_LOG_INFO ("Install 6LoWPAN.");
  SixLowPanHelper sixlowpan;
  sixlowpan.SetDeviceAttribute("Rfc6282", BooleanValue(false));
  NetDeviceContainer six1 = sixlowpan.Install (devContainer);

 /* for (uint32_t i = 0; i < nodesFlood.GetN (); i++)
  {
    Ptr<NetDevice> dev = nodesFlood.Get (i);
    dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
    dev->SetAttribute ("MeshUnderRadius", UintegerValue (10)); // number of hops
  }
*/

  NS_LOG_INFO ("Assign addresses.");
  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer interfaces = ipv6.AssignWithoutOnLink (six1);

  for(uint32_t i = 0; i < nodes.GetN(); i++)
  {
    interfaces.SetForwarding (i, true);
    nodes.Get(i)->GetObject<Icmpv6L4Protocol>()->SetAttribute("DAD", BooleanValue(false));

    Ptr<mpl::RoutingProtocol> routingProt = nodes.Get(i)->GetObject<mpl::RoutingProtocol>();
    Ipv6Address domain("FF03::FC");

    routingProt->SubscribeToDataDomain(domain);

    std::cout << "Mpl: Interface " << i << " Local-Link-IPv6: " << interfaces.GetAddress(i, 0) << std::endl;
    std::cout << "Mpl: Interface " << i << " Global-IPv6: " << interfaces.GetAddress(i, 1) << std::endl;
  }

  NS_LOG_INFO ("Create Applications.");
  UdpEchoClientHelper echoClientHelper(Ipv6Address("FF03::FC"), 5000);

  echoClientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
  echoClientHelper.SetAttribute("MaxPackets", UintegerValue(numPackets));
  //echoClientHelper.SetAttribute("Interval", TimeValue(Minutes(10)));

  ApplicationContainer apps = echoClientHelper.Install (nodes.Get(0));

  apps.Start (Seconds (0.0));
  apps.Stop (Minutes (20.0));

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
