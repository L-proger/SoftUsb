#ifndef USBIPTYPES_H
#define USBIPTYPES_H

#include <cstdint>

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
};


struct PacketHeader{
    uint16_t version;
    Command command;
    uint32_t status = 0;
};


#pragma pack(pop)

#endif // USBIPTYPES_H
