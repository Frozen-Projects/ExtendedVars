// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Containers/Queue.h"
#include "JsonObjectWrapper.h"
#include "JsonUtilities.h"

#include "RuntimeLogger.generated.h"

// Forward Declarations.
class FRuntimeLogger_Thread;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDelegateRuntimeLogger, FString, Out_UUID, FString, Out_Log);

UCLASS()
class EXTENDEDVARS_API URuntimeLoggerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	FRuntimeLogger_Thread* LoggerThread = nullptr;
	
	TQueue<FString> LogQueue;
	TMap<FString, FString> LogDb;

	TUniquePtr<IFileHandle> LogFileHandle;

	virtual void OpenLogFile();
	virtual void CleanupLogs();

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void RecordMessages();

	/**
	 * Logs a message with a unique identifier.
	 * @param Message A map containing key-value pairs of the message to log. Ideally, it should contain relevant information such as plugin name, function name, and any other details.
	 * @return A unique identifier for the logged message.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Logger")
	virtual FString LogMessage(TMap<FString, FString> Message);

	UFUNCTION(BlueprintCallable, Category = "Runtime Logger")
	virtual void StartLogging();

	UFUNCTION(BlueprintCallable, Category = "Runtime Logger")
	virtual TMap<FString, FString> GetLogDb();

	UPROPERTY(BlueprintAssignable, Category = "FF_RealSense")
	FDelegateRuntimeLogger Delegate_Runtime_Logger;

	/*
	* This function is just a placeholder to demonstrate the use of __FUNCSIG__ Don't use it in production code.
	* __FUNCSIG__ doesn't work on blueprints. So, you have to fill the function name manually.
	*/
	virtual void SampleFunction()
	{
		TMap<FString, FString> LogData;
		LogData.Add("PluginName", TEXT("ExtendedVars"));
		LogData.Add("FunctionName", FString(ANSI_TO_TCHAR(__FUNCSIG__)));
		LogData.Add("Details", TEXT("This is a sample function for logging purposes."));

		const FString LogString = this->LogMessage(LogData);
		UE_LOG(LogTemp, Log, TEXT("SampleFunction executed: %s"), *LogString);
	}
};