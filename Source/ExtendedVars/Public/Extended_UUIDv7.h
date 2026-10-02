#pragma once

THIRD_PARTY_INCLUDES_START
#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <array>
THIRD_PARTY_INCLUDES_END

#include "Extended_UUIDv7.generated.h"

USTRUCT(BlueprintType)
struct EXTENDEDVARS_API FExtended_UUIDv7
{
    GENERATED_BODY()

private:

    static uint64_t getTimestampMs();
    static void fillRandomBytes(std::array<uint8_t, 16>& buffer);

public:
    
    static std::array<uint8_t, 16> generateBytes();
    static std::string generateString();
};