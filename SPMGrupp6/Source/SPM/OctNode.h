// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SPM_API FOctNode
{
public:
	float HalfSize;
	FVector Position;
	FOctNode* Parent;
	FOctNode* Children[8];
	bool isLeaf = false;
};
