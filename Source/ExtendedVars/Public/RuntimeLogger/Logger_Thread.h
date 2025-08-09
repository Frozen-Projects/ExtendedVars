#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

// Forward Declarations.
class FRunnableThread;
class URuntimeLoggerSubsystem;

class FRuntimeLogger_Thread : public FRunnable
{

public:

	// Sets default values for this actor's properties.
	FRuntimeLogger_Thread(URuntimeLoggerSubsystem* Manager);

	// Destructor.
	virtual ~FRuntimeLogger_Thread() override;

	// Game Thread.
	virtual bool Init() override;

	// Runnable Thread.
	virtual uint32 Run() override;

	// Game Thread.
	virtual void Stop() override;

private:

	bool bStartThread = false;
	FRunnableThread* RunnableThread = nullptr;
	const FString ThreadName = "RuntimeLogger_Thread";
	URuntimeLoggerSubsystem* LoggerSubsystem = nullptr;

};