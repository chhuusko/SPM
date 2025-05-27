// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SPM_API Node 
{
public:
	bool IsClearAndNotVisited();
	bool IsClear = false;
	bool IsVisited = false;
};
