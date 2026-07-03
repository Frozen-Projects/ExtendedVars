#include "Extended_BPLib.h"

FString UExtendedVarsBPLibrary::Bytes_x86_To_Hex(TArray<uint8> In_Bytes, int32 Hex_Start, int32 Hex_End, bool bIsFull)
{
    if (In_Bytes.Num() == 0)
    {
        return "";
    }

    if (bIsFull)
    {
        std::stringstream Stream;
        Stream << std::hex << std::setfill('0');
        for (int Index = 0; Index < In_Bytes.Num(); Index++)
        {
            Stream << std::hex << std::setw(2) << static_cast<int>(In_Bytes.GetData()[Index]);
        }

        return Stream.str().c_str();

        /**
        * UE's implementation.
        * return BytesToHex(In_Bytes.GetData(), In_Bytes.Num());
        */
    }

    else if (Hex_Start <= Hex_End && Hex_End < In_Bytes.Num())
    {
        std::stringstream Stream;
        Stream << std::hex << std::setfill('0');
        for (int Index = Hex_Start; Index <= Hex_End; Index++)
        {
            Stream << std::hex << std::setw(2) << static_cast<int>(In_Bytes.GetData()[Index]);
        }

        return Stream.str().c_str();
    }

    else
    {
        return "";
    }
}

FString UExtendedVarsBPLibrary::Bytes_x86_To_Base64(TArray<uint8> In_Bytes, bool bUseUrl)
{
    if (In_Bytes.Num() == 0)
    {
        return "";
    }

    return FBase64::Encode(In_Bytes.GetData(), In_Bytes.GetAllocatedSize(), bUseUrl ? EBase64Mode::UrlSafe : EBase64Mode::Standard);
}

FString UExtendedVarsBPLibrary::Bytes_x86_To_UTF8(TArray<uint8> In_Bytes)
{
    if (In_Bytes.Num() == 0)
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

    if (Length > In_Bytes.Num() - Index)
    {
        Length = In_Bytes.Num() - Index;
    }

    const FUTF8ToTCHAR Src(reinterpret_cast<const ANSICHAR*>(In_Bytes.GetData() + Index), Length);
    return FString(Src.Length(), Src.Get());
}

EImageExtensions UExtendedVarsBPLibrary::DetectImageExtension(TArray<uint8> In_Bytes)
{
    // JPG
    if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == "ffd8" && UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, (In_Bytes.Num() - 2), (In_Bytes.Num() - 1), false) == "ffd9")
    {
        return EImageExtensions::EXT_JPEG;
    }

    // BMP
    else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == "424d")
    {
        return EImageExtensions::Ext_BMP;
    }

    // PNG
    else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 7, false) == "89504e470d0a1a0a")
    {
        return EImageExtensions::EXT_PNG;
    }

    else
    {
        return EImageExtensions::Ext_None;
    }
}

TArray<uint8> UExtendedVarsBPLibrary::Hex_To_Bytes_x86(FString In_Hex)
{
    TArray<uint8> Array_Bytes;

    if (In_Hex.IsEmpty())
    {
        return Array_Bytes;
    }

    Array_Bytes.AddUninitialized(In_Hex.Len() / 2);
    HexToBytes(In_Hex, Array_Bytes.GetData());

    return Array_Bytes;
}

TArray<uint8> UExtendedVarsBPLibrary::UTF8_To_Bytes_x86(FString In_UTF8)
{
    TArray<uint8> Array_Bytes;

    if (In_UTF8.IsEmpty())
    {
        return Array_Bytes;
    }

    TStringConversion<TStringConvert<FString::ElementType, UTF8CHAR>> StringConverter = StringCast<UTF8CHAR>(*In_UTF8);
    Array_Bytes.Append((uint8*)StringConverter.Get(), StringConverter.Length());

    return Array_Bytes;
}

TArray<uint8> UExtendedVarsBPLibrary::Base64_To_Bytes_x86(FString In_Base64, bool bUseUrl)
{
    TArray<uint8> Array_Bytes;

    if (In_Base64.IsEmpty())
    {
        return Array_Bytes;
    }

    FBase64::Decode(In_Base64, Array_Bytes, bUseUrl ? EBase64Mode::UrlSafe : EBase64Mode::Standard);

    return Array_Bytes;
}