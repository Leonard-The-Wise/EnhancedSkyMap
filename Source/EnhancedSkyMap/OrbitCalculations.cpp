// Fill out your copyright notice in the Description page of Project Settings.
// Space formulas at: http://www.stargazing.net/kepler/altaz.html
//

#include "OrbitCalculations.h"

#include <windows.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "Ephemeris/Ephemeris.h"

/*
* UNREAL Blueprint Functions
*/

#define LIMIT_DEGREES_TO_360(value) (value) >= 0 ? ((value)-(long)((value)*0.0027777778)*360) : (((value)-(long)((value)*0.0027777778)*360)+360)



// Helper function to split string by delimiter
std::vector<std::string> SplitString(const std::string& str, char delimiter) {
	std::vector<std::string> elements;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delimiter)) {
		elements.push_back(item);
	}
	return elements;
}

// Function to parse date string based on format
bool ParseDate(const FString& DateString, const FString& FormatString, FDateTime& OutDateTime) {
	// Lowercase the format string
	FString Format = FormatString.ToLower();

	// Identify the positions of day, month, and year in the format
	size_t DayPos = Format.Find("dd");
	size_t MonthPos = Format.Find("mm");
	size_t YearPos = Format.Find("yyyy");

	// Split the date string
	auto DateParts = SplitString(std::string(TCHAR_TO_UTF8(*DateString)), '/');

	if (DateParts.size() != 3) {
		return false; // Incorrect date format
	}

	int Day = -1, Month = -1, Year = -1;

	// Calculate the indexes for the day, month, and year
	int DayIndex = Format.Find("dd") == 0 ? 0 : (Format.Find("dd") == 3 ? 1 : 2);
	int MonthIndex = Format.Find("mm") == 0 ? 0 : (Format.Find("mm") == 3 ? 1 : 2);
	int YearIndex = Format.Find("yyyy") == 0 ? 0 : (Format.Find("yyyy") == 3 ? 1 : 2);

	// Extract the date components
	Day = std::stoi(DateParts[DayIndex]);
	Month = std::stoi(DateParts[MonthIndex]);
	Year = std::stoi(DateParts[YearIndex]);

	// Validate and create the FDateTime object
	if (FDateTime::Validate(Year, Month, Day, 0, 0, 0, 0)) {
		OutDateTime = FDateTime(Year, Month, Day);
		return true;
	}

	return false;
}

FDateTime UOrbitCalculations::AddTimespanToDateTime(const FDateTime& DateTimeIn, const FTimespan& TimespanIn)
{
	return DateTimeIn + TimespanIn;
}

FDateTime UOrbitCalculations::GetSystemDateTime()
{
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);


	return FDateTime(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
}

FDateTime UOrbitCalculations::GetLocalDateTime()
{
	SYSTEMTIME userTime;
	GetLocalTime(&userTime);

	return FDateTime(userTime.wYear, userTime.wMonth, userTime.wDay, userTime.wHour, userTime.wMinute, userTime.wSecond, userTime.wMilliseconds);
}

FDateTime UOrbitCalculations::GetLocalDateTimeFromUtcDateTime(const FDateTime& DateTimeIn)
{
	datetime currentDT;
	currentDT.setdatetime(DateTimeIn.GetYear(), DateTimeIn.GetMonth(), DateTimeIn.GetDay(), DateTimeIn.GetHour(), DateTimeIn.GetMinute(), DateTimeIn.GetSecond());

	time_t now = std::time(nullptr);
	auto const tm = *std::localtime(&now);
	std::ostringstream os;
	os << std::put_time(&tm, "%z");
	std::string s = os.str();
	// s is in ISO 8601 format: "±HHMM"
	int h = std::stoi(s.substr(0, 3), nullptr, 10);
	int m = std::stoi(s[0] + s.substr(3), nullptr, 10);

	currentDT.addhours(h);
	currentDT.addminutes(m);

	return FDateTime(currentDT.year(), currentDT.month(), currentDT.day(), currentDT.hour(), currentDT.minute(), currentDT.second());
}

FString UOrbitCalculations::GetCurrentSystemDateFormat()
{
	wchar_t dateFormat[100];
	GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SSHORTDATE, dateFormat, 100);

	return FString(std::wstring(dateFormat).c_str());
}

bool UOrbitCalculations::ParseDateString(const FString& DateString, FDateTime& OutDateTime)
{
	// Try to parse date using current system locale.
	FString DateFormat = GetCurrentSystemDateFormat();

	return ParseDate(DateString, DateFormat, OutDateTime);
}

bool UOrbitCalculations::ParseTimeString(const FString& TimeString, FDateTime& OutDateTime)
{
	// Assuming the input is always "HH:mm:ss"
	TArray<FString> Parts;
	TimeString.ParseIntoArray(Parts, TEXT(":"), true);

	if (Parts.Num() == 3)
	{
		int32 Hours = FCString::Atoi(*Parts[0]);
		int32 Minutes = FCString::Atoi(*Parts[1]);
		int32 Seconds = FCString::Atoi(*Parts[2]);

		// Use the current date with the given time
		FDateTime CurrentDate = FDateTime::Now();

		if (FDateTime::Validate(CurrentDate.GetYear(), CurrentDate.GetMonth(), CurrentDate.GetDay(), Hours, Minutes, Seconds, 0))
		{
			OutDateTime = FDateTime(CurrentDate.GetYear(), CurrentDate.GetMonth(), CurrentDate.GetDay(), Hours, Minutes, Seconds);
			return true;
		}
	}

	return false;
}

FText UOrbitCalculations::LocalizedDate(const FDateTime& Date, const FString& TimeZone)
{	
	return FText::AsDate(Date, EDateTimeStyle::Short, TimeZone);
}

float UOrbitCalculations::IncrementAngleAndLimitTo360(float OriginalAngle, float Increment)
{
	float RealIncrement = LIMIT_DEGREES_TO_360(Increment);

	return LIMIT_DEGREES_TO_360(OriginalAngle + RealIncrement);
}

FText UOrbitCalculations::DegreesFloatToDegreesMinutesSeconds(double Degrees)
{
	int DegreesInt{}, MinutesInt{};
	double SecondsDouble{};

	Ephemeris::floatingDegreesToDegreesMinutesSeconds(Degrees, &DegreesInt, &MinutesInt, &SecondsDouble);

	FNumberFormattingOptions MinutesFormatingOptions;
	MinutesFormatingOptions.MinimumIntegralDigits = 2;
	FNumberFormattingOptions SecondsFormatingOptions;
	SecondsFormatingOptions.MinimumIntegralDigits = 2;
	SecondsFormatingOptions.MaximumFractionalDigits = 1;
	SecondsFormatingOptions.MinimumFractionalDigits = 1;

	FText Return = FText::Format(
		NSLOCTEXT("OrbitCalculations", "DegreesMinutesSeconds", "{0}º{1}'{2}\""),
		FText::AsNumber(DegreesInt),
		FText::AsNumber(MinutesInt, &MinutesFormatingOptions),
		FText::AsNumber(SecondsDouble, &SecondsFormatingOptions)
	);

	return Return;
}

FCelestialPosition UOrbitCalculations::GetSunPosition(double Latitude, double Longitude, int Year, int Month, int Day, int Hours, int Minutes, int Seconds, int TimeZone, bool IsDST)
{
	// Current Date
	datetime myTime = datetime();
	myTime.setdatetime(Year, Month, Day, Hours, Minutes, Seconds);
	myTime.addhours(TimeZone);

	// Adjust Daylight saving time
	if (IsDST)
	{
		myTime.addhours(1);
	}

	// Set observer's location on earth
	Ephemeris::setLocationOnEarth(Latitude, Longitude);

	// Get sun
	SolarSystemObject sun = Ephemeris::solarSystemObjectAtDateAndTime(SolarSystemObjectIndex::Sun,
		static_cast<unsigned int>(myTime.day()), static_cast<unsigned int>(myTime.month()), static_cast<unsigned int>(myTime.year()),
		static_cast<unsigned int>(myTime.hour()), static_cast<unsigned int>(myTime.minute()), static_cast<unsigned int>(myTime.second()));

	// Return altitude and azimuth
	return { sun.horiCoordinates.alt, sun.horiCoordinates.azi };
}


FCelestialPosition UOrbitCalculations::GetMoonPosition(double Latitude, double Longitude, int Year, int Month, int Day, int Hours, int Minutes, int Seconds, int TimeZone, bool IsDST)
{
	// Current Date
	datetime myTime = datetime();
	myTime.setdatetime(Year, Month, Day, Hours, Minutes, Seconds);
	myTime.addhours(TimeZone);

	// Adjust Daylight saving time
	if (IsDST)
	{
		myTime.addhours(1);
	}

	// Set observer's location on earth
	Ephemeris::setLocationOnEarth(Latitude, Longitude);

	// Get moon
	SolarSystemObject moon = Ephemeris::solarSystemObjectAtDateAndTime(SolarSystemObjectIndex::EarthsMoon,
		static_cast<unsigned int>(myTime.day()), static_cast<unsigned int>(myTime.month()), static_cast<unsigned int>(myTime.year()),
		static_cast<unsigned int>(myTime.hour()), static_cast<unsigned int>(myTime.minute()), static_cast<unsigned int>(myTime.second()));

	// Return altitude and azimuth
	return { moon.horiCoordinates.alt, moon.horiCoordinates.azi };
}


FCelestialPosition UOrbitCalculations::GetSkyPosition(double Latitude, double Longitude, int Year, int Month, int Day, int Hours, int Minutes, int Seconds, int TimeZone, bool IsDST)
{
	// Current Date
	datetime myTime = datetime();
	myTime.setdatetime(Year, Month, Day, Hours, Minutes, Seconds);
	myTime.addhours(TimeZone);

	// Adjust Daylight saving time
	if (IsDST)
	{
		myTime.addhours(1);
	}

	// Set observer's location on earth
	Ephemeris::setLocationOnEarth(Latitude, Longitude);

	EquatorialCoordinates skyLocation{ 0,0 }; // Set Sky location to the First Point of Aries

	// Test location: Sheratan (second star in Aries constelation)
	//skyLocation = { Ephemeris::hoursMinutesSecondsToFloatingHours(1, 55, 58.4), Ephemeris::degreesMinutesSecondsToFloatingDegrees(20, 55, 34.9) };

	SolarSystemObject sky = Ephemeris::solarSystemObjectAtDateAndTime(SolarSystemObjectIndex::Star,
		static_cast<unsigned int>(myTime.day()), static_cast<unsigned int>(myTime.month()), static_cast<unsigned int>(myTime.year()),
		static_cast<unsigned int>(myTime.hour()), static_cast<unsigned int>(myTime.minute()), static_cast<unsigned int>(myTime.second()),
		skyLocation.ra, skyLocation.dec);

	// Return altitude and azimuth
	return { sky.horiCoordinates.alt, sky.horiCoordinates.azi };

}


FRotator UOrbitCalculations::GetRotatorForSky(FCelestialPosition SkyPosition)
{
	// Normalize Elevation to range [-90, 90] and Azimuth to range [0, 360]
	double Elevation = FMath::Clamp(SkyPosition.Elevation, -90.0, 90.0);
	double Azimuth = FMath::Fmod(SkyPosition.Azimuth, 360.0);
	if (Azimuth < 0) Azimuth += 360.0;

	double AzimuthRad = FMath::DegreesToRadians(Azimuth);
	double ElevationRad = FMath::DegreesToRadians(Elevation);

	double Pitch{}, Yaw{}, Roll{};
	FRotator FReturn;

	Roll = -FMath::Cos(AzimuthRad) * FMath::Cos(ElevationRad) * 180.0 / PI; // Roll is inverted
	Pitch = Elevation;
	Yaw = 0;

	// When Azimuth is past 180, we adjust Pitch
	if (Azimuth >= 180)
	{
		if (Elevation >= 0)
			Pitch = 180 - Elevation;
		if (Elevation < 0)
			Pitch = 180 + FMath::Abs(Elevation);
	}

	// Makes Pitch in 0-360 range
	if (Pitch < 0) Pitch += 360;

	// We also need to compensate Pitch by Azimuth's proximity to 0 or 180, 
	// since elevation would almost never reach 90º in a real world's sky.
	double PitchCorrection{};
	double AzimuthMod = FMath::Fmod(FMath::Abs(Azimuth), 180.0);
	double DistanceTo90 = FMath::Abs(90.0 - AzimuthMod) / 90.0;
	if (Elevation >= 0)
		PitchCorrection = (90 - Elevation) * DistanceTo90;
	else
		PitchCorrection = -(90 - FMath::Abs(Elevation)) * DistanceTo90;

	// Invert correction depending on sphere side (west side, elevation is inverted)
	if (Azimuth > 180.0)
		PitchCorrection *= -1;

	Pitch += PitchCorrection;

	FReturn.Roll = Roll;
	FReturn.Pitch = Pitch;
	FReturn.Yaw = Yaw;  // == 0;

	return FReturn;
}
