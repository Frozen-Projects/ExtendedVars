#pragma once

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
};

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateBytes_32, bool, bIsSuccessfull, FString, ErrorCode, const TArray<uint8>&, Out_Bytes);