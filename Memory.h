#pragma once

#include <cstdint>

namespace Memory
{
    template<typename T>
    uint8_t readByte(T& ram, uint16_t offset)
    {
        offset = offset & 0xFFF;

        return ram[offset];
    }

    template<typename T>
    uint16_t readWord(T& ram, uint16_t offset)
    {
        offset = offset & 0xFFF;

        return (readByte(ram, offset) << 8) | readByte(ram, offset + 1);
    }


    template<typename T>
    void writeByte(T& ram, uint16_t offset, uint8_t value)
    {
        ram[offset] = value;
    }

    template<typename T>
    void writeWord(T& ram, uint16_t offset, uint16_t value)
    {
        ram[offset + 1] = value & 0xFF;
        ram[offset] = (value >> 8) & 0xFF;
    }
};