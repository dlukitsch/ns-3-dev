#include <fstream>
#include <utility>
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
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include "ns3/topology-reader-helper.h"
#include "ns3/custom-topology-reader.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/statistics-helper.h"
#include "ns3/node.h"
#include "ns3/energy-module.h"
#include "ns3/lr-wpan-radio-energy-model-helper.h"

using namespace ns3;

static const std::string filename = "Generic_Model";

NS_LOG_COMPONENT_DEFINE (filename);

int main (int argc, char *argv[])
{
  std::string simName = filename;
  std::string protocol = "Flooding";
  std::string input = "./examples/multicast/100_NodesRandomRectangle.csv";

  uint32_t packetSize = 50;
  uint32_t numPackets = 100;
  uint32_t simEndTime = 60;

  uint32_t interferPacketSize = 50;
  uint32_t interferenceSendIntervall = 100;
  uint32_t sendIntervall = 500;

  double txPower = 0;
  uint channelNumber = 11;
  double intTxPower = 0;
  uint intChannelNumber = 11;

  //protocol parameter configs
  uint32_t numHops = 50;

  bool proactiveMode = true;
  bool reactiveMode = true;
  uint32_t seedSetLifetime = 30; // Minutes
  uint32_t dataMessageIMin = 1000; // Milliseconds
  uint32_t dataMessageIMax = 0; // MaxTime = dataMessageIMin*2^dataMessageIMax
  uint32_t dataMessageK = 1;
  uint32_t dataMessageTimerExpirations = 3;
  uint32_t controlMessageIMin = 1000; // Milliseconds
  uint32_t controlMessageIMax = 0; // MaxTime = controlMessageIMin*2^controlMessageIMax
  uint32_t controlMessageK = 1;
  uint32_t controlMessageTimerExpirations = 10;

  uint32_t numRuns = 5;
  uint32_t seed = 0;
  std::string mobilityMode = "fixed";

  Config::SetGlobal("ChecksumEnabled", BooleanValue(true));

  CommandLine cmd (__FILE__);
  cmd.AddValue ("simName", "Output name of the current simulation.",simName);
  cmd.AddValue ("simRuns", "Number of independent runs for this simulation", numRuns);
  cmd.AddValue ("protocol", "Protocol to simulate.",protocol);
  cmd.AddValue ("input", "Name of the input file.",input);
  cmd.AddValue ("txPower", "Sending Power of the normal nodes.",txPower);
  cmd.AddValue ("channel", "The channel number for the normal sender nodes",channelNumber);
  cmd.AddValue ("packetSize", "Packet Size of the normal sender nodes.",packetSize);
  cmd.AddValue ("numPackets", "Number of Packets to send for the normal sender nodes.",numPackets);
  cmd.AddValue ("sendIntervall", "Sending interval of the sender nodes in milliseconds.", sendIntervall);
  cmd.AddValue ("simEndTime", "Simulation end-time in seconds.", simEndTime);
  cmd.AddValue ("interferencePacketSize", "Packet Size of the interference sender nodes.", interferPacketSize);
  cmd.AddValue ("interferenceSendIntervall", "Sending interval of the interference sender nodes in milliseconds.", interferenceSendIntervall);
  cmd.AddValue ("interferenceTxPower", "Sending Power of the interference nodes.",intTxPower);
  cmd.AddValue ("interferenceChannel", "The channel number for the interference sender nodes",intChannelNumber);

  cmd.AddValue ("proactiveForwarding","Enables the MPL Proactive-Forwarding mode.", proactiveMode);
  cmd.AddValue ("reactiveForwarding","Enables the MPL Reactive-Forwarding mode.", reactiveMode);
  cmd.AddValue ("seedSetEntryLifetime", "The minimum lifetime for an entry in the Seed Set.", seedSetLifetime);
  cmd.AddValue ("dataMessageIMin", "The minimum Trickle timer interval for MPL Data Message transmissions.",dataMessageIMin);
  cmd.AddValue ("dataMessageIMax", "The maximum Trickle timer interval, in doublings of the DataMessageIMin, for MPL Data Message transmissions.",dataMessageIMax);
  cmd.AddValue ("dataMessageK","The redundancy constant for MPL Data Message transmissions.", dataMessageK);
  cmd.AddValue ("dataMessageTimerExpirations","The number of Trickle timer expirations that occur before terminating the Trickle algorithm's retransmission of a given MPL Data Message.", dataMessageTimerExpirations);
  cmd.AddValue ("controlMessageIMin", "The minimum Trickle timer interval for MPL Control Message transmissions.", controlMessageIMin);
  cmd.AddValue ("controlMessageIMax", "The maximum Trickle timer interval for MPL Control Message transmissions.",controlMessageIMax);
  cmd.AddValue ("controlMessageK","The redundancy constant for MPL Control Message transmissions.",controlMessageK);
  cmd.AddValue ("controlMessageTimerExpirations","The number of Trickle timer expirations that occur before terminating the Trickle algorithm's retransmission of a given MPL Control Message.",controlMessageTimerExpirations);

  cmd.AddValue ("numHops","The number of hops a packet can make with the flooding algorithm", numHops);
  cmd.AddValue ("rngInit","Seed-Value to initialize the RNG-Generator.", seed);
  cmd.AddValue ("mobilityMode","The mobility-model to use for the simulation.", mobilityMode);

  cmd.Parse (argc, argv);

  LogComponentEnable (filename.c_str(), LOG_LEVEL_ALL);
  LogComponentEnable ("CustomTopologyReader", LOG_LEVEL_ALL);

  TopologyReaderHelper topologyHelper;
  std::map<uint , std::tuple<Vector, int, int, int, int > > topology;

  topologyHelper.SetFileName(input);
  topologyHelper.SetFileType("Custom");
  Ptr<CustomTopologyReader> topReader = DynamicCast<CustomTopologyReader>(topologyHelper.GetTopologyReader());

  topology = topReader->ReadTopology();
  NS_LOG_INFO ("Found " << topology.size() << " nodes in configured topology file");

  std::string path = "./output/" + simName + "/";

  std::ofstream seedFile;
  seedFile.open(path + "RNG_Init_Values.txt", std::ofstream::out | std::ofstream::trunc);
  seed = seed ? seed : std::time(0);

  for(uint simRun = 1; simRun <= numRuns; simRun++)
  {
    std::string filename =  simName + "_" + std::to_string(simRun);

    // initialize the simulation Random generator for an indpendant run
    RngSeedManager::SetSeed (seed);  // Changes seed to the epoch seconds
    RngSeedManager::SetRun (simRun);   // Changes run number to the current run number
    seedFile << "Seed-Value: " << seed << " Run: " << simRun << std::endl;

    //create all the containers and helpers which we will later install on the nodes
    NS_LOG_INFO ("Create node Container.");
    NodeContainer nodes(topology.size(),1);

    // Install mobility
    MobilityHelper mobility;
    // distribute the nodes
    Ptr<ListPositionAllocator> nodesPositionAlloc = CreateObject<ListPositionAllocator> ();
    for(auto it = topology.begin(); it != topology.end(); it++)
    {
      // install the nodes at their given positions
      nodesPositionAlloc->Add (std::get<0>(it->second));
    }

    mobility.SetPositionAllocator (nodesPositionAlloc);

    if(mobilityMode == "fixed")
      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    else if(mobilityMode == "random")
    {
      mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Mode", StringValue ("Time"),
                               "Distance", DoubleValue (20.0),
                               "Time", StringValue ("2s"),
                               "Speed", StringValue ("ns3::UniformRandomVariable[Min=50.0|Max=50.0]"),
                               "Bounds", RectangleValue (Rectangle (-1000.0, 1000.0, -1000.0, 1000.0)));
    }
    else
      NS_LOG_ERROR("Error: Wrong mobility-model specified!");


    mobility.Install(nodes);
    //AsciiTraceHelper ascii;
    //MobilityHelper::EnableAsciiAll (ascii.CreateFileStream (path + filename + ".mob"));

    LrWpanHelper lrWpanHelper;
    StatisticsHelper statHelper;

    NetDeviceContainer devContainer = lrWpanHelper.Install(nodes);
    NetDeviceContainer panIDContainers[2];
    NodeContainer normalNodes;
    LrWpanSpectrumValueHelper svh;
    Ptr<SpectrumValue> psd = svh.CreateTxPowerSpectralDensity (txPower, channelNumber);
    Ptr<SpectrumValue> intPsd = svh.CreateTxPowerSpectralDensity(intTxPower, intChannelNumber);

    uint normalNodeIndex = 0;
    uint interferenceNodeIndex = 0;

    // iterate over list to associate to correct PAN-ID and set the TX-Power and channel to use
    for(auto it = topology.begin(); it != topology.end(); it++)
    {
      Ptr<LrWpanPhy> phy = devContainer.Get(it->first)->GetObject<LrWpanNetDevice>()->GetPhy();

      if(std::get<3>(it->second) == -1 && std::get<4>(it->second) == -1)
      {
        normalNodes.Add(nodes.Get(it->first));
        phy->SetTxPowerSpectralDensity(psd);
        panIDContainers[0].Add(devContainer.Get(it->first));
        statHelper.Install(nodes.Get(it->first), DynamicCast<LrWpanNetDevice>(devContainer.Get(it->first)));
        normalNodeIndex++;
      }
      else
      {
        phy->SetTxPowerSpectralDensity(intPsd);
        panIDContainers[1].Add(devContainer.Get(it->first));
        interferenceNodeIndex++;
      }
    }

    /** Energy Model **/
    /***************************************************************************/
    /* energy source */
    BasicEnergySourceHelper basicSourceHelper;
    basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (0.01));
    EnergySourceContainer sources = basicSourceHelper.Install(normalNodes);
    LrWpanRadioEnergyModelHelper radioEnergyHelper;
    DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (panIDContainers[0], sources);
    /***************************************************************************/

    lrWpanHelper.AssociateToPan (devContainer, 10);
//    lrWpanHelper.EnablePcap(path+filename , panIDContainers[0], true); // enable pcap generation only for normal nodes
//    lrWpanHelper.EnableAsciiAll (ascii.CreateFileStream (path  + filename + ".tr"));


    /* Install IPv4/IPv6 stack */
    NS_LOG_INFO ("Install Internet stack.");
    InternetStackHelper internetv6;

    // Install IPv6 Stack on all nodes without MPL
    for(auto it = topology.begin(); it != topology.end(); it++)
    {
      if(protocol == "Flooding" || std::get<3>(it->second) != -1 || std::get<4>(it->second) != -1)
        internetv6.Install (nodes.Get(it->first));
    }

    // Install IPv6 Stack on all nodes with MPL
    if(protocol == "Mpl")
    {
      Ipv6ListRoutingHelper routingHelperWithMpl;
      MplHelper mplHelper;
      mplHelper.Set("ProactiveForwarding", BooleanValue(proactiveMode));
      mplHelper.Set("ReactiveForwarding", BooleanValue(reactiveMode));
      mplHelper.Set("SeedSetEntryLifetime", TimeValue(Minutes(seedSetLifetime)));
      mplHelper.Set("DataMessageIMin", TimeValue(MilliSeconds(dataMessageIMin)));
      mplHelper.Set("DataMessageIMax", UintegerValue(dataMessageIMax));
      mplHelper.Set("DataMessageK", UintegerValue(dataMessageK));
      mplHelper.Set("DataMessageTimerExpirations", UintegerValue(dataMessageTimerExpirations));
      mplHelper.Set("ControlMessageIMin", TimeValue(MilliSeconds(controlMessageIMin)));
      mplHelper.Set("ControlMessageIMax", UintegerValue(controlMessageIMax));
      mplHelper.Set("ControlMessageK", UintegerValue(controlMessageK));
      mplHelper.Set("ControlMessageTimerExpirations", UintegerValue(controlMessageTimerExpirations));

      routingHelperWithMpl.Add(mplHelper, 0);
      internetv6.SetRoutingHelper(routingHelperWithMpl);

      for(auto it = topology.begin(); it != topology.end(); it++)
      {
        if(std::get<3>(it->second) == -1 && std::get<4>(it->second) == -1)
          internetv6.Install (nodes.Get(it->first));
      }
    }

    // Install 6LowPan layer
    NS_LOG_INFO ("Install 6LoWPAN.");
    SixLowPanHelper sixlowpan;
    sixlowpan.SetDeviceAttribute("Rfc6282", BooleanValue(false));
    NetDeviceContainer six1 = sixlowpan.Install (devContainer);

    NS_LOG_INFO ("Assign addresses.");
    Ipv6AddressHelper ipv6;
    Ipv6InterfaceContainer interfaces = ipv6.AssignWithoutAddress(six1);

    lrWpanHelper.AssociateToPan (panIDContainers[1], 9);
    lrWpanHelper.AssociateToPan (panIDContainers[0], 10);

    NS_LOG_INFO ("Create Applications.");
    UdpClientHelper clientHelper;
    UdpServerHelper serverHelper;
    ApplicationContainer apps = ApplicationContainer();

    uint numSenders = 0;
    uint numRecievers = 0;
    uint numIntSenders = 0;
    uint numIntReceivers = 0;

    uint nodeIndex;

    for(auto it = topology.begin(); it != topology.end(); it++)
    {
      nodeIndex = it->first;

      if(std::get<3>(it->second) == -1 && std::get<4>(it->second) == -1) // check if it is not an interference sender/receiver
      {
        NS_LOG_INFO("Normal: Interface " << 0 << " Local-Link-IPv6: " << interfaces.GetLinkLocalAddress(nodeIndex));

        // handle the protocol specific configurations here
        if(protocol == "Mpl")
        {
          nodes.Get(nodeIndex)->GetObject<Icmpv6L4Protocol>()->SetAttribute("DAD", BooleanValue(false));
          nodes.Get(nodeIndex)->GetObject<mpl::RoutingProtocol>()->SubscribeToDataDomain(Ipv6Address("FF03::FC"));
        }
        else if (protocol == "Flooding")
        {
          Ptr<NetDevice> dev = six1.Get(nodeIndex);
          dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
          dev->SetAttribute ("MeshUnderRadius", UintegerValue (numHops)); // number of hops

          Ptr<Icmpv6L4Protocol> icmpProtocol = nodes.Get(nodeIndex)->GetObject<Icmpv6L4Protocol>();

          icmpProtocol->SetAttribute("DAD", BooleanValue(false));
          icmpProtocol->SetAttribute("MaxUnicastSolicit", IntegerValue(0));
          icmpProtocol->SetAttribute("MaxMulticastSolicit", IntegerValue(0));

          nodes.Get(nodeIndex)->GetObject<Ipv6L3Protocol>()->SetAttribute ("SendIcmpv6Redirect", BooleanValue (false));
        }

        interfaces.SetForwarding (nodeIndex, true);

        if(std::get<1>(it->second) != -1) // handle normal sender
        {
          if(protocol == "Mpl")
            clientHelper.SetAttribute("RemoteAddress", AddressValue (Ipv6Address("FF03::FC")));
          else if(protocol == "Flooding")
          {
            clientHelper.SetAttribute("RemoteAddress", AddressValue (interfaces.GetLinkLocalAddress(std::get<1>(it->second))));
          }

          clientHelper.SetAttribute("RemotePort", UintegerValue(10));
          clientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
          clientHelper.SetAttribute("MaxPackets", UintegerValue(numPackets));
          clientHelper.SetAttribute("Interval", TimeValue(MilliSeconds(sendIntervall)));
          apps.Add(clientHelper.Install(nodes.Get(nodeIndex)));
          numSenders++;
        }
        else if(std::get<2>(it->second) != -1) //handle normal receiver
        {
          serverHelper.SetAttribute ("Port", UintegerValue (10));
          apps.Add(serverHelper.Install (nodes.Get(nodeIndex)));
          numRecievers++;
        }
      }
      else
      {
        Ptr<NetDevice> dev = six1.Get(nodeIndex);
        dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
        dev->SetAttribute ("MeshUnderRadius", UintegerValue (numHops)); // number of hops

        Ptr<Icmpv6L4Protocol> icmpProtocol = nodes.Get(nodeIndex)->GetObject<Icmpv6L4Protocol>();

        icmpProtocol->SetAttribute("DAD", BooleanValue(false));
        icmpProtocol->SetAttribute("MaxUnicastSolicit", IntegerValue(1));
        icmpProtocol->SetAttribute("MaxMulticastSolicit", IntegerValue(1));

        nodes.Get(nodeIndex)->GetObject<Ipv6L3Protocol>()->SetAttribute ("SendIcmpv6Redirect", BooleanValue (false));

        NS_LOG_INFO("Int: Interface " << 0 << " Local-Link-IPv6: " << interfaces.GetLinkLocalAddress(nodeIndex));

        if(std::get<3>(it->second) != -1) // handle interference sender
        {
          clientHelper.SetAttribute("RemoteAddress",  AddressValue(interfaces.GetLinkLocalAddress(std::get<3>(it->second))));
          clientHelper.SetAttribute("RemotePort", UintegerValue(9));
          clientHelper.SetAttribute("PacketSize", UintegerValue(interferPacketSize));
          clientHelper.SetAttribute("MaxPackets", UintegerValue(UINT32_MAX));
          clientHelper.SetAttribute("Interval", TimeValue(MilliSeconds(interferenceSendIntervall)));
          apps.Add(clientHelper.Install(nodes.Get(nodeIndex)));
          numIntSenders++;
        }
        else // handle interference receiver
        {
          serverHelper.SetAttribute ("Port", UintegerValue (9));
          apps.Add(serverHelper.Install (nodes.Get(nodeIndex)));
          numIntReceivers++;
        }
      }
    }

    NS_LOG_INFO(std::endl <<  "There are a total of " << nodes.GetN() << " nodes." );
    NS_LOG_INFO("Created " << devContainer.GetN() << " Net-Devices: " << numRecievers+numSenders << " normal Net-Devices and " << numIntReceivers+numIntSenders << " interference Net-Devices.");
    NS_LOG_INFO("Installed " << numSenders << " normal Senders and " << numRecievers << " normal Receivers, that use the " << protocol << " routing protocol.");
    NS_LOG_INFO("Installed " << numIntSenders << " interference Senders and " << numIntReceivers << " interference Receivers to create traffic on the medium." << std::endl);

    apps.Start (Seconds (0.0));
    apps.Stop (Seconds(simEndTime));

    AnimationInterface anim (path + filename + ".xml");
    anim.EnablePacketMetadata (); // Optional

    NS_LOG_INFO ("Start Run " << simRun << "/" << numRuns << " of " << simName);

    Simulator::Stop(Seconds(simEndTime));
    Simulator::Run ();

    /*
    for (DeviceEnergyModelContainer::Iterator iter = deviceModels.Begin (); iter != deviceModels.End (); iter ++)
      {
        double energyConsumed = (*iter)->GetTotalEnergyConsumption ();
        NS_LOG_UNCOND ("End of simulation (" << Simulator::Now ().GetSeconds ()
                       << "s) Total energy consumed by radio = " << energyConsumed << "J");
      }
*/
    Simulator::Destroy ();

    // TODO:
    // percentage of control data vs. use-data
    // percentage of duplicated-use-data vs. new-use-data
    // delay from sending to receiving (timestamp is inside of tseq-header)
    // reception percentage
    // num received/sent bytes/packets
    // Time of enabled RX/TX component
    // ...

    // store the results of the different runs in a common folder
    filename = "results";
    std::system(("mkdir -p " + path + filename).c_str());
    statHelper.PrintResultsCsvStyle(nodes, path + filename + "/" + filename + "_" + std::to_string(simRun) + ".csv", true, true);

    NS_LOG_INFO ("Finished Run " << simRun << "/" << numRuns << " of " << simName);
  }

  seedFile.close();
  NS_LOG_INFO ("All runs done.");
}
