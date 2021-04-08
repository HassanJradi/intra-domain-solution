/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "mobile-node-net-device.h"
#include "authentication-server-net-device.h"

NS_LOG_COMPONENT_DEFINE("MobileNodeNetDevice");

namespace ns3
{
    NS_OBJECT_ENSURE_REGISTERED(MobileNodeNetDevice);

    TypeId MobileNodeNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::MobileNodeNetDevice")
                                .SetParent<BasicNetDevice>()
                                .SetGroupName("BasicNetDevice")
                                .AddConstructor<MobileNodeNetDevice>();
                                
        return tid;
    }

    MobileNodeNetDevice::MobileNodeNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    void MobileNodeNetDevice::LaunchAuthentication()
    {
        isAccessRequestSent = true;
        
        m_timeTrace();
        uint32_t T1 = rand() % 0xFFFFFFFF;
        uint32_t hashKey = Hash32(ConvertUint32ToString(R)) ^ Hash32(ConvertUint32ToString(J));
        uint32_t MIC = Hash32(ConvertUint32ToString(ID) + ConvertUint32ToString(T1) + ConvertUint32ToString(hashKey));

        uint8_t *message = new uint8_t[idLength + timestampLength + hashLength];
        memcpy(message, ConvertUint32ToPointerUint8(ID), idLength);
        memcpy(message + idLength, ConvertUint32ToPointerUint8(T1), timestampLength);
        memcpy(message + idLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

        NS_LOG_INFO("[*] Sending authentication request from mobile node to authentication server: "
                    << "ID = " << ID << ", "
                    << "T1 = " << T1 << ", "
                    << "MIC = " << MIC << " "
                    << "with Hashkey = " << hashKey);

        m_timeTrace();
        SendSignalingMessage(ACCESS_REQUEST, message, idLength + timestampLength + hashLength);
    }

    bool MobileNodeNetDevice::Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
    {
        if (!isAccessRequestSent)
            LaunchAuthentication();
        
        return true;
    }

    bool MobileNodeNetDevice::HandleSignalingMessage(Ptr<const Packet> packet)
    {
        // NS_LOG_FUNCTION(this);

        SignalingMessage *signalingMessage = ExtractSignalingMessage(packet);
        uint8_t *data = signalingMessage->data;
        NS_LOG_FUNCTION(this << signalingMessage->command);

        switch (signalingMessage->command)
        {

        case ACCESS_RESPONSE_AUS_MN:
        {
            m_timeTrace();
            uint32_t W = ConvertPointerUint8ToUint32(data);
            uint32_t T2 = ConvertPointerUint8ToUint32(data + hashLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + hashLength + timestampLength);

            NS_LOG_INFO("[*] Receiving authentication response from authentication server: "
                        << "W = " << W << ", "
                        << "T2 = " << T2 << ", "
                        << "MIC = " << MIC);

            uint32_t hashKey = Hash32(ConvertUint32ToString(R)) ^ Hash32(ConvertUint32ToString(J));
            uint32_t expectedMIC = Hash32(ConvertUint32ToString(W) + ConvertUint32ToString(T2) + ConvertUint32ToString(hashKey));

            NS_LOG_INFO("[*] Calculating expected MIC = " << expectedMIC);

            if (expectedMIC == MIC)
            {
                NS_LOG_INFO("[*] Authentication server authenticated by the mobile node");
                V = W ^ Hash32(ConvertUint32ToString(hashKey));
                NS_LOG_INFO("[*] Calculating V = " << V << " from W and the hash of HashKey");

                NS_LOG_INFO("[*] Updating keys in the mobile node");
                NS_LOG_INFO("[*] Mobile node key R was = " << R << " and key J was = " << J);
                R = Hash32(ConvertUint32ToString(R));
                J = Hash32(ConvertUint32ToString(J));
                NS_LOG_INFO("[*] Mobile node key R becomes = " << R << " and key J becomes = " << J);

                NS_LOG_INFO("[*] Mobile node waiting media access gateway authentication request");

                uint8_t *message = new uint8_t[idLength];
                memcpy(message, ConvertUint32ToPointerUint8(ID), idLength);

                m_timeTrace();
                SendSignalingMessage(WAITING, message, idLength);
                return true;
            }
        }

        case MAG_AUTH:
        {
            m_timeTrace();
            uint32_t T3 = ConvertPointerUint8ToUint32(data);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + timestampLength);

            NS_LOG_INFO("[*] Mobile node receiving authentication request from media access gateway: "
                        << "T3 = " << T3 << ", "
                        << "MIC = " << MIC);

            uint32_t expectedMIC = Hash32(ConvertUint32ToString(T3) + ConvertUint32ToString(V));

            if (expectedMIC == MIC)
            {
                NS_LOG_INFO("[*] Media access gateway authenticated by the mobile node");

                uint32_t T4 = rand() % 0xFFFFFFFF;
                MIC = Hash32(ConvertUint32ToString(T4) + ConvertUint32ToString(V));

                uint8_t *message = new uint8_t[idLength + timestampLength + hashLength];
                memcpy(message, ConvertUint32ToPointerUint8(ID), idLength);
                memcpy(message + idLength, ConvertUint32ToPointerUint8(T4), timestampLength);
                memcpy(message + idLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

                NS_LOG_INFO("[*] Sending authentication request from mobile node to media access gateway: "
                            << "T4 = " << T4 << ", "
                            << "MIC = " << MIC);

                m_timeTrace();
                SendSignalingMessage(MN_AUTH, message, idLength + timestampLength + hashLength);
                return true;
            }
        }
        }

        return false;
    }
}