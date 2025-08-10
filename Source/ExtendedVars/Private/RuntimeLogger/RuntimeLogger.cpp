#include "RuntimeLogger/RuntimeLogger.h"
#include "RuntimeLogger/Logger_Thread.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/App.h"

void URuntimeLoggerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URuntimeLoggerSubsystem::Deinitialize()
{
	this->CleanupLogs();
	Super::Deinitialize();
}

void URuntimeLoggerSubsystem::OpenLogFile()
{
	const FString ProjectName = FApp::GetProjectName();
	FString SaveDir = UKismetSystemLibrary::GetProjectSavedDirectory() + "/" + ProjectName + "_RuntimeLogger_" + FDateTime::Now().ToString() + ".log";
	FPaths::MakePlatformFilename(SaveDir);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	this->LogFileHandle.Reset(PlatformFile.OpenWrite(*SaveDir, true, true));
}

void URuntimeLoggerSubsystem::CleanupLogs()
{
	if (this->LoggerThread)
	{
		delete this->LoggerThread;
	}

	if (this->LogFileHandle.IsValid())
	{
		this->LogFileHandle->Flush();
	}

	this->LoggerThread = nullptr;
	this->LogQueue.Empty();
	this->LogDb.Empty();
}

void URuntimeLoggerSubsystem::RecordMessages()
{
	FString RawString;

	if (this->LogQueue.Dequeue(RawString))
	{
		FJsonObjectWrapper TempJson;
		TempJson.JsonObjectFromString(RawString);

		const FString UUID = TempJson.JsonObject->GetStringField(TEXT("UUID"));
		TempJson.JsonObject->RemoveField(TEXT("UUID"));

		FString LogString;
		TempJson.JsonObjectToString(LogString);

		this->LogDb.Add(UUID, LogString);

		if (LogFileHandle.IsValid())
		{
			const FString LogLine = UUID + " = " + LogString + "\n";
			FTCHARToUTF8 Utf8(*LogLine);
			this->LogFileHandle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
		}

		AsyncTask(ENamedThreads::GameThread, [this, UUID, LogString]()
			{
				this->Delegate_Runtime_Logger.Broadcast(UUID, LogString);
			}
		);
	}
}

FString URuntimeLoggerSubsystem::LogMessage(TMap<FString, FString> Message)
{
	if (!Message.IsEmpty())
	{
		FJsonObjectWrapper TempJson;

		for (const TPair<FString, FString> EachParam : Message)
		{
			TempJson.JsonObject->SetStringField(EachParam.Key, EachParam.Value);
		}

		const FString UUID = FGuid::NewGuid().ToString();
		TempJson.JsonObject->SetStringField(TEXT("UUID"), UUID);

		const FString LogTime = FDateTime::Now().ToString();
		TempJson.JsonObject->SetStringField(TEXT("LogTime"), LogTime);

		FString LogString;
		TempJson.JsonObjectToString(LogString);
		
		if (!this->LogQueue.Enqueue(LogString))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Failed to enqueue log message ! : " + LogString);
			UE_LOG(LogTemp, Error, TEXT("Failed to enqueue log message ! : %s"), *LogString);
		}

		return UUID;
	}

	return FString();
}

void URuntimeLoggerSubsystem::StartLogging()
{
	this->OpenLogFile();
	this->LoggerThread = new FRuntimeLogger_Thread(this);
}

TMap<FString, FString> URuntimeLoggerSubsystem::GetLogDb()
{
	return this->LogDb;
}