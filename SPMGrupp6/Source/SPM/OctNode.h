// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SPM_API FOctNode
{
	
public:
	FOctNode(FVector Position, FVector Size) : Position(Position), Size(Size), Parent(nullptr) {
        for (int i = 0; i < 8; ++i) {
            Children[i] = nullptr;
        }
    }
	FVector Position;
	FVector Size;
	FOctNode* Parent;
	FOctNode* Children[8];
	bool isLeaf = false;
	void AddChildren();
};

