// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "JsonObjectWrapper.h"
#include "JsonUtilities.h"

THIRD_PARTY_INCLUDES_START
#include <string>
#include <sstream>

#ifdef _WIN64
#define WIN32_LEAN_AND_MEAN
#include "Windows/AllowWindowsPlatformTypes.h"
#include <memoryapi.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

THIRD_PARTY_INCLUDES_END

#include "VirtualFileMap.generated.h"

struct FFileMapHandles
{

public:

#ifdef _WIN64
	HANDLE MappingHandle = nullptr;
#endif // _WIN64

	void* MappedMemory = nullptr;
	size_t MappedSize = 0;
	FJsonObjectWrapper Header;
};

UCLASS()
class EXTENDEDVARS_API UVirtualFileSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	mutable FCriticalSection VFM_Guard;

#ifdef _WIN64

	TMap<FString, FFileMapHandles> VirtualFileMaps;
	virtual std::string GetErrorString(DWORD ErrorCode);

#endif // _WIN64

	virtual bool FileAddCallback(FString& Out_Code, FString FileName, TMap<FString, FString> Headers, const TArray<uint8>& FileData);
	virtual void FileRemoveCallback(FString FileName);
	virtual void CleanUpFileHandles();

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest | Extended Variables | Virtual File Map")
	bool AddFile(FString& Out_Code, FString FileName, TMap<FString, FString> Headers, const TArray<uint8>& FileData, bool bAllowUpdate);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest | Extended Variables | Virtual File Map")
	bool RemoveFile(FString& Out_Code, FString FileName);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest | Extended Variables | Virtual File Map")
	bool GetFile(TArray<uint8>& Out_Buffer, FString& Out_Code, FString FileName);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest | Extended Variables | Virtual File Map")
	bool GetFileHeader(TMap<FString, FString>& Out_Headers, FString& Out_Code, FString FileName);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest | Extended Variables | Virtual File Map")
	bool GetFileNames(TArray<FString>& Out_Names, FString& Out_Code);

};
