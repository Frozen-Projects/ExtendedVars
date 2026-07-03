#include "Extended_BPLib.h"

#pragma region Int_Group

uint8 UExtendedVarsBPLibrary::Int32_To_Byte(int32 In_Number)
{
    if (In_Number >= 0 && In_Number <= 255)
    {
        return In_Number;
    }

    else if (In_Number < 0)
    {
        return 0;
    }

    else if (In_Number > 255)
    {
        return 255;
    }

    return 0;
}

int32 UExtendedVarsBPLibrary::Int32_Place_Family(int32 TargetInteger)
{
    /*
    10 power integer character lenght minus 1 will equal its place family.
    For example to find 10's place family, 10^2(Char Lenght) - 1 = 10. 10's place family will 10.
    */

    return (int32)FMath::Pow(10.0, (double)((FString::FromInt(TargetInteger).Len()) - 1));
}

int32 UExtendedVarsBPLibrary::Int32_Truncate_To_Big(int32 TargetInteger)
{
    double Remainder;
    UKismetMathLibrary::FMod(TargetInteger, UExtendedVarsBPLibrary::Int32_Place_Family(TargetInteger), Remainder);

    if (Remainder != 0)
    {
        const FString IntString = FString::FromInt(TargetInteger);

        int32 PlaceDigit = FCString::Atoi(*(UKismetStringLibrary::GetCharacterArrayFromString(IntString)[0]));
        float PlaceFamily = (int32)FMath::Pow(10.0, (double)((IntString.Len()) - 1));

        return PlaceFamily * (PlaceDigit + 1);
    }

    else
    {
        return TargetInteger;
    }
}

int32 UExtendedVarsBPLibrary::Int32_Truncate_To_Small(int32 TargetInteger)
{
    double Remainder;
    UKismetMathLibrary::FMod(TargetInteger, 10, Remainder);

    if (Remainder != 0)
    {
        const FString IntString = FString::FromInt(TargetInteger);

        int32 LastDigit = FCString::Atoi(*(UKismetStringLibrary::GetCharacterArrayFromString(IntString).Last()));
        int32 Difference = 10 - LastDigit;

        return TargetInteger + Difference;
    }

    else
    {
        return TargetInteger;
    }
}

#pragma endregion Int_Group

#pragma region Float_Group

double UExtendedVarsBPLibrary::Float_Precision(double TargetFloat, int32 Precision)
{
    int32 PrecisionFixer = pow(10, Precision);
    return floor(PrecisionFixer * TargetFloat) / PrecisionFixer;
}

int32 UExtendedVarsBPLibrary::Float_Fraction_Count(float TargetFloat)
{
    FString FractionString;
    (FString::SanitizeFloat(UKismetMathLibrary::Fraction(TargetFloat))).Split(TEXT("."), NULL, &FractionString, ESearchCase::IgnoreCase, ESearchDir::FromStart);

    return FractionString.Len();
}

float UExtendedVarsBPLibrary::Float_Round_Next(float TargetFloat, double Decimal)
{
    int32 FractionCount = UExtendedVarsBPLibrary::Float_Fraction_Count(TargetFloat);

    if (FractionCount > Decimal)
    {
        return (FMath::TruncToInt(TargetFloat * FMath::Pow(10.0, Decimal)) + 1) / FMath::Pow(10.0, Decimal);
    }

    else
    {
        return TargetFloat;
    }
}

#pragma endregion Float_Group

#pragma region Time_Group

FDateTime UExtendedVarsBPLibrary::Increment_Date(FDateTime Start, int32 In_Years, int32 In_Months, int32 In_Days)
{
    const FDateTime TargetTime = (Start == FDateTime()) ? FDateTime::Now() : Start;

    int32 Year = TargetTime.GetYear() + In_Years;
    int32 Month = TargetTime.GetMonth();
    int32 Day = TargetTime.GetDay();

    // Convert month to zero-based, add In_Months, then carry over to Year
    const int32 ZeroBasedMonth = (Month - 1) + In_Months;
    Year += ZeroBasedMonth / 12;                                            // carry over
    Month = (ZeroBasedMonth % 12) + 1;                                      // back to 1..12

    // Clamp Day if it exceeds the new month's length. e.g., from Jan 31 to a month with only 30 days
    const int32 DaysInNewMonth = FDateTime::DaysInMonth(Year, Month);
    if (Day > DaysInNewMonth)
    {
        Day = DaysInNewMonth;
    }

    FDateTime Intermediate(Year, Month, Day, TargetTime.GetHour(), TargetTime.GetMinute(), TargetTime.GetSecond(), TargetTime.GetMillisecond());
    FTimespan AdditionalDays(In_Days, 0, 0, 0);

    return Intermediate + AdditionalDays;
}

bool UExtendedVarsBPLibrary::GetDateTimeWithZone(FString& Out_Print, FString& Out_Zone, FString& Out_Geo, FString& Out_Day, FString& Out_Month, bool bUseShort)
{
#ifdef _WIN64
    FDateTime Now = FDateTime::UtcNow();
    EDayOfWeek DayOfWeek = Now.GetDayOfWeek();
    EMonthOfYear MonthOfYear = Now.GetMonthOfYear();

    switch (DayOfWeek)
    {
    case EDayOfWeek::Monday:
        Out_Day = bUseShort ? "Mon" : "Monday";
        break;
    case EDayOfWeek::Tuesday:
        Out_Day = bUseShort ? "Tue" : "Tuesday";
        break;
    case EDayOfWeek::Wednesday:
        Out_Day = bUseShort ? "Wed" : "Wednesday";
        break;
    case EDayOfWeek::Thursday:
        Out_Day = bUseShort ? "Thu" : "Thursday";
        break;
    case EDayOfWeek::Friday:
        Out_Day = bUseShort ? "Fri" : "Friday";
        break;
    case EDayOfWeek::Saturday:
        Out_Day = bUseShort ? "Sat" : "Saturday";
        break;
    case EDayOfWeek::Sunday:
        Out_Day = bUseShort ? "Sun" : "Sunday";
        break;
    }

    switch (MonthOfYear)
    {
    case EMonthOfYear::January:
        Out_Month = bUseShort ? "Jan" : "January";
        break;
    case EMonthOfYear::February:
        Out_Month = bUseShort ? "Feb" : "February";
        break;
    case EMonthOfYear::March:
        Out_Month = bUseShort ? "Mar" : "March";
        break;
    case EMonthOfYear::April:
        Out_Month = bUseShort ? "Apr" : "April";
        break;
    case EMonthOfYear::May:
        Out_Month = bUseShort ? "May" : "May";
        break;
    case EMonthOfYear::June:
        Out_Month = bUseShort ? "Jun" : "June";
        break;
    case EMonthOfYear::July:
        Out_Month = bUseShort ? "Jul" : "July";
        break;
    case EMonthOfYear::August:
        Out_Month = bUseShort ? "Aug" : "August";
        break;
    case EMonthOfYear::September:
        Out_Month = bUseShort ? "Sep" : "September";
        break;
    case EMonthOfYear::October:
        Out_Month = bUseShort ? "Oct" : "October";
        break;
    case EMonthOfYear::November:
        Out_Month = bUseShort ? "Nov" : "November";
        break;
    case EMonthOfYear::December:
        Out_Month = bUseShort ? "Dec" : "December";
        break;
    }

    std::stringstream TimeStream;
    const std::chrono::zoned_time cur_time
    {
        std::chrono::current_zone(),
        std::chrono::system_clock::now()
    };

    TimeStream << cur_time;

    FString ChronoString = UTF8_TO_TCHAR(TimeStream.str().c_str());
    TArray<FString> Array_Sections = UKismetStringLibrary::ParseIntoArray(ChronoString, " ");
    Out_Zone = Array_Sections.Last();

    std::stringstream ZoneStream;
    ZoneStream << std::chrono::current_zone()->name();
    Out_Geo = ZoneStream.str().c_str();

    Out_Print = FString::Printf(TEXT("%s, %d %s %d %d:%d:%d %s %s"), *Out_Day, Now.GetDay(), *Out_Month, Now.GetYear(), Now.GetHour(), Now.GetMinute(), Now.GetSecond(), *Out_Zone, *Out_Geo);

    return true;

#else
    return false;
#endif
}

#pragma endregion Time_Group