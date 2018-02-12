#include "UsbIpServer.h"
#include <stdexcept>
#include <UsbConnection.h>

UsbIpServer::UsbIpServer(){
    qDebug() << "Starting USB IP server";
    _tcpServer = new QTcpServer(this);

    connect(_tcpServer, &QTcpServer::newConnection, this, &UsbIpServer::onNewConnection);

    if(!_tcpServer->listen(QHostAddress::Any, 3240)){
        throw std::runtime_error("Failed to start listen");
    }
}

void UsbIpServer::onNewConnection(){
    qDebug() << "USB IP server: new connection";
    auto socket = _tcpServer->nextPendingConnection();
    auto connection = new UsbConnection(socket);
}

