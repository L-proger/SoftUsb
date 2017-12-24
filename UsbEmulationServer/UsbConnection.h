#ifndef USBCONNECTION_H
#define USBCONNECTION_H

#include <QObject>
#include <QTcpSocket>

class UsbConnection
{
public:
    UsbConnection(QTcpSocket* socket);
    void onDisconnect();
    void onReadyRead();
private:
    QTcpSocket* _socket;
};

#endif // USBCONNECTION_H
