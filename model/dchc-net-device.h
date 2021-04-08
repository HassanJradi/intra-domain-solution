/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DCHC_NET_DEVICE_H
#define DCHC_NET_DEVICE_H

#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/ipv6-header.h"
#include "ns3/traced-callback.h"
#include "basic-net-device.h"
#include "utils.h"

#include "context-manager.h"
#include "compressed-header.h"

namespace ns3
{

	class ContextManager;
	class Context;
	class RuleField;

	enum SignalingCommand : uint8_t
	{
		SETUP_CONNECTION_REQUEST = 0xAA,
		SETUP_CONNECTION_RESPONSE_CONTEXT_FOUND,
		SETUP_CONNECTION_RESPONSE_CONTEXT_NOT_FOUND,
		CONTEXT_AGREEMENT_REQUEST,
		CONTEXT_AGREEMENT_RESPONSE,
		SETUP_CONNECTION_DONE,
		SWITCH_TECHNOLOGY
	};
	enum LinkLayerTechnology : uint8_t
	{
		LoRaWAN,
		Sigfox,
		DASH7
	};
	enum DatabaseMode : uint8_t
	{
		Light,
		Unlimited
	};

	class DCHCNetDevice : public NetDevice
	{
	public:
		static TypeId GetTypeId(void);
		DCHCNetDevice();
		Ptr<NetDevice> GetNetDevice() const;
		void SetNetDevice(Ptr<NetDevice> device);

		virtual void AddLinkChangeCallback(Callback<void> callback);
		virtual Address GetAddress(void) const;
		virtual Address GetBroadcast(void) const;
		virtual Ptr<Channel> GetChannel(void) const;
		virtual uint32_t GetIfIndex(void) const;
		virtual uint16_t GetMtu(void) const;
		virtual Address GetMulticast(Ipv4Address multicastGroup) const;
		virtual Address GetMulticast(Ipv6Address addr) const;
		virtual Ptr<Node> GetNode(void) const;
		virtual bool IsBridge(void) const;
		virtual bool IsBroadcast(void) const;
		virtual bool IsLinkUp(void) const;
		virtual bool IsMulticast(void) const;
		virtual bool IsPointToPoint(void) const;
		virtual bool NeedsArp(void) const;
		virtual bool Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber);
		virtual bool SendFrom(Ptr<Packet> packet, const Address &source, const Address &dest, uint16_t protocolNumber);
		virtual void SetAddress(Address address);
		virtual void SetIfIndex(const uint32_t index);
		virtual bool SetMtu(const uint16_t mtu);
		virtual void SetNode(Ptr<Node> node);
		virtual void SetPromiscReceiveCallback(PromiscReceiveCallback cb);
		virtual void SetReceiveCallback(ReceiveCallback cb);
		virtual bool SupportsSendFrom(void) const;

	private:
		Ptr<ContextManager> contextManager;
		std::vector<LinkLayerTechnology> technologies;
		DatabaseMode databaseMode;
		bool isConnectionSetupSent = false;
		uint64_t ID = rand() % 10000;
		uint64_t peerID;

		Ipv6Header tempHeader;

		bool m_forceProtocolNumber;
		uint16_t m_protocolNumber;
		uint32_t m_ifIndex;
		Ptr<Node> m_node;
		Ptr<NetDevice> m_netDevice;

		NetDevice::ReceiveCallback m_rxCallback;
		NetDevice::PromiscReceiveCallback m_promiscRxCallback;
		TracedCallback<Ptr<const Packet>, Ptr<DCHCNetDevice>, uint32_t> m_txTrace;
		TracedCallback<Ptr<const Packet>, Ptr<DCHCNetDevice>, uint32_t> m_rxTrace;
		typedef void (*RxTxTracedCallback)(Ptr<const Packet> packet, Ptr<DCHCNetDevice> device, uint32_t ifIndex);

		/* Private Functions */

		void ReceiveFromDevice(Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, Address const &source, Address const &destination, PacketType packetType);

		SignalingMessage *ExtractSignalingMessage(Ptr<const Packet> packet);
		bool SendSignalingMessage(uint8_t command, uint8_t *data, uint16_t dataLength);
		bool HandleSignalingMessage(Ptr<const Packet> packet);
		void SwitchTechnology(LinkLayerTechnology technology); //continue implementation

		uint8_t FindRuleID(Ipv6Header *header, Context *context); //Not Implemented
		void ProcessPacketHeaderForCompression(Ptr<Packet> packet);
		CompressedHeader *CompressHeader(Ipv6Header *header, Context *context, uint8_t ruleID);
		string ApplyRuleCompressionMode(RuleField *ruleField, string headerValueAsString);
		void ProcessPacketHeaderForDecompression(Ptr<Packet> packet);
		Ipv6Header *DecompressHeader(CompressedHeader *compressedHeader, Context *context, uint8_t ruleID);
		string ApplyRuleDecompressionMode(RuleField *ruleField, string residue);

	protected:
		virtual void DoDispose(void);
	};
	
}

#endif /* DCHC_NET_DEVICE_H */