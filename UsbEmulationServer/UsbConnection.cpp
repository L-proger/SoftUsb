#include "UsbConnection.h"

UsbConnection::UsbConnection(QTcpSocket* socket):_socket(socket){
    qDebug() << "USB connection: connected";

    connect(_socket, &QTcpSocket::readyRead, this, &UsbConnection::onReadyRead);
    connect(_socket, &QTcpSocket::disconnected, this, &UsbConnection::onDisconnect);
}

void UsbConnection::onDisconnect(){
    qDebug() << "USB connection: disconnected";
    _socket->close();
    _socket = nullptr;
}

void UsbConnection::onReadyRead(){
    qDebug() << "USB connection: ready read" << _socket->bytesAvailable();

   /* while(_socket->bytesAvailable() > 0){
        auto data = _tcpSocket->readAll();
    }
    _socket->re*/
}
