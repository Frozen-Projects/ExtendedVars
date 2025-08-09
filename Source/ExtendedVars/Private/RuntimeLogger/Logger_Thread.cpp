#include "RuntimeLogger/Logger_Thread.h"
#include "RuntimeLogger/RuntimeLogger.h"

FRuntimeLogger_Thread::FRuntimeLogger_Thread(URuntimeLoggerSubsystem* Manager)
{
	if (!IsValid(Manager))
	{
		return;
	}

	this->LoggerSubsystem = Manager;
	this->RunnableThread = FRunnableThread::Create(this, *this->ThreadName);
}

FRuntimeLogger_Thread::~FRuntimeLogger_Thread()
{
	if (RunnableThread)
	{
		RunnableThread->Kill(true);
		delete RunnableThread;
		RunnableThread = nullptr;
	}
}

bool FRuntimeLogger_Thread::Init()
{
	this->bStartThread = true;
	return true;
}

uint32 FRuntimeLogger_Thread::Run()
{
	while (this->bStartThread)
	{
		if (IsValid(this->LoggerSubsystem))
		{
			this->LoggerSubsystem->RecordMessages();
		}
	}
	
	return 0;
}

void FRuntimeLogger_Thread::Stop()
{
	this->bStartThread = false;
}