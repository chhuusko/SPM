// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneBullet.h"

#include "ShooterCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADroneBullet::ADroneBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->MaxSpeed = 2300.f;
	ProjectileMovementComponent->InitialSpeed = 2300.f;
	
	
}

// Called when the game starts or when spawned
void ADroneBullet::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ADroneBullet::OnHit);
}

// Called every frame
void ADroneBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ADroneBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (AShooterCharacter* player = Cast<AShooterCharacter>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(player,  BaseDamage,GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
	}
	Destroy();
}

