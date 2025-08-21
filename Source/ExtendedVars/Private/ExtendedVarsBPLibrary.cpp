// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendedVarsBPLibrary.h"
#include "ExtendedVars.h"

UExtendedVarsBPLibrary::UExtendedVarsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

#pragma region Fonts_Group

URuntimeFont* UExtendedVarsBPLibrary::Runtime_Font_Load(TArray<uint8> In_Bytes, FString FontName)
{
    UFontFace* FontFace = NewObject<UFontFace>();
    FontFace->LoadingPolicy = EFontLoadingPolicy::Inline;
    FontFace->FontFaceData = FFontFaceData::MakeFontFaceData(CopyTemp(In_Bytes));
    FontFace->AddToRoot();

    UFont* Font = NewObject<UFont>();
    Font->FontCacheType = EFontCacheType::Runtime;
    FTypefaceEntry& TypefaceEntry = Font->CompositeFont.DefaultTypeface.Fonts[Font->CompositeFont.DefaultTypeface.Fonts.AddDefaulted()];
    TypefaceEntry.Font = FFontData(FontFace);
    Font->AddToRoot();

    URuntimeFont* RuntimeFont = NewObject<URuntimeFont>();
    RuntimeFont->Font_Face = FontFace;
    RuntimeFont->Font = Font;
    RuntimeFont->FontName = FontName;

    return RuntimeFont;
}

#pragma endregion Fonts_Group

#pragma region Sorters

TArray<FString> UExtendedVarsBPLibrary::Sort_String(TArray<FString> TargetArray, bool bIsDescending)
{
    TArray<FString> SortedArray = TargetArray;

    if (bIsDescending == true)
    {
        SortedArray.Sort([](const FString Value1, const FString Value2)
            {
                return Value1 < Value2;
            });
    }

    else
    {
        SortedArray.Sort([](const FString Value1, const FString Value2)
            {
                return Value1 > Value2;
            });
    }

    return SortedArray;
}

TArray<int32> UExtendedVarsBPLibrary::Sort_Int32(TArray<int32> TargetArray, bool bIsDescending)
{
    TArray<int32> SortedArray = TargetArray;

    if (bIsDescending == true)
    {
        SortedArray.Sort([](const int32 Value1, const int32 Value2)
            {
                return Value1 < Value2;
            });
    }

    else
    {
        SortedArray.Sort([](const int32 Value1, const int32 Value2)
            {
                return Value1 > Value2;
            });
    }

    return SortedArray;
}

TArray<float> UExtendedVarsBPLibrary::Sort_Float(TArray<float> TargetArray, bool bIsDescending)
{
    TArray<float> SortedArray = TargetArray;

    if (bIsDescending == true)
    {
        SortedArray.Sort([](const float Value1, const float Value2)
            {
                return Value1 < Value2;
            });
    }

    else
    {
        SortedArray.Sort([](const float Value1, const float Value2)
            {
                return Value1 > Value2;
            });
    }

    return SortedArray;
}

TArray<double> UExtendedVarsBPLibrary::Sort_Double(TArray<double> TargetArray, bool bIsDescending)
{
    TArray<double> SortedArray = TargetArray;

    if (bIsDescending == true)
    {
        SortedArray.Sort([](const float Value1, const float Value2)
            {
                return Value1 < Value2;
            });
    }

    else
    {
        SortedArray.Sort([](const float Value1, const float Value2)
            {
                return Value1 > Value2;
            });
    }

    return SortedArray;
}

TArray<FDateTime> UExtendedVarsBPLibrary::Sort_Time(TArray<FDateTime> TargetArray, bool bIsDescending)
{
    TArray<FDateTime> SortedArray = TargetArray;

    if (bIsDescending == true)
    {
        SortedArray.Sort([](const FDateTime Value1, const FDateTime Value2)
            {
                return Value1 < Value2;
            });
    }

    else
    {
        SortedArray.Sort([](const FDateTime Value1, const FDateTime Value2)
            {
                return Value1 > Value2;
            });
    }

    return SortedArray;
}

#pragma endregion Sorters

#pragma region Read_Group

FString UExtendedVarsBPLibrary::Android_Path_Helper(FString In_FileName)
{
    if (UGameplayStatics::GetPlatformName() == "Android" || UGameplayStatics::GetPlatformName() == "IOS")
    {
        FString Path_Referance = FPlatformFileManager::Get().GetPlatformFile().ConvertToAbsolutePathForExternalAppForRead(*(FPaths::ProjectSavedDir()));

        TArray<FString> Path_Sections;
        Path_Referance.ParseIntoArray(Path_Sections, TEXT("/"), true);
        
        return "/" + Path_Sections[0] + "/" + Path_Sections[1] + "/" + Path_Sections[2] + "/" + In_FileName;
    }

    else
    {
        return "";
    }
}

bool UExtendedVarsBPLibrary::Get_Folder_Contents(TArray<FFolderContents>& OutContents, FString& ErrorCode, FString InPath)
{
    if (InPath.IsEmpty() == true)
    {
        ErrorCode = "Path is empty.";
        return false;
    }

    if (FPaths::DirectoryExists(InPath) == false)
    {
        ErrorCode = "Directory doesn't exist.";
        return false;
    }

    FFindDirectories GetFoldersVisitor;
    FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*InPath, GetFoldersVisitor);

    OutContents = GetFoldersVisitor.Array_Contents;

    return true;
}

void UExtendedVarsBPLibrary::Search_In_Folder(FDelegateFolderContents DelegateFolderContents, FString InPath, FString InSearch, bool bSearchExact)
{
    if (InPath.IsEmpty() == true)
    {
        return;
    }

    if (InSearch.IsEmpty() == true)
    {
        return;
    }

    if (FPaths::DirectoryExists(InPath) == false)
    {
        return;
    }

    AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateFolderContents, InPath, InSearch, bSearchExact]()
        {
            IFileManager& FileManager = FFileManagerGeneric::Get();

            TArray<FString> Array_Contents;
            TArray<FFolderContents> Array_Founds;

            FileManager.FindFilesRecursive(Array_Contents, *InPath, TEXT("*"), true, true, false);

            for (int32 ContentIndex = 0; ContentIndex < Array_Contents.Num(); ContentIndex++)
            {
                FFolderContents EachContent;

                if (bSearchExact == true)
                {
                    if (FPaths::GetBaseFilename(Array_Contents[ContentIndex]) == InSearch)
                    {
                        EachContent.Name = FPaths::GetCleanFilename(Array_Contents[ContentIndex]);
                        EachContent.Path = Array_Contents[ContentIndex];
                        EachContent.bIsFile = FPaths::FileExists(Array_Contents[ContentIndex]);

                        Array_Founds.Add(EachContent);
                    }
                }

                else
                {
                    if (FPaths::GetBaseFilename(Array_Contents[ContentIndex]).Contains(InSearch) == true)
                    {
                        EachContent.Name = FPaths::GetCleanFilename(Array_Contents[ContentIndex]);
                        EachContent.Path = Array_Contents[ContentIndex];
                        EachContent.bIsFile = FPaths::FileExists(Array_Contents[ContentIndex]);

                        Array_Founds.Add(EachContent);
                    }
                }
            }

            AsyncTask(ENamedThreads::GameThread, [DelegateFolderContents, Array_Founds]()
                {
                    DelegateFolderContents.ExecuteIfBound(true, "Success", Array_Founds);
                }
            );
        }
    );
}

bool UExtendedVarsBPLibrary::Read_File_From_Path_64(UBytesObject_64*& Out_Bytes_Object, FString In_Path, bool bUseLowLevel)
{
    if (In_Path.IsEmpty())
    {
        return false;
    }
    
    FPaths::NormalizeFilename(In_Path);
    FString Path = FPlatformFileManager::Get().GetPlatformFile().ConvertToAbsolutePathForExternalAppForRead(*In_Path);

    if (!FPaths::FileExists(Path))
    {
        return false;
    }

    TArray64<uint8> ByteArray;

    if (bUseLowLevel)
    {
        FPaths::MakePlatformFilename(Path);

        FILE* File = nullptr;

#ifdef _WIN64

        fopen_s(&File, TCHAR_TO_UTF8(*Path), "rb");
 
#else

        File = fopen(TCHAR_TO_UTF8(*Path), "rb");

#endif // _WIN64

        fseek(File, 0L, SEEK_END);
        const long FileSize = ftell(File);
        fseek(File, 0L, SEEK_SET);

        if (FileSize == 0)
        {
            fclose(File);
            return false;
        }

        uint8* Buffer = (uint8*)malloc(FileSize);
        fread(Buffer, sizeof(uint8), FileSize, File);

        ByteArray.SetNum(FileSize);
        FMemory::Memcpy(ByteArray.GetData(), Buffer, FileSize);
        
        fclose(File);
        free(Buffer);
        Buffer = nullptr;
    }

    else
    {
        FFileHelper::LoadFileToArray(ByteArray, *Path, FILEREAD_AllowWrite);

        if (ByteArray.IsEmpty())
        {
            return false;
        }
    }

    Out_Bytes_Object = NewObject<UBytesObject_64>();
    Out_Bytes_Object->ByteArray = ByteArray;

    return true;
}

bool UExtendedVarsBPLibrary::Read_File_From_Path_32(TArray<uint8>& Out_Bytes, FString In_Path, bool bUseLowLevel)
{
    if (In_Path.IsEmpty())
    {
        return false;
    }

    FPaths::NormalizeFilename(In_Path);
    FString Path = FPlatformFileManager::Get().GetPlatformFile().ConvertToAbsolutePathForExternalAppForRead(*In_Path);

    if (!FPaths::FileExists(Path))
    {
        return false;
    }

    TArray<uint8> ByteArray;

    if (bUseLowLevel)
    {
        FPaths::MakePlatformFilename(Path);

        FILE* File = nullptr;

#ifdef _WIN64

        fopen_s(&File, TCHAR_TO_UTF8(*Path), "rb");

#else

        File = fopen(TCHAR_TO_UTF8(*Path), "rb");

#endif // _WIN64

        fseek(File, 0L, SEEK_END);
        const long FileSize = ftell(File);
        fseek(File, 0L, SEEK_SET);

        if (FileSize == 0)
        {
            fclose(File);
            return false;
        }

        uint8* Buffer = (uint8*)FMemory::Malloc(FileSize);
        fread(Buffer, sizeof(uint8), FileSize, File);
        
        ByteArray.SetNum(FileSize);
        FMemory::Memcpy(ByteArray.GetData(), Buffer, FileSize);
       
        fclose(File);
        free(Buffer);
        Buffer = nullptr;

        Out_Bytes = ByteArray;
        return true;
    }

    else
    {
        FFileHelper::LoadFileToArray(ByteArray, *Path, FILEREAD_AllowWrite);

        if (ByteArray.IsEmpty())
        {
            return false;
        }

        else
        {
            Out_Bytes = ByteArray;
            return true;
        }
    }
}

bool UExtendedVarsBPLibrary::Read_Text_From_Path(FString& Out_String, FString In_Path)
{
    if (In_Path.IsEmpty())
    {
        return false;
    }

    FPaths::MakeStandardFilename(In_Path);
    if (!FPaths::FileExists(In_Path))
    {

        return false;
    }

    return FFileHelper::LoadFileToString(Out_String, *In_Path);
}

#pragma endregion Read_Group

#pragma region Write_Group

bool UExtendedVarsBPLibrary::Write_File_To_Path(FString& Out_Code, TArray<uint8> In_Bytes, FString In_Path)
{
    if (In_Path.IsEmpty())
    {
		Out_Code = "Path is empty.";
        return false;
    }

    if (In_Bytes.IsEmpty() || !In_Bytes.GetData())
    {
		Out_Code = "Bytes are empty.";
        return false;
    }

	FPaths::MakeStandardFilename(In_Path);
    FString HeaderExtension;

    // JPG
    if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == "ffd8" && UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, (In_Bytes.Num() - 2), (In_Bytes.Num() - 1), false) == "ffd9")
    {
		HeaderExtension = "jpg";
    }

    // BMP
    else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == "424d")
    {
		HeaderExtension = "bmp";
    }

    // PNG
    else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 7, false) == "89504e470d0a1a0a")
    {
		HeaderExtension = "png";
    }

    // PDF
    else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 3, false) == "25504446")
    {
		HeaderExtension = "pdf";
    }

	const FString PathExtension = FPaths::GetExtension(In_Path, false);

	// If the path has an extension, we need to check if it matches the header extension.
    if (!PathExtension.IsEmpty() && PathExtension != HeaderExtension)
    {
		Out_Code = "Path extension does not match header extension.";
        return false;
    }

    In_Path += "." + HeaderExtension;
    FPaths::MakeStandardFilename(In_Path);
	bool bIsSuccess = FFileHelper::SaveArrayToFile(In_Bytes, *In_Path);

	Out_Code = bIsSuccess ? "File saved successfully." : "Failed to save file.";
    return bIsSuccess;
}

#pragma endregion Write_Group

#pragma region Bytes_Group

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

    FTCHARToUTF8 Source = FTCHARToUTF8(In_UTF8.GetCharArray().GetData());
    Array_Bytes.Append((uint8*)Source.Get(), Source.Length());

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

#pragma endregion Bytes_Group

#pragma region String_Group

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

    FTCHARToUTF8 Source = FTCHARToUTF8(SourceString.GetCharArray().GetData());
   
    TArray<uint8> Temp_Buffer;
    Temp_Buffer.Append((uint8*)Source.Get(), Source.Length());
    
    Out_Hex = bIsLower ? BytesToHexLower(Temp_Buffer.GetData(), Temp_Buffer.Num()) : BytesToHex(Temp_Buffer.GetData(), Temp_Buffer.Num());

    return true;
}

bool UExtendedVarsBPLibrary::String_To_Base64(FString& Out_Base64, FString SourceString, bool bUseUrl)
{
    if (SourceString.IsEmpty())
    {
        return false;
    }

    TArray<uint8> Temp_Buffer;
    FTCHARToUTF8 Source = FTCHARToUTF8(SourceString.GetCharArray().GetData());
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

FString UExtendedVarsBPLibrary::Beautify_Json(FString In_Json)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(In_Json);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        return FString();
    }

    FString BeautifiedJson;
    TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&BeautifiedJson);

    return FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer) ? BeautifiedJson : FString();
}

#pragma endregion String_Group

#pragma region Int_Group

uint8 UExtendedVarsBPLibrary::Int32_To_Byte(int32 In_Number)
{
    if (In_Number >= 0 && In_Number <= 255)
    {
        return In_Number;
    }

    else if (In_Number < 0)
    {
        return 0;
    }

    else if (In_Number > 255)
    {
        return 255;
    }

    return 0;
}

int32 UExtendedVarsBPLibrary::Int32_Place_Family(int32 TargetInteger)
{
    /*
    10 power integer character lenght minus 1 will equal its place family.
    For example to find 10's place family, 10^2(Char Lenght) - 1 = 10. 10's place family will 10.
    */
    
    return (int32)FMath::Pow(10.0, (double)((FString::FromInt(TargetInteger).Len()) - 1));
}

int32 UExtendedVarsBPLibrary::Int32_Truncate_To_Big(int32 TargetInteger)
{
    double Remainder;
    UKismetMathLibrary::FMod(TargetInteger, UExtendedVarsBPLibrary::Int32_Place_Family(TargetInteger), Remainder);

    if (Remainder != 0)
    {
        const FString IntString = FString::FromInt(TargetInteger);

        int32 PlaceDigit = FCString::Atoi(*(UKismetStringLibrary::GetCharacterArrayFromString(IntString)[0]));
        float PlaceFamily = (int32)FMath::Pow(10.0, (double)((IntString.Len()) - 1));

        return PlaceFamily * (PlaceDigit + 1);
    }

    else
    {
        return TargetInteger;
    }
}

int32 UExtendedVarsBPLibrary::Int32_Truncate_To_Small(int32 TargetInteger)
{
    double Remainder;
    UKismetMathLibrary::FMod(TargetInteger, 10, Remainder);
    
    if (Remainder != 0)
    {
        const FString IntString = FString::FromInt(TargetInteger);

        int32 LastDigit = FCString::Atoi(*(UKismetStringLibrary::GetCharacterArrayFromString(IntString).Last()));
        int32 Difference = 10 - LastDigit;

        return TargetInteger + Difference;
    }

    else
    {
        return TargetInteger;
    }
}

#pragma endregion Int_Group

#pragma region Float_Group

double UExtendedVarsBPLibrary::Float_Precision(double TargetFloat, int32 Precision)
{
    int32 PrecisionFixer = pow(10, Precision);
    return floor(PrecisionFixer * TargetFloat) / PrecisionFixer;
}

int32 UExtendedVarsBPLibrary::Float_Fraction_Count(float TargetFloat)
{
    FString FractionString;
    (FString::SanitizeFloat(UKismetMathLibrary::Fraction(TargetFloat))).Split(TEXT("."), NULL, &FractionString, ESearchCase::IgnoreCase, ESearchDir::FromStart);
   
    return FractionString.Len();
}

float UExtendedVarsBPLibrary::Float_Round_Next(float TargetFloat, double Decimal)
{
    int32 FractionCount = UExtendedVarsBPLibrary::Float_Fraction_Count(TargetFloat);
    
    if (FractionCount > Decimal)
    {
        return (FMath::TruncToInt(TargetFloat * FMath::Pow(10.0, Decimal)) + 1) / FMath::Pow(10.0, Decimal);
    }

    else
    {
        return TargetFloat;
    }
}

#pragma endregion Float_Group

#pragma region Time_Group

FDateTime UExtendedVarsBPLibrary::Increment_Date(FDateTime Start, int32 In_Years, int32 In_Months, int32 In_Days)
{
    const FDateTime TargetTime = (Start == FDateTime()) ? FDateTime::Now() : Start;

    int32 Year = TargetTime.GetYear() + In_Years;
    int32 Month = TargetTime.GetMonth();
    int32 Day = TargetTime.GetDay();

    // Convert month to zero-based, add In_Months, then carry over to Year
    const int32 ZeroBasedMonth = (Month - 1) + In_Months;
    Year += ZeroBasedMonth / 12;                                            // carry over
    Month = (ZeroBasedMonth % 12) + 1;                                      // back to 1..12

    // Clamp Day if it exceeds the new month's length. e.g., from Jan 31 to a month with only 30 days
    const int32 DaysInNewMonth = FDateTime::DaysInMonth(Year, Month);
    if (Day > DaysInNewMonth)
    {
        Day = DaysInNewMonth;
    }

    FDateTime Intermediate(Year, Month, Day, TargetTime.GetHour(), TargetTime.GetMinute(), TargetTime.GetSecond(), TargetTime.GetMillisecond());
    FTimespan AdditionalDays(In_Days, 0, 0, 0);
    
    return Intermediate + AdditionalDays;
}

bool UExtendedVarsBPLibrary::GetDateTimeWithZone(FString& Out_Print, FString& Out_Zone, FString& Out_Geo, FString& Out_Day, FString& Out_Month, bool bUseShort)
{
#ifdef _WIN64
    FDateTime Now = FDateTime::UtcNow();
    EDayOfWeek DayOfWeek = Now.GetDayOfWeek();
    EMonthOfYear MonthOfYear = Now.GetMonthOfYear();

    switch (DayOfWeek)
    {
        case EDayOfWeek::Monday:
            Out_Day = bUseShort ? "Mon" : "Monday";
            break;
        case EDayOfWeek::Tuesday:
            Out_Day = bUseShort ? "Tue" : "Tuesday";
            break;
        case EDayOfWeek::Wednesday:
            Out_Day = bUseShort ? "Wed" : "Wednesday";
            break;
        case EDayOfWeek::Thursday:
            Out_Day = bUseShort ? "Thu" : "Thursday";
            break;
        case EDayOfWeek::Friday:
            Out_Day = bUseShort ? "Fri" : "Friday";
            break;
        case EDayOfWeek::Saturday:
            Out_Day = bUseShort ? "Sat" : "Saturday";
            break;
        case EDayOfWeek::Sunday:
            Out_Day = bUseShort ? "Sun" : "Sunday";
            break;
    }

    switch (MonthOfYear)
    {
        case EMonthOfYear::January:
            Out_Month = bUseShort ? "Jan" : "January";
            break;
        case EMonthOfYear::February:
            Out_Month = bUseShort ? "Feb" : "February";
            break;
        case EMonthOfYear::March:
            Out_Month = bUseShort ? "Mar" : "March";
            break;
        case EMonthOfYear::April:
            Out_Month = bUseShort ? "Apr" : "April";
            break;
        case EMonthOfYear::May:
            Out_Month = bUseShort ? "May" : "May";
            break;
        case EMonthOfYear::June:
            Out_Month = bUseShort ? "Jun" : "June";
            break;
        case EMonthOfYear::July:
            Out_Month = bUseShort ? "Jul" : "July";
            break;
        case EMonthOfYear::August:
            Out_Month = bUseShort ? "Aug" : "August";
            break;
        case EMonthOfYear::September:
            Out_Month = bUseShort ? "Sep" : "September";
            break;
        case EMonthOfYear::October:
            Out_Month = bUseShort ? "Oct" : "October";
            break;
        case EMonthOfYear::November:
            Out_Month = bUseShort ? "Nov" : "November";
            break;
        case EMonthOfYear::December:
            Out_Month = bUseShort ? "Dec" : "December";
            break;
    }

    std::stringstream TimeStream;
    const std::chrono::zoned_time cur_time
    {
        std::chrono::current_zone(),
        std::chrono::system_clock::now()
    };

    TimeStream << cur_time;

    FString ChronoString = UTF8_TO_TCHAR(TimeStream.str().c_str());
    TArray<FString> Array_Sections = UKismetStringLibrary::ParseIntoArray(ChronoString, " ");
    Out_Zone = Array_Sections.Last();

    std::stringstream ZoneStream;
    ZoneStream << std::chrono::current_zone()->name();
    Out_Geo = ZoneStream.str().c_str();

    Out_Print = FString::Printf(TEXT("%s, %d %s %d %d:%d:%d %s %s"), *Out_Day, Now.GetDay(), *Out_Month, Now.GetYear(), Now.GetHour(), Now.GetMinute(), Now.GetSecond(), *Out_Zone, *Out_Geo);

    return true;

#else
    return false;
#endif
}

#pragma endregion Time_Group

#pragma region Render_Group

EGammaSpaceBp UExtendedVarsBPLibrary::ConvertGammaSpaceBp(EGammaSpace InGammaSpace)
{
    switch (InGammaSpace)
    {
        case EGammaSpace::Pow22:
		    return EGammaSpaceBp::Pow22;

        case EGammaSpace::Linear:
		    return EGammaSpaceBp::Linear;

        case EGammaSpace::sRGB:
		    return EGammaSpaceBp::sRGB;

        default:
            return EGammaSpaceBp::Invalid;
    }
}

EGammaSpace UExtendedVarsBPLibrary::ConvertGammaSpace(EGammaSpaceBp InGammaSpaceBp)
{
    switch (InGammaSpaceBp)
    {
    case EGammaSpaceBp::Pow22:
        return EGammaSpace::Pow22;

    case EGammaSpaceBp::Linear:
        return EGammaSpace::Linear;

    case EGammaSpaceBp::sRGB:
        return EGammaSpace::sRGB;

    default:
        return EGammaSpace::Invalid;
    }
}

UTextureRenderTarget2D* UExtendedVarsBPLibrary::Widget_To_TRT2D(FString& OutCode, UUserWidget* InWidget, FVector2D InDrawSize)
{
    if (IsValid(InWidget) == false)
    {
        OutCode = "Source widget is not valid";
        return nullptr;
    }

    UTextureRenderTarget2D* TextureTarget = FWidgetRenderer::CreateTargetFor(InDrawSize, TextureFilter::TF_Default, false);
    TextureTarget->RenderTargetFormat = RTF_RGBA8;

    FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(true);
    WidgetRenderer->DrawWidget(TextureTarget, InWidget->TakeWidget(), InDrawSize, 0, false);

    if (IsValid(TextureTarget) == true)
    {
        return TextureTarget;
    }

    else
    {
        OutCode = "Unable to create Texture Render Target 2D";
        return nullptr;
    }
}

void UExtendedVarsBPLibrary::TRT2D_To_T2D(FDelegateTexture2D Delegate_T2D, UTextureRenderTarget2D* In_TRT_2D)
{
    if (!IsValid(In_TRT_2D))
    {
        AsyncTask(ENamedThreads::GameThread, [Delegate_T2D]()
            {
                Delegate_T2D.ExecuteIfBound(false, "In_TRT_2D is not valid.", nullptr);
            }
        );

        return;
    }

    TEnumAsByte<ETextureRenderTargetFormat> RT_Format = In_TRT_2D->RenderTargetFormat;

    if (RT_Format != RTF_RGBA8 || RT_Format != RTF_RGBA8_SRGB)
    {
        if (!IsValid(In_TRT_2D))
        {
            AsyncTask(ENamedThreads::GameThread, [Delegate_T2D]()
                {
                    Delegate_T2D.ExecuteIfBound(false, "Render Target format is not correct. Please use RTF_RGBA8 or RTF_RGBA8_SRGB.", nullptr);
                }
            );

            return;
        }
    }

    ENQUEUE_RENDER_COMMAND(Convert_TRT2D_To_T2D)([Delegate_T2D, In_TRT_2D, RT_Format](FRHICommandListImmediate& RHICmdList)
        {
            // Get Texture Render Target 2D Properties.
            const int32 Size_X = In_TRT_2D->SizeX;
            const int32 Size_Y = In_TRT_2D->SizeY;
			const size_t Lenght = Size_X * Size_Y * sizeof(FColor);

            // Create Copy Descriptions.
            FRHITextureCreateDesc Desc =
                FRHITextureCreateDesc::Create2D(TEXT("CopiedTextureDescription"))
                .SetExtent(Size_X, Size_Y)
                .SetFormat(PF_B8G8R8A8)
                .SetInitialState(ERHIAccess::CopySrc);

            // Copy Texture Render Target 2D.
            FTextureRHIRef CopiedTextureResource = RHICreateTexture(Desc);
            FRHICopyTextureInfo CopyTextureInfo;
            RHICmdList.CopyTexture(In_TRT_2D->GetResource()->GetTextureRHI(), CopiedTextureResource, CopyTextureInfo);

            FRHILockTextureArgs LockArgs = FRHILockTextureArgs::Lock2D(CopiedTextureResource, 0, EResourceLockMode::RLM_ReadOnly, false, true);
            FRHILockTextureResult LockResult = RHICmdList.LockTexture(LockArgs);
			void* TextureData = LockResult.Data;
            RHICmdList.UnlockTexture(LockArgs);

            AsyncTask(ENamedThreads::GameThread, [Delegate_T2D, Size_X, Size_Y, Lenght, TextureData, RT_Format]()
                {
                    // Create Texture
                    UTexture2D* Texture = UTexture2D::CreateTransient(Size_X, Size_Y, PF_B8G8R8A8);
                    Texture->NeverStream = true;
                    Texture->SRGB = RT_Format == RTF_RGBA8_SRGB ? true : false;

                    FTexture2DMipMap& Texture2D_Mip = Texture->GetPlatformData()->Mips[0];
                    void* Texture2D_Data = Texture2D_Mip.BulkData.Lock(LOCK_READ_WRITE);
                    FMemory::Memcpy(Texture2D_Data, TextureData, Lenght);
                    Texture2D_Mip.BulkData.Unlock();
                    Texture->UpdateResource();
                    
                    Delegate_T2D.ExecuteIfBound(true, "Sucessfull.", Texture);
                }
            );

            return;
        }
    );
}

bool UExtendedVarsBPLibrary::Export_ImageBuf_File(FString& Out_Path, TArray<uint8> ImageBuffer, FVector2D ImageRes, FString In_Path, EGammaSpaceBp GammaSpaceBp, EImageExtensions Extension)
{
    if (ImageBuffer.IsEmpty())
    {
        return false;
    }

    if (ImageRes.X <= 0 || ImageRes.Y <= 0)
    {
        return false;
    }

    if (In_Path.IsEmpty())
    {
        return false;
    }

    FPaths::MakeStandardFilename(In_Path);

    switch (Extension)
    {
        case EImageExtensions::Ext_None:

            return false;

        case EImageExtensions::Ext_BMP:
        {
            Out_Path = In_Path + ".bmp";

            TArray<FColor> Array_Colors;
            Array_Colors.SetNum(ImageRes.X * ImageRes.Y);
            FMemory::Memcpy(Array_Colors.GetData(), reinterpret_cast<FColor*>(ImageBuffer.GetData()), ImageBuffer.Num());

            return FFileHelper::CreateBitmap(*Out_Path, ImageRes.X, ImageRes.Y, Array_Colors.GetData(), NULL, &IFileManager::Get(), NULL, true);
        }

        case EImageExtensions::EXT_JPEG:
        {
            FImageView ImageView;
            ImageView.Format = ERawImageFormat::BGRA8;
            ImageView.SizeX = ImageRes.X;
            ImageView.SizeY = ImageRes.Y;
            ImageView.GammaSpace = UExtendedVarsBPLibrary::ConvertGammaSpace(GammaSpaceBp);
            ImageView.NumSlices = 1;
            ImageView.RawData = malloc(ImageBuffer.Num());
            FMemory::Memcpy(ImageView.RawData, (uint8*)ImageBuffer.GetData(), ImageBuffer.Num());

            Out_Path = In_Path + ".jpg";
            return FImageUtils::SaveImageByExtension(*Out_Path, ImageView, 0);
        }

        case EImageExtensions::EXT_PNG:
        {
            FImageView ImageView;
            ImageView.Format = ERawImageFormat::BGRA8;
            ImageView.SizeX = ImageRes.X;
            ImageView.SizeY = ImageRes.Y;
            ImageView.GammaSpace = UExtendedVarsBPLibrary::ConvertGammaSpace(GammaSpaceBp);
            ImageView.NumSlices = 1;
            ImageView.RawData = malloc(ImageBuffer.Num());
            FMemory::Memcpy(ImageView.RawData, (uint8*)ImageBuffer.GetData(), ImageBuffer.Num());

            Out_Path = In_Path + ".png";
            return FImageUtils::SaveImageByExtension(*Out_Path, ImageView, 0);
        }
       
        default:
            return false;
    }
}

bool UExtendedVarsBPLibrary::Export_T2D_Colors(TArray<FColor>& Out_Array, UTexture2D* Texture)
{
    if (IsValid(Texture) == false)
    {
        return false;
    }

    if (Texture->GetPixelFormat() != EPixelFormat::PF_B8G8R8A8)
    {
        return false;
    }

    if (Texture->LODGroup != TextureGroup::TEXTUREGROUP_UI)
    {
        return false;
    }

    TextureCompressionSettings OldCompressionSettings = Texture->CompressionSettings;
    Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    Texture->UpdateResource();

    int32 Texture_Width = Texture->GetSizeX();
    int32 Texture_Height = Texture->GetSizeY();
    size_t Lenght = static_cast<size_t>(Texture_Width * Texture_Height * sizeof(FColor));

    FTexture2DMipMap& Texture_Mip = Texture->GetPlatformData()->Mips[0];
    void* Texture_Data = Texture_Mip.BulkData.Lock(LOCK_READ_WRITE);

    if (!Texture_Data)
    {
        return false;
    }

    Out_Array.SetNum(Texture_Width * Texture_Height);
    FMemory::Memcpy(Out_Array.GetData(), static_cast<FColor*>(Texture_Data), Lenght);

    Texture_Mip.BulkData.Unlock();

    Texture->CompressionSettings = OldCompressionSettings;
    Texture->UpdateResource();

    return true;
}

bool UExtendedVarsBPLibrary::Export_T2D_Bytes(TArray<uint8>& Out_Array, FString& Out_Code, UTexture2D* Texture)
{
    if (IsValid(Texture) == false)
    {
        Out_Code = "Texture not valid.";
        return false;
    }

    if (Texture->GetPixelFormat() != EPixelFormat::PF_B8G8R8A8)
    {
        Out_Code = "Texture pixel format should be PF_B8G8R8A8.";
        return false;
    }
    
    if (Texture->LODGroup != TextureGroup::TEXTUREGROUP_UI)
    {
        Out_Code = "Texture LOD group should be UI.";
        return false;
    }

    TextureCompressionSettings OldCompressionSettings = Texture->CompressionSettings;
    Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    Texture->UpdateResource();

    int32 Texture_Width = Texture->GetSizeX();
    int32 Texture_Height = Texture->GetSizeY();
    size_t Lenght = static_cast<size_t>(Texture_Width * Texture_Height * sizeof(FColor));

    FTexture2DMipMap& Texture_Mip = Texture->GetPlatformData()->Mips[0];
    const void* Texture_Data = Texture_Mip.BulkData.LockReadOnly();

    if (!Texture_Data)
    {
        Texture_Mip.BulkData.Unlock();

        Out_Code = "Texture buffer is not valid.";
        return false;
    }

    Out_Array.SetNum(Lenght);
    FMemory::Memcpy(Out_Array.GetData(), (uint8*)Texture_Data, Lenght);

    Texture_Mip.BulkData.Unlock();

    Texture->CompressionSettings = OldCompressionSettings;
    Texture->UpdateResource();

    return true;
}

void UExtendedVarsBPLibrary::Export_Texture_Bytes_RT(FDelegateImageBuffer DelegateImageBuffer, UTexture* TargetTexture)
{    
    ENQUEUE_RENDER_COMMAND(Cmd_Export_Texture)([TargetTexture, DelegateImageBuffer](FRHICommandListImmediate& RHICmdList)
        {
            if (!IsValid(TargetTexture))
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "Target texture is not valid.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }
            
            // Initialize Variables.
            EPixelFormat PixelFormat = PF_Unknown;
			bool bIsTexture2D = false;

            // Get Texture Resource and Pixel Format.
            if (UTexture2D* T2D = Cast<UTexture2D>(TargetTexture))
            {
                if (T2D->GetPixelFormat() != EPixelFormat::PF_B8G8R8A8)
                {
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(false, "Target Texture2D format is not B8G8R8A8.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                        }
                    );

                    return;
                }

                if (T2D->CompressionSettings.GetIntValue() != 5 && T2D->CompressionSettings.GetIntValue() != 7)
                {
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(false, "Texture compression setting is not correct.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                        }
                    );

                    return;
                }

                PixelFormat = PF_B8G8R8A8;
                bIsTexture2D = true;
            }

            if (UTextureRenderTarget2D* TRT_2D = Cast<UTextureRenderTarget2D>(TargetTexture))
            {
                if (TRT_2D->RenderTargetFormat.GetValue() != ETextureRenderTargetFormat::RTF_RGBA8 && TRT_2D->RenderTargetFormat.GetValue() != ETextureRenderTargetFormat::RTF_RGBA8_SRGB)
                {
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(false, "Target TextureRenderTarget2D format is not RTF_RGBA8 or RTF_RGBA_SRGB.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                        }
                    );

                    return;
                }

                PixelFormat = PF_B8G8R8A8;
                bIsTexture2D = false;
            }

            if (UMediaTexture* MT_2D = Cast<UMediaTexture>(TargetTexture))
            {
                PixelFormat = PF_R8G8B8A8;
                bIsTexture2D = false;
            }

            // Get Image Properties.
            FTextureResource* TextureResource = TargetTexture->GetResource();
			const EGammaSpace TargetGammaSpace = TargetTexture->GetGammaSpace();
            const EGammaSpaceBp GammaSpaceBp = UExtendedVarsBPLibrary::ConvertGammaSpaceBp(TargetGammaSpace);
            const int32 Size_X = TextureResource->GetSizeX();
            const int32 Size_Y = TextureResource->GetSizeY();

            // Create Copy Texture Descriptions
            FRHITextureCreateDesc Desc = FRHITextureCreateDesc::Create2D(TEXT("CopiedTextureDescription"))
                .SetExtent(Size_X, Size_Y)
                .SetFormat(PixelFormat)
                .SetInitialState(ERHIAccess::CopySrc);
            FTextureRHIRef CopiedTextureRHI = RHICreateTexture(Desc);

            FTextureRHIRef TargetTextureRHI = nullptr;

            if (bIsTexture2D)
            {
				TargetTextureRHI = TextureResource->GetTexture2DRHI();
            }

            else
            {
				TargetTextureRHI = TextureResource->GetTextureRHI();
            }

            FRHICopyTextureInfo CopyTextureInfo;
            RHICmdList.CopyTexture(TargetTextureRHI, CopiedTextureRHI, CopyTextureInfo);

            // We need tightly packed BGRA8 in FColor.
            TArray<FColor> SurfaceColor;
            FReadSurfaceDataFlags Flags(RCM_UNorm, CubeFace_MAX);
            RHICmdList.ReadSurfaceData(CopiedTextureRHI, FIntRect(0, 0, Size_X, Size_Y), SurfaceColor, Flags);

            if (!SurfaceColor.GetData())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "Texture buffer is not valid.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );
                
                return;
            }

            AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [DelegateImageBuffer, SurfaceColor, Size_X, Size_Y, GammaSpaceBp]()
                {
                    TArray<uint8> BufferArray;
                    const size_t BufferSize = SurfaceColor.Num() * sizeof(FColor);
                    BufferArray.SetNum(BufferSize);
                    FMemory::Memcpy(BufferArray.GetData(), (uint8*)SurfaceColor.GetData(), BufferSize);
                    
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, BufferArray, Size_X, Size_Y, GammaSpaceBp]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(true, "Export texture as raw buffer is successful.", BufferArray, FVector2D(Size_X, Size_Y), GammaSpaceBp);
                        }
                    );
                }
            );
        }
    );
}

void UExtendedVarsBPLibrary::Encode_Api_Old(FDelegateImageBuffer DelegateImageBuffer, TArray<uint8> Texture_Data, FVector2D ImageRes, EImageExtensions CompressFormat)
{    
    ENQUEUE_RENDER_COMMAND(Cmd_Export_Texture)([DelegateImageBuffer, Texture_Data, ImageRes, CompressFormat](FRHICommandListImmediate& RHICmdList)
        {
            IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

            bool bUseRgba = false;
            FString FormatString = "";
            EImageFormat ImageFormat = EImageFormat::Invalid;

            switch (CompressFormat)
            {
                case EImageExtensions::Ext_None:
                {
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(false, "You have to define a proper extension for encoding like JPG/PNG/BMP.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                        }
                    );

                    return;
                }

                case EImageExtensions::Ext_BMP:

                    bUseRgba = false;
                    FormatString = "BMP";
                    ImageFormat = EImageFormat::BMP;

                    break;

                case EImageExtensions::EXT_JPEG:

                    bUseRgba = true;
                    FormatString = "JPG";
                    ImageFormat = EImageFormat::JPEG;

                    break;

                case EImageExtensions::EXT_PNG:

                    bUseRgba = true;
                    FormatString = "PNG";
                    ImageFormat = EImageFormat::PNG;

                    break;

                default:

                    bUseRgba = true;
                    FormatString = "PNG";
                    ImageFormat = EImageFormat::PNG;

                    break;
            }

            TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

            if (!ImageWrapper.IsValid())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "Function couldn't initialize ImageWrapper.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            TArray<FColor> Array_Colors_RGBA;
			const size_t Lenght = Texture_Data.Num();

            if (bUseRgba)
            {
                const size_t Total = static_cast<size_t>(ImageRes.X * ImageRes.Y);

                TArray<FColor> Array_Colors_BGRA;
                Array_Colors_BGRA.SetNum(Total);
                FMemory::Memcpy(Array_Colors_BGRA.GetData(), Texture_Data.GetData(), Lenght);

                // Convert BGRA to RGBA
                Array_Colors_RGBA = Array_Colors_BGRA;
                for (int32 i = 0; i < Total; i++)
                {
                    Array_Colors_RGBA[i].R = Array_Colors_BGRA[i].B;
                    Array_Colors_RGBA[i].B = Array_Colors_BGRA[i].R;
                }
            }

			void* RawData = bUseRgba ? (void*)Array_Colors_RGBA.GetData() : (void*)Texture_Data.GetData();

            if (!ImageWrapper->SetRaw(RawData, Lenght, ImageRes.X, ImageRes.Y, bUseRgba ? ERGBFormat::RGBA : ERGBFormat::BGRA, 8))
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, FormatString]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, FormatString + " compression is NOT successfull for target texture with old api. \"ImageWrapperModule->SetRaw\" failed.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }
			
            TArray64<uint8> Compressed_64 = ImageWrapper->GetCompressed(100);
			const size_t Compressed_64_Size = Compressed_64.Num();
            
            if (Compressed_64_Size <= 0)
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, FormatString]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "There is no compressed bytes.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            if (Compressed_64_Size > TNumericLimits<int32>::Max())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, FormatString]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "Image size exceeds TArray<uint> limits for this extension : " + FormatString, TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            TArray<uint8> Compressed_86;
			Compressed_86.SetNumUninitialized(static_cast<int32>(Compressed_64_Size));
			FMemory::Memcpy(Compressed_86.GetData(), Compressed_64.GetData(), Compressed_64_Size);

            if (!Compressed_86.GetData())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, FormatString]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, FormatString + " compression is NOT successfull for target texture with old api. \"CompressedData\" is empty.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, Compressed_86, FormatString, ImageRes]()
                {
                    DelegateImageBuffer.ExecuteIfBound(true, FormatString + " compression is successfull for target texture with old api.", Compressed_86, ImageRes, EGammaSpaceBp::Invalid);
                }
            );
        }
    );
}

void UExtendedVarsBPLibrary::Encode_Api_New(FDelegateImageBuffer DelegateImageBuffer, TArray<uint8> Texture_Data, FVector2D ImageRes, EImageExtensions CompressFormat, EGammaSpaceBp GammaSpaceBp)
{
    ENQUEUE_RENDER_COMMAND(Cmd_Export_Texture)([DelegateImageBuffer, Texture_Data, ImageRes, CompressFormat, GammaSpaceBp](FRHICommandListImmediate& RHICmdList)
        {
            IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

            const size_t Lenght = Texture_Data.Num();

            FImageView ImageView;
            ImageView.Format = ERawImageFormat::BGRA8;
            ImageView.SizeX = ImageRes.X;
            ImageView.SizeY = ImageRes.Y;
            ImageView.GammaSpace = UExtendedVarsBPLibrary::ConvertGammaSpace(GammaSpaceBp);
            ImageView.NumSlices = 1;
            ImageView.RawData = malloc(Lenght);
            FMemory::Memcpy(ImageView.RawData, Texture_Data.GetData(), Lenght);

            FString FormatString = "";
            EImageFormat ImageFormat = EImageFormat::Invalid;

            switch (CompressFormat)
            {
                case EImageExtensions::Ext_None:
                {
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(false, "You have to define a proper extension for encoding like JPG/PNG/BMP.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                        }
                    );

                    return;
                }

                case EImageExtensions::Ext_BMP:

                    FormatString = "BMP";
                    ImageFormat = EImageFormat::BMP;

                    break;

                case EImageExtensions::EXT_JPEG:

                    FormatString = "JPG";
                    ImageFormat = EImageFormat::JPEG;

                    break;

                case EImageExtensions::EXT_PNG:

                    FormatString = "PNG";
                    ImageFormat = EImageFormat::PNG;

                    break;

                default:

                    FormatString = "PNG";
                    ImageFormat = EImageFormat::PNG;

                    break;
            }

            TArray64<uint8> Compressed_64;
            if (!ImageWrapperModule.CompressImage(Compressed_64, ImageFormat, ImageView, (int32)EImageCompressionQuality::Max))
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, FormatString]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, FormatString + " compression is NOT successfull for target texture with new api. \"ImageWrapperModule.CompressImage\" failed.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            if (Compressed_64.IsEmpty())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, FormatString]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, FormatString + " compression is NOT successfull for target texture with new api. \"CompressedData\" is empty.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            const size_t Compressed_64_Size = Compressed_64.Num();

            if (Compressed_64_Size > TNumericLimits<int32>::Max())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, FormatString]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "Image size exceeds TArray<uint> limits for this extension : " + FormatString, TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            TArray<uint8> Compressed_86;
            Compressed_86.SetNumUninitialized(static_cast<int32>(Compressed_64_Size));
            FMemory::Memcpy(Compressed_86.GetData(), Compressed_64.GetData(), Compressed_64_Size);

            if (!Compressed_86.GetData())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, FormatString]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, FormatString + " compression is NOT successfull for target texture with old api. \"CompressedData\" is empty.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, Compressed_86, FormatString, ImageRes]()
                {
                    DelegateImageBuffer.ExecuteIfBound(true, FormatString + " compression is successfull for target texture with old api.", Compressed_86, ImageRes, EGammaSpaceBp::Invalid);
                }
            );
        }
    );
}

void UExtendedVarsBPLibrary::DecompressImage(FDelegateImageBuffer DelegateImageBuffer, TArray<uint8> In_Bytes)
{
    ENQUEUE_RENDER_COMMAND(Cmd_Decompress_Image)([DelegateImageBuffer, In_Bytes](FRHICommandList& RHICmdList)
        {
            if (In_Bytes.IsEmpty())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "There is no byte to decompres.", TArray<uint8>(), FVector2D(), EGammaSpaceBp::Invalid);
                    }
                );

                return;
            }

            IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
            
            FImage DecompressedImage;
            ImageWrapperModule.DecompressImage(In_Bytes.GetData(), In_Bytes.Num(), DecompressedImage);

            size_t Lenght = static_cast<size_t>(DecompressedImage.SizeX * DecompressedImage.SizeY * sizeof(FColor));

            TArray<uint8> DecompressedBuffer;
            DecompressedBuffer.SetNum(Lenght);
            FMemory::Memcpy(DecompressedBuffer.GetData(), DecompressedImage.AsBGRA8().GetData(), Lenght);

            FVector2D ImageSize = FVector2D(DecompressedImage.SizeX, DecompressedImage.SizeY);

            AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, DecompressedBuffer, ImageSize]()
                {
                    DelegateImageBuffer.ExecuteIfBound(true, "Decompress is successful.", DecompressedBuffer, ImageSize, EGammaSpaceBp::Invalid);
                }
            );
        }
    );
}

bool UExtendedVarsBPLibrary::Import_T2D_Bytes(UTexture2D*& Out_Texture, TArray<uint8> In_Bytes, bool bUseSrgb)
{
    if (In_Bytes.Num() == 0)
    {
        return false;
    }

    UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(In_Bytes);
    Texture->SRGB = bUseSrgb;

    if (IsValid(Texture) == true)
    {
        Out_Texture = Texture;
        return true;
    }

    else
    {
        return false;
    }
}

bool UExtendedVarsBPLibrary::Import_T2D_Bytes_LowLevel(UTexture2D*& Out_Texture, TArray<uint8> In_Bytes, FVector2D Size, bool bUseSrgb)
{
    if (In_Bytes.Num() == 0)
    {
        return false;
    }

    UTexture2D* Texture = UTexture2D::CreateTransient(Size.X, Size.Y, PF_B8G8R8A8);
    Texture->SRGB = bUseSrgb;

    FTexture2DMipMap& Texture_Mip = Texture->GetPlatformData()->Mips[0];
    void* Texture_Data = Texture_Mip.BulkData.Lock(LOCK_READ_WRITE);

    FMemory::Memcpy(Texture_Data, In_Bytes.GetData(), In_Bytes.Num());
    Texture_Mip.BulkData.Unlock();
    Texture->UpdateResource();

    Out_Texture = Texture;

    return true;
}

double UExtendedVarsBPLibrary::GetPaintRatio(UCanvasRenderTarget2D* InCRT, FLinearColor InWantedColor, uint8 Tolerance, bool bLogEachColor)
{
    if (!IsValid(InCRT))
    {
        return (double)-1;
    }

	UWorld* CurrentWorld = GEngine->GetCurrentPlayWorld();

    if (!IsValid(CurrentWorld))
    {
        return (double)-1;
    }

    TArray<FColor> Array_Colors;
    UKismetRenderingLibrary::ReadRenderTarget(CurrentWorld, InCRT, Array_Colors, false);
    const size_t TotalColors = Array_Colors.Num();

    if (TotalColors == 0)
    {
        return (double)-1;
    }

	// Each thread will process this many colors. (This value equals to 256x256 pixels)
    const size_t BlockSize = 65536;

	// Calculate the number of blocks we need to process all colors.
    const size_t NumBlocks = (TotalColors + BlockSize - 1) / BlockSize;
    std::atomic<int64> GlobalCount = { 0 };

    const FColor TargetColor = InWantedColor.ToFColor(false);

    ParallelFor(NumBlocks, [&](int32 BlockIdx)
        {
            const int32 Start = BlockIdx * BlockSize;
            const int32 End = FMath::Min(Start + BlockSize, TotalColors);

            int64 LocalCount = 0;
            // Tight loop over our slice; compare exact FColor equality.
            for (int32 Index = Start; Index < End; ++Index)
            {
				const FColor& Each_Color = Array_Colors[Index];

                if (bLogEachColor)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Color : %s"), *Each_Color.ToString());
                }

                const uint8_t R = Each_Color.R;
				const uint8_t G = Each_Color.G;
				const uint8_t B = Each_Color.B;

				bool bDoesMatch = FMath::IsNearlyEqual(R, TargetColor.R, (double)Tolerance) && FMath::IsNearlyEqual(G, TargetColor.G, (double)Tolerance) && FMath::IsNearlyEqual(B, TargetColor.B, (double)Tolerance);

                if (bDoesMatch)
                {
                    ++LocalCount;
                }
            }

            // Single atomic add per block keeps contention tiny.
            if (LocalCount)
            {
                GlobalCount.fetch_add(LocalCount, std::memory_order_relaxed);
            }
        }
    );

    const double Ratio = (double)GlobalCount / (double)TotalColors;
	return Ratio;
}

#pragma endregion Render_Group

#pragma region Logs

void UExtendedVarsBPLibrary::LogString(int32 InLogLevel, FString Log)
{
    switch (InLogLevel)
    {
        case 0:

            UE_LOG(LogTemp, Display, TEXT("%s"), *FString(Log));
            return;
    
        case 1:
        
            UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(Log));
            return;
    
        case 2:

            UE_LOG(LogTemp, Error, TEXT("%s"), *FString(Log));
            return;

        default:
            break;
    }
}

#pragma endregion Logs

#pragma region Saves

bool UExtendedVarsBPLibrary::GetVariableByName(FVariableContainer& Out_Container, UObject* In_Parent, FName In_Name)
{
    if (!IsValid(In_Parent))
    {
        return false;
    }

    UClass* Class = In_Parent->GetClass();

    if (!IsValid(Class))
    {
        return false;
    }

    if (In_Name.IsNone() || !In_Name.IsValid())
    {
        return false;
    }

    const FProperty* FoundProperty = Class->FindPropertyByName(In_Name);

    if (!FoundProperty)
    {
        return false;
    }

    void* Buffer = FoundProperty->ContainerPtrToValuePtr<void>(In_Parent);

    if (!Buffer)
    {
        return false;
    }

    FVariableContainer TempContainer;
    TempContainer.Name = In_Name;
    TempContainer.Parent = In_Parent;
    TempContainer.RecordTime = FDateTime::Now();

    if (const FStrProperty* StrProp = CastField<FStrProperty>(FoundProperty))
    {
        TempContainer.Serialized = StrProp->GetPropertyValue(Buffer);
        TempContainer.VariableType = "FString";
        Out_Container = TempContainer;
        return true;
    }

    else if (const FTextProperty* TextProp = CastField<FTextProperty>(FoundProperty))
    {
        TempContainer.Serialized = TextProp->GetPropertyValue(Buffer).ToString();
        TempContainer.VariableType = "FText";
        Out_Container = TempContainer;
        return true;
    }

    else if (const FIntProperty* IntProp = CastField<FIntProperty>(FoundProperty))
    {
        TempContainer.Serialized = FString::FromInt(IntProp->GetPropertyValue(Buffer));
        TempContainer.VariableType = "int32";
        Out_Container = TempContainer;
        return true;
    }

    else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(FoundProperty))
    {
        TempContainer.Serialized = FString::SanitizeFloat(FloatProp->GetPropertyValue(Buffer));
        TempContainer.VariableType = "Float";
        Out_Container = TempContainer;
        return true;
    }

    else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(FoundProperty))
    {
        TempContainer.Serialized = BoolProp->GetPropertyValue(Buffer) ? "True" : "False";
        TempContainer.VariableType = "Boolean";
        Out_Container = TempContainer;
        return true;
    }

    else if (const FStructProperty* StructProp = CastField<FStructProperty>(FoundProperty))
    {
        const FString VariableType = StructProp->Struct->GetName();
        TempContainer.VariableType = VariableType;

        if (VariableType == TEXT("Vector"))
        {
            FVector* Value = reinterpret_cast<FVector*>(Buffer);
            TempContainer.Serialized = Value->ToString();
        }

        else if (VariableType == TEXT("Rotator"))
        {
            FRotator* Value = reinterpret_cast<FRotator*>(Buffer);
            TempContainer.Serialized = Value->ToString();
        }

        else if (VariableType == TEXT("Transform"))
        {
            FTransform* Value = reinterpret_cast<FTransform*>(Buffer);
            TempContainer.Serialized = Value->ToString();
        }

        Out_Container = TempContainer;
        return true;
    }

    else
    {
        return false;
    }
}

bool UExtendedVarsBPLibrary::GetAllVariables(FVariablePool& Out_Containers, UObject* In_Parent, bool bIncludeEditor, bool bIncludeNative, bool bIncludeRoot)
{
    if (!IsValid(In_Parent))
    {
        return false;
    }

    UClass* Class = In_Parent->GetClass();

    if (!IsValid(Class))
    {
        return false;
    }

    FVariablePool Temp_Pool;

    for (TFieldIterator<FProperty> EachFiled(Class); EachFiled; ++EachFiled)
    {
        const FProperty* FoundProperty = *EachFiled;

        if (!FoundProperty)
        {
            continue;
        }

        void* Buffer = FoundProperty->ContainerPtrToValuePtr<void>(In_Parent);

        if (!Buffer)
        {
            continue;
        }

        if (!bIncludeEditor)
        {
            if (FoundProperty->IsEditorOnlyProperty())
            {
                continue;
            }
        }

        if (!bIncludeNative)
        {
            if (FoundProperty->IsNative())
            {
                continue;
            }
        }

        if (!bIncludeRoot)
        {
            if (FoundProperty->IsRooted())
            {
                continue;
            }
        }

        FVariableContainer TempContainer;
        TempContainer.bIsEditorOnly = FoundProperty->IsEditorOnlyProperty();
        TempContainer.bIsNative = FoundProperty->IsNative();
        TempContainer.bIsRooted = FoundProperty->IsRooted();
        TempContainer.Name = *FoundProperty->GetName();
        TempContainer.Parent = In_Parent;
        TempContainer.RecordTime = FDateTime::Now();

        if (const FStrProperty* StrProp = CastField<FStrProperty>(FoundProperty))
        {
            TempContainer.Serialized = StrProp->GetPropertyValue(Buffer);
            TempContainer.VariableType = "FString";
            Temp_Pool.Containers.Add(TempContainer);
            continue;
        }

        else if (const FTextProperty* TextProp = CastField<FTextProperty>(FoundProperty))
        {
            TempContainer.Serialized = TextProp->GetPropertyValue(Buffer).ToString();
            TempContainer.VariableType = "FText";
            Temp_Pool.Containers.Add(TempContainer);
            continue;
        }

        else if (const FIntProperty* IntProp = CastField<FIntProperty>(FoundProperty))
        {
            TempContainer.Serialized = FString::FromInt(IntProp->GetPropertyValue(Buffer));
            TempContainer.VariableType = "int32";
            Temp_Pool.Containers.Add(TempContainer);
            continue;
        }

        else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(FoundProperty))
        {
            TempContainer.Serialized = FString::SanitizeFloat(FloatProp->GetPropertyValue(Buffer));
            TempContainer.VariableType = "Float";
            Temp_Pool.Containers.Add(TempContainer);
            continue;
        }

        else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(FoundProperty))
        {
            TempContainer.Serialized = BoolProp->GetPropertyValue(Buffer) ? "True" : "False";
            TempContainer.VariableType = "Boolean";
            Temp_Pool.Containers.Add(TempContainer);
            continue;
        }

        else if (const FStructProperty* StructProp = CastField<FStructProperty>(FoundProperty))
        {
            const FString VariableType = StructProp->Struct->GetName();
            TempContainer.VariableType = VariableType;

            if (VariableType == TEXT("Vector"))
            {
                FVector* Value = reinterpret_cast<FVector*>(Buffer);
                TempContainer.Serialized = Value->ToString();
                Temp_Pool.Containers.Add(TempContainer);
                continue;
            }

            else if (VariableType == TEXT("Rotator"))
            {
                FRotator* Value = reinterpret_cast<FRotator*>(Buffer);
                TempContainer.Serialized = Value->ToString();
                Temp_Pool.Containers.Add(TempContainer);
                continue;
            }

            else if (VariableType == TEXT("Transform"))
            {
                FTransform* Value = reinterpret_cast<FTransform*>(Buffer);
                TempContainer.Serialized = Value->ToString();
                Temp_Pool.Containers.Add(TempContainer);
                continue;
            }

            else
            {
                continue;
            }
        }

        else
        {
            continue;
        }
    }

    Out_Containers = Temp_Pool;
    return true;
}

bool UExtendedVarsBPLibrary::SetVariable(UObject* TargetParent, FName TargetVariable, FString NewData)
{
    if (!IsValid(TargetParent))
    {
        return false;
    }

    UClass* Class = TargetParent->GetClass();

    if (!IsValid(Class))
    {
        return false;
    }

    if (TargetVariable.IsNone() || !TargetVariable.IsValid())
    {
        return false;
    }

    FProperty* FoundProperty = Class->FindPropertyByName(TargetVariable);

    if (!FoundProperty)
    {
        return false;
    }

    void* PropertyValue = FoundProperty->ContainerPtrToValuePtr<void>(TargetParent);

    if (FStrProperty* StrProp = CastField<FStrProperty>(FoundProperty))
    {
        StrProp->SetPropertyValue(PropertyValue, NewData);
        return true;
    }

    else if (FTextProperty* TextProp = CastField<FTextProperty>(FoundProperty))
    {
        TextProp->SetPropertyValue(PropertyValue, FText::FromString(NewData));
        return true;
    }

    else if (FIntProperty* IntProp = CastField<FIntProperty>(FoundProperty))
    {
        IntProp->SetPropertyValue(PropertyValue, FCString::Atoi(*NewData));
        return true;
    }

    else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(FoundProperty))
    {
        FloatProp->SetPropertyValue(PropertyValue, FCString::Atof(*NewData));
        return true;
    }

    else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(FoundProperty))
    {
        if (NewData == "True" || NewData == "true")
        {
            BoolProp->SetPropertyValue(PropertyValue, true);
            return true;
        }

        else if (NewData == "False" || "false")
        {
            BoolProp->SetPropertyValue(PropertyValue, false);
            return true;
        }

        else
        {
            return false;
        }
    }

    else if (const FStructProperty* StructProp = CastField<FStructProperty>(FoundProperty))
    {
        const FString VariableType = StructProp->Struct->GetName();

        if (VariableType == TEXT("Vector"))
        {
            FVector NewValue;
            NewValue.InitFromString(NewData);

            FVector* OldValue = reinterpret_cast<FVector*>(PropertyValue);
            OldValue->X = NewValue.X;
            OldValue->Y = NewValue.Y;
            OldValue->Z = NewValue.Z;

            return true;
        }

        else if (VariableType == TEXT("Rotator"))
        {
            FRotator NewValue;
            NewValue.InitFromString(NewData);

            FRotator* OldValue = reinterpret_cast<FRotator*>(PropertyValue);
            OldValue->Pitch = NewValue.Pitch;
            OldValue->Yaw = NewValue.Yaw;
            OldValue->Roll = NewValue.Roll;

            return true;
        }

        else if (VariableType == TEXT("Transform"))
        {
            FTransform NewValue;
            NewValue.InitFromString(NewData);

            FTransform* OldValue = reinterpret_cast<FTransform*>(PropertyValue);
            OldValue->SetLocation(NewValue.GetLocation());
            OldValue->SetRotation(NewValue.GetRotation());
            OldValue->SetScale3D(NewValue.GetScale3D());

            return true;
        }

        else
        {
            return false;
        }
    }

    else
    {
        return false;
    }
}

void UExtendedVarsBPLibrary::SaveGameToFile(FDelegateSaveToFile DelegateSave, USaveGame* Instance_Save, FString In_Path)
{
    if (!IsValid(Instance_Save))
    {
        DelegateSave.ExecuteIfBound(false, "Save instance is not valid !");
        return;
    }

    if (!FPaths::DirectoryExists(FPaths::GetPath(In_Path)))
    {
        DelegateSave.ExecuteIfBound(false, "Write directory is not exist !");
        return;
    }

    AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateSave, Instance_Save, In_Path]()
        {
            TArray<uint8> SaveData;
            FMemoryWriter MemoryWriter(SaveData, true);
            FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, false);
            Instance_Save->Serialize(Archive);

            if (FFileHelper::SaveArrayToFile(SaveData, *In_Path))
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateSave]()
                    {
                        DelegateSave.ExecuteIfBound(true, "Save successful.");
                    }
                );

                return;
            }

            else
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateSave]()
                    {
                        DelegateSave.ExecuteIfBound(false, "Save is not successful !");
                    }
                );

                return;
            }
        }
    );
}

void UExtendedVarsBPLibrary::SaveGameToMemory(FDelegateSaveToMemory DelegateSave, USaveGame* Instance_Save)
{
    if (!IsValid(Instance_Save))
    {
        DelegateSave.ExecuteIfBound(false, "Save instance is not valid !", TArray<uint8>());
        return;
    }

    AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateSave, Instance_Save]()
        {
            TArray<uint8> SaveData;
            FMemoryWriter MemoryWriter(SaveData, true);
            FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, false);
            Instance_Save->Serialize(Archive);

            if (SaveData.IsEmpty())
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateSave]()
                    {
                        DelegateSave.ExecuteIfBound(false, "Save buffer is empty !", TArray<uint8>());
                    }
                );

                return;
            }

            else
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateSave, SaveData]()
                    {
                        DelegateSave.ExecuteIfBound(true, "Save successful.", SaveData);
                    }
                );
            }
        }
    );
}

void UExtendedVarsBPLibrary::LoadGameFromFile(FDelegateLoadSave DelegateLoad, FString In_Path, TSubclassOf<USaveGame> SaveGameClass)
{
    if (!FPaths::FileExists(In_Path))
    {
        DelegateLoad.ExecuteIfBound(false, "Save file is not exist !", nullptr);
        return;
    }

    USaveGame* LoadedGame = NewObject<USaveGame>(GetTransientPackage(), SaveGameClass);

    if (!IsValid(LoadedGame))
    {
        DelegateLoad.ExecuteIfBound(false, "There was a problem while creating empty save object with given class !", nullptr);
        return;
    }

    AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateLoad, In_Path, LoadedGame]()
        {
            TArray<uint8> LoadData;
            if (!FFileHelper::LoadFileToArray(LoadData, *In_Path))
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateLoad]()
                    {
                        DelegateLoad.ExecuteIfBound(false, "There was a problem while loading save data !", nullptr);
                    }
                );

                return;
            }

            FMemoryReader MemoryReader(LoadData, true);
            FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
            LoadedGame->Serialize(Archive);

            AsyncTask(ENamedThreads::GameThread, [DelegateLoad, LoadedGame]()
                {
                    DelegateLoad.ExecuteIfBound(true, "Load successful.", LoadedGame);
                }
            );
        }
    );
}

void UExtendedVarsBPLibrary::LoadGameFromMemory(FDelegateLoadSave DelegateLoad, TArray<uint8> In_Buffer, TSubclassOf<USaveGame> SaveGameClass)
{
    if (In_Buffer.IsEmpty())
    {
        DelegateLoad.ExecuteIfBound(false, "Buffer is empty !", nullptr);
        return;
    }

    USaveGame* LoadedGame = NewObject<USaveGame>(GetTransientPackage(), SaveGameClass);

    if (!IsValid(LoadedGame))
    {
        DelegateLoad.ExecuteIfBound(false, "There was a problem while creating empty save object with given class !", nullptr);
        return;
    }

    AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateLoad, In_Buffer, LoadedGame]()
        {
            FMemoryReader MemoryReader(In_Buffer, true);
            FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
            LoadedGame->Serialize(Archive);

            AsyncTask(ENamedThreads::GameThread, [DelegateLoad, LoadedGame]()
                {
                    DelegateLoad.ExecuteIfBound(true, "Load successful.", LoadedGame);
                }
            );
        }
    );
}

#pragma endregion Saves