#ifndef USBCONNECTION_H
#define USBCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include "UsbIpTypes.h"
#include <QtEndian>
#include <QSemaphore>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class UsbConnection : public QObject
{
    Q_OBJECT
public:
    UsbConnection(QTcpSocket* socket);
    void onDisconnect();
    void onReadyRead();
private:
    bool handleDeviceListCmd();
    bool handleAttachCmd();
    bool handleDeviceCommand();
    bool handleUsbRequest(UsbIp::DeviceCommand cmd);

    bool handleControlTransfer(UsbIp::DeviceCommand cmd);
    void handleUnknownControlTransfer(UsbIp::DeviceCommand cmd);
    bool handleDataEndpoint(UsbIp::DeviceCommand cmd);
    bool handleSetConfiguration(UsbIp::DeviceCommand cmd, UsbIp::ControlHeader header);
    bool handleGetDescriptor(UsbIp::DeviceCommand cmd, UsbIp::ControlHeader header);

    UsbIp::UsbDeviceDescriptor getDeviceDescriptor();

    template<typename T>
    bool receive(T& value) {
        if(_rxOffset + sizeof(T) <= _rxBuffer.size()){
            value = *(reinterpret_cast<T*>(_rxBuffer.data() + _rxOffset));
            _rxOffset += sizeof(T);
            return true;
        }
        return false;
    }

    void parseRx();
    bool _attached = false;

    int _rxOffset = 0;
    QTcpSocket* _socket;
    QByteArray _rxBuffer;
    UsbIp::Response::DeviceList getDeviceList();
};

#endif // USBCONNECTION_H
