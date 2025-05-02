// Fill out your copyright notice in the Description page of Project Settings.


#include "PulseGrenade.h"

void APulseGrenade::Explode()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionShape ExplosionSphere = FCollisionShape::MakeSphere(ExplosionRadius);

	bool bHitSomething = GetWorld()->OverlapMultiByChannel(Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECC_PhysicsBody,
		ExplosionSphere
		);
}
