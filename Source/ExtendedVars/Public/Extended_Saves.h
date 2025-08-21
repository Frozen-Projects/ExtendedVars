#pragma once

#include "GameFramework/SaveGame.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Kismet/GameplayStatics.h"

#include "Extended_Saves.generated.h"

USTRUCT(BlueprintType)
struct EXTENDEDVARS_API FVariableContainer
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	UObject* Parent = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FName Name;

	UPROPERTY(BlueprintReadWrite)
	FString Serialized;

	UPROPERTY(BlueprintReadWrite)
	FString VariableType;

	UPROPERTY(BlueprintReadWrite)
	bool bIsNative = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsRooted = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsEditorOnly = false;

	UPROPERTY(BlueprintReadWrite)
	FDateTime RecordTime;

	bool operator == (const FVariableContainer& Other) const
	{
		return Parent == Other.Parent && Name == Other.Name && Serialized == Other.Serialized && VariableType == Other.VariableType && bIsNative == Other.bIsNative && bIsRooted == Other.bIsRooted && bIsEditorOnly == Other.bIsEditorOnly && RecordTime == Other.RecordTime;
	}

	bool operator != (const FVariableContainer& Other) const
	{
		return !(*this == Other);
	}

};

FORCEINLINE uint32 GetTypeHash(const FVariableContainer& Key)
{
	uint32 Hash_Parent = GetTypeHash(Key.Parent);
	uint32 Hash_Name = GetTypeHash(Key.Name);
	uint32 Hash_Serialized = GetTypeHash(Key.Serialized);
	uint32 Hash_VariableType = GetTypeHash(Key.VariableType);
	uint32 Hash_bIsNative = GetTypeHash(Key.bIsNative);
	uint32 Hash_bIsRooted = GetTypeHash(Key.bIsRooted);
	uint32 Hash_bIsEditorOnly = GetTypeHash(Key.bIsEditorOnly);
	uint32 Hash_RecordTime = GetTypeHash(Key.RecordTime);

	uint32 GenericHash;
	FMemory::Memset(&GenericHash, 0, sizeof(uint32));
	GenericHash = HashCombine(GenericHash, Hash_Parent);
	GenericHash = HashCombine(GenericHash, Hash_Name);
	GenericHash = HashCombine(GenericHash, Hash_Serialized);
	GenericHash = HashCombine(GenericHash, Hash_VariableType);
	GenericHash = HashCombine(GenericHash, Hash_bIsNative);
	GenericHash = HashCombine(GenericHash, Hash_bIsRooted);
	GenericHash = HashCombine(GenericHash, Hash_bIsEditorOnly);
	GenericHash = HashCombine(GenericHash, Hash_RecordTime);

	return GenericHash;
}

USTRUCT(BlueprintType)
struct EXTENDEDVARS_API FVariablePool
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	TArray<FVariableContainer> Containers;

	bool operator == (const FVariablePool& Other) const
	{
		return Containers == Other.Containers;
	}

	bool operator != (const FVariablePool& Other) const
	{
		return !(*this == Other);
	}

};

FORCEINLINE uint32 GetTypeHash(const FVariablePool& Key)
{
	uint32 Hash_Containers = GetTypeHash(Key.Containers);

	uint32 GenericHash;
	FMemory::Memset(&GenericHash, 0, sizeof(uint32));

	return GenericHash;
}

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateSaveToFile, bool, bIsSuccessfull, FString, ErrorCode);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateSaveToMemory, bool, bIsSuccessfull, FString, ErrorCode, const TArray<uint8>&, Out_Buffer);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateLoadSave, bool, bIsSuccessfull, FString, ErrorCode, USaveGame*, Out_Save);