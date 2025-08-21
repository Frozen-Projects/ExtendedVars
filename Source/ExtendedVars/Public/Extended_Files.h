#pragma once

#include "Extended_Files.generated.h"

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

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateFolderContents, bool, bIsSuccessfull, FString, ErrorCode, const TArray<FFolderContents>&, Out_Contents);