/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/dchc-net-device.h"
#include "ns3/context-manager.h"
#include "ns3/dchc-helper.h"
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/pointer.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DCHCExample");

void SwitchNetwork(NetDeviceContainer DCHCDeviceContainer, NodeContainer net2);

int main(int argc, char* argv[])
{
	bool verbose = true;

	CommandLine cmd;
	cmd.AddValue("verbose", "Tell application to log if true", verbose);

	cmd.Parse(argc, argv);

	LogComponentEnable("DCHCExample", LOG_LEVEL_ALL);
	LogComponentEnable("Ping6Application", LOG_LEVEL_ALL);
	LogComponentEnable("DCHCNetDevice", LOG_LEVEL_ALL);
	LogComponentEnable("DCHCHelper", LOG_LOGIC);
	LogComponentEnable("ContextManager", LOG_LEVEL_ALL);
	//LogComponentEnable("CompressedHeader", LOG_LEVEL_ALL);
	//LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_ALL);

	NS_LOG_INFO("Create nodes.");

	Ptr<Node> mobileNode = CreateObject<Node>();
	Ptr<Node> networkServer = CreateObject<Node>();
	Ptr<Node> servingGateway = CreateObject<Node>();

	NodeContainer net1(mobileNode, networkServer);
	NodeContainer net2(mobileNode, servingGateway);
	NodeContainer allNodes(mobileNode, networkServer, servingGateway);

	NS_LOG_INFO("Create IPv6 Internet Stack");
	InternetStackHelper internetv6;
	internetv6.Install(allNodes);

	NS_LOG_INFO("Create channels.");

	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
	NetDeviceContainer netDeviceContainer1 = pointToPoint.Install(net1);
	NetDeviceContainer netDeviceContainer2 = pointToPoint.Install(net2);

	NetDeviceContainer allNetDeviceContainer;
	allNetDeviceContainer.Add(netDeviceContainer1.Get(0));
	allNetDeviceContainer.Add(netDeviceContainer1.Get(1));
	allNetDeviceContainer.Add(netDeviceContainer2.Get(1));

	NS_LOG_INFO("Create DCHC");

	DCHCHelper dchc;
	NetDeviceContainer DCHCDeviceContainer = dchc.Install(allNetDeviceContainer);

	NS_LOG_INFO("Create networks and assign IPv6 Addresses.");

	Ipv6AddressHelper ipv6;
	ipv6.SetBase(Ipv6Address("2001:db8::"), Ipv6Prefix(64));
	Ipv6InterfaceContainer netInterfaces1 = ipv6.Assign(NetDeviceContainer(DCHCDeviceContainer.Get(0), DCHCDeviceContainer.Get(1)));

	uint32_t packetSize = 1024;
	uint32_t maxPacketCount = 2;
	Time interPacketInterval = Seconds(4);

	Ping6Helper ping6;
	ping6.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
	ping6.SetAttribute("Interval", TimeValue(interPacketInterval));
	ping6.SetAttribute("PacketSize", UintegerValue(packetSize));

	ping6.SetLocal(netInterfaces1.GetAddress(0, 1));
	ping6.SetRemote(netInterfaces1.GetAddress(1, 1));

	ApplicationContainer apps;
	apps = ping6.Install(net1.Get(0));
	NS_LOG_INFO("Packets in network 1.");
	apps.Start(Seconds(2.0));
	apps.Stop(Seconds(10.0));

	Simulator::Schedule(Seconds(18), &SwitchNetwork, DCHCDeviceContainer, net2);

	NS_LOG_INFO("Run Simulation.");
	Simulator::Run();
	Simulator::Destroy();
	NS_LOG_INFO("Done.");
}

void SwitchNetwork(NetDeviceContainer DCHCDeviceContainer, NodeContainer net2)
{
	NS_LOG_INFO("Moving to network operator 2.");

	ApplicationContainer apps;
	Ipv6AddressHelper ipv6;
	Ping6Helper ping6;

	uint32_t packetSize = 1024;
	uint32_t maxPacketCount = 2;
	Time interPacketInterval = Seconds(4);
	ping6.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
	ping6.SetAttribute("Interval", TimeValue(interPacketInterval));
	ping6.SetAttribute("PacketSize", UintegerValue(packetSize));

	ipv6.SetBase(Ipv6Address("2001:dba::"), Ipv6Prefix(64));
	Ipv6InterfaceContainer netInterfaces2 = ipv6.Assign(NetDeviceContainer(DCHCDeviceContainer.Get(0), DCHCDeviceContainer.Get(2)));

	ping6.SetLocal(netInterfaces2.GetAddress(0, 1));
	ping6.SetRemote(netInterfaces2.GetAddress(1, 1));
	apps = ping6.Install(net2.Get(0));
	apps.Start(Seconds(18.0));
	apps.Stop(Seconds(32.0));
}

//bool verbose = true;
//
//CommandLine cmd;
//cmd.AddValue("verbose", "Tell application to log if true", verbose);
//
//cmd.Parse(argc, argv);
//
//LogComponentEnable("DCHCExample", LOG_LEVEL_ALL);
//LogComponentEnable("Ping6Application", LOG_LEVEL_ALL);
//LogComponentEnable("DCHCNetDevice", LOG_LEVEL_ALL);
////LogComponentEnable("ContextManager", LOG_LEVEL_ALL);
////LogComponentEnable("CompressedHeader", LOG_LEVEL_ALL);
////LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_ALL);
//
//NS_LOG_INFO("Create nodes.");
//NodeContainer nodeContainer;
//nodeContainer.Create(2);
//
//NS_LOG_INFO("Create IPv6 Internet Stack");
//InternetStackHelper internetv6;
//internetv6.Install(nodeContainer);
//
//NS_LOG_INFO("Create channels.");
//
//PointToPointHelper pointToPoint;
//pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
//pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
//NetDeviceContainer netDeviceContainer = pointToPoint.Install(nodeContainer);
//
//NS_LOG_INFO("Create DCHC");
//
//DCHCHelper dchc;
//NetDeviceContainer DCHCDeviceContainer = dchc.Install(netDeviceContainer);
//
//NS_LOG_INFO("Create networks and assign IPv6 Addresses.");
//Ipv6AddressHelper ipv6;
//Ipv6InterfaceContainer interfaces = ipv6.Assign(DCHCDeviceContainer);
//
//uint32_t packetSize = 1024;
//uint32_t maxPacketCount = 4;
//Time interPacketInterval = Seconds(5);
//
//Ping6Helper ping6;
//ping6.SetLocal(interfaces.GetAddress(0, 1));
//ping6.SetRemote(interfaces.GetAddress(1, 1));
//ping6.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
//ping6.SetAttribute("Interval", TimeValue(interPacketInterval));
//ping6.SetAttribute("PacketSize", UintegerValue(packetSize));
//
//ApplicationContainer apps = ping6.Install(nodeContainer.Get(0));
//apps.Start(Seconds(2.0));
//apps.Stop(Seconds(20.0));
//
//NS_LOG_INFO("Run Simulation.");
//Simulator::Run();
//Simulator::Destroy();
//NS_LOG_INFO("Done.");