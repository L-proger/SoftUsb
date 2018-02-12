#ifndef USBIPTYPES_H
#define USBIPTYPES_H

#include <cstdint>
#include <QVector>
#include <array>
#include <QtEndian>

#pragma pack(push, 1)

namespace UsbIp {

enum class CommandDirection : uint16_t {
    Response = 0x0000,
    Request = 0x8000
};

enum class CommandId : uint16_t {
    DeviceList = 5,
    AttachDevice = 3,
};


struct Command {
    uint16_t value;

    CommandDirection getDirection() const {
        return static_cast<CommandDirection>(value & 0x8000);
    }
    Command& setDirection(CommandDirection direction) {
        value = (value & 0x7fff) | static_cast<uint16_t>(direction);
        return *this;
    }
    uint16_t getId() const {
        return value & 0x7fff;
    }
    Command& setId(uint16_t id) {
        auto dir = getDirection();
        value = id;
        setDirection(dir);
        return *this;
    }

    Command& operator =(uint16_t val) {
        value = val;
        return *this;
    }

    bool operator ==(uint16_t val) const {
        return value == val;
    }
};


#pragma pack(push, 1)
struct PacketHeader{
    uint16_t version;
    Command command;
    uint32_t status = 0;
};

#pragma pack(pop)
using BusID = std::array<char, 32>;

#pragma pack(push, 1)
struct UsbDeviceDescriptor {
    std::array<char, 256> path;
    BusID busID;
    uint32_t busnum;
    uint32_t devnum;
    uint32_t speed;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bConfigurationValue;
    uint8_t bNumConfigurations;
    uint8_t bNumInterfaces;
};

struct UsbInterfaceDescriptor {
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t padding;
};

struct ControlHeader{
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint8_t wValue0;
  uint8_t wValue1;
  uint8_t wIndex0;
  uint8_t wIndex1;
  uint16_t wLength;
};

struct DeviceCommand {
    uint32_t command;
    uint32_t seqnum;
    uint32_t devid;
    uint32_t direction;
    uint32_t ep;
    uint32_t transfer_flags;
    uint32_t transfer_buffer_length;
    uint32_t start_frame;
    uint32_t number_of_packets;
    uint32_t interval;
    uint64_t setup;

    template<typename Functor>
    void forEach(Functor f){
        f(command);
        f(seqnum);
        f(devid);
        f(direction);
        f(ep);
        f(transfer_flags);
        f(transfer_buffer_length);
        f(start_frame);
        f(number_of_packets);
        f(interval);
        f(setup);
    }

    void toNetwork(){
        forEach([](auto& value){ value = qToBigEndian(value); });
    }
    void toHost(){
        forEach([](auto& value){ value = qFromBigEndian(value); });
    }
};


#pragma pack(pop)
namespace Response {
    struct DeviceListHeader : public PacketHeader {
        uint32_t deviceCount;
    };

    struct Device{
        UsbDeviceDescriptor descriptor;
        QVector<UsbInterfaceDescriptor> interfaces;
    };

    struct DeviceList {
        DeviceListHeader header;
        QVector<Device> devices;
    };
}

}

#pragma pack(pop)

#endif // USBIPTYPES_H
