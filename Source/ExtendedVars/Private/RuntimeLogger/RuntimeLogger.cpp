#include "RuntimeLogger/RuntimeLogger.h"
#include "RuntimeLogger/Logger_Thread.h"

void URuntimeLoggerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URuntimeLoggerSubsystem::Deinitialize()
{
	delete this->LoggerThread;
	this->LoggerThread = nullptr;
	this->LogQueue.Empty();
	this->LogDb.Empty();

	Super::Deinitialize();
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
	this->LoggerThread = new FRuntimeLogger_Thread(this);
}

TMap<FString, FString> URuntimeLoggerSubsystem::GetLogDb()
{
	return this->LogDb;
}