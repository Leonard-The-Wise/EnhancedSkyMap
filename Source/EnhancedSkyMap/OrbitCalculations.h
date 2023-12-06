// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OrbitCalculations.generated.h"

USTRUCT(BlueprintType)
struct FCelestialPosition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Celestial Position")
	double Elevation;
	UPROPERTY(BlueprintReadWrite, Category = "Celestial Position")
	double Azimuth;


	FCelestialPosition()
	{
		Elevation = 0.0;
		Azimuth = 0.0;
	}

	FCelestialPosition(double altitude, double azimuth)
	{
		Elevation = altitude;
		Azimuth = azimuth;
	}
};


/**
 *
 */
UCLASS()
class ENHANCEDSKYMAP_API UOrbitCalculations : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "DateTime")
	static FDateTime AddTimespanToDateTime(const FDateTime& DateTimeIn, const FTimespan& TimespanIn);

	UFUNCTION(BlueprintPure, Category = "DateTime")
	static FDateTime GetSystemDateTime();

	UFUNCTION(BlueprintPure, Category = "DateTime")
	static FDateTime GetLocalDateTime();

	UFUNCTION(BlueprintPure, Category = "DateTime")
	static FDateTime GetLocalDateTimeFromUtcDateTime(const FDateTime& DateTimeIn);

	UFUNCTION(BlueprintCallable, Category = "DateTime")
	static FString GetCurrentSystemDateFormat();

	UFUNCTION(BlueprintCallable, Category = "DateTime")
	static bool ParseDateString(const FString& DateString, FDateTime& OutDateTime);

	UFUNCTION(BlueprintCallable, Category = "DateTime")
	static bool ParseTimeString(const FString& TimeString, FDateTime& OutDateTime);

	UFUNCTION(BlueprintPure, Category = "DateTime")
	static FText LocalizedDate(const FDateTime& Date, const FString& TimeZone = "");

	UFUNCTION(BlueprintPure, Category = "Math")
	static float IncrementAngleAndLimitTo360(float OriginalAngle, float Increment);

	UFUNCTION(BluePrintCallable, Category = "Utilities")
	static FText DegreesFloatToDegreesMinutesSeconds(double Degrees);

	UFUNCTION(BlueprintCallable, Category = "Celestial Position")
	static FCelestialPosition GetSunPosition(double Latitude, double Longitude, int Year, int Month, int Day, int Hours, int Minutes, int Seconds, int TimeZone, bool IsDST);

	UFUNCTION(BlueprintCallable, Category = "Celestial Position")
	static FCelestialPosition GetMoonPosition(double Latitude, double Longitude, int Year, int Month, int Day, int Hours, int Minutes, int Seconds, int TimeZone, bool IsDST);

	UFUNCTION(BlueprintCallable, Category = "Celestial Position")
	static FCelestialPosition GetSkyPosition(double Latitude, double Longitude, int Year, int Month, int Day, int Hours, int Minutes, int Seconds, int TimeZone, bool IsDST);

	UFUNCTION(BlueprintPure, Category = "Celestial Position")
	static FRotator GetRotatorForSky(FCelestialPosition SkyPosition);

};
