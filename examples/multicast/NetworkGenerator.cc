#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/node.h"
#include "ns3/object.h"

using namespace ns3;

static const std::string filename = "NetworkGenerator";

NS_LOG_COMPONENT_DEFINE (filename);

typedef enum
{
  EListPositionAllocator = 0,
  EGridPositionAllocator = 1,
  ERandomRectanglePositionAllocator = 2,
  ERandomBoxPositionAllocator = 3,
  ERandomDiscPositionAllocator = 4,
  EUniformDiscPositionAllocator = 5,
} positionAllocatorsType;

int main (int argc, char **argv)
{
  uint32_t numNodes = 100;
  std::string modelName = "./examples/multicast/temp.csv";
  uint32_t seed = 0;

  positionAllocatorsType allocator = ERandomRectanglePositionAllocator;

  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  LogComponentEnable (filename.c_str(), LOG_LEVEL_ALL);

  Ptr<PositionAllocator> pAllocator;

  seed = seed ? seed : std::time(NULL);
  RngSeedManager::SetSeed (seed);  // Changes seed to the epoch seconds
  RngSeedManager::SetRun (0);   // Changes run number to the current run number

  switch(allocator)
  {
    case EGridPositionAllocator:
    {
      Ptr<GridPositionAllocator> localAllocator = CreateObject<GridPositionAllocator>();
      localAllocator->SetAttribute("MinX", DoubleValue (3300.0)); // X starting value
      localAllocator->SetAttribute("MinY", DoubleValue (1540.0)); // Y starting value
      localAllocator->SetAttribute("DeltaX", DoubleValue (110.0)); // x increment
      localAllocator->SetAttribute("DeltaY", DoubleValue (110.0)); // y increment after one line
      localAllocator->SetAttribute("GridWidth", UintegerValue (15)); // number of nodes in one line
      localAllocator->SetAttribute("LayoutType", StringValue ("RowFirst"));
      pAllocator = localAllocator;
      break;
    }
    case ERandomRectanglePositionAllocator:
    {
      Ptr<RandomRectanglePositionAllocator> localAllocator = CreateObject<RandomRectanglePositionAllocator>();
      localAllocator->SetAttribute("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
      localAllocator->SetAttribute("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
      localAllocator->SetAttribute("Z", DoubleValue (0.0));
      pAllocator = localAllocator;
      break;
    }
    case ERandomBoxPositionAllocator:
    {
      Ptr<RandomBoxPositionAllocator> localAllocator = CreateObject<RandomBoxPositionAllocator>();
      localAllocator->SetAttribute("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=800.0]"));
      localAllocator->SetAttribute("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=800.0]"));
      localAllocator->SetAttribute("Z", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=0.0]"));
      pAllocator = localAllocator;
      break;
    }
    case ERandomDiscPositionAllocator:
    {
      Ptr<RandomDiscPositionAllocator> localAllocator = CreateObject<RandomDiscPositionAllocator>();
      // angle-range (gradients)
      localAllocator->SetAttribute("Theta", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=6.2830]"));
      // radius-range
      localAllocator->SetAttribute("Rho", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=800.0]"));
      localAllocator->SetAttribute("X", DoubleValue(0.0));
      localAllocator->SetAttribute("Y", DoubleValue(0.0));
      localAllocator->SetAttribute("Z", DoubleValue(0.0));
      pAllocator = localAllocator;
      break;
    }
    case EUniformDiscPositionAllocator:
    {
      Ptr<UniformDiscPositionAllocator> localAllocator = CreateObject<UniformDiscPositionAllocator>();
      // disc radius
      localAllocator->SetAttribute("rho", DoubleValue(800.0));
      localAllocator->SetAttribute("X", DoubleValue(0.0));
      localAllocator->SetAttribute("Y", DoubleValue(0.0));
      localAllocator->SetAttribute("Z", DoubleValue(0.0));
      pAllocator = localAllocator;
      break;
    }
    default:
    {
      NS_LOG_ERROR("ERROR: Cannot create a Network-Layout with the List-Position-Allocator!");

    }
  }

  std::ofstream modelFile;
  modelFile.open (modelName, std::ios::out | std::ios::trunc);
  modelFile << "ID,x,y,z,Sender,Receiver,Interference Sender,Interference Receiver,SeedValue: " << seed << " RunValue: 0"<< std::endl;

  // iterate over the mobility-model to get the positions
  for(uint i = 0; i < numNodes; i++)
  {
    Vector3D position = pAllocator->GetNext();

    modelFile << std::to_string(i) + ",";
    modelFile << std::to_string(position.x) + ",";
    modelFile << std::to_string(position.y) + ",";
    modelFile << std::to_string(position.z) + ",";
    modelFile << ",,," << std::endl;// manually set the sender/receiver and interference nodes
  }
  modelFile.close();
}
