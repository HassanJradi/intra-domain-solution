/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MOBILE_NODE_NET_DEVICE_H
#define MOBILE_NODE_NET_DEVICE_H

#include "basic-net-device.h"

namespace ns3
{
    class MobileNodeNetDevice : public BasicNetDevice
    {
    public:
        static TypeId GetTypeId(void);
        MobileNodeNetDevice();
        virtual bool Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber);

    private:
        uint32_t ID = 16;
        uint32_t R = 1, J = 2;
        uint32_t V;
        bool isAccessRequestSent = false;

        void LaunchAuthentication();
        virtual bool HandleSignalingMessage(Ptr<const Packet> packet);
    };
}

#endif /* MOBILE_NODE_NET_DEVICE_H */