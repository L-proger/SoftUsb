#include <QCoreApplication>
#include "UsbIpServer.h"

#include "UsbIpPhy.h"
#include <lframework/USB/USBDevice.h>
//#include "VirtualBulkDevice.h"

using namespace LFramework;
using namespace LFramework::USB;

template<typename Interface>
class UsbFunction {
public:
    static constexpr size_t interfacesCount = 1;
};

template<typename...Interfaces>
class UsbFunction<LFramework::USB::InterfaceAssociation<Interfaces...>> {
public:
    static constexpr size_t interfacesCount = sizeof...(Interfaces);
};

using BulkInterface = UsbInterface<>;
using BulkFunction = UsbFunction<BulkInterface>;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    using BulkConfiguration = UsbConfiguration<BulkFunction>;

    using BulkDevice = LFramework::USB::UsbDevice<UsbIpPhy, BulkConfiguration>;



    using Phy = typename BulkDevice::Phy;

    BulkFunction func;
    BulkConfiguration config(&func);

    Phy* phy = new Phy();
    BulkDevice* device = new BulkDevice(&config);



    auto usbServer = new UsbIpServer();
    return a.exec();
}
