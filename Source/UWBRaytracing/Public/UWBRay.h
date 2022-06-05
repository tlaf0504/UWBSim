// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class UWBRAYTRACING_API UWBRay
{
public:
	FVector origin;
	FVector direction;
	double power_db;

public:
	UWBRay(FVector origin, FVector direction);
	~UWBRay();
};
