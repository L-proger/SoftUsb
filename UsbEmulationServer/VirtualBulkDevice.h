#ifndef VIRTUALBULKDEVICE_H
#define VIRTUALBULKDEVICE_H

#include <lframework/USB/USBDevice.h>

class VirtualBulkDevice : public LFramework::USB::UsbDevice<>
{
public:
    VirtualBulkDevice::VirtualBulkDevice()
    {

    }

};

#endif // VIRTUALBULKDEVICE_H
