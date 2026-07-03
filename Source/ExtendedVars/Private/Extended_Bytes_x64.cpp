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

bool UBytesObject_64::SetBytesFrom32(const TArray<uint8>& In_Bytes)
{
    if (In_Bytes.IsEmpty() || !In_Bytes.GetData())
    {
        return false;
    }

	this->ByteArray.Empty();
    this->ByteArray.Append(In_Bytes);
    return true;
}

FString UBytesObject_64::Bytes_x64_To_Hex(int32 Hex_Start, int32 Hex_End, bool bIsFull)
{
    if (this->ByteArray.IsEmpty())
    {
        return "";
    }

    if (bIsFull)
    {
        std::stringstream Stream;
        Stream << std::hex << std::setfill('0');
        for (int Index = 0; Index < this->ByteArray.Num(); Index++)
        {
            Stream << std::hex << std::setw(2) << static_cast<int>(this->ByteArray.GetData()[Index]);
        }

        return Stream.str().c_str();
    }

    else if (Hex_Start <= Hex_End && Hex_End < this->ByteArray.Num())
    {
        std::stringstream Stream;
        Stream << std::hex << std::setfill('0');
        for (int Index = Hex_Start; Index <= Hex_End; Index++)
        {
            Stream << std::hex << std::setw(2) << static_cast<int>(this->ByteArray.GetData()[Index]);
        }

        return Stream.str().c_str();
    }

    else
    {
        return "";
    }
}

FString UBytesObject_64::Bytes_x64_To_Base64(bool bUseUrl)
{
    if (this->ByteArray.IsEmpty())
    {
        return "";
    }

    return FBase64::Encode(this->ByteArray.GetData(), this->ByteArray.Num(), bUseUrl ? EBase64Mode::UrlSafe : EBase64Mode::Standard);
}

FString UBytesObject_64::Bytes_x64_To_UTF8()
{
    if (this->ByteArray.IsEmpty())
    {
        return "";
    }

    int32 Index = 0;
    int32 Length = 0x7FFFFFFF;

    if (Index < 0)
    {
        Length += Index;
        Index = 0;
    }

    if (Length > this->ByteArray.Num() - Index)
    {
        Length = this->ByteArray.Num() - Index;
    }

    const FUTF8ToTCHAR Src(reinterpret_cast<const ANSICHAR*>(this->ByteArray.GetData() + Index), Length);
    return FString(Src.Length(), Src.Get());
}

bool UBytesObject_64::Hex_To_Bytes_x64(FString In_Hex)
{
    if (In_Hex.IsEmpty())
    {
        return false;
    }

    FString Decoded_String;
    for (size_t Index_Chars = 0; Index_Chars < In_Hex.Len(); Index_Chars += 2)
    {
        //taking two characters from hex string
        FString Part = UKismetStringLibrary::GetSubstring(In_Hex, Index_Chars, 2);

        //changing it into base 16
        char Character = std::stoul(TCHAR_TO_UTF8(*Part), nullptr, 16);

        //putting it into the ASCII string
        Decoded_String += Character;
    }

    FTCHARToUTF8 Source = FTCHARToUTF8(Decoded_String.GetCharArray().GetData());
    this->ByteArray.Append((uint8*)Source.Get(), Source.Length());

    return true;
}

bool UBytesObject_64::UTF8_To_Bytes_x64(FString In_UTF8)
{
    if (In_UTF8.IsEmpty())
    {
        return false;
    }

    FTCHARToUTF8 Source = FTCHARToUTF8(In_UTF8.GetCharArray().GetData());
    this->ByteArray.Append((uint8*)Source.Get(), Source.Length());

    return true;
}

bool UBytesObject_64::Base64_To_Bytes_x64(FString In_Base64, bool bUseUrl)
{
    if (In_Base64.IsEmpty())
    {
        return false;
    }

    FWideStringView String = In_Base64;
    FBase64::Decode(String.GetData(), String.Len(), this->ByteArray.GetData(), bUseUrl ? EBase64Mode::UrlSafe : EBase64Mode::Standard);

    return true;
}