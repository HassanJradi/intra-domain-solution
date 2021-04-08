/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "authentication-server-net-device.h"

NS_LOG_COMPONENT_DEFINE("AuthenticationServerNetDevice");

namespace ns3
{
    NS_OBJECT_ENSURE_REGISTERED(AuthenticationServerNetDevice);

    TypeId AuthenticationServerNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::AuthenticationServerNetDevice")
                                .SetParent<BasicNetDevice>()
                                .SetGroupName("BasicNetDevice")
                                .AddConstructor<AuthenticationServerNetDevice>();
                                
        return tid;
    }

    AuthenticationServerNetDevice::AuthenticationServerNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    bool AuthenticationServerNetDevice::HandleSignalingMessage(Ptr<const Packet> packet)
    {
        // NS_LOG_FUNCTION(this);

        SignalingMessage *signalingMessage = ExtractSignalingMessage(packet);
        uint8_t *data = signalingMessage->data;
        NS_LOG_FUNCTION(this << signalingMessage->command);

        switch (signalingMessage->command)
        {

        case ACCESS_REQUEST:
        {
            m_timeTrace();
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);
            uint32_t T1 = ConvertPointerUint8ToUint32(data + idLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + idLength + timestampLength);

            NS_LOG_INFO("[*] Authentication server receiving authentication request from mobile node: "
                        << "Mobile Node ID = " << nodeID << ", "
                        << "T1 = " << T1 << ", "
                        << "MIC = " << MIC);

            uint32_t mobileNodeHashKey = Hash32(ConvertUint32ToString(MobileNodeKeys[nodeID][0])) ^ Hash32(ConvertUint32ToString(MobileNodeKeys[nodeID][1]));
            uint32_t expectedMIC = Hash32(ConvertUint32ToString(nodeID) + ConvertUint32ToString(T1) + ConvertUint32ToString(mobileNodeHashKey));

            NS_LOG_INFO("[*] Authentication server calculates mobile node hash key = " << mobileNodeHashKey
                                                                                       << " and expected MIC = " << expectedMIC);

            if (expectedMIC == MIC)
            {
                NS_LOG_INFO("[*] Mobile node authenticated by the authentication server");

                uint32_t V = rand() % 0xFFFFFFFF;

                uint8_t *message = new uint8_t[idLength + hashLength];
                memcpy(message, ConvertUint32ToPointerUint8(nodeID), idLength);
                memcpy(message + idLength, ConvertUint32ToPointerUint8(V), hashLength);

                NS_LOG_INFO("[*] Authentication server generates random number V = " << V << " and sends it to media access gateway over secure link");

                m_timeTrace();
                SendSignalingMessage(ACCESS_RESPONSE_AUS_MAG, message, idLength + hashLength);

                uint32_t W = V ^ Hash32(ConvertUint32ToString(mobileNodeHashKey));
                NS_LOG_INFO("[*] Authentication server calculates W = " << W << " to be sent to mobile node");

                uint32_t T2 = rand() % 0xFFFFFFFF;
                MIC = Hash32(ConvertUint32ToString(W) + ConvertUint32ToString(T2) + ConvertUint32ToString(mobileNodeHashKey));

                message = new uint8_t[hashLength + timestampLength + hashLength];
                memcpy(message, ConvertUint32ToPointerUint8(W), hashLength);
                memcpy(message + hashLength, ConvertUint32ToPointerUint8(T2), timestampLength);
                memcpy(message + hashLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

                NS_LOG_INFO("[*] Updating mobile node keys in the authentication server database");
                NS_LOG_INFO("[*] Mobile node key R was = " << MobileNodeKeys[nodeID][0] << " and key J was = " << MobileNodeKeys[nodeID][1]);
                MobileNodeKeys[nodeID][0] = Hash32(ConvertUint32ToString(MobileNodeKeys[nodeID][0]));
                MobileNodeKeys[nodeID][1] = Hash32(ConvertUint32ToString(MobileNodeKeys[nodeID][1]));
                NS_LOG_INFO("[*] Mobile node key R becomes = " << MobileNodeKeys[nodeID][0] << " and key J becomes = " << MobileNodeKeys[nodeID][1]);

                NS_LOG_INFO("[*] Sending authentication response from authentication server to mobile node: "
                            << "W = " << W << ", "
                            << "T2 = " << T2 << ", "
                            << "MIC = " << MIC);

                m_timeTrace();
                SendSignalingMessage(ACCESS_RESPONSE_AUS_MN, message, hashLength + timestampLength + hashLength);

                return true;
            }
        }
        }

        return false;
    }
}