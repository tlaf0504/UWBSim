// Fill out your copyright notice in the Description page of Project Settings.


#include "Raytracing/RecursiveRaytracing.h"
#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

bool URecursiveRaytracing::RecursiveRaytracing(class AActor* object, TArray<AActor*> target_anchors, double power_db, int N_hops)
{
	for (double azi = 0; azi < 360; azi+= DELTA_MULTIPLATH_ANGLE_DEGREE)
	{
		for (double ele = 0; ele <= 90; ele += DELTA_MULTIPLATH_ANGLE_DEGREE)
		{
			double azi_rad = azi * PI / 180;
			double ele_rad = ele * PI / 180;

			FVector* ray_direction_object_cooords = new FVector(
				cos(ele_rad),
				cos(azi_rad) * sin(ele_rad),
				sin(azi_rad) * sin(ele_rad)
			);

			FVector ray_direction_world_coords = object->GetTransform().TransformVector(*ray_direction_object_cooords);
			ray_direction_world_coords.Normalize();
			FVector trace_end = object->GetActorLocation() + 15000 * ray_direction_world_coords;
			RecursiveRaytracingInternal(object, target_anchors, object->GetActorLocation(), trace_end, power_db, N_hops);
		}
	}
	return true;
}

bool URecursiveRaytracing::RecursiveRaytracingInternal(AActor* object, TArray<AActor*> target_anchors, FVector trace_start, FVector trace_end, double power_db, int N_hops)
{
	TArray<FHitResult>* hit_results = new TArray<FHitResult>();
	FCollisionQueryParams TraceParams;
	FCollisionObjectQueryParams* ObjectTraceParams = new FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic));

	FVector ray_direction = trace_end - trace_start;
	ray_direction.Normalize();


	UWorld* world = object->GetWorld();

	// Cast a line-of-sight ray from tag to one anchor, returning all objects hit by the ray.
	bool hit = world->LineTraceMultiByObjectType(
		*hit_results,
		trace_start,
		trace_end,
		*ObjectTraceParams,
		TraceParams);

	bool target_hit = false;
	for (int32 k = 0; (k < hit_results->Num()) & (k < 1); k++)
	{
		FHitResult hit_result = (*hit_results)[k];
		for (int l = 0; l < target_anchors.Num(); l++)
		{
			if (hit_result.GetActor() == target_anchors[l])
			{
				DrawDebugLine(
					world,
					trace_start,
					hit_result.ImpactPoint,
					FColor::Orange);
				target_hit = true;

				double aoa_azi;
				double aoa_ele;

				FVector _ray_direction = hit_result.ImpactPoint - trace_start;
				_ray_direction.Normalize();
				Calculate3DAoA(hit_result.GetActor(), _ray_direction, &aoa_azi, &aoa_ele);
				UE_LOG(LogTemp, Display, TEXT("%s: AoA Azimuth = %f degree, AoA Elevation = %f degree"), *(hit_result.GetActor()->GetName()), aoa_azi, aoa_ele);
			}
		}
	}

	
	if ((N_hops > 0) & (!target_hit))
	{
		// Iterate over hits and spawn reflection rays.
		// Attention: Only hits with even indices are considered, as
		// hits with odd indices determine the hits when the ray is leaving
		// the previously entered body.
		// for (int32 k = 0; k < hit_results->Num(); k++)
		for (int32 k = 0; (k < hit_results->Num()) & (k < 1); k++)
		{
			FHitResult hit_result = (*hit_results)[k];

			double ray_power_at_hit_point = power_db - 2 * log(hit_result.Distance / 100.0);

			// If ray power at hit point is below a certain limit, omit subsequent ray casting.
			if (ray_power_at_hit_point < RAYTRACING_LOWER_POWER_LIMIT_DB)
				continue;
			

			FVector new_ray_direction = UKismetMathLibrary::GetReflectionVector(ray_direction, hit_result.ImpactNormal);
			// Start new ray slightly above surface. Tests showed that otherwise a hit will be located at the ray's spawn point due to numerical issues.
			FVector new_ray_start = hit_result.ImpactPoint + 0.1 * hit_result.ImpactNormal;

			// ToDo: Remove hard-coded max ray length. (Make adaptive according to world size).
			FVector new_ray_end = new_ray_start + 15000 * new_ray_direction;
			target_hit |= RecursiveRaytracingInternal(object, target_anchors, new_ray_start, new_ray_end, ray_power_at_hit_point, N_hops - 1);

			if (target_hit)
			{
				DrawDebugLine(
					world,
					trace_start,
					new_ray_start,
					FColor::Orange);
			}
		}
	}
	return target_hit;
}


void URecursiveRaytracing::Calculate3DAoA(AActor* object, FVector incident_ray_direction, double* ele_deg_out, double* azi_deg_out)
{
	FVector incident_ray_direction_local = object->GetTransform().InverseTransformVector(-incident_ray_direction);
	incident_ray_direction_local.Normalize();

	double ele_rad = acos(incident_ray_direction_local[0]);
	double azi_rad = acos(incident_ray_direction[1] / sin(ele_rad));
	*ele_deg_out = ele_rad * 180 / PI;
	*azi_deg_out = azi_rad * 180 / PI;
}
