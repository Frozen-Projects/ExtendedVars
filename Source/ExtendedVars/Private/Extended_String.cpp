#include "Extended_BPLib.h"

FString UExtendedVarsBPLibrary::FDateTime_To_String(FDateTime In_Time, FString CustomFormat, bool bUseIso8601)
{
    if (!CustomFormat.IsEmpty())
    {
        return In_Time.ToFormattedString(*CustomFormat);
    }

    if (bUseIso8601)
    {
        return In_Time.ToIso8601();
    }

    else
    {
        return In_Time.ToString();
    }
}

FString UExtendedVarsBPLibrary::Int64_To_String(int64 TargetInt64)
{
    return FString::Printf(TEXT("%llu"), TargetInt64);
}

bool UExtendedVarsBPLibrary::String_To_Int32(int32& Out_Int32, FString SourceString)
{
    if (SourceString.IsEmpty())
    {
        return false;
    }

    Out_Int32 = FCString::Atoi(*SourceString);

    return true;
}

bool UExtendedVarsBPLibrary::String_To_Int64(int64& Out_Int64, FString SourceString)
{
    if (SourceString.IsEmpty())
    {
        return false;
    }

    Out_Int64 = FCString::Atoi64(*SourceString);

    return true;
}

bool UExtendedVarsBPLibrary::String_To_Double(double& Out_Double, FString SourceString)
{
    if (SourceString.IsEmpty())
    {
        return false;
    }

    Out_Double = FCString::Atod(*SourceString);

    return true;
}

bool UExtendedVarsBPLibrary::String_To_Float(float& Out_Float, FString SourceString)
{
    if (SourceString.IsEmpty())
    {
        return false;
    }

    Out_Float = FCString::Atof(*SourceString);

    return true;
}

bool UExtendedVarsBPLibrary::String_To_Hex(FString& Out_Hex, FString SourceString, bool bIsLower)
{
    if (SourceString.IsEmpty())
    {
        return false;
    }

    TStringConversion<TStringConvert<FString::ElementType, UTF8CHAR>> StringConverter = StringCast<UTF8CHAR>(*SourceString);

    TArray<uint8> Temp_Buffer;
    Temp_Buffer.Append((uint8*)StringConverter.Get(), StringConverter.Length());

    Out_Hex = bIsLower ? BytesToHexLower(Temp_Buffer.GetData(), Temp_Buffer.Num()) : BytesToHex(Temp_Buffer.GetData(), Temp_Buffer.Num());

    return true;
}

bool UExtendedVarsBPLibrary::String_To_Base64(FString& Out_Base64, FString SourceString, bool bUseUrl)
{
    if (SourceString.IsEmpty())
    {
        return false;
    }

    TStringConversion<TStringConvert<FString::ElementType, UTF8CHAR>> Source = StringCast<UTF8CHAR>(*SourceString);

    TArray<uint8> Temp_Buffer;
    Temp_Buffer.Append((uint8*)Source.Get(), Source.Length());

    Out_Base64 = FBase64::Encode(Temp_Buffer, bUseUrl ? EBase64Mode::UrlSafe : EBase64Mode::Standard);

    return true;
}

bool UExtendedVarsBPLibrary::Hex_To_String(FString& Out_Decoded, FString In_Hex)
{
    if (In_Hex.IsEmpty())
    {
        return false;
    }

    TArray<uint8> Temp_Buffer;
    Temp_Buffer.AddUninitialized(In_Hex.Len() / 2);
    HexToBytes(In_Hex, Temp_Buffer.GetData());

    int32 Index = 0;
    int32 Length = 0x7FFFFFFF;

    if (Index < 0)
    {
        Length += Index;
        Index = 0;
    }

    if (Length > Temp_Buffer.Num() - Index)
    {
        Length = Temp_Buffer.Num() - Index;
    }

    const FUTF8ToTCHAR Src(reinterpret_cast<const ANSICHAR*>(Temp_Buffer.GetData() + Index), Length);
    Out_Decoded = FString(Src.Length(), Src.Get());

    return true;
}

bool UExtendedVarsBPLibrary::Base64_To_String(FString& Out_Decoded, FString In_Base64, bool bUseUrl)
{
    if (In_Base64.IsEmpty())
    {
        return false;
    }

    TArray<uint8> Temp_Array;
    FBase64::Decode(In_Base64, Temp_Array, bUseUrl ? EBase64Mode::UrlSafe : EBase64Mode::Standard);

    int32 Index = 0;
    int32 Length = 0x7FFFFFFF;

    if (Index < 0)
    {
        Length += Index;
        Index = 0;
    }

    if (Length > Temp_Array.Num() - Index)
    {
        Length = Temp_Array.Num() - Index;
    }

    const FUTF8ToTCHAR Src(reinterpret_cast<const ANSICHAR*>(Temp_Array.GetData() + Index), Length);
    Out_Decoded = FString(Src.Length(), Src.Get());

    return true;
}

FString UExtendedVarsBPLibrary::Merge_Map_To_String(const TMap<FString, FString>& In_Map)
{
    if (In_Map.IsEmpty())
    {
        return FString();
    }

    FString MergedString;

    for (const TPair<FString, FString>& EachPair : In_Map)
    {
        MergedString += EachPair.Key + " = " + EachPair.Value + LINE_TERMINATOR;
    }

    return MergedString;
}