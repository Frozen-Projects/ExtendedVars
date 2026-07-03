#include "Extended_BPLib.h"

#pragma region External_Apps

bool UExtendedVarsBPLibrary::RunExternalApp_Internal(FJsonObjectWrapper& Out_Code, FString AppPath, const FString& Parameters, bool bIsAdmin)
{
    Out_Code.JsonObject->SetStringField(TEXT("PluginName"), TEXT("Extended Variables"));
    Out_Code.JsonObject->SetStringField(TEXT("FunctionName"), TEXT(__FUNCTION__));

    FPaths::NormalizeFilename(AppPath);

    if (AppPath.IsEmpty())
    {
        Out_Code.JsonObject->SetStringField(TEXT("Error"), TEXT("AppPath is empty."));
        return false;
    }

    FString Result, ErrorString;
    int32 ProcessId = 0;
    bool bIsSuccessful = false;

    const FString Temp_Path = FString::Printf(TEXT("\"%s\""), *AppPath);
    bIsSuccessful = bIsAdmin ? FPlatformProcess::ExecElevatedProcess(*Temp_Path, *Parameters, &ProcessId) : FPlatformProcess::ExecProcess(*Temp_Path, *Parameters, &ProcessId, &Result, &ErrorString);

    Out_Code.JsonObject->SetStringField(TEXT("AppPath"), AppPath);
    Out_Code.JsonObject->SetStringField(TEXT("Parameters"), Parameters);
    Out_Code.JsonObject->SetNumberField(TEXT("ProcessId"), ProcessId);

    FJsonObjectWrapper ResultJson;
    TArray<TSharedPtr<FJsonValue>> StdOutArray;

    if (ResultJson.JsonObjectFromString(Result))
    {
        Out_Code.JsonObject->SetObjectField(TEXT("Result"), ResultJson.JsonObject);
    }

    else if (UExtendedVarsBPLibrary::StringToJsonObjectArray(StdOutArray, Result))
    {
        Out_Code.JsonObject->SetArrayField(TEXT("Result"), StdOutArray);
    }

    else
    {
        Out_Code.JsonObject->SetStringField(TEXT("Result"), Result);
    }

    Out_Code.JsonObject->SetStringField(TEXT("Error"), ErrorString);

    return bIsSuccessful;
}

void UExtendedVarsBPLibrary::WindowsTerminalHelper(FString& Out_Path, FString& Out_Params, const FString& In_Params, bool bIsPowerShell)
{
    Out_Path = bIsPowerShell ? TEXT("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe") : TEXT("C:\\Windows\\System32\\cmd.exe");
    Out_Params = bIsPowerShell ? FString::Printf(TEXT("-Command %s"), *In_Params) : FString::Printf(TEXT("/c %s"), *In_Params);
}

void UExtendedVarsBPLibrary::RunExternalApp(FDelegateTerminalResult DelegateResult, FString AppPath, const FString& Parameters, bool bIsAdmin)
{
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [DelegateResult, AppPath, Parameters, bIsAdmin]()
        {
            FJsonObjectWrapper Out_Code;
            const bool bIsSuccessful = UExtendedVarsBPLibrary::RunExternalApp_Internal(Out_Code, AppPath, Parameters, bIsAdmin);

            AsyncTask(ENamedThreads::GameThread, [DelegateResult, Out_Code, bIsSuccessful]()
                {
                    DelegateResult.ExecuteIfBound(bIsSuccessful, Out_Code);
                }
            );
        }
    );
}

void UExtendedVarsBPLibrary::RunExternalAppWithPipes(FDelegateTerminalResult DelegateResult, FDelegatePipes DelegatePipe, FString AppPath, const FString& Parameters)
{
    FPaths::NormalizeFilename(AppPath);

    if (AppPath.IsEmpty())
    {
        FJsonObjectWrapper Out_Code;
        Out_Code.JsonObject->SetStringField(TEXT("PluginName"), TEXT("Extended Variables"));
        Out_Code.JsonObject->SetStringField(TEXT("FunctionName"), TEXT(__FUNCTION__));
        Out_Code.JsonObject->SetStringField(TEXT("Error"), TEXT("AppPath is empty."));

        DelegateResult.ExecuteIfBound(false, Out_Code);
        return;
    }

    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [DelegateResult, DelegatePipe, AppPath, Parameters]()
        {
            FJsonObjectWrapper Out_Code;
            Out_Code.JsonObject->SetStringField(TEXT("PluginName"), TEXT("Extended Variables"));
            Out_Code.JsonObject->SetStringField(TEXT("FunctionName"), TEXT(__FUNCTION__));

            void* PipeRead;
            void* PipeWrite;
            int32 ProcessId = 0;

            if (!FWindowsPlatformProcess::CreatePipe(PipeRead, PipeWrite))
            {
                AsyncTask(ENamedThreads::GameThread, [DelegateResult, &Out_Code]()
                    {
                        Out_Code.JsonObject->SetStringField(TEXT("Error"), TEXT("Pipe creation failed !"));
                        DelegateResult.ExecuteIfBound(false, Out_Code);
                    }
                );

                return;
            }

            FProcHandle Process = FPlatformProcess::CreateProc(*AppPath, *Parameters, false, true, true, (uint32_t*)&ProcessId, 0, nullptr, PipeWrite, nullptr);

            if (!Process.IsValid())
            {
                Out_Code.JsonObject->SetStringField(TEXT("Error"), TEXT("Process creation failed !"));

                AsyncTask(ENamedThreads::GameThread, [DelegateResult, Out_Code]()
                    {
                        DelegateResult.ExecuteIfBound(false, Out_Code);
                    }
                );

                return;
            }

            TArray<TSharedPtr<FJsonValue>> PipeResult;

            while (FWindowsPlatformProcess::IsProcRunning(Process))
            {
                const FString PipeString = FWindowsPlatformProcess::ReadPipe(PipeRead);

                if (PipeString.IsEmpty() || PipeString.Equals(LINE_TERMINATOR))
                {
                    continue;
                }

                FJsonObjectWrapper PipeJson;

                if (PipeJson.JsonObjectFromString(PipeString))
                {
                    PipeResult.Add(MakeShareable(new FJsonValueObject(PipeJson.JsonObject)));
                }

                else
                {
                    PipeResult.Add(MakeShareable(new FJsonValueString(PipeString)));
                }

                AsyncTask(ENamedThreads::GameThread, [DelegatePipe, PipeString]()
                    {
                        DelegatePipe.ExecuteIfBound(PipeString);
                    }
                );
            }

            int32 ReturnCode = 0;
            FPlatformProcess::GetProcReturnCode(Process, &ReturnCode);

            FWindowsPlatformProcess::ClosePipe(PipeRead, PipeWrite);
            FWindowsPlatformProcess::CloseProc(Process);

            Out_Code.JsonObject->SetStringField(TEXT("AppPath"), AppPath);
            Out_Code.JsonObject->SetStringField(TEXT("Parameters"), Parameters);
            Out_Code.JsonObject->SetNumberField(TEXT("ProcessId"), ProcessId);
            Out_Code.JsonObject->SetNumberField(TEXT("ReturnCode"), ReturnCode);

            Out_Code.JsonObject->SetArrayField(TEXT("Result"), PipeResult);

            AsyncTask(ENamedThreads::GameThread, [DelegateResult, Out_Code]()
                {
                    DelegateResult.ExecuteIfBound(true, Out_Code);
                }
            );
        }
    );
}

#pragma endregion External_Apps