#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EStandartFonts : uint8
{
	Helvetica				UMETA(DisplayName = "Helvetica/Arial"),
	Helvetica_Italic		UMETA(DisplayName = "Helvetica/Arial Italic"),
	Helvetica_Bold			UMETA(DisplayName = "Helvetica/Arial Bold"),
	Helvetica_BoldItalic	UMETA(DisplayName = "Helvetica/Arial Bold Italic"),

	Times_Roman				UMETA(DisplayName = "Times Roman"),
	Times_Bold				UMETA(DisplayName = "Times Bold"),
	Times_BoldItalic		UMETA(DisplayName = "Times Bold Italic"),
	Times_Italic			UMETA(DisplayName = "Times Italic"),

	Courier					UMETA(DisplayName = "Courier"),
	Courier_Bold			UMETA(DisplayName = "Courier Bold"),
	Courier_Oblique			UMETA(DisplayName = "Courier Oblique"),
	Courier_BoldOblique		UMETA(DisplayName = "Courier Bold Oblique"),

	Symbol					UMETA(DisplayName = "Symbol"),
	ZapfDingbats			UMETA(DisplayName = "ZapfDingbats"),
};
ENUM_CLASS_FLAGS(EStandartFonts)

UENUM(BlueprintType)
enum class EExternalFonts : uint8
{
	TrueType				UMETA(DisplayName = "True Type"),
	TYPE1					UMETA(DisplayName = "Type 1"),
};
ENUM_CLASS_FLAGS(EExternalFonts)

UENUM(BlueprintType)
enum class EFontEncodings : uint8
{
	FONT_SPECIFIC  UMETA(DisplayName = "FONT_SPECIFIC"),
	STANDARD       UMETA(DisplayName = "STANDARD"),
	MAC_ROMAN      UMETA(DisplayName = "MAC_ROMAN"),
	WIN_ANSI       UMETA(DisplayName = "WIN_ANSI"),
	ISO8859_2      UMETA(DisplayName = "ISO8859_2"),
	ISO8859_3      UMETA(DisplayName = "ISO8859_3"),
	ISO8859_4      UMETA(DisplayName = "ISO8859_4"),
	ISO8859_5      UMETA(DisplayName = "ISO8859_5"),
	ISO8859_6	   UMETA(DisplayName = "ISO8859_6"),
	ISO8859_7      UMETA(DisplayName = "ISO8859_7"),
	ISO8859_8      UMETA(DisplayName = "ISO8859_8"),
	ISO8859_9      UMETA(DisplayName = "ISO8859_9"),
	ISO8859_10     UMETA(DisplayName = "ISO8859_10"),
	ISO8859_11     UMETA(DisplayName = "ISO8859_11"),
	ISO8859_13     UMETA(DisplayName = "ISO8859_13"),
	ISO8859_14     UMETA(DisplayName = "ISO8859_14"),
	ISO8859_15     UMETA(DisplayName = "ISO8859_15"),
	ISO8859_16     UMETA(DisplayName = "ISO8859_16"),
	CP1250         UMETA(DisplayName = "CP1250"),
	CP1251         UMETA(DisplayName = "CP1251"),
	CP1252         UMETA(DisplayName = "CP1252"),
	CP1253         UMETA(DisplayName = "CP1253"),
	CP1254         UMETA(DisplayName = "CP1254"),
	CP1255         UMETA(DisplayName = "CP1255"),
	CP1256         UMETA(DisplayName = "CP1256"),
	CP1257         UMETA(DisplayName = "CP1257"),
	CP1258         UMETA(DisplayName = "CP1258"),
	KOI8_R         UMETA(DisplayName = "KOI8_R"),
};
ENUM_CLASS_FLAGS(EFontEncodings)

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