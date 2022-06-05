// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "UWBRay.h"
#include "RecursiveRaytracing.generated.h"


#define DELTA_MULTIPLATH_ANGLE_DEGREE 5
/**
 * 
 */
UCLASS()
class UWBRAYTRACING_API URecursiveRaytracing : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	inline static double RAYTRACING_LOWER_POWER_LIMIT_DB = -120;
private:
	inline static TArray<AActor*>* actors_to_ignore = new TArray<AActor*>();
	inline static FLinearColor* trace_color = new FLinearColor(1, 0, 0, 1);
	inline static FLinearColor* trace_hit_color = new FLinearColor(0, 1, 0, 0);

public:
	UFUNCTION(BlueprintCallable)
		static bool RecursiveRaytracing(class AActor* object, TArray<AActor*> target_anchors, double power_db, int N_hops);

private:
	static bool RecursiveRaytracingInternal(AActor* object, TArray<AActor*> target_anchors, FVector trace_start, FVector trace_end, double power_db, int N_hops);
	static void Calculate3DAoA(AActor* object, FVector incident_ray_direction, double* ele_deg_out, double* azi_deg_out);
	
	

};

