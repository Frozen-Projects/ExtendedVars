#pragma once

#include "Extended_Includes.h"
#include "Extended_Bytes.generated.h"

UCLASS(BlueprintType)
class EXTENDEDVARS_API UBytesObject_64 : public UObject
{
	GENERATED_BODY()

public:

	TArray64<uint8> ByteArray;

	UFUNCTION(BlueprintPure)
	virtual int64 GetSize();

	UFUNCTION(BlueprintCallable)
	virtual TArray<uint8> GetBytesAs32();

	UFUNCTION(BlueprintCallable)
	virtual bool SetBytesFrom32(const TArray<uint8>& In_Bytes);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bytes x64 To Hex", Keywords = "bytes, string, fstring, convert, to, hex"), Category = "Frozen Forest|Extended Variables|Bytes")
	virtual FString Bytes_x64_To_Hex(int32 Hex_Start, int32 Hex_End, bool bIsFull);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bytes x64 To Base64", Keywords = "bytes, string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|Bytes")
	virtual FString Bytes_x64_To_Base64(bool bUseUrl);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bytes x64 To UTF8", Keywords = "bytes, string, fstring, convert, to, utf8"), Category = "Frozen Forest|Extended Variables|Bytes")
	virtual FString Bytes_x64_To_UTF8();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Hex To Bytes x64", Keywords = "bytes, string, fstring, convert, to, hex"), Category = "Frozen Forest|Extended Variables|Bytes")
	virtual bool Hex_To_Bytes_x64(FString In_Hex);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UTF8 To Bytes x64", Keywords = "bytes, string, fstring, convert, to, utf8"), Category = "Frozen Forest|Extended Variables|Bytes")
	virtual bool UTF8_To_Bytes_x64(FString In_UTF8);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Base64 To Bytes x64", Keywords = "bytes, string, fstring, convert, to, base64"), Category = "Frozen Forest|Extended Variables|Bytes")
	virtual bool Base64_To_Bytes_x64(FString In_Base64, bool bUseUrl);
};

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateBytes_32, bool, bIsSuccessfull, FString, ErrorCode, const TArray<uint8>&, Out_Bytes);