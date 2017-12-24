#include <QCoreApplication>
#include "UsbIpServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto usbServer = new UsbIpServer();

    return a.exec();
}
