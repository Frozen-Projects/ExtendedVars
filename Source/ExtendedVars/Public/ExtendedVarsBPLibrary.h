// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

// Custom Includes.
#include "Extended_Includes.h"

#include "ExtendedVarsBPLibrary.generated.h"

class FFindDirectories : public IPlatformFile::FDirectoryVisitor
{
public:

	TArray<FFolderContents> Array_Contents;

	FFindDirectories();
	virtual bool Visit(const TCHAR* CharPath, bool bIsDirectory) override;
};

// Select folder from dialog (each content).
USTRUCT(BlueprintType)
struct EXTENDEDVARS_API FFolderContents
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FString Path = "";

	UPROPERTY(BlueprintReadOnly)
	FString Name = "";

	UPROPERTY(BlueprintReadOnly)
	bool bIsFile = false;

	bool operator == (const FFolderContents& Other) const
	{
		return Path == Other.Path && Name == Other.Name && bIsFile == Other.bIsFile;
	}

	bool operator != (const FFolderContents& Other) const
	{
		return !(*this == Other);
	}
};

FORCEINLINE uint32 GetTypeHash(const FFolderContents& Key)
{
	uint32 Hash_Path = GetTypeHash(Key.Path);
	uint32 Hash_Name = GetTypeHash(Key.Name);
	uint32 Hash_bIsFile = GetTypeHash(Key.bIsFile);

	uint32 GenericHash;
	FMemory::Memset(&GenericHash, 0, sizeof(uint32));
	GenericHash = HashCombine(GenericHash, Hash_Path);
	GenericHash = HashCombine(GenericHash, Hash_Name);
	GenericHash = HashCombine(GenericHash, Hash_bIsFile);

	return GenericHash;
}

UCLASS(BlueprintType)
class EXTENDEDVARS_API UBytesObject_64 : public UObject
{
	GENERATED_BODY()

public:

	TArray64<uint8> ByteArray;

	UFUNCTION(BlueprintPure)
	virtual int64 GetSize()
	{
		return ByteArray.Num();
	};
};

UCLASS(BlueprintType)
class EXTENDEDVARS_API URuntimeFont : public UObject
{
	GENERATED_BODY()

public:

	// ~URuntimeFont start.
	void BeginDestroy();
	// ~URuntimeFont finish.

	UPROPERTY(BlueprintReadOnly)
	FString FontName;

	UPROPERTY(BlueprintReadOnly)
	UFont* Font;

	UFontFace* Font_Face;

};

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateFolderContents, bool, bIsSuccessfull, FString, ErrorCode, const TArray<FFolderContents>&, Out_Contents);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateBytes_32, bool, bIsSuccessfull, FString, ErrorCode, const TArray<uint8>&, Out_Bytes);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_FourParams(FDelegateImageBuffer, bool, bIsSuccessfull, FString, ErrorCode, const TArray<uint8>&, Out_Bytes, FVector2D, Out_Size);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateTexture2D, bool, bIsSuccessfull, FString, ErrorCode, UTexture2D*, Out);

UCLASS()
class UExtendedVarsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	// Render Callbacks

	static EXTENDEDVARS_API bool Encode_Api_Old(TArray<uint8>& Encoded_Data, FString& Out_Code, void* Texture_Data, EImageExtensions CompressFormat, int32 Size_X, int32 Size_Y, size_t Lenght);

	static EXTENDEDVARS_API bool Encode_Api_New(TArray<uint8>& Encoded_Data, FString& Out_Code, void* Texture_Data, EImageExtensions CompressFormat, int32 Size_X, int32 Size_Y, size_t Lenght, EGammaSpace GammaSpace);

	// Fonts Group.

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Runtime Font Load", ToolTip = "", Keywords = "runtime, font, load"), Category = "Frozen Forest|Extended Variables|Font")
	static EXTENDEDVARS_API URuntimeFont* Runtime_Font_Load(TArray<uint8> In_Bytes, FString FontName = "RuntimeFont");

	// Sorters.

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sort Strings", ToolTip = "", Keywords = "string, fstring, ascending, descending, sort"), Category = "Frozen Forest|Extended Variables|Sorters")
	static EXTENDEDVARS_API TArray<FString> Sort_String(TArray<FString> TargetArray, bool bIsDescending);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sort Integers", ToolTip = "", Keywords = "sort, int32, ascending, descending"), Category = "Frozen Forest|Extended Variables|Sorters")
	static EXTENDEDVARS_API TArray<int32> Sort_Int32(TArray<int32> TargetArray, bool bIsDescending);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sort Floats", ToolTip = ".", Keywords = "sort, float, double, ascending, descending"), Category = "Frozen Forest|Extended Variables|Sorters")
	static EXTENDEDVARS_API TArray<float> Sort_Float(TArray<float> TargetArray, bool bIsDescending);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sort Double", ToolTip = ".", Keywords = "sort, float, double, ascending, descending"), Category = "Frozen Forest|Extended Variables|Sorters")
	static EXTENDEDVARS_API TArray<double> Sort_Double(TArray<double> TargetArray, bool bIsDescending);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sort Times", ToolTip = "", Keywords = "sort, times, ascending, descending"), Category = "Frozen Forest|Extended Variables|Sorters")
	static EXTENDEDVARS_API TArray<FDateTime> Sort_Time(TArray<FDateTime> TargetArray, bool bIsDescending);

	// Read Group.

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Android Path Helper", ToolTip = "This node automatically gives Internal Storage absolute path. So, you just need to define sub-folder and file. Example: Download/sample.pdf", Keywords = "android, ios, mobile, folder, file, path, helper, absolute"), Category = "Frozen Forest|Extended Variables|Read")
	static EXTENDEDVARS_API FString Android_Path_Helper(FString In_FileName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Folder Contents", ToolTip = "", Keywords = "explorer, load, file, folder, content"), Category = "Frozen Forest|Extended Variables|Read")
	static EXTENDEDVARS_API bool Get_Folder_Contents(TArray<FFolderContents>& OutContents, FString& ErrorCode, FString InPath);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Search In Folder", ToolTip = "", Keywords = "explorer, load, file, folder, content"), Category = "Frozen Forest|Extended Variables|Read")
	static EXTENDEDVARS_API void Search_In_Folder(FDelegateFolderContents DelegateFolderContents, FString InPath, FString InSearch, bool bSearchExact);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read File from Path (64 Bit)", ToolTip = "You need to use absolute path.", Keywords = "read, load, path, bytes, import, file"), Category = "Frozen Forest|Extended Variables|Read")
	static EXTENDEDVARS_API bool Read_File_From_Path_64(UBytesObject_64*& Out_Bytes_Object, FString In_Path, bool bUseLowLevel = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read File from Path (32 Bit)", ToolTip = "You need to use absolute path.", Keywords = "read, load, path, bytes, import, file"), Category = "Frozen Forest|Extended Variables|Read")
	static EXTENDEDVARS_API bool Read_File_From_Path_32(TArray<uint8>& Out_Bytes, FString In_Path, bool bUseLowLevel = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Text from Path", ToolTip = "You need to use absolute path.", Keywords = "read, load, path, bytes, import, text"), Category = "Frozen Forest|Extended Variables|Read")
	static EXTENDEDVARS_API bool Read_Text_From_Path(FString& Out_String, FString In_Path);

	// Write Group.

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write File To Path", ToolTip = "You need to use absolute path.", Keywords = "write, path, bytes, export"), Category = "Frozen Forest|Extended Variables|Write")
	static EXTENDEDVARS_API bool Write_File_To_Path(TArray<uint8> In_Bytes, FString In_Path);

	// Bytes Group | Convert To.

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bytes To Bytes64 Object", ToolTip = "", Keywords = "bytes, object, x64"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API bool Bytes_To_Object(UBytesObject_64*& Out_Bytes_Object, TArray<uint8> In_Bytes);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes x64 To Hex", Keywords = "bytes, string, fstring, convert, to, hex"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API FString Bytes_x64_To_Hex(UBytesObject_64* B64_Object, int32 Hex_Start, int32 Hex_End, bool bIsFull);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes x64 To Base64", Keywords = "bytes, string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API FString Bytes_x64_To_Base64(UBytesObject_64* B64_Object, bool bUseUrl);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes x64 To UTF8", Keywords = "bytes, string, fstring, convert, to, utf8"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API FString Bytes_x64_To_UTF8(UBytesObject_64* B64_Object);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes x86 To Hex", Keywords = "bytes, string, fstring, convert, to, hex"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API FString Bytes_x86_To_Hex(TArray<uint8> In_Bytes, int32 Hex_Start, int32 Hex_End, bool bIsFull);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes x86 To Base64", Keywords = "bytes, string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API FString Bytes_x86_To_Base64(TArray<uint8> In_Bytes, bool bUseUrl);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes x86 To UTF8", Keywords = "bytes, string, fstring, convert, to, utf8"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API FString Bytes_x86_To_UTF8(TArray<uint8> In_Bytes);

	// Bytes Group | Convert From.

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Hex To Bytes x64", Keywords = "bytes, string, fstring, convert, to, hex"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API UBytesObject_64* Hex_To_Bytes_x64(FString In_Hex);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "UTF8 To Bytes x64", Keywords = "bytes, string, fstring, convert, to, utf8"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API UBytesObject_64* UTF8_To_Bytes_x64(FString In_UTF8);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Base64 To Bytes x64", Keywords = "bytes, string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API UBytesObject_64* Base64_To_Bytes_x64(FString In_Base64, bool bUseUrl);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Hex To Bytes x86", Keywords = "bytes, string, fstring, convert, to, hex"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API TArray<uint8> Hex_To_Bytes_x86(FString In_Hex);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "UTF8 To Bytes x86", Keywords = "bytes, string, fstring, convert, to, utf8"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API TArray<uint8> UTF8_To_Bytes_x86(FString In_UTF8);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Base64 To Bytes x86", Keywords = "bytes, string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API TArray<uint8> Base64_To_Bytes_x86(FString In_Base, bool bUseUrl);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Detect Image Extension", Keywords = "detect, image, extension, bytes, buffer"), Category = "Frozen Forest|Extended Variables|Bytes")
	static EXTENDEDVARS_API EImageExtensions DetectImageExtension(TArray<uint8> In_Bytes);

	// String Group.

	UFUNCTION(BlueprintPure, meta = (DisplayName = "FDateTime To String", ToolTip = "", Keywords = "string, fstring, parse, date, time, convert, to"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API FString FDateTime_To_String(FDateTime In_Time, FString CustomFormat, bool bUseIso8601 = true);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Int64 To String", Keywords = "string, fstring, convert, int64"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API FString Int64_To_String(int64 TargetInt64);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Int32", ToolTip = "It converts string to int32 with Atoi.", Keywords = "string, fstring, convert, to, int, int32, int64"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API bool String_To_Int32(int32& Out_Int32, FString SourceString);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Int64", ToolTip = "It converts string to int64 with Atoi64.", Keywords = "string, fstring, convert, to, int, int32, int64"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API bool String_To_Int64(int64& Out_Int64, FString SourceString);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Double", ToolTip = "It converts string to double with Atod.", Keywords = "string, fstring, convert, to, float, double"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API bool String_To_Double(double& Out_Double, FString SourceString);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Float", ToolTip = "It converts string to float with Atof.", Keywords = "string, fstring, convert, to, float, double"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API bool String_To_Float(float& Out_Float, FString SourceString);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Hex", ToolTip = "", Keywords = "string, fstring, convert, to, hex"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API bool String_To_Hex(FString& Out_Hex, FString SourceString, bool bIsLower);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Base64", ToolTip = "", Keywords = "string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API bool String_To_Base64(FString& Out_Base64, FString SourceString, bool bUseUrl);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Hex To String", ToolTip = "", Keywords = "string, fstring, convert, to, hex"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API bool Hex_To_String(FString& Out_Decoded, FString In_Hex);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Base64 To String", ToolTip = "", Keywords = "string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API bool Base64_To_String(FString& Out_Decoded, FString In_Hex, bool bUseUrl);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Beautify Json", ToolTip = "", Keywords = "string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|String")
	static EXTENDEDVARS_API FString Beautify_Json(FString In_Json);

	// Integer Group.

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Number To Byte", ToolTip = "It converts number to a byte form.", Keywords = "byte, string, fstring, convert"), Category = "Frozen Forest|Extended Variables|Integer")
	static EXTENDEDVARS_API uint8 Int32_To_Byte(int32 In_Number);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Int32 Place Family", Keywords = "int32, get, place, family"), Category = "Frozen Forest|Extended Variables|Integer")
	static EXTENDEDVARS_API int32 Int32_Place_Family(int32 TargetInteger);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Int32 Truncate To Whole (Big)", Keywords = "int32, truncate, truncation, full, whole, big"), Category = "Frozen Forest|Extended Variables|Integer")
	static EXTENDEDVARS_API int32 Int32_Truncate_To_Big(int32 TargetInteger);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Int32 Truncate To Whole (Small)", Keywords = "int32, truncate, truncation, full, whole, small"), Category = "Frozen Forest|Extended Variables|Integer")
	static EXTENDEDVARS_API int32 Int32_Truncate_To_Small(int32 TargetInteger);

	// Float/Double Group.

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float Precision", ToolTip = "", Keywords = "float, get, precision"), Category = "Frozen Forest|Extended Variables|Float")
	static EXTENDEDVARS_API float Float_Precision(float TargetFloat, int32 Precision);
	
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Fraction Count", ToolTip = "", Keywords = "float, get, fraction, count"), Category = "Frozen Forest|Extended Variables|Float")
	static EXTENDEDVARS_API int32 Float_Fraction_Count(float TargetFloat);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Round Next Float", ToolTip = "", Keywords = "float, round, next"), Category = "Frozen Forest|Extended Variables|Float")
	static EXTENDEDVARS_API float Float_Round_Next(float TargetFloat, double Decimal);
	
	// Date & Time Group.

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Time Counter To FDateTime", ToolTip = "", Keywords = "string, fstring, parse, date, time, convert, to"), Category = "Frozen Forest|Extended Variables|Time")
	static EXTENDEDVARS_API bool Time_Counter_To_FDateTime(FDateTime& Out_Time, FString In_Time, FString Delimiter, EStringToDate ConvertType);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Increment Date", ToolTip = "If there is no \"Start\" input, function will increment \"Now\".", Keywords = "date, time, increment, auto, calculate"), Category = "FF_MBedTLS")
	static EXTENDEDVARS_API FDateTime Increment_Date(FDateTime Start, int32 In_Years, int32 In_Months, int32 In_Days);

	// Render Group.

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Widget To Texture Render Target 2D", Keywords = "texture, render, target, 2d, widget, convert"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API UTextureRenderTarget2D* Widget_To_TRT2D(FString& OutCode, UUserWidget* InWidget, FVector2D InDrawSize);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Texture Render Target 2D to UTexture2D", Keywords = "texture, render, target, widget, convert"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API void TRT2D_To_T2D(FDelegateTexture2D Delegate_T2D, UTextureRenderTarget2D* In_TRT_2D);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Texture2D As Bitmap", ToolTip = "If \"In_Path\" is not defined, it will export it to \Project's saved directory/Temp\".", Keywords = "t2d, texture, texture2d, utexture2d, export, bitmap, bmp"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API bool Export_T2D_File(FString& Out_Path, UTexture2D* Texture, FString In_Path, EImageExtensions Extension = EImageExtensions::Ext_BMP);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Export Texture2D As Colors", Keywords = "t2d, texture, texture2d, utexture2d, get, export, color, fcolor, array"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API bool Export_T2D_Colors(TArray<FColor>& Out_Array, UTexture2D* Texture);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Export Texture2D As Bytes", Keywords = "t2d, texture, texture2d, utexture2d, get, export, byte, bytes, array"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API bool Export_T2D_Bytes(TArray<uint8>& Out_Array, FString& Out_Code, UTexture2D* Texture);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Texture with Render Thread", ToolTip = "In extension is none, function won't use \"ImageWrapperModule\". So, API doesn't matter.", Keywords = "t2d, trt2d, texture, texture2d, utexture2d, render, target, media, get, export, byte, bytes, array, render, thread"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API void Export_Texture_Bytes_Render_Thread(FDelegateImageBuffer DelegateImageBuffer, UTexture* TargetTexture, bool bUseOldApi, EImageExtensions Extension = EImageExtensions::Ext_None);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Decompress Image", ToolTip = "", Keywords = "image, texture, render, decompress"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API void DecompressImage(FDelegateImageBuffer DelegateImageBuffer, TArray<uint8> In_Bytes);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Import Texture2D From Bytes (Game Thread Image Utils)", Keywords = "t2d, texture, texture2d, utexture2d, import, create, bytes"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API bool Import_T2D_Bytes(UTexture2D*& Out_Texture, TArray<uint8> In_Bytes, bool bUseSrgb);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Import Texture2D From Bytes (Game Thread - Low Level)", Keywords = "t2d, texture, texture2d, utexture2d, import, create, bytes, low, level"), Category = "Frozen Forest|Extended Variables|Render")
	static EXTENDEDVARS_API bool Import_T2D_Bytes_LowLevel(UTexture2D*& Out_Texture, TArray<uint8> In_Bytes, FVector2D Size, bool bUseSrgb);

	// Logs.

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Log String", Keywords = "log, display, string, result, return, code"), Category = "Frozen Forest|Extended Variables|Logs")
	static void LogString(int32 InLogLevel, FString Log);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Date With Timezone", Keywords = "get, date, time, zone, timezone, geo, geography, geographic, day, month, short, long"), Category = "Frozen Forest|Extended Variables|DateTime")
	static bool GetDateTimeWithZone(FString& Out_Print, FString& Out_Zone, FString& Out_Geo, FString& Out_Day, FString& Out_Month, bool bUseShort);

};