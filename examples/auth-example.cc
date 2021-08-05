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
#include "ns3/csma-helper.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/mobile-node-net-device.h"
#include "ns3/authentication-server-net-device.h"
#include "ns3/trace-helper.h"
#include "ns3/basic-net-device-helper.h"
#include "ns3/media-access-gateway-net-device.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AuthenticationExample");

bool verbose = true;
uint64_t dataRate = 100;
uint64_t delay = 0;

int main(int argc, char *argv[])
{
	CommandLine cmd;
	cmd.AddValue("verbose", "Tell application to log if true", verbose);
	cmd.AddValue("data-rate", "Channel data rate (kbps)", dataRate);
	cmd.AddValue("delay", "Channel delay (ms)", delay);

	cmd.Parse(argc, argv);

	if (verbose)
	{
		LogComponentEnable("AuthenticationExample", LOG_INFO);
		LogComponentEnable("MobileNodeNetDevice", LOG_INFO);
		LogComponentEnable("MediaAccessGatewayNetDevice", LOG_INFO);
		LogComponentEnable("AuthenticationServerNetDevice", LOG_INFO);
		LogComponentEnable("Ping6Application", LOG_INFO);
		LogComponentEnable("BasicNetDeviceHelper", LOG_LEVEL_ALL);
	}

	// LogComponentEnable("AuthenticationExample", LOG_DEBUG);
	// LogComponentEnable("BasicNetDevice", LOG_LEVEL_ALL);
	// LogComponentEnable("BasicNetDeviceHelper", LOG_LEVEL_ALL);
	// LogComponentEnable("MobileNodeNetDevice", LOG_LEVEL_ALL);
	// LogComponentEnable("MediaAccessGatewayNetDevice", LOG_LEVEL_ALL);
	// LogComponentEnable("AuthenticationServerNetDevice", LOG_LEVEL_ALL);
	// LogComponentEnable("Ping6Application", LOG_LEVEL_ALL);

	NS_LOG_INFO("Create nodes.");

	Ptr<Node> mobileNode = CreateObject<Node>();
	Ptr<Node> mediaAccessGateway = CreateObject<Node>();
	Ptr<Node> authenticationServerNode = CreateObject<Node>();

	NodeContainer network(mobileNode, mediaAccessGateway, authenticationServerNode);
	NodeContainer radioNetwork(mobileNode, mediaAccessGateway);
	NodeContainer IPNetwork(mediaAccessGateway, authenticationServerNode);

	NS_LOG_INFO("Create channels.");

	CsmaHelper csma;
	csma.SetChannelAttribute("DataRate", StringValue(to_string(dataRate) + "kbps"));
	csma.SetChannelAttribute("Delay", StringValue(to_string(delay) + "ms"));
	NetDeviceContainer csmaNetDeviceContainer = csma.Install(radioNetwork);

	PointToPointHelper pointToPoint;
	NetDeviceContainer pointToPointNetDeviceContainer = pointToPoint.Install(IPNetwork);

	NS_LOG_INFO("Create mobile node device.");
	Ptr<MobileNodeNetDevice> mobileNodeNetDevice = CreateObject<MobileNodeNetDevice>();
	Ptr<NetDevice> mobileNodeCsmaDevice = csmaNetDeviceContainer.Get(0);
	mobileNode->AddDevice(mobileNodeNetDevice);
	mobileNodeNetDevice->SetNetDevice(mobileNodeCsmaDevice);

	NS_LOG_INFO("Create media access gateway device.");
	Ptr<MediaAccessGatewayNetDevice> mediaAccessGatewayNetDevice = CreateObject<MediaAccessGatewayNetDevice>();
	mediaAccessGateway->AddDevice(mediaAccessGatewayNetDevice);

	Ptr<NetDevice> mediaAccessGatewayCsmaDevice = csmaNetDeviceContainer.Get(1);
	mediaAccessGatewayNetDevice->SetNetDevice(mediaAccessGatewayCsmaDevice);

	Ptr<NetDevice> mediaAccessGatewayP2PDevice = pointToPointNetDeviceContainer.Get(0);
	mediaAccessGatewayNetDevice->SetAuxNetDevice(mediaAccessGatewayP2PDevice);

	NS_LOG_INFO("Create authentication server device.");
	Ptr<AuthenticationServerNetDevice> authenticationServerNetDevice = CreateObject<AuthenticationServerNetDevice>();
	Ptr<NetDevice> authenticationServerP2PDevice = pointToPointNetDeviceContainer.Get(1);
	authenticationServerNode->AddDevice(authenticationServerNetDevice);
	authenticationServerNetDevice->SetNetDevice(authenticationServerP2PDevice);

	AsciiTraceHelper asciiTraceHelper;
	Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream(to_string(dataRate) + "kbps" + ".dat", std::ios_base::app);
	*stream->GetStream() << std::endl;
	BasicNetDeviceHelper basicNetDeviceHelper;
	basicNetDeviceHelper.EnableAscii(stream, mobileNodeNetDevice);
	basicNetDeviceHelper.EnableAscii(stream, mediaAccessGatewayNetDevice);
	basicNetDeviceHelper.EnableAscii(stream, authenticationServerNetDevice);

	NetDeviceContainer ndc;
	ndc.Add(mobileNodeNetDevice);
	ndc.Add(mediaAccessGatewayNetDevice);
	ndc.Add(authenticationServerNetDevice);

	InternetStackHelper internetv6;
	internetv6.Install(network);

	Ipv6AddressHelper ipv6;
	ipv6.SetBase(Ipv6Address("2001:db8::"), Ipv6Prefix(64));
	Ipv6InterfaceContainer netInterfaces = ipv6.Assign(ndc);

	uint32_t packetSize = 1024;
	uint32_t maxPacketCount = 1;
	Time interPacketInterval = Seconds(4);

	Ping6Helper ping6;
	ping6.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
	ping6.SetAttribute("Interval", TimeValue(interPacketInterval));
	ping6.SetAttribute("PacketSize", UintegerValue(packetSize));

	ping6.SetLocal(netInterfaces.GetAddress(0, 0));
	ping6.SetRemote(netInterfaces.GetAddress(2, 0));

	ApplicationContainer apps;
	apps = ping6.Install(network.Get(0));

	apps.Start(Seconds(2.0));
	apps.Stop(Seconds(15.0));

	NS_LOG_INFO("Run Simulation.");
	Simulator::Run();
	Simulator::Destroy();
	NS_LOG_INFO("Done.");
}