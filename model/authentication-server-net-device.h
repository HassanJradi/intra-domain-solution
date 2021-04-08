/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef AUTHENTICATION_SERVER_NET_DEVICE_H
#define AUTHENTICATION_SERVER_NET_DEVICE_H

#include "basic-net-device.h"

namespace ns3
{
    enum CommandID : uint8_t
    {
        ACCESS_REQUEST = 0xAA,
        ACCESS_RESPONSE,
        ACCESS_RESPONSE_AUS_MAG,
        ACCESS_RESPONSE_AUS_MN,
        WAITING,
        MAG_AUTH,
        MN_AUTH
    };

    class AuthenticationServerNetDevice : public BasicNetDevice
    {
    public:
        static TypeId GetTypeId(void);
        AuthenticationServerNetDevice();
        
    private:
        std::map<uint32_t, std::vector<uint32_t>> MobileNodeKeys{{16, {1, 2}}}; // {ID,{R,J}}
        virtual bool HandleSignalingMessage(Ptr<const Packet> packet);
    };
}

#endif /* AUTHENTICATION_SERVER_NET_DEVICE_H */