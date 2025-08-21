#include "Extended_Bytes.h"

int64 UBytesObject_64::GetSize()
{
    return ByteArray.Num();
}

TArray<uint8> UBytesObject_64::GetBytesAs32()
{
    const size_t Size = ByteArray.Num();

    if (Size > TNumericLimits<int32>::Max())
    {
        return TArray<uint8>();
    }

    TArray<uint8> Out_Bytes;
    Out_Bytes.SetNumUninitialized(Size);
    FMemory::Memcpy(Out_Bytes.GetData(), ByteArray.GetData(), Size);
    return Out_Bytes;
}