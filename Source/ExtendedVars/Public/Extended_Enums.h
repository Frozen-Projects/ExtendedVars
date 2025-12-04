#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EStringToDate : uint8
{
	None			UMETA(DisplayName = "None"),
	UnrealDateTime	UMETA(DisplayName = "FDateTime.ToString"),
	Http			UMETA(DisplayName = "Http"),
	Iso8601			UMETA(DisplayName = "Iso8601"),
	Custom			UMETA(DisplayName = "Custom"),
};
ENUM_CLASS_FLAGS(EStringToDate)

UENUM(BlueprintType)
enum class EByteConverters : uint8
{
	None			UMETA(DisplayName = "None"),
	Base64			UMETA(DisplayName = "Base64"),
	Base64Url		UMETA(DisplayName = "Base64Url"),
	Hex_Full		UMETA(DisplayName = "Hex Full"),
	Hex_Indexed		UMETA(DisplayName = "Hex Indexed"),
	UTF8_UE			UMETA(DisplayName = "UTF8 (UE Method)"),
	UTF8_Native		UMETA(DisplayName = "UTF8 (Native Method)"),
};
ENUM_CLASS_FLAGS(EByteConverters)

UENUM(BlueprintType)
enum class EImageExtensions : uint8
{
	Ext_None		UMETA(DisplayName = "None"),
	Ext_BMP			UMETA(DisplayName = "BMP"),
	EXT_JPEG		UMETA(DisplayName = "JPG"),
	EXT_PNG			UMETA(DisplayName = "PNG"),
};
ENUM_CLASS_FLAGS(EImageExtensions)

UENUM(BlueprintType)
enum class EGammaSpaceBp : uint8
{
	Invalid		UMETA(DisplayName = "Invalid"),
	Linear		UMETA(DisplayName = "Linear"),
	Pow22		UMETA(DisplayName = "Pow22"),
	sRGB		UMETA(DisplayName = "sRGB"),
};
ENUM_CLASS_FLAGS(EGammaSpaceBp)

UENUM(BlueprintType)
enum class ERegeditRoot : uint8
{
	Machine		UMETA(DisplayName = "Machine"),
	User		UMETA(DisplayName = "User"),
};
ENUM_CLASS_FLAGS(ERegeditRoot)

UENUM(BlueprintType)
enum class ETerminalTarget : uint8
{
	CMD			UMETA(DisplayName = "CMD"),
	Powershell	UMETA(DisplayName = "Powershell"),
};
ENUM_CLASS_FLAGS(ETerminalTarget)

UENUM(BlueprintType)
enum class ETerminalVisibility : uint8
{
	CMD_Hide			UMETA(DisplayName = "CMD_Hide"),
	CMD_ShowKillAfter	UMETA(DisplayName = "CMD_ShowKillAfter"),
	CMD_ShowLetItLive	UMETA(DisplayName = "CMD_ShowLetItLive"),
	PS_Hide				UMETA(DisplayName = "PS_Hide"),
	PS_ShowKillAfter	UMETA(DisplayName = "PS_ShowKillAfter"),
	PS_ShowLetItLive	UMETA(DisplayName = "PS_ShowLetItLive"),
};
ENUM_CLASS_FLAGS(ETerminalVisibility)

UENUM(BlueprintType)
enum class EPipeThreadPriority : uint8
{
	HighThreadNormalTask		UMETA(DisplayName = "High Priority Thread for Normal Task"),
	HighThreadHighTask			UMETA(DisplayName = "High Priority Thread for High Task"),
	NormalThreadNormalTask		UMETA(DisplayName = "Normal Priority Thread for Normal Task"),
	NormalThreadHighTask		UMETA(DisplayName = "Normal Priority Thread for High Task"),
	BackgroundNormal			UMETA(DisplayName = "Background Thread for Normal Task"),
	BackgroundHigh				UMETA(DisplayName = "Background Thread for High Task"),
};
ENUM_CLASS_FLAGS(EPipeThreadPriority)