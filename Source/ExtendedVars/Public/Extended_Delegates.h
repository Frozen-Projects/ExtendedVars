#pragma once

#include "CoreMinimal.h"
#include "Extended_Delegates.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegatePipes, FString, Out_Result);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateTerminalResult, bool, bIsSuccessed, FJsonObjectWrapper, Out_Result);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateMonitorNames, bool, bIsSuccessed, FString, Out_Code, const TArray<FString>&, Out_Monitor_Names);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateImageBuffer, bool, bIsSuccessfull, FString, ErrorCode, const TArray<uint8>&, Out_Bytes);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateTexture2D, bool, bIsSuccessfull, FString, ErrorCode, UTexture2D*, Out);