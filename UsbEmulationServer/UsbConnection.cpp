#include "UsbConnection.h"
#include <cstring>

UsbConnection::UsbConnection(QTcpSocket* socket):_socket(socket){
    qDebug() << "USB connection: connected";

    connect(_socket, &QTcpSocket::readyRead, this, &UsbConnection::onReadyRead);
    connect(_socket, &QTcpSocket::disconnected, this, &UsbConnection::onDisconnect);
}

void UsbConnection::parseRx(){

    bool handled = false;
    if(!_attached){
        UsbIp::PacketHeader header;
        if(receive<UsbIp::PacketHeader>(header)){
            header.version = qFromBigEndian(header.version);
            header.command = qFromBigEndian(header.command.value);


            if(header.command == 0x8005){
                handled = handleDeviceListCmd();
            }else if(header.command == 0x8003){
                qDebug() << "Attach begin";
                handled = handleAttachCmd();
                qDebug() << "Attach end";
            }else{
                throw std::exception("Unknown command");
            }

        }
    }else{
        handled = handleDeviceCommand();
    }

    if(handled){
        _rxBuffer.remove(0, _rxOffset);
        _rxOffset = 0;
    }
}

bool UsbConnection::handleUsbRequest(UsbIp::DeviceCommand cmd){
    qDebug() << "Handling usb request ...";

    if(cmd.ep == 0){
        return handleControlTransfer(cmd);
    }else{
        return handleDataEndpoint(cmd);
    }
}

bool UsbConnection::handleControlTransfer(UsbIp::DeviceCommand cmd){
    UsbIp::ControlHeader header;

    header.bmRequestType=  (cmd.setup & 0xFF00000000000000)>>56;
    header.bRequest=       (cmd.setup & 0x00FF000000000000)>>48;
    header.wValue0=        (cmd.setup & 0x0000FF0000000000)>>40;
    header.wValue1=        (cmd.setup & 0x000000FF00000000)>>32;
    header.wIndex0=        (cmd.setup & 0x00000000FF000000)>>24;
    header.wIndex1=        (cmd.setup & 0x0000000000FF0000)>>16;
    header.wLength=         cmd.setup & 0x000000000000FFFF;

    bool handled = false;
    if(header.bmRequestType == 0x80) {
        if(header.bRequest == 0x06) { // Get descriptor
            handled = handleGetDescriptor(cmd, header);
        }
        if(header.bRequest == 0x00) { // Get status

        }
    }else if(header.bmRequestType == 0x00) {
        if(header.bRequest == 0x09) { // Set Configuration
            handled = handleSetConfiguration(cmd, header);
        }
    }else if(header.bmRequestType == 0x01) {
        if(header.bRequest == 0x0B) { //Set interface

        }
    }

    if(!handled)
        handleUnknownControlTransfer(cmd);
    return true;
}

bool UsbConnection::handleGetDescriptor(UsbIp::DeviceCommand cmd, UsbIp::ControlHeader header){
    bool handled = false;

    if(header.wValue1 == 0x1) { //Device descriptor

    }else if(header.wValue1 == 0x2) { //Configuration descriptor

    }else if(header.wValue1 == 0x3) { //String descriptor

    }else if(header.wValue1 == 0x6) { //Device qualifier descriptor

    }
    return handled;
}

bool UsbConnection::handleSetConfiguration(UsbIp::DeviceCommand cmd, UsbIp::ControlHeader header){
    return true;
}

void UsbConnection::handleUnknownControlTransfer(UsbIp::DeviceCommand cmd){

}

bool UsbConnection::handleDataEndpoint(UsbIp::DeviceCommand cmd){
    return true;
}

bool UsbConnection::handleDeviceCommand(){
    qDebug() << "Handle attached device command";

    UsbIp::DeviceCommand cmd;
    if(!receive(cmd)){
        return false;
    }

    cmd.toHost();

    if(cmd.command == 1){
        return handleUsbRequest(cmd);
    }else if(cmd.command == 2){
        //unlink urb
        qDebug() << "I don't know how to handle urb unlink command";
        _socket->close();
    }else{
        qDebug() << "Unknown device command";
        _socket->close();
    }

    qDebug() << cmd.command;

    return true;
}

void UsbConnection::onDisconnect(){
    qDebug() << "USB connection: disconnected";
    _socket->close();
    _socket = nullptr;
    qDebug() << "Exit parse thread handled";
}

void UsbConnection::onReadyRead(){
    int rxCount = 0;
    while(_socket->bytesAvailable() > 0){
        auto buffer = _socket->readAll();
        rxCount += buffer.size();
        _rxBuffer.append(buffer);
    }
    qDebug() << "Received " << rxCount << " bytes";
    parseRx();
}


bool UsbConnection::handleDeviceListCmd(){
    qDebug() << "Get device list command received";
    //Get device list command
    auto devList = getDeviceList();

    _socket->write(reinterpret_cast<char*>(&devList.header), sizeof(devList.header));

    for(int i = 0; i < devList.devices.size(); ++i){
        auto& device = devList.devices[i];

        _socket->write(reinterpret_cast<char*>(&device.descriptor), sizeof(device.descriptor));
        _socket->waitForBytesWritten();

         for(int j = 0; j < device.interfaces.size(); ++j){
             auto& interface = device.interfaces[j];
             _socket->write(reinterpret_cast<char*>(&interface), sizeof(interface));
            _socket->waitForBytesWritten();
         }
    }
    return true;
}

bool UsbConnection::handleAttachCmd(){
    UsbIp::BusID id;
    if(!receive(id)){
        return false;
    }
    qDebug() << "Attaching device with ID = " << id.data();


#pragma pack(push, 1)
    struct SuccessResponse{
        UsbIp::PacketHeader header;
        UsbIp::UsbDeviceDescriptor descriptor;
    };


    #pragma pack(pop)
    SuccessResponse response;

    response.header.version=qToBigEndian<uint16_t>(0x106);
    response.header.command.value=qToBigEndian<uint16_t>(3);
    response.header.status=0;

    response.descriptor = getDeviceDescriptor();

    _socket->write(reinterpret_cast<char*>(&response), sizeof(response));
    _socket->waitForBytesWritten();

    _attached = true;
    return true;
}

UsbIp::UsbDeviceDescriptor UsbConnection::getDeviceDescriptor(){
    UsbIp::UsbDeviceDescriptor descriptor;

    memset(descriptor.path.data(), 0, 256);
    strcpy(descriptor.path.data(),"/sys/devices/pci0000:00/0000:00:01.2/usb1/1-1");
    memset(descriptor.busID.data(),0,32);
    strcpy(descriptor.busID.data(),"1-1");
    descriptor.busnum=qToBigEndian<uint32_t>(1);
    descriptor.devnum=qToBigEndian<uint32_t>(2);
    descriptor.speed=qToBigEndian<uint32_t>(2);
    descriptor.idVendor=qToBigEndian<uint16_t>(0x0300);
    descriptor.idProduct=qToBigEndian<uint16_t>(0x0C1B);
    descriptor.bcdDevice=qToBigEndian<uint16_t>(1);
    descriptor.bDeviceClass= 0xff;
    descriptor.bDeviceSubClass=0;
    descriptor.bDeviceProtocol=0;
    descriptor.bConfigurationValue=0;
    descriptor.bNumConfigurations=1;
    descriptor.bNumInterfaces=1;

    return descriptor;
}

UsbIp::Response::DeviceList UsbConnection::getDeviceList(){
    UsbIp::Response::DeviceList result;
    UsbIp::Response::Device device;

    result.header.version = qToBigEndian<uint16_t>(0x106);
    result.header.command=qToBigEndian<uint16_t>(5);
    result.header.status = 0;
    result.header.deviceCount = qToBigEndian<uint32_t>(1);

    device.descriptor = getDeviceDescriptor();

    UsbIp::UsbInterfaceDescriptor interface;
    interface.bInterfaceClass = 0xff;
    interface.bInterfaceSubClass = 0xff;
    interface.bInterfaceProtocol = 0xff;
    interface.padding = 0;

    device.interfaces.append(interface);

    result.devices.append(device);

    return result;
}
