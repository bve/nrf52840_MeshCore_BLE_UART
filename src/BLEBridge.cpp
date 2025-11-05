#include "BLEBridge.h"
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <led.h>

String readString(Uart *uart, unsigned long timeout = 10000)
{
    uart->setTimeout(timeout);
    String data = uart->readStringUntil('\n');
    data.trim();
    return data;
}

BLEBridge::BLEBridge(Uart *uart, SerialBLEInterface bleInterface)
    : uart(uart),
      bleInterface(bleInterface),
      bleScratch{}
{
    uart->println("BLEBridge Initialized");
}

void BLEBridge::startAdv()
{
    bleInterface.startAdv();
}
void BLEBridge::stopAdv()
{
    bleInterface.stopAdv();
}

void BLEBridge::begin(const char *device_name, uint32_t pin_code)
{
    uart->println("BLEBridge Begin: " + String(device_name) + " PIN: " + String(pin_code));
    bleInterface.begin(device_name, pin_code);
}

void BLEBridge::enable()
{
    bleInterface.enable();
}

void BLEBridge::disable()
{
    bleInterface.disable();
}

bool BLEBridge::isEnabled() const
{
    return bleInterface.isEnabled();
}

bool BLEBridge::isConnected() const
{
    return bleInterface.isConnected();
}

bool BLEBridge::isWriteBusy() const
{
    return bleInterface.isWriteBusy();
}

size_t BLEBridge::writeFrame(const uint8_t src[], size_t len)
{
    return bleInterface.writeFrame(src, len);
}

size_t BLEBridge::checkRecvFrame(uint8_t dest[])
{
    return bleInterface.checkRecvFrame(dest);
}

void BLEBridge::handleStartAdv()
{
    startAdv();
    uart->println("OK");
}

void BLEBridge::handleStopAdv()
{
    stopAdv();
    uart->println("OK");
}

void BLEBridge::handleEnable()
{
    enable();
    uart->println("OK");
}

void BLEBridge::handleDisable()
{
    disable();
    uart->println("OK");
}

void BLEBridge::handleBegin(const char *args)
{
    String str = String(args);
    String name = str.substring(0, str.indexOf(' '));
    String pin = str.substring(str.indexOf(' ') + 1);

    if (name.length() > 0 && pin.length() > 0)
    {
        uint32_t pinCode = strtoul(pin.c_str(), nullptr, 10);
        begin(name.c_str(), pinCode);
        uart->println("OK");
    }
    else
    {
        uart->println("ERR Usage: BEGIN <name> <pin>");
    }
}

void BLEBridge::handleStatus()
{
    uart->print("ENABLED=");
    uart->print(isEnabled() ? 1 : 0);
    uart->print(" CONNECTED=");
    uart->print(isConnected() ? 1 : 0);
    uart->print(" WRITE_BUSY=");
    uart->println(isWriteBusy() ? 1 : 0);
}

void BLEBridge::handleIsEnabled()
{
    uart->println(isEnabled() ? "1" : "0");
}

void BLEBridge::handleIsConnected()
{
    uart->println(isConnected() ? "1" : "0");
}

void BLEBridge::handleIsWriteBusy()
{
    uart->println(isWriteBusy() ? "1" : "0");
}

void BLEBridge::handleWrite(size_t len)
{
    uart->setTimeout(5000);
    uart->print("RDY");

    size_t bytesRead = uart->readBytes(bleScratch, len);

    if (bytesRead != len)
    {
        uart->println("ERR Expected to read " + String(len) + " bytes, but got " + String(bytesRead));
    }

    if (bytesRead == 0)
    {
        uart->println(0);
    }

    size_t written = writeFrame(bleScratch, bytesRead);
    if (written == bytesRead)
    {
        uart->println(written);
    }
    else
    {
        uart->println(0);
    }
}

void BLEBridge::handleCheckRecv()
{
    size_t len = checkRecvFrame(bleScratch);

    if (len == 0)
    {
        uart->println(len);
    }
    else
    {
        uart->println(len);
        uart->flush();
        uart->find("RDY");
        uart->write(bleScratch, len);
        uart->flush();
    }
}

void BLEBridge::dispatchCommand(const char *line, const char *args)
{
    if (strcmp(line, "STA") == 0)
    {
        handleStartAdv();
    }
    else if (strcmp(line, "STP") == 0)
    {
        handleStopAdv();
    }
    else if (strcmp(line, "ENA") == 0)
    {
        handleEnable();
    }
    else if (strcmp(line, "DIS") == 0)
    {
        handleDisable();
    }
    else if (strcmp(line, "BEG") == 0)
    {
        handleBegin(args);
    }
    else if (strcmp(line, "STS") == 0)
    {
        handleStatus();
    }
    else if (strcmp(line, "ISE") == 0)
    {
        handleIsEnabled();
    }
    else if (strcmp(line, "ISC") == 0)
    {
        handleIsConnected();
    }
    else if (strcmp(line, "ISW") == 0)
    {
        handleIsWriteBusy();
    }
    else if ((strcmp(line, "WRI") == 0) && isEnabled())
    {
        size_t len = static_cast<size_t>(std::strtol(args, nullptr, 10));
        handleWrite(len);
    }
    else if ((strcmp(line, "CHE") == 0) && isEnabled())
    {
        handleCheckRecv();
    }
    else
    {
        if (isEnabled())
        {
            uart->println("ERR Unknown command: " + String(line) + " " + String(args) + " Enabled: 1");
        }
        else
        {
            uart->println("REBOOT");
        }
    }
}

void BLEBridge::loop()
{
    uart->setTimeout(50);
    if (!uart->find('&'))
    {
        return;
    }
    String cmd = readString(uart, 1000);
    if (cmd.length() > 0)
    {
        int spaceIndex = cmd.indexOf(' ');
        if (spaceIndex == -1)
        {
            dispatchCommand(cmd.c_str(), "");
        }
        else
        {
            String line = cmd.substring(0, spaceIndex);
            String args = cmd.substring(spaceIndex + 1);
            dispatchCommand(line.c_str(), args.c_str());
        }
    }
}