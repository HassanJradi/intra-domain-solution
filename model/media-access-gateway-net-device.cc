/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "media-access-gateway-net-device.h"
#include "authentication-server-net-device.h"

NS_LOG_COMPONENT_DEFINE("MediaAccessGatewayNetDevice");

namespace ns3
{
    NS_OBJECT_ENSURE_REGISTERED(MediaAccessGatewayNetDevice);

    TypeId MediaAccessGatewayNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::MediaAccessGatewayNetDevice")
                                .SetParent<BasicNetDevice>()
                                .SetGroupName("BasicNetDevice")
                                .AddConstructor<MediaAccessGatewayNetDevice>();
        return tid;
    }

    MediaAccessGatewayNetDevice::MediaAccessGatewayNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    void MediaAccessGatewayNetDevice::SetAuxNetDevice(Ptr<NetDevice> device)
    {
        NS_LOG_FUNCTION(this << device);
        NS_ASSERT_MSG(m_netDevice != 0, "MediaAccessGatewayNetDevice: set default net device before assigning auxiliary net device " << m_netDevice);

        m_iniNetDevice = m_netDevice;
        m_auxNetDevice = device;
        m_node->RegisterProtocolHandler(MakeCallback(&MediaAccessGatewayNetDevice::ReceiveFromDevice, this), m_protocolNumber, device, false);
    }

    bool MediaAccessGatewayNetDevice::HandleSignalingMessage(Ptr<const Packet> packet)
    {
        // NS_LOG_FUNCTION(this);

        SignalingMessage *signalingMessage = ExtractSignalingMessage(packet);
        uint8_t *data = signalingMessage->data;
        NS_LOG_FUNCTION(this << signalingMessage->command);

        switch (signalingMessage->command)
        {

        case ACCESS_REQUEST:
            m_netDevice = m_auxNetDevice;
            m_timeTrace();
            SendSignalingMessage(signalingMessage->command, signalingMessage->data, signalingMessage->dataLength);
            return true;

        case ACCESS_RESPONSE_AUS_MN:
            m_netDevice = m_iniNetDevice;
            m_timeTrace();
            SendSignalingMessage(signalingMessage->command, signalingMessage->data, signalingMessage->dataLength);
            return true;

        case ACCESS_RESPONSE_AUS_MAG:
        {
            m_timeTrace();
            MobileNodeVs.insert(pair<uint32_t, uint32_t>(ConvertPointerUint8ToUint32(data), ConvertPointerUint8ToUint32(data + idLength)));
            NS_LOG_INFO("[*] Media access gateway receiving random number V = " << ConvertPointerUint8ToUint32(data + idLength) << " from authentication server over secure link");
            return true;
        }

        case WAITING:
        {
            m_timeTrace();

            uint32_t nodeID = ConvertPointerUint8ToUint32(data);

            uint32_t T3 = rand() % 0xFFFFFFFF;
            uint32_t MIC = Hash32(ConvertUint32ToString(T3) + ConvertUint32ToString(MobileNodeVs[nodeID]));

            uint8_t *message = new uint8_t[timestampLength + hashLength];
            memcpy(message, ConvertUint32ToPointerUint8(T3), timestampLength);
            memcpy(message + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

            NS_LOG_INFO("[*] Sending authentication request from media access gateway to mobile node: "
                        << "T3 = " << T3 << ", "
                        << "MIC = " << MIC);

            m_timeTrace();
            SendSignalingMessage(MAG_AUTH, message, timestampLength + hashLength);
            return true;
        }

        case MN_AUTH:
        {
            m_timeTrace();
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);
            uint32_t T4 = ConvertPointerUint8ToUint32(data + idLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + idLength + timestampLength);

            NS_LOG_INFO("[*] Media access gateway receiving authentication request from mobile node: "
                        << "T4 = " << T4 << ", "
                        << "MIC = " << MIC);

            uint32_t expectedMIC = Hash32(ConvertUint32ToString(T4) + ConvertUint32ToString(MobileNodeVs[nodeID]));

            if (expectedMIC == MIC)
            {
                NS_LOG_INFO("[*] Mobile node authenticated by the media access gateway");
                m_timeTrace();
                return true;
            }
        }
        }

        return false;
    }
}