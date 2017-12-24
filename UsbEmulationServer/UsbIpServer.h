#ifndef USBIPSERVER_H
#define USBIPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QSharedPointer>
#include <QVector>
#include <UsbConnection.h>

class UsbIpServer : public QObject {
    Q_OBJECT
public:
    UsbIpServer();

public slots:
    void onNewConnection();
private:
    QTcpServer* _tcpServer;
    QVector<QSharedPointer<UsbConnection>> _connections;
};

#endif // USBIPSERVER_H
