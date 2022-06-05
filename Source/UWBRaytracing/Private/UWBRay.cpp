// Fill out your copyright notice in the Description page of Project Settings.


#include "UWBRay.h"

UWBRay::UWBRay(FVector origin, FVector direction)
{
	origin = origin;
	direction = direction;
	direction.Normalize();
}

UWBRay::~UWBRay()
{
}
