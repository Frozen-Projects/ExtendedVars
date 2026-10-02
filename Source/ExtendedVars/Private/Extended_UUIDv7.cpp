#include "Extended_UUIDv7.h"

uint64_t FExtended_UUIDv7::getTimestampMs()
{
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    const std::chrono::system_clock::duration duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

void FExtended_UUIDv7::fillRandomBytes(std::array<uint8_t, 16>& buffer)
{
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint16_t> dis(0, 255);

    for (auto& byte : buffer)
    {
        byte = static_cast<uint8_t>(dis(gen));
    }
}

std::array<uint8_t, 16> FExtended_UUIDv7::generateBytes()
{
    std::array<uint8_t, 16> uuid{};

    // 1. Get current timestamp in milliseconds
    uint64_t timestamp = getTimestampMs();

    // 2. Fill the entire UUID buffer with random bytes first
    // This covers rand_a and rand_b fields automatically
    fillRandomBytes(uuid);

    // 3. Write the 48-bit Timestamp into the first 6 bytes (Big Endian)
    uuid[0] = (timestamp >> 40) & 0xFF;
    uuid[1] = (timestamp >> 32) & 0xFF;
    uuid[2] = (timestamp >> 24) & 0xFF;
    uuid[3] = (timestamp >> 16) & 0xFF;
    uuid[4] = (timestamp >> 8) & 0xFF;
    uuid[5] = timestamp & 0xFF;

    // 4. Set Version to 7 (0111)
    // uuid[6] looks like: 0111xxxx
    uuid[6] = (uuid[6] & 0x0F) | 0x70;

    // 5. Set Variant to 2 (10)
    // uuid[8] looks like: 10xxxxxx
    uuid[8] = (uuid[8] & 0x3F) | 0x80;

    return uuid;
}

std::string FExtended_UUIDv7::generateString()
{
    std::array<uint8_t, 16> bytes = FExtended_UUIDv7::generateBytes();
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    // Format: 8-4-4-4-12
    for (int i = 0; i < 16; ++i) 
    {
        if (i == 4 || i == 6 || i == 8 || i == 10) 
        {
            ss << "-";
        }

        ss << std::setw(2) << static_cast<int>(bytes[i]);
    }

    return ss.str();
}