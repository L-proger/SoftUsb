#ifndef USBIPPHY_H
#define USBIPPHY_H

#include <cstdint>

template<typename Device>
class UsbIpPhy {
public:
    static constexpr uint8_t MaxEndpointsCount = 16;
    static constexpr uint8_t MaxPacketSize = 64;

    void connect() {

    }

    void disconnect(){

    }

};


#endif // USBIPPHY_H
