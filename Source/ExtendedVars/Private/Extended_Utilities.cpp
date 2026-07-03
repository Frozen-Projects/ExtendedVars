#include "Extended_BPLib.h"

#pragma region System

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

#pragma endregion System

#pragma region Profiling

void UExtendedVarsBPLibrary::GetMonitorNamesFromPowershell(FString& Out_Path, FString& Out_Params)
{
#ifdef _WIN64
    Out_Path = TEXT("powershell.exe");

    Out_Params =
        TEXT("-NoProfile -ExecutionPolicy Bypass -Command \"")
        TEXT("$Monitors = @(Get-CimInstance -Namespace root\\wmi -ClassName WmiMonitorID | ")
        TEXT("ForEach-Object { ")
        TEXT("$Manufacturer = -join ($_.ManufacturerName | Where-Object { $_ -ne 0 } | ForEach-Object { [char]$_ }); ")
        TEXT("$Name = -join ($_.UserFriendlyName | Where-Object { $_ -ne 0 } | ForEach-Object { [char]$_ }); ")
        TEXT("$Serial = -join ($_.SerialNumberID | Where-Object { $_ -ne 0 } | ForEach-Object { [char]$_ }); ")
        TEXT("[pscustomobject]@{ ")
        TEXT("Manufacturer = $Manufacturer; ")
        TEXT("Name = $Name; ")
        TEXT("FriendlyName = ($Manufacturer + '_' + $Name); ")
        TEXT("SerialNumber = $Serial; ")
        TEXT("InstanceName = $_.InstanceName ")
        TEXT("} ")
        TEXT("}); ")
        TEXT("[pscustomobject]@{ Monitors = $Monitors } | ConvertTo-Json -Depth 5 -Compress")
        TEXT("\"");
#endif
}

void UExtendedVarsBPLibrary::GetMonitorInfos(FJsonObjectWrapper& OutMonitorInfos)
{
#ifdef _WIN64
    DISPLAY_DEVICE AdapterDevices;
    AdapterDevices.cb = sizeof(AdapterDevices);
    int32 AdapterIndex = 0;

    TArray<TSharedPtr<FJsonValue>> Details;

    while (EnumDisplayDevicesW(NULL, AdapterIndex, &AdapterDevices, EDD_GET_DEVICE_INTERFACE_NAME))
    {
        std::wstring AdapterName = AdapterDevices.DeviceName;

        DISPLAY_DEVICE MonitorDevices;
        MonitorDevices.cb = sizeof(MonitorDevices);
        int32 MonitorIndex = 0;

        while (EnumDisplayDevicesW(AdapterName.c_str(), MonitorIndex, &MonitorDevices, 0))
        {
            FJsonObjectWrapper EachMonitor;
            EachMonitor.JsonObject->SetStringField(TEXT("MonitorID"), AdapterDevices.DeviceName);
            EachMonitor.JsonObject->SetStringField(TEXT("MonitorKey"), MonitorDevices.DeviceKey);
            EachMonitor.JsonObject->SetStringField(TEXT("MonitorName"), MonitorDevices.DeviceName);
            EachMonitor.JsonObject->SetStringField(TEXT("MonitorString"), MonitorDevices.DeviceString);

            Details.Add(MakeShareable(new FJsonValueObject(EachMonitor.JsonObject)));
            MonitorIndex++;
        }

        AdapterIndex++;
    }

    OutMonitorInfos.JsonObject->SetArrayField(TEXT("Details"), Details);
#endif
}

void UExtendedVarsBPLibrary::GetDesktopResolution(int32 MonitorIndex, FVector2D& PrimaryResolution, FVector2D& TotalResolution, FVector2D& MonitorStart, FVector2D& MonitorResolution, float& MonitorDPI)
{
    FDisplayMetrics Display;
    FDisplayMetrics::RebuildDisplayMetrics(Display);

    PrimaryResolution.X = Display.PrimaryDisplayWidth;
    PrimaryResolution.Y = Display.PrimaryDisplayHeight;

    TotalResolution.X = Display.VirtualDisplayRect.Right - Display.VirtualDisplayRect.Left;
    TotalResolution.Y = Display.VirtualDisplayRect.Bottom - Display.VirtualDisplayRect.Top;

    const FMonitorInfo Monitor = Display.MonitorInfo[MonitorIndex];
    MonitorStart.X = Monitor.WorkArea.Left;
    MonitorStart.Y = Monitor.WorkArea.Top;

    MonitorResolution.X = Monitor.DisplayRect.Right - Monitor.DisplayRect.Left;
    MonitorResolution.Y = Monitor.DisplayRect.Bottom - Monitor.DisplayRect.Top;

    MonitorDPI = Monitor.DPI;
}

void UExtendedVarsBPLibrary::GetCPU(FString& CPUBrand, int32& CoreCount, int32& ThreadCount)
{
    CPUBrand = FPlatformMisc::GetCPUBrand();
    CoreCount = FPlatformMisc::NumberOfCores();
    ThreadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
}

void UExtendedVarsBPLibrary::GetAppPerformanceMetrics(int32& OutFPS, float& OutRenderThreadTime, float& OutGameThreadTime, float& OutGPUTime)
{
    OutFPS = FMath::TruncToInt32(1000 / (1000 * FApp::GetDeltaTime()));

    extern RENDERCORE_API uint32 GRenderThreadTime;
    extern RENDERCORE_API uint32 GGameThreadTime;
    OutRenderThreadTime = FPlatformTime::ToMilliseconds(GRenderThreadTime);
    OutGameThreadTime = FPlatformTime::ToMilliseconds(GGameThreadTime);
    OutGPUTime = FGenericPlatformTime::ToMilliseconds(RHIGetGPUFrameCycles());
}

void UExtendedVarsBPLibrary::GetNetworkInfos(TArray<FString>& Out_Adapters, FString& OutHostname, FString& OutHostAddr, FString& OutMac)
{
    /*
    * Windows Only Version

    char name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(name);
    GetComputerNameA(name, &size);
    */

    // Create and initialize platform socket subsystem.
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    // Get Device Name as host name.
    SocketSubsystem->GetHostName(OutHostname);

    // Get all active adapters.
    TArray<TSharedPtr<FInternetAddr>> Array_Adapters;
    SocketSubsystem->GetLocalAdapterAddresses(Array_Adapters);

    for (int32 IndexAddress = 0; IndexAddress < Array_Adapters.Num(); IndexAddress++)
    {
        Out_Adapters.Add(Array_Adapters[IndexAddress].ToSharedRef().Get().ToString(false));
    }

    // Get Local IP Address as host address.
    bool canBind = false;
    TSharedRef<FInternetAddr> HostAddress = SocketSubsystem->GetLocalHostAddr(*GLog, canBind);
    OutHostAddr = HostAddress->ToString(false);

    // Get MAC Address
#ifdef _WIN64

    DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);

    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(AdapterInfo);
        AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);

        if (AdapterInfo == NULL)
        {
            return;
        }
    }

    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
    {
        char mac_addr[18];
        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

        while (pAdapterInfo)
        {
            sprintf_s(mac_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
                pAdapterInfo->Address[0],
                pAdapterInfo->Address[1],
                pAdapterInfo->Address[2],
                pAdapterInfo->Address[3],
                pAdapterInfo->Address[4],
                pAdapterInfo->Address[5]);

            pAdapterInfo = pAdapterInfo->Next;
        }

        OutMac = mac_addr;

        free(AdapterInfo);
        AdapterInfo = nullptr;

        return;
    }
#endif
}

void UExtendedVarsBPLibrary::GetCpuFromPowershell(FString& Out_Path, FString& Out_Params)
{
#ifdef _WIN64
    Out_Path = TEXT("powershell.exe");
    Out_Params = TEXT("-NoProfile -ExecutionPolicy Bypass -Command Get-CimInstance Win32_Processor | Select-Object Caption,DeviceID,Name,NumberOfCores,MaxClockSpeed,Status | ConvertTo-Json");
#endif
}

void UExtendedVarsBPLibrary::GetGpuFromPowershell(FString& Out_Path, FString& Out_Params)
{
#ifdef _WIN64
    Out_Path = TEXT("powershell.exe");
    Out_Params = TEXT("-NoProfile -ExecutionPolicy Bypass -Command Get-CimInstance Win32_VideoController | Select-Object Name,VideoProcessor,AdapterCompatibility,PNPDeviceID,DriverVersion,DriverDate,VideoModeDescription,CurrentHorizontalResolution,CurrentVerticalResolution,CurrentRefreshRate,AdapterRAM,Status | ConvertTo-Json");
#endif
}

void UExtendedVarsBPLibrary::GetNetworkFromPowershell(FString& Out_Path, FString& Out_Params)
{
#ifdef _WIN64
    Out_Path = TEXT("powershell.exe");

    const FString Command = TEXT("$Adapters = @(Get-CimInstance Win32_NetworkAdapterConfiguration | Select-Object Description, MACAddress, DHCPEnabled, DHCPServer, DNSHostName, DNSDomain, DNSDomainSuffixSearchOrder, DNSServerSearchOrder, IPAddress, IPSubnet, DefaultIPGateway, InterfaceIndex); [PSCustomObject]@{ NetworkAdapters = $Adapters } | ConvertTo-Json -Depth 1");
    Out_Params = FString::Printf(TEXT("-NoProfile -ExecutionPolicy Bypass -Command \"%s\""), *Command);
#endif
}

void UExtendedVarsBPLibrary::SendPingWithPowershell(FString& Out_Path, FString& Out_Params, const FString IPAddress, int32 PingCount, uint8 PackageBytes)
{
#ifdef _WIN64
    Out_Path = TEXT("powershell.exe");
    Out_Params = FString::Printf(TEXT("-NoProfile -ExecutionPolicy Bypass -Command \"Test-Connection -ComputerName '%s' -Count %d -BufferSize %d | ForEach-Object { [pscustomobject]@{ Address = $_.Address; ResponseTimeMs = $_.ResponseTime; StatusCode = $_.StatusCode; PackageBytes = %d; ReplyBytes = $_.ReplySize; Success = ($_.StatusCode -eq 0) } | ConvertTo-Json -Compress }\""), *IPAddress, PingCount, PackageBytes, PackageBytes);
#endif
}

#pragma endregion Profiling