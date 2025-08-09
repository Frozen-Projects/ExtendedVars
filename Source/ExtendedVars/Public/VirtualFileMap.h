// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Extended_Includes.h"

#include "VirtualFileMap.generated.h"

struct FFileMapHandles
{

public:

	HANDLE MappingHandle = nullptr;
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

	TMap<FString, FFileMapHandles> VirtualFileMaps;
	
	virtual std::string GetErrorString(DWORD ErrorCode);
	virtual bool FileAddCallback(FString& Out_Code, FString FileName, TMap<FString, FString> Headers, const TArray<uint8>& FileData);
	virtual void FileRemoveCallback(FString FileName);
	virtual void CleanUpFileHandles();

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "VirtualFileMap")
	bool AddFile(FString& Out_Code, FString FileName, TMap<FString, FString> Headers, const TArray<uint8>& FileData, bool bAllowUpdate);

	UFUNCTION(BlueprintCallable, Category = "VirtualFileMap")
	bool RemoveFile(FString& Out_Code, FString FileName);

	UFUNCTION(BlueprintCallable, Category = "VirtualFileMap")
	bool GetFile(TArray<uint8>& Out_Buffer, FString& Out_Code, FString FileName);

	UFUNCTION(BlueprintCallable, Category = "VirtualFileMap")
	bool GetFileHeader(TMap<FString, FString>& Out_Headers, FString& Out_Code, FString FileName);

	UFUNCTION(BlueprintCallable, Category = "VirtualFileMap")
	bool GetFileNames(TArray<FString>& Out_Names, FString& Out_Code);

};
