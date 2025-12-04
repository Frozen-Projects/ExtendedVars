#pragma once

#include "Networking.h"								// You need to add this before everything to prevent "SetPortW: is not a member of "FInternetAddr" problem. Source: https://forums.unrealengine.com/t/setportw-is-not-a-member-of-finternetaddr/435236/2
#include "Engine.h"
#include "SocketSubsystem.h"                        // Network Infos.
#include <IPAddress.h>						        // Network Infos.

// Math
#include "Algo/Reverse.h"							// Reverse array
#include "Kismet/KismetMathLibrary.h"

// Strings
#include "Containers/UnrealString.h"				// Hex to Bytes + Bytes to Hex
#include "Kismet/KismetStringLibrary.h"

// File System
#include "Misc/Paths.h"								// RunSubProcess-> FPaths and other functions's get extension features.
#include "Misc/Base64.h"							// Encode & Decode Base64
#include "Misc/FileHelper.h"						// Load File to Array
#include "HAL/FileManager.h"						// Save texture as bitmap and select file from dialog.
#include "HAL/FileManagerGeneric.h"

// Widgets
#include "Slate/WidgetRenderer.h"					// Widget to Texture2D
#include "Runtime/UMG/Public/UMG.h"					// Widget to Texture2D

// Render
#include "ImageUtils.h"								// Save Texture as File
#include "RHICommandList.h"							// Encode Texture as JPG/BMP/PNG
#include "IImageWrapper.h"							// Encode Texture as JPG/BMP/PNG
#include "IImageWrapperModule.h"					// Encode Texture as JPG/BMP/PNG
#include "Kismet/KismetRenderingLibrary.h"			// Texture2D
#include "MediaTexture.h"							// Media Texture

#include "JsonObjectWrapper.h"
#include "JsonUtilities.h"

// Profiling
#include "GenericPlatform/GenericPlatformMisc.h"    // UE Platform Functions.
#include "GenericPlatform/GenericApplication.h"     // GetMonitorInformations > FDisplayMetrics.
#include "GenericPlatform/GenericPlatformFile.h"    // GetMonitorInformations > Remove File (Remove Poweshell result after read).
#include "GenericPlatform/GenericPlatformTime.h"    // GetAppPerformanceMetrics > FGenericPlatformTime::ToMilliseconds(RHIGetGPUFrameCycles())
#include "Misc/SecureHash.h"                        // DeviceID MD5 Hash creation.
#include "Misc/DateTime.h"

THIRD_PARTY_INCLUDES_START
#include <iomanip>
#include <sstream>
#include <iostream>                                 // GetMonitorInformations (Create Powershell File)
#include <fstream>                                  // GetMonitorInformations (Create Powershell File)
#include <string>
#include <chrono>

#ifdef _WIN64
#include "Windows/WindowsHWrapper.h"                // Regedit error handling.
#include <array>                                    // CMD STDOUT buffer creation.
#include <winreg.h>                                 // Regedit access.
#include "iphlpapi.h"
#endif
THIRD_PARTY_INCLUDES_END
