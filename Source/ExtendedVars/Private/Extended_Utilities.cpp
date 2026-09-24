#include "Extended_BPLib.h"

bool UExtendedVarsBPLibrary::ReplaceObject(UPARAM(ref)UObject*& Old, UPARAM(ref)UObject*& New)
{
    if (!IsValid(New))
    {
        return false;
    }

    if (IsValid(Old))
    {
        Old->ConditionalBeginDestroy();
        Old = nullptr;
    }

    Old = New;
    return true;
}

bool UExtendedVarsBPLibrary::GetWorkingDir(FString& DirPath)
{
#ifdef _WIN64

    DirPath = FWindowsPlatformProcess::GetCurrentWorkingDirectory();
    return true;

#else 

    return false;

#endif
}

bool UExtendedVarsBPLibrary::DisablePluginAtRuntime(const FString PluginName)
{
    return FGenericPlatformMisc::ShouldDisablePluginAtRuntime(PluginName);
}

bool UExtendedVarsBPLibrary::ReadRegeditValue(FString& OutRegedit, ERegeditRoot RegistryRoot, const FString KeyName, const FString ValueName)
{
#ifdef _WIN64
    // Initial varlables.
    HKEY hKey;
    LONG Result;

    // Switch for search directory.
    switch (RegistryRoot)
    {
    case ERegeditRoot::Machine:
        Result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, *KeyName, 0, KEY_READ, &hKey);
        break;

    case ERegeditRoot::User:
        Result = RegOpenKeyExW(HKEY_CURRENT_USER, *KeyName, 0, KEY_READ, &hKey);
        break;

    default:
        Result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, *KeyName, 0, KEY_READ, &hKey);
        break;
    }

    if (Result != ERROR_SUCCESS)
    {
        // Handle error. 
    }

    // Execute search.
    TCHAR* Buffer = (TCHAR*)malloc(MAX_PATH);
    DWORD BufferSize = sizeof(TCHAR) * MAX_PATH;
    HRESULT hResult = RegQueryValueEx(hKey, *ValueName, 0, nullptr, reinterpret_cast<LPBYTE>(Buffer), &BufferSize);

    if (hResult != ERROR_SUCCESS)
    {
        // Handle error. 
    }

    OutRegedit = FString(Buffer);

    free(Buffer);
    Buffer = nullptr;

    return true;

#else

    return false;

#endif
}

void UExtendedVarsBPLibrary::ForceShutdownPC()
{
#ifdef _WIN64
    system("c:\\windows\\system32\\shutdown /p /f");
#endif
}

bool UExtendedVarsBPLibrary::RestartApplication()
{

#ifdef _WIN64

    FWindowsPlatformMisc::RestartApplication();
    return true;

#else

    return false;

#endif
}

void UExtendedVarsBPLibrary::SplashScreenVisibility(bool HideSplashScreen)
{
#ifdef _WIN64
    FWindowsPlatformMisc::PlatformHandleSplashScreen(HideSplashScreen);
#endif
}

bool UExtendedVarsBPLibrary::SetClipboard(const FString& Input)
{
    if (Input.IsEmpty())
    {
        return false;
    }

#ifdef _WIN64
    FWindowsPlatformApplicationMisc::ClipboardCopy(*Input);
#elif __ANDROID__
    FAndroidApplicationMisc::ClipboardCopy(*Input);
#elif PLATFORM_LINUX
    FLinuxPlatformApplicationMisc::ClipboardCopy(*Input);
#elif PLATFORM_MAC
    FMacPlatformApplicationMisc::ClipboardCopy(*Input);
#elif PLATFORM_IOS
    FIOSPlatformApplicationMisc::ClipboardCopy(*Input);
#else
    return false;
#endif
    return true;
}

FString UExtendedVarsBPLibrary::GetClipboard()
{
    FString ClipboardContent;

#ifdef _WIN64
    FWindowsPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
#elif __ANDROID__
    FAndroidApplicationMisc::ClipboardPaste(ClipboardContent);
#elif PLATFORM_LINUX
    FLinuxPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
#elif PLATFORM_MAC
    FMacPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
#elif PLATFORM_IOS
    FIOSPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
#else
    return false;
#endif

    return ClipboardContent.IsEmpty() ? FString() : ClipboardContent;
}