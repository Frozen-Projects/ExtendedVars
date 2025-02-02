// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendedVarsBPLibrary.h"
#include "ExtendedVars.h"

UExtendedVarsBPLibrary::UExtendedVarsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UExtendedVarsBPLibrary::Encode_Api_Old(TArray<uint8>& Encoded_Data, FString& Out_Code, void* Texture_Data, EImageExtensions CompressFormat, int32 Size_X, int32 Size_Y, size_t Lenght)
{
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

    bool bUseRgba = false;
    FString CompressedFormatString = "";
    EImageFormat ImageFormat = EImageFormat::Invalid;

    switch (CompressFormat)
    {
        case EImageExtensions::Ext_None:

            Out_Code = "You have to define a proper extension for encoding like JPG/PNG/BMP.";
            return false;

        case EImageExtensions::Ext_BMP:

            bUseRgba = false;
            CompressedFormatString = "BMP";
            ImageFormat = EImageFormat::BMP;
        
            break;

        case EImageExtensions::EXT_JPEG:

            bUseRgba = true;
            CompressedFormatString = "JPG";
            ImageFormat = EImageFormat::JPEG;
        
            break;

        case EImageExtensions::EXT_PNG:

            bUseRgba = true;
            CompressedFormatString = "PNG";
            ImageFormat = EImageFormat::PNG;

            break;

        default:

            bUseRgba = true;
            CompressedFormatString = "PNG";
            ImageFormat = EImageFormat::PNG;

            break;
    }

    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

    if (!ImageWrapper.IsValid())
    {
        return false;
    }

    TArray<FColor> Array_Colors_RGBA;
    
    if (bUseRgba)
    {
        const size_t Total = static_cast<size_t>(Size_X * Size_Y);

        TArray<FColor> Array_Colors_BGRA;
        Array_Colors_BGRA.SetNum(Total);
        FMemory::Memcpy(Array_Colors_BGRA.GetData(), static_cast<FColor*>(Texture_Data), Lenght);

        // Convert BGRA to RGBA
        Array_Colors_RGBA = Array_Colors_BGRA;
        for (int32 i = 0; i < Total; i++)
        {
            Array_Colors_RGBA[i].R = Array_Colors_BGRA[i].B;
            Array_Colors_RGBA[i].B = Array_Colors_BGRA[i].R;
        }
    }

    if (!ImageWrapper->SetRaw(bUseRgba ? Array_Colors_RGBA.GetData() : Texture_Data, Lenght, Size_X, Size_Y, bUseRgba ? ERGBFormat::RGBA : ERGBFormat::BGRA, 8))
    {
        Out_Code = CompressedFormatString + " compression is NOT successfull for target texture with old api. \"ImageWrapperModule->SetRaw\" failed.";
        return false;
    }

    TArray64<uint8> CompressedData = ImageWrapper->GetCompressed(100);

    if (CompressedData.IsEmpty())
    {
        Out_Code = CompressedFormatString + " compression is NOT successfull for target texture with old api. \"CompressedData\" is empty.";
        return false;
    }

    Out_Code = CompressedFormatString + " compression is successfull for target texture with old api.";
    Encoded_Data = CompressedData;
    return true;
}

bool UExtendedVarsBPLibrary::Encode_Api_New(TArray<uint8>& Encoded_Data, FString& Out_Code, void* Texture_Data, EImageExtensions CompressFormat, int32 Size_X, int32 Size_Y, size_t Lenght, EGammaSpace GammaSpace)
{
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

    FImageView ImageView;
    ImageView.Format = ERawImageFormat::BGRA8;
    ImageView.SizeX = Size_X;
    ImageView.SizeY = Size_Y;
    ImageView.GammaSpace = GammaSpace;
    ImageView.NumSlices = 1;
    ImageView.RawData = malloc(Lenght);
    FMemory::Memcpy(ImageView.RawData, (uint8*)Texture_Data, Lenght);

    EImageFormat Format = EImageFormat::PNG;
    FString CompressedFormatString = "";

    switch (CompressFormat)
    {
        case EImageExtensions::Ext_None:

            Out_Code = "You have to define a proper extension for encoding like JPG/PNG/BMP.";
            return false;

        case EImageExtensions::Ext_BMP:

            CompressedFormatString = "BMP";
            Format = EImageFormat::BMP;
            break;
    
        case EImageExtensions::EXT_JPEG:

            CompressedFormatString = "JPG";
            Format = EImageFormat::JPEG;
            break;
   
        case EImageExtensions::EXT_PNG:

            CompressedFormatString = "PNG";
            Format = EImageFormat::PNG;
            break;
   
        default:

            CompressedFormatString = "PNG";
            Format = EImageFormat::PNG;
            break;
    }

    TArray64<uint8> CompressedData;
    if (!ImageWrapperModule.CompressImage(CompressedData, Format, ImageView, (int32)EImageCompressionQuality::Max))
    {
        Out_Code = CompressedFormatString + " compression is NOT successfull for target texture with new api. \"ImageWrapperModule.CompressImage\" failed.";
        return false;
    }

    if (CompressedData.IsEmpty())
    {
        Out_Code = CompressedFormatString + " compression is NOT successfull for target texture with new api. \"CompressedData\" is empty.";
        return false;
    }

    Out_Code = CompressedFormatString + " compression is successfull for target texture with new api.";
    Encoded_Data = CompressedData;
    return true;
}

// Fonts Group.

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

// Sorters.

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

// Read Group.

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

        uint8* Buffer = (uint8*)malloc(FileSize);
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

bool UExtendedVarsBPLibrary::Write_File_To_Path(TArray<uint8> In_Bytes, FString In_Path)
{
    if (In_Path.IsEmpty())
    {
        return false;
    }

    if (In_Bytes.IsEmpty())
    {
        return false;
    }

    if (FPaths::GetExtension(In_Path, false).IsEmpty())
    {
        // JPG
        if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == "ffd8" && UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, (In_Bytes.Num() - 2), (In_Bytes.Num() - 1), false) == "ffd9")
        {
            In_Path = In_Path + ".jpg";
        }

        // BMP
        else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == "424d")
        {
            In_Path = In_Path + ".bmp";
        }

        // PNG
        else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 7, false) == "89504e470d0a1a0a")
        {
            In_Path = In_Path + ".png";
        }

        // PDF
        else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 3, false) == "25504446")
        {
            In_Path = In_Path + ".pdf";
        }

        else
        {
            return false;
        }
    }

    FPaths::MakeStandardFilename(In_Path);
    return FFileHelper::SaveArrayToFile(In_Bytes, *In_Path);
}

// Bytes Group | Convert To.

bool UExtendedVarsBPLibrary::Bytes_To_Object(UBytesObject_64*& Out_Bytes_Object, TArray<uint8> In_Bytes)
{
    if (In_Bytes.Num() == 0)
    {
        return false;
    }

    TArray64<uint8> ByteArray;
    ByteArray.SetNum(In_Bytes.Num(), EAllowShrinking::Yes);
    FMemory::Memcpy(ByteArray.GetData(), In_Bytes.GetData(), In_Bytes.GetAllocatedSize());

    UBytesObject_64* BytesObject_64 = NewObject<UBytesObject_64>();
    BytesObject_64->ByteArray = ByteArray;

    Out_Bytes_Object = BytesObject_64;

    return true;
}

FString UExtendedVarsBPLibrary::Bytes_x64_To_Hex(UBytesObject_64* B64_Object, int32 Hex_Start, int32 Hex_End, bool bIsFull)
{
    if (B64_Object->ByteArray.Num() == 0)
    {
        return "";
    }

    if (bIsFull)
    {
        std::stringstream Stream;
        Stream << std::hex << std::setfill('0');
        for (int Index = 0; Index < B64_Object->ByteArray.Num(); Index++)
        {
            Stream << std::hex << std::setw(2) << static_cast<int>(B64_Object->ByteArray.GetData()[Index]);
        }

        return Stream.str().c_str();
    }

    else if (Hex_Start <= Hex_End && Hex_End < B64_Object->ByteArray.Num())
    {
        std::stringstream Stream;
        Stream << std::hex << std::setfill('0');
        for (int Index = Hex_Start; Index <= Hex_End; Index++)
        {
            Stream << std::hex << std::setw(2) << static_cast<int>(B64_Object->ByteArray.GetData()[Index]);
        }

        return Stream.str().c_str();
    }

    else
    {
        return "";
    }
}

FString UExtendedVarsBPLibrary::Bytes_x64_To_Base64(UBytesObject_64* B64_Object, bool bUseUrl)
{
    if (B64_Object->ByteArray.Num() == 0)
    {
        return "";
    }

    return FBase64::Encode(B64_Object->ByteArray.GetData(), B64_Object->ByteArray.Num(), bUseUrl ? EBase64Mode::UrlSafe : EBase64Mode::Standard);
}

FString UExtendedVarsBPLibrary::Bytes_x64_To_UTF8(UBytesObject_64* B64_Object)
{
    if (B64_Object->ByteArray.Num() == 0)
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

    if (Length > B64_Object->ByteArray.Num() - Index)
    {
        Length = B64_Object->ByteArray.Num() - Index;
    }

    const FUTF8ToTCHAR Src(reinterpret_cast<const ANSICHAR*>(B64_Object->ByteArray.GetData() + Index), Length);
    return FString(Src.Length(), Src.Get());
}

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

// Bytes Group | Convert From.

UBytesObject_64* UExtendedVarsBPLibrary::Hex_To_Bytes_x64(FString In_Hex)
{
    if (In_Hex.IsEmpty())
    {
        return nullptr;
    }

    UBytesObject_64* BytesObject = NewObject<UBytesObject_64>();
    
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
    BytesObject->ByteArray.Append((uint8*)Source.Get(), Source.Length());

    return BytesObject;
}

UBytesObject_64* UExtendedVarsBPLibrary::UTF8_To_Bytes_x64(FString In_UTF8)
{
    if (In_UTF8.IsEmpty())
    {
        return nullptr;
    }

    UBytesObject_64* BytesObject = NewObject<UBytesObject_64>();

    FTCHARToUTF8 Source = FTCHARToUTF8(In_UTF8.GetCharArray().GetData());
    BytesObject->ByteArray.Append((uint8*)Source.Get(), Source.Length());

    return BytesObject;
}

UBytesObject_64* UExtendedVarsBPLibrary::Base64_To_Bytes_x64(FString In_Base64, bool bUseUrl)
{
    if (In_Base64.IsEmpty())
    {
        return nullptr;
    }

    FWideStringView String = In_Base64;

    UBytesObject_64* BytesObject = NewObject<UBytesObject_64>();

    FBase64::Decode(String.GetData(), String.Len(), BytesObject->ByteArray.GetData(), bUseUrl ? EBase64Mode::UrlSafe : EBase64Mode::Standard);

    return BytesObject;
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

// String Group.

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

// Integer Group.

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

// Float/Double Group.

float UExtendedVarsBPLibrary::Float_Precision(float TargetFloat, int32 Precision)
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

// Time Group.

bool UExtendedVarsBPLibrary::Time_Counter_To_FDateTime(FDateTime& Out_Time, FString In_Time, FString Delimiter, EStringToDate ConvertType)
{
    if (In_Time.IsEmpty())
    {
        return false;
    }

    TArray<FString> TimeSections;

    switch (ConvertType)
    {
    case EStringToDate::None:

        return false;

    case EStringToDate::UnrealDateTime:

        return FDateTime::Parse(In_Time, Out_Time);

    case EStringToDate::Http:

        return FDateTime::ParseHttpDate(In_Time, Out_Time);

    case EStringToDate::Iso8601:

        return FDateTime::ParseIso8601(*In_Time, Out_Time);

    case EStringToDate::Custom:

        TimeSections = UKismetStringLibrary::ParseIntoArray(In_Time, Delimiter, false);

        if (TimeSections.Num() == 3)
        {
            Out_Time = FDateTime(1, 1, 1, FCString::Atoi(*TimeSections[0]), FCString::Atoi(*TimeSections[1]), FCString::Atoi(*TimeSections[2]), 0);
            return true;
        }
        
        else
        {
            return false;
        }

    default:
        return false;
    }
}

FDateTime UExtendedVarsBPLibrary::Increment_Date(FDateTime Start, int32 In_Years, int32 In_Months, int32 In_Days)
{
    if (Start < FDateTime(1970, 1, 1))
    {
        return FDateTime();
    }

    using namespace std::chrono;
    
    system_clock::time_point now;

    if (Start == FDateTime())
    {
        now = system_clock::now();
    }

    else
    {
        int64 UnixSeconds = Start.ToUnixTimestamp();
        int32 ExtraMilliseconds = Start.GetMillisecond();
        int64 TotalMilliseconds = UnixSeconds * 1000 + ExtraMilliseconds;
        now = system_clock::time_point(milliseconds(TotalMilliseconds));
    }

    // 1. Separate 'day' and 'time-of-day' (fractional day).
    time_point floorDays = floor<days>(now);                       // Truncates to whole days
    time_point sysDaysNow = time_point_cast<days>(floorDays);      // A sys_days representation
   
    // 2. Convert sys_days -> year_month_day
    year_month_day ymd(sysDaysNow);

    // 3. Add the desired years, months, and days as calendar arithmetic
    ymd += years(In_Years);
    ymd += months(In_Months);
    ymd = year_month_day(sys_days(ymd) + days(In_Days));

    // 4. Convert back to sys_days, then add the time-of-day back
    sys_days newSysDays = sys_days(ymd);       // This is a day-precision time_point in UTC
    auto timeOfDay = now - floorDays;          // The leftover hours, minutes, seconds, ms
    auto future = newSysDays + timeOfDay;      // Reattach the original time of day

    // Convert to time_t for localtime
    std::time_t future_c = system_clock::to_time_t(future);
    std::tm local_tm = *std::localtime(&future_c);

    // Extract milliseconds for demonstration
    auto future_sec = floor<seconds>(future);
    int32 MilliSeconds = duration_cast<milliseconds>(future - future_sec).count();

    int32 Year = local_tm.tm_year + 1900;
    int32 Month = local_tm.tm_mon + 1;
    int32 Day = local_tm.tm_mday;
    int32 Hour = local_tm.tm_hour;
    int32 Minute = local_tm.tm_min;
    int32 Second = local_tm.tm_sec;

    return FDateTime(Year, Month, Day, Hour, Minute, Second, MilliSeconds);
}

// Render Group.

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
    ENQUEUE_RENDER_COMMAND(Convert_TRT2D_To_T2D)([Delegate_T2D, In_TRT_2D](FRHICommandList& RHICmdList)
        {
            if (IsValid(In_TRT_2D) == false)
            {
                AsyncTask(ENamedThreads::GameThread, [Delegate_T2D]()
                    {
                        Delegate_T2D.ExecuteIfBound(false, "In_TRT_2D is not valid.", nullptr);
                    }
                );

                return;
            }

            // Get Texture Render Target 2D Properties.
            int32 Size_X = In_TRT_2D->SizeX;
            int32 Size_Y = In_TRT_2D->SizeY;
            int32 Size_Pixel = sizeof(FColor);
            size_t Total = static_cast<size_t>(Size_X * Size_Y);
            size_t Lenght = Total * Size_Pixel;

            // Create Copy Descriptions.
            FRHITextureCreateDesc Desc =
                FRHITextureCreateDesc::Create2D(TEXT("CopiedTextureDescription"))
                .SetExtent(Size_X, Size_Y)
                .SetFormat(EPixelFormat::PF_R8G8B8A8)
                .SetInitialState(ERHIAccess::CopySrc);

            // Copy Texture Render Target 2D.
            FRHITexture* CopiedTextureResource = RHICreateTexture(Desc);
            FRHICopyTextureInfo CopyTextureInfo;
            RHICmdList.CopyTexture(In_TRT_2D->GetResource()->GetTextureRHI(), CopiedTextureResource, CopyTextureInfo);

            // Get Texture Render Target 2D Buffer.
            uint32 DestStride_TRT2D = 0;
            void* Texture_Data_TRT2D = RHILockTexture2D(CopiedTextureResource, 0, RLM_ReadOnly, DestStride_TRT2D, false);

            RHIUnlockTexture2D(CopiedTextureResource, 0, false, true);

            AsyncTask(ENamedThreads::GameThread, [Delegate_T2D, Size_X, Size_Y, Lenght, Texture_Data_TRT2D]()
                {
                    // Create Texture
                    UTexture2D* Texture = UTexture2D::CreateTransient(Size_X, Size_Y, PF_B8G8R8A8);
                    Texture->NeverStream = true;
                    Texture->SRGB = true;

                    FTexture2DMipMap& Texture2D_Mip = Texture->GetPlatformData()->Mips[0];
                    void* Texture2D_Data = Texture2D_Mip.BulkData.Lock(LOCK_READ_WRITE);
                    FMemory::Memcpy(Texture2D_Data, Texture_Data_TRT2D, Lenght);
                    Texture2D_Mip.BulkData.Unlock();
                    Texture->UpdateResource();
                    
                    Delegate_T2D.ExecuteIfBound(true, "Sucessfull.", Texture);
                }
            );

            return;
        }
    );
}

bool UExtendedVarsBPLibrary::Export_T2D_File(FString& Out_Path, UTexture2D* Texture, FString In_Path, EImageExtensions Extension)
{
    if (IsValid(Texture) == false)
    {
        return false;
    }

    FString FileName = "";

    if (In_Path.IsEmpty())
    {
        FileName = FPaths::ProjectSavedDir() + "Temp/" + Texture->GetFName().ToString();
    }

    else
    {
        FPaths::MakeStandardFilename(In_Path);
        FileName = In_Path;
    }

    FImage ImageExporter;
    TArray<FColor> Array_Colors;

    switch (Extension)
    {
        case EImageExtensions::Ext_None:

            return false;

        case EImageExtensions::Ext_BMP:

            Out_Path = FileName + ".bmp";
        
            if (UExtendedVarsBPLibrary::Export_T2D_Colors(Array_Colors, Texture))
            {
                return FFileHelper::CreateBitmap(*Out_Path, Texture->GetSizeX(), Texture->GetSizeY(), Array_Colors.GetData(), NULL, &IFileManager::Get(), NULL, true);
            }
        
            else
            {
                return false;
            }

        case EImageExtensions::EXT_JPEG:

            Out_Path = FileName + ".jpg";
            FImageUtils::GetTexture2DSourceImage(Texture, ImageExporter);
            return FImageUtils::SaveImageByExtension(*Out_Path, ImageExporter, 0);

            break;

        case EImageExtensions::EXT_PNG:
        
            Out_Path = FileName + ".png";
            FImageUtils::GetTexture2DSourceImage(Texture, ImageExporter);
            return FImageUtils::SaveImageByExtension(*Out_Path, ImageExporter, 0);

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
    size_t Lenght = static_cast<size_t>(Texture_Width * Texture_Height * 4);

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

void UExtendedVarsBPLibrary::Export_Texture_Bytes_Render_Thread(FDelegateImageBuffer DelegateImageBuffer, UTexture* TargetTexture, bool bUseOldApi, EImageExtensions Extension)
{    
    ENQUEUE_RENDER_COMMAND(Cmd_Export_Texture)([TargetTexture, DelegateImageBuffer, Extension, bUseOldApi](FRHICommandList& RHICmdList)
        {
            if (!IsValid(TargetTexture))
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "Target texture is not valid.", TArray<uint8>(), FVector2D());
                    }
                );

                return;
            }
            
            // Initialize Variables.
            EPixelFormat PixelFormat = PF_Unknown;

            // Get Texture Resource and Pixel Format.
            if (UTexture2D* T2D = Cast<UTexture2D>(TargetTexture))
            {
                if (T2D->GetPixelFormat() != EPixelFormat::PF_B8G8R8A8)
                {
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(false, "Target Texture2D format is not B8G8R8A8.", TArray<uint8>(), FVector2D());
                        }
                    );

                    return;
                }

                if (T2D->CompressionSettings.GetIntValue() != 5 && T2D->CompressionSettings.GetIntValue() != 7)
                {
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(false, "Texture compression setting is not correct.", TArray<uint8>(), FVector2D());
                        }
                    );

                    return;
                }

                PixelFormat = PF_B8G8R8A8;
            }

            if (UTextureRenderTarget2D* TRT_2D = Cast<UTextureRenderTarget2D>(TargetTexture))
            {
                if (TRT_2D->RenderTargetFormat.GetValue() != ETextureRenderTargetFormat::RTF_RGBA8 && TRT_2D->RenderTargetFormat.GetValue() != ETextureRenderTargetFormat::RTF_RGBA8_SRGB)
                {
                    AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                        {
                            DelegateImageBuffer.ExecuteIfBound(false, "Target TextureRenderTarget2D format is not RTF_RGBA8 or RTF_RGBA_SRGB.", TArray<uint8>(), FVector2D());
                        }
                    );

                    return;
                }

                PixelFormat = PF_R8G8B8A8;
            }

            if (UMediaTexture* MT_2D = Cast<UMediaTexture>(TargetTexture))
            {
                PixelFormat = PF_R8G8B8A8;
            }

            // Get Image Properties.
            FTextureResource* TextureResource = TargetTexture->GetResource();
            EGammaSpace GammaSpace = TargetTexture->GetGammaSpace();
            const int32 Size_X = TextureResource->GetSizeX();
            const int32 Size_Y = TextureResource->GetSizeY();
            const size_t Lenght = static_cast<size_t>(Size_X * Size_Y * sizeof(FColor));

            // Create Copy Texture Descriptions
            FRHITextureCreateDesc Desc =
                FRHITextureCreateDesc::Create2D(TEXT("CopiedTextureDescription"))
                .SetExtent(Size_X, Size_Y)
                .SetFormat(PixelFormat)
                .SetInitialState(ERHIAccess::CopySrc);

            FRHITexture* CopiedTextureResource = RHICreateTexture(Desc);
            FRHICopyTextureInfo CopyTextureInfo;
            
            // Copy Texture to Process.
            if (PixelFormat == EPixelFormat::PF_B8G8R8A8)
            {
                RHICmdList.CopyTexture(TextureResource->GetTexture2DRHI(), CopiedTextureResource, CopyTextureInfo);
            }
            
            else if (PixelFormat == EPixelFormat::PF_R8G8B8A8)
            {
                RHICmdList.CopyTexture(TextureResource->GetTextureRHI(), CopiedTextureResource, CopyTextureInfo);
            }

            uint32 DestStride = 0;
            void* Texture_Data = RHILockTexture2D(CopiedTextureResource, 0, RLM_ReadOnly, DestStride, false);
            RHIUnlockTexture2D(CopiedTextureResource, 0, false, true);

            if (!Texture_Data)
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(false, "Texture buffer is not valid.", TArray<uint8>(), FVector2D());
                    }
                );
                
                return;
            }

            // Initialize Compressed Image Buffer.
            TArray<uint8> CompressedData;

            if (Extension == EImageExtensions::Ext_None)
            {
                CompressedData.SetNum(Lenght);
                FMemory::Memcpy(CompressedData.GetData(), (uint8*)Texture_Data, Lenght);

                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, CompressedData, Size_X, Size_Y]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(true, "Export texture without extension (raw buffer) is successful.", CompressedData, FVector2D(Size_X, Size_Y));
                    }
                );

                return;
            }

            FString Out_Code = "";
            bool EncodeResult = false;

            if (Extension == EImageExtensions::Ext_BMP || Extension == EImageExtensions::EXT_JPEG || Extension == EImageExtensions::EXT_PNG)
            {
                if (bUseOldApi)
                {
                    EncodeResult = UExtendedVarsBPLibrary::Encode_Api_Old(CompressedData, Out_Code, Texture_Data, Extension, Size_X, Size_Y, Lenght);
                }

                else
                {
                    EncodeResult = UExtendedVarsBPLibrary::Encode_Api_New(CompressedData, Out_Code, Texture_Data, Extension, Size_X, Size_Y, Lenght, GammaSpace);
                }

                AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, CompressedData, EncodeResult, Out_Code, Size_X, Size_Y]()
                    {
                        DelegateImageBuffer.ExecuteIfBound(EncodeResult, Out_Code, CompressedData, EncodeResult ? FVector2D(Size_X, Size_Y) : FVector2D());
                    }
                );

                return;
            }
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
                        DelegateImageBuffer.ExecuteIfBound(false, "There is no byte to decompres.", TArray<uint8>(), FVector2D());
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
                    DelegateImageBuffer.ExecuteIfBound(true, "Decompress is successful.", DecompressedBuffer, ImageSize);
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

// Logs.

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
