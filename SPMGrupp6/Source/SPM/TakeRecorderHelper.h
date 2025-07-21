// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TakeRecorderSource.h"
#include "LevelSequence.h"
#include "Recorder/TakeRecorder.h"

#include "CoreMinimal.h"

/**
 * 
 */
class SPM_API TakeRecorderHelper
{
public:
	
	void AddActorToTakeRecorder(AActor* Actor);
	TakeRecorderHelper();
	~TakeRecorderHelper();
	
};
