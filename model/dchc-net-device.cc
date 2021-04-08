/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/ipv6-l3-protocol.h"

#include "dchc-net-device.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE("DCHCNetDevice");

namespace ns3
{

	NS_OBJECT_ENSURE_REGISTERED(DCHCNetDevice);

	/* Public Functions */

	TypeId DCHCNetDevice::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::DCHCNetDevice")
								.SetParent<NetDevice>()
								.SetGroupName("DCHC")
								.AddConstructor<DCHCNetDevice>()
								.AddAttribute("ForceProtocolNumber",
											  "Force a specific Protocol Number in L3 packets.",
											  BooleanValue(true),
											  MakeBooleanAccessor(&DCHCNetDevice::m_forceProtocolNumber),
											  MakeBooleanChecker())
								.AddAttribute("ProtocolNumber",
											  "The specific Protocol Number to be used in L3 packets.",
											  UintegerValue(Ipv6L3Protocol::PROT_NUMBER),
											  MakeUintegerAccessor(&DCHCNetDevice::m_protocolNumber),
											  MakeUintegerChecker<uint16_t>())
								.AddAttribute("DatabaseMode",
											  "Database mode: Light, Unlimited",
											  EnumValue(Light),
											  MakeEnumAccessor(&DCHCNetDevice::databaseMode),
											  MakeEnumChecker(Light, "Light", Unlimited, "Unlimited"))
								.AddTraceSource("Tx",
												"Send - packet (including DCHC header), "
												"DCHC Ptr, interface index.",
												MakeTraceSourceAccessor(&DCHCNetDevice::m_txTrace),
												"ns3::DCHCNetDevice::RxTxTracedCallback")
								.AddTraceSource("Rx",
												"Receive - packet (including DCHC header), "
												"DCHC Ptr, interface index.",
												MakeTraceSourceAccessor(&DCHCNetDevice::m_rxTrace),
												"ns3::DCHCNetDevice::RxTxTracedCallback");

		return tid;
	}

	DCHCNetDevice::DCHCNetDevice() : m_ifIndex(0), m_node(0), m_netDevice(0)
	{
		NS_LOG_FUNCTION(this);
		contextManager = CreateObject<ContextManager>();
	}

	Ptr<NetDevice> DCHCNetDevice::GetNetDevice() const
	{
		//NS_LOG_FUNCTION(this);
		return m_netDevice;
	}

	void DCHCNetDevice::SetNetDevice(Ptr<NetDevice> device)
	{
		//NS_LOG_FUNCTION(this);
		m_netDevice = device;

		NS_LOG_INFO("RegisterProtocolHandler for " << device->GetInstanceTypeId().GetName());

		uint16_t protocolType = 0;

		if (m_forceProtocolNumber)
		{
			protocolType = m_protocolNumber;
		}

		m_node->RegisterProtocolHandler(MakeCallback(&DCHCNetDevice::ReceiveFromDevice, this), protocolType, device, false);
	}

	/* Inherited Functions */

	void DCHCNetDevice::AddLinkChangeCallback(Callback<void> callback)
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->AddLinkChangeCallback(callback);
	}

	Address DCHCNetDevice::GetAddress(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->GetAddress();
	}

	Address DCHCNetDevice::GetBroadcast(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->GetBroadcast();
	}

	Ptr<Channel> DCHCNetDevice::GetChannel(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->GetChannel();
	}

	uint32_t DCHCNetDevice::GetIfIndex(void) const
	{
		//NS_LOG_FUNCTION(this);
		return m_ifIndex;
	}

	uint16_t DCHCNetDevice::GetMtu(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->GetMtu();
	}

	Address DCHCNetDevice::GetMulticast(Ipv4Address multicastGroup) const
	{
		//NS_LOG_FUNCTION(this << multicastGroup);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->GetMulticast(multicastGroup);
	}

	Address DCHCNetDevice::GetMulticast(Ipv6Address addr) const
	{
		//NS_LOG_FUNCTION(this << addr);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->GetMulticast(addr);
	}

	Ptr<Node> DCHCNetDevice::GetNode(void) const
	{
		//NS_LOG_FUNCTION(this);
		return m_node;
	}

	bool DCHCNetDevice::IsBridge(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->IsBridge();
	}

	bool DCHCNetDevice::IsBroadcast(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->IsBroadcast();
	}

	bool DCHCNetDevice::IsLinkUp(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->IsLinkUp();
	}

	bool DCHCNetDevice::IsMulticast(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->IsMulticast();
	}

	bool DCHCNetDevice::IsPointToPoint(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->IsPointToPoint();
	}

	bool DCHCNetDevice::NeedsArp(void) const
	{
		//NS_LOG_FUNCTION(this);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->NeedsArp();
	}

	bool DCHCNetDevice::Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
	{
		NS_LOG_FUNCTION(this << packet->GetUid());

		if (packet->GetUid() == 0 || packet->GetUid() == 1 || packet->GetUid() == 2 || packet->GetUid() == 3 || packet->GetUid() == 12 || packet->GetUid() == 13 || packet->GetUid() == 4 || packet->GetUid() == 5 || packet->GetUid() == 6)
			return false;

		if (!isConnectionSetupSent)
		{
			isConnectionSetupSent = true;
			packet->PeekHeader(tempHeader);
			return SendSignalingMessage(SETUP_CONNECTION_REQUEST, ConvertUint64ToPointerUint8(ID), 8);
		}

		ProcessPacketHeaderForCompression(packet);

		//m_txTrace(packet, m_node->GetObject<DCHCNetDevice>(), GetIfIndex());
		return m_netDevice->Send(packet, dest, protocolNumber);
	}

	bool DCHCNetDevice::SendFrom(Ptr<Packet> packet, const Address &src, const Address &dest, uint16_t protocolNumber)
	{
		//NS_LOG_FUNCTION(this);

		if (packet->GetUid() == 0 || packet->GetUid() == 1 || packet->GetUid() == 2 || packet->GetUid() == 3 || packet->GetUid() == 12 || packet->GetUid() == 13)
		{
			//m_txTrace(packet, m_node->GetObject<DCHCNetDevice>(), GetIfIndex());
			return m_netDevice->SendFrom(packet, src, dest, protocolNumber);
		}

		if (!isConnectionSetupSent)
		{
			isConnectionSetupSent = true;
			packet->PeekHeader(tempHeader);
			return SendSignalingMessage(SETUP_CONNECTION_REQUEST, ConvertUint64ToPointerUint8(ID), 8);
		}

		ProcessPacketHeaderForCompression(packet);
		//m_txTrace(packet, m_node->GetObject<DCHCNetDevice>(), GetIfIndex());
		return m_netDevice->SendFrom(packet, src, dest, protocolNumber);
	}

	void DCHCNetDevice::SetAddress(Address address)
	{
		//NS_LOG_FUNCTION(this << address);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		m_netDevice->SetAddress(address);
	}

	void DCHCNetDevice::SetIfIndex(const uint32_t index)
	{
		NS_LOG_FUNCTION(this);
		m_ifIndex = index;
	}

	bool DCHCNetDevice::SetMtu(const uint16_t mtu)
	{
		//NS_LOG_FUNCTION(this << mtu);
		NS_ASSERT_MSG(m_netDevice != 0, "DCHC: can't find any lower-layer protocol " << m_netDevice);

		return m_netDevice->SetMtu(mtu);
	}

	void DCHCNetDevice::SetNode(Ptr<Node> node)
	{
		//NS_LOG_FUNCTION(this << node);
		m_node = node;
	}

	void DCHCNetDevice::SetPromiscReceiveCallback(NetDevice::PromiscReceiveCallback cb)
	{
		//NS_LOG_FUNCTION(this << &cb);
		m_promiscRxCallback = cb;
	}

	void DCHCNetDevice::SetReceiveCallback(NetDevice::ReceiveCallback cb)
	{
		//NS_LOG_FUNCTION(this << &cb);
		m_rxCallback = cb;
	}

	bool DCHCNetDevice::SupportsSendFrom() const
	{
		//NS_LOG_FUNCTION(this);
		return true;
	}

	/* Private Functions*/

	void DCHCNetDevice::ReceiveFromDevice(Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocolNumber, Address const &source, Address const &destination, PacketType packetType)
	{
		NS_LOG_FUNCTION(this);

		bool isSignalingMessage = HandleSignalingMessage(packet);

		if (!isSignalingMessage)
		{
			Ptr<Packet> copyPacket = packet->Copy();
			ProcessPacketHeaderForDecompression(copyPacket);

			if (!m_promiscRxCallback.IsNull())
				m_promiscRxCallback(this, copyPacket, protocolNumber, source, destination, packetType);
			m_rxCallback(this, copyPacket, protocolNumber, source);
			//m_rxTrace(copyPacket, m_node->GetObject<DCHCNetDevice>(), GetIfIndex());
		}
	}

	/* Signaling Message Handling */

	SignalingMessage *DCHCNetDevice::ExtractSignalingMessage(Ptr<const Packet> packet)
	{
		uint16_t length = packet->GetSize();
		uint8_t *serializedSignalingMessage = new uint8_t[length];
		packet->CopyData(serializedSignalingMessage, length);
		return SignalingMessage::Deserialize(serializedSignalingMessage, length);
	}

	bool DCHCNetDevice::SendSignalingMessage(uint8_t command, uint8_t *data, uint16_t dataLength)
	{
		NS_LOG_FUNCTION(this << command);

		SignalingMessage *message = new SignalingMessage();
		message->command = command;
		message->data = data;
		message->dataLength = dataLength;

		uint16_t length;
		uint8_t *serializedSignalingMessage = SignalingMessage::Serialize(message, length);
		Ptr<Packet> packet = new Packet(serializedSignalingMessage, length);

		//m_txTrace(packet, m_node->GetObject<DCHCNetDevice>(), GetIfIndex());
		return m_netDevice->Send(packet, GetBroadcast(), Ipv6L3Protocol::PROT_NUMBER);
	}

	bool DCHCNetDevice::HandleSignalingMessage(Ptr<const Packet> packet)
	{
		//NS_LOG_FUNCTION(this);

		SignalingMessage *signalingMessage = ExtractSignalingMessage(packet);
		NS_LOG_FUNCTION(this << signalingMessage->command);

		switch (signalingMessage->command)
		{

		case SETUP_CONNECTION_REQUEST: //dev -> net
		{
			isConnectionSetupSent = true;
			peerID = ConvertPointerUint8ToUint64(signalingMessage->data);
			bool isDeviceContextExist = contextManager->IsContextExist(peerID);

			if (isDeviceContextExist)
			{
				SendSignalingMessage(SETUP_CONNECTION_RESPONSE_CONTEXT_FOUND, ConvertUint64ToPointerUint8(ID), 8);
			}
			else
			{
				SendSignalingMessage(SETUP_CONNECTION_RESPONSE_CONTEXT_NOT_FOUND, ConvertUint64ToPointerUint8(ID), 8);
			}

			return true;
		}

		case SETUP_CONNECTION_RESPONSE_CONTEXT_FOUND: //net -> dev
		{
			peerID = ConvertPointerUint8ToUint64(signalingMessage->data);
			bool isNetworkContextExist = contextManager->IsContextExist(peerID);

			if (isNetworkContextExist)
			{
				SendSignalingMessage(SETUP_CONNECTION_DONE, NULL, 0);
			}
			else
			{
				uint16_t length;
				uint8_t *serializedHeader = SerializeHeader(&tempHeader, length);
				SendSignalingMessage(CONTEXT_AGREEMENT_REQUEST, serializedHeader, length);
			}

			return true;
		}

		case SETUP_CONNECTION_RESPONSE_CONTEXT_NOT_FOUND: //net -> dev
		{
			peerID = ConvertPointerUint8ToUint64(signalingMessage->data);
			uint16_t length;
			uint8_t *serializedHeader = SerializeHeader(&tempHeader, length);
			SendSignalingMessage(CONTEXT_AGREEMENT_REQUEST, serializedHeader, length);

			return true;
		}

		case CONTEXT_AGREEMENT_REQUEST: //dev -> net
		{
			Ipv6Header *header = DeserializeHeader(signalingMessage->data, signalingMessage->dataLength);
			Context *context = contextManager->CreateContext(header);
			context->ID = peerID;
			contextManager->DispatchContext(context, databaseMode);

			uint16_t length;
			uint8_t *serializedContext = Context::Serialize(context, length);
			SendSignalingMessage(CONTEXT_AGREEMENT_RESPONSE, serializedContext, length);

			return true;
		}

		case CONTEXT_AGREEMENT_RESPONSE: //net -> dev
		{
			Context *context = Context::Deserialize(signalingMessage->data, signalingMessage->dataLength);
			context->ID = peerID;
			contextManager->SwitchSourceDestinationRuleFields(context);
			contextManager->DispatchContext(context, databaseMode);

			NS_LOG_INFO("Link is Up");
			SendSignalingMessage(SETUP_CONNECTION_DONE, NULL, 0);

			return true;
		}

		case SETUP_CONNECTION_DONE: //dev -> net
		{
			NS_LOG_INFO("Link is Up");
			return true;
		}

		case SWITCH_TECHNOLOGY:
		{
			LinkLayerTechnology technology = (LinkLayerTechnology)*signalingMessage->data;
			SwitchTechnology(technology);
		}
		}

		return false;
	}

	void DCHCNetDevice::SwitchTechnology(LinkLayerTechnology technology)
	{
		NS_LOG_FUNCTION(this);

		switch (technology)
		{

		case LoRaWAN:
			//m_netDevice = CreateLoRaWANNetDevice();
			break;

		case Sigfox:
			//m_netDevice = CreateSigfoxNetDevice();
			break;

		case DASH7:
			//m_netDevice = CreateDASH7NetDevice();
			break;
		}
	}

	/* Compression Decompression Functions */

	uint8_t DCHCNetDevice::FindRuleID(Ipv6Header *header, Context *context)
	{
		NS_LOG_FUNCTION(this);
		return 0;
	}

	void DCHCNetDevice::ProcessPacketHeaderForCompression(Ptr<Packet> packet)
	{
		//NS_LOG_FUNCTION(this);

		Context *context = contextManager->GetContext();

		if (context != NULL)
		{
			Ipv6Header *ipv6Header = new Ipv6Header();
			packet->RemoveHeader(*ipv6Header);

			uint8_t ruleID = FindRuleID(ipv6Header, context);
			CompressedHeader *compressedHeader = CompressHeader(ipv6Header, context, ruleID);
			packet->AddHeader(*compressedHeader);
		}
	}

	CompressedHeader *DCHCNetDevice::CompressHeader(Ipv6Header *header, Context *context, uint8_t ruleID)
	{
		NS_LOG_FUNCTION(this);

		CompressedHeader *compressedHeader = new CompressedHeader();
		compressedHeader->ruleID = ruleID;

		Rule *compressionRule = context->rules[ruleID];

		for (uint8_t i = 0; i < compressionRule->ruleFields.size(); i++)
		{
			RuleField *ruleField = compressionRule->ruleFields[i];

			switch (ruleField->fieldID)
			{
			case IPv6_DiffServ:
			{
				compressedHeader->diffServResidue = ApplyRuleCompressionMode(ruleField, to_string(header->GetTrafficClass()));
				break;
			}
			case IPv6_FlowLabel:
			{
				compressedHeader->flowLabelResidue = ApplyRuleCompressionMode(ruleField, to_string(header->GetFlowLabel()));
				break;
			}
			case IPv6_PayloadLength:
			{
				compressedHeader->payloadLengthResidue = ApplyRuleCompressionMode(ruleField, to_string(header->GetPayloadLength()));
				break;
			}
			case IPv6_NextHeader:
			{
				compressedHeader->nextHeaderResidue = ApplyRuleCompressionMode(ruleField, to_string(header->GetNextHeader()));
				break;
			}
			case IPv6_HopLimit:
			{
				compressedHeader->hopLimitResidue = ApplyRuleCompressionMode(ruleField, to_string(header->GetHopLimit()));
				break;
			}
			case IPv6_SourcePrefix:
			{
				uint8_t sourceAddress[16];
				header->GetSourceAddress().Serialize(sourceAddress);
				compressedHeader->sourcePrefixResidue = ApplyRuleCompressionMode(ruleField, ConvertPointerUint8ToString(sourceAddress, 8));
				break;
			}
			case IPv6_SourceHost:
			{
				uint8_t sourceAddress[16];
				header->GetSourceAddress().Serialize(sourceAddress);
				compressedHeader->sourceHostResidue = ApplyRuleCompressionMode(ruleField, ConvertPointerUint8ToString(sourceAddress + 8, 8));
				break;
			}
			case IPv6_DestinationPrefix:
			{
				uint8_t destinationAddress[16];
				header->GetDestinationAddress().Serialize(destinationAddress);
				compressedHeader->destinationPrefixResidue = ApplyRuleCompressionMode(ruleField, ConvertPointerUint8ToString(destinationAddress, 8));
				break;
			}
			case IPv6_DestinationHost:
			{
				uint8_t destinationAddress[16];
				header->GetDestinationAddress().Serialize(destinationAddress);
				compressedHeader->destinationHostResidue = ApplyRuleCompressionMode(ruleField, ConvertPointerUint8ToString(destinationAddress + 8, 8));
				break;
			}

			default:
				break;
			}
		}

		return compressedHeader;
	}

	string DCHCNetDevice::ApplyRuleCompressionMode(RuleField *ruleField, string headerValueAsString)
	{
		//NS_LOG_FUNCTION(this);

		string residue;

		CompressionDecompressionAction action = ruleField->compressionDecompressionAction;
		MatchingOperator mOperator = ruleField->matchingOperator;

		if (mOperator == Equal && action == NotSent && headerValueAsString == ruleField->targetValue)
		{
			residue = "";
		}

		if (mOperator == Ignore && action == ValueSent)
		{
			residue = headerValueAsString;
		}

		return residue;
	}

	void DCHCNetDevice::ProcessPacketHeaderForDecompression(Ptr<Packet> packet)
	{
		//NS_LOG_FUNCTION(this);

		Context *context = contextManager->GetContext();

		if (context != NULL)
		{
			CompressedHeader *compressedHeader = new CompressedHeader();
			packet->RemoveHeader(*compressedHeader);

			uint8_t ruleID = compressedHeader->ruleID;
			Ipv6Header *ipv6Header = DecompressHeader(compressedHeader, context, ruleID);
			packet->AddHeader(*ipv6Header);
		}
	}

	Ipv6Header *DCHCNetDevice::DecompressHeader(CompressedHeader *header, Context *context, uint8_t ruleID)
	{
		NS_LOG_FUNCTION(this);

		Ipv6Header *ipv6Header = new Ipv6Header();
		Rule *compressionRule = context->rules[ruleID];

		uint8_t sourceAddress[16];
		uint8_t destinationAddress[16];

		for (uint8_t i = 0; i < compressionRule->ruleFields.size(); i++)
		{
			RuleField *ruleField = compressionRule->ruleFields[i];

			switch (ruleField->fieldID)
			{
			case IPv6_DiffServ:
			{
				string diffServAsString = ApplyRuleDecompressionMode(ruleField, header->diffServResidue);
				ipv6Header->SetTrafficClass(stoi(diffServAsString));
				break;
			}
			case IPv6_FlowLabel:
			{
				string flowLabelAsString = ApplyRuleDecompressionMode(ruleField, header->flowLabelResidue);
				ipv6Header->SetFlowLabel(stoi(flowLabelAsString));
				break;
			}
			case IPv6_PayloadLength:
			{
				string payloadLengthAsString = ApplyRuleDecompressionMode(ruleField, header->payloadLengthResidue);
				ipv6Header->SetPayloadLength(stoi(payloadLengthAsString));
				break;
			}
			case IPv6_NextHeader:
			{
				string nextHeaderAsString = ApplyRuleDecompressionMode(ruleField, header->nextHeaderResidue);
				ipv6Header->SetNextHeader(stoi(nextHeaderAsString));
				break;
			}
			case IPv6_HopLimit:
			{
				string hopLimitAsString = ApplyRuleDecompressionMode(ruleField, header->hopLimitResidue);
				ipv6Header->SetHopLimit(stoi(hopLimitAsString));
				break;
			}
			case IPv6_SourcePrefix:
			{
				string sourcePrefixAsString = ApplyRuleDecompressionMode(ruleField, header->sourcePrefixResidue);

				for (int i = 0; i < 8; i++)
				{
					sourceAddress[i] = static_cast<uint8_t>(sourcePrefixAsString[i]);
				}

				ipv6Header->SetSourceAddress(Ipv6Address::Deserialize(sourceAddress));
				break;
			}
			case IPv6_SourceHost:
			{
				string sourceHostAsString = ApplyRuleDecompressionMode(ruleField, header->sourceHostResidue);

				for (int i = 8; i < 16; i++)
				{
					sourceAddress[i] = static_cast<uint8_t>(sourceHostAsString[i - 8]);
				}

				ipv6Header->SetSourceAddress(Ipv6Address::Deserialize(sourceAddress));
				break;
			}
			case IPv6_DestinationPrefix:
			{
				string destinationPrefixAsString = ApplyRuleDecompressionMode(ruleField, header->destinationPrefixResidue);

				for (int i = 0; i < 8; i++)
				{
					destinationAddress[i] = static_cast<uint8_t>(destinationPrefixAsString[i]);
				}

				ipv6Header->SetDestinationAddress(Ipv6Address::Deserialize(destinationAddress));
				break;
			}
			case IPv6_DestinationHost:
			{
				string destinationHostAsString = ApplyRuleDecompressionMode(ruleField, header->destinationHostResidue);

				for (int i = 8; i < 16; i++)
				{
					destinationAddress[i] = static_cast<uint8_t>(destinationHostAsString[i - 8]);
				}

				ipv6Header->SetDestinationAddress(Ipv6Address::Deserialize(destinationAddress));
				break;
			}
			default:
				break;
			}
		}

		return ipv6Header;
	}

	string DCHCNetDevice::ApplyRuleDecompressionMode(RuleField *ruleField, string residue)
	{
		//NS_LOG_FUNCTION(this);

		string headerValueAsString;

		CompressionDecompressionAction action = ruleField->compressionDecompressionAction;
		MatchingOperator mOperator = ruleField->matchingOperator;

		if (mOperator == Equal && action == NotSent)
		{
			headerValueAsString = ruleField->targetValue;
		}

		if (mOperator == Ignore && action == ValueSent)
		{
			headerValueAsString = residue;
		}

		return headerValueAsString;
	}

	void DCHCNetDevice::DoDispose()
	{
		NS_LOG_FUNCTION(this);

		m_netDevice = 0;
		m_node = 0;

		NetDevice::DoDispose();
	}

	//bool DCHCNetDevice::CheckIfDeviceHasTechnology(LinkLayerTechnology technology)
	//{
	//	for (uint8_t i = 0; i < technologies.size(); i++) {
	//		if (technologies[i] == technology)
	//			return true;
	//	}
	//	return false;
	//}
}