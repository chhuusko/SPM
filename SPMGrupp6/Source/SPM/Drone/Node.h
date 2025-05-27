// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SPM_API FNode 
{
	
public:
	FNode(FVector Position, FVector Size) : Position(Position), Size(Size) {}
	FVector Position;
	FVector Size;
	bool IsClearAndNotVisited() const;
	bool IsClear = false;
	bool IsVisited = false;
};
