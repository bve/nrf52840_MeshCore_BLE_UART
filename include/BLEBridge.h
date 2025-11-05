#pragma once

#include "BaseSerialInterface.h"
#include "SerialBLEInterface.h"

class BLEBridge : public BaseSerialInterface
{
public:
    static constexpr size_t CommandBufferSize = 256;

private:
    Uart *uart;
    SerialBLEInterface bleInterface;

    struct PendingFrame
    {
        size_t len;
        uint8_t data[MAX_FRAME_SIZE + 1];
    };

    uint8_t bleScratch[MAX_FRAME_SIZE + 1];

    void dispatchCommand(const char *line, const char *args);
    void handleStartAdv();
    void handleStopAdv();
    void handleEnable();
    void handleDisable();
    void handleBegin(const char *args);
    void handleStatus();
    void handleIsEnabled();
    void handleIsConnected();
    void handleIsWriteBusy();
    void handleWrite(size_t len);
    void handleCheckRecv();
    void pumpBleFrames();

public:
    BLEBridge(Uart *uart, SerialBLEInterface bleInterface);

    void startAdv();
    void stopAdv();
    void begin(const char *device_name, uint32_t pin_code);

    // BaseSerialInterface methods
    void enable() override;
    void disable() override;
    bool isEnabled() const override;

    bool isConnected() const override;

    bool isWriteBusy() const override;
    size_t writeFrame(const uint8_t src[], size_t len) override;
    size_t checkRecvFrame(uint8_t dest[]) override;

    void loop();
};
