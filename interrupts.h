#pragma once
#include "types.h"
#include "port.h"
#include "gdt.h"
class InterruptManager;

class InterruptHandler
{
protected:
    uint8_t intteruptNumber;
    InterruptManager *interruptManager;
    InterruptHandler(uint8_t interruptNumber, InterruptManager *manager);
    ~InterruptHandler();

public:
    virtual uint32_t handle(uint32_t esp);
};

class InterruptManager
{
    friend class InterruptHandler;

protected:
    static InterruptManager *current;
    InterruptHandler *handlers[256];
    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    } __attribute__((packed));

    static GateDescriptor interruptDescriptorTable[256];

    struct InterruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));

    static void setInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t descriptorPrivilegeLevel,
        uint8_t descriptorType);

    // these ports are for speaking with
    // programmable interrupt controllers (pic)
    // there are 2 pics one master and one slave

    Port8Slow picMasterCommand;
    Port8Slow picMasterData;
    Port8Slow picSlaveCommand;
    Port8Slow picSlaveData;

public:
    InterruptManager(GDT *gdt);
    ~InterruptManager();
    void activate();
    void deactivate();
    static uint32_t handleInterrupt(uint8_t int_num, uint32_t esp);
    uint32_t doHandleInterrupt(uint8_t int_num, uint32_t esp);
    static void ignoreInterruptRequest();
    static void handleInterruptRequest0x00();
    static void handleInterruptRequest0x01();
};