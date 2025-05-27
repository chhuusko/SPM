// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Node.h"

/**
 * 
 */
class SPM_API FOctNode : public FNode
{
	
public:
	FOctNode(FVector Position, FVector Size) : FNode(Position, Size), Parent(nullptr) {
        for (int i = 0; i < 8; ++i) {
            Children[i] = nullptr;
        }
    }
	FOctNode* Parent;
	FOctNode* Children[8];
	bool IsLeaf = false;
	bool IsVisited = false;
	void AddChildren();
};

