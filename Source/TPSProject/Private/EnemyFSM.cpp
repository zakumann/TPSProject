// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// search target of ATPSPlayer in world
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	// ATPSPlayer type cast
	target = Cast<ATPSPlayer>(actor);
	// bring owner actor
	me = Cast<AEnemy>(GetOwner());
	
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

// Idlestate
void UEnemyFSM::IdleState()
{
	// Target destination
	FVector destination = target->GetActorLocation();
	// Direction
	FVector dir = destination - me->GetActorLocation();
	// move
	me->AddMovementInput(dir.GetSafeNormal());

	if (dir.Size() < attackRange)
	{
		mState = EEnemyState::Attack;
	}
};
	// Movestate
	void UEnemyFSM::MoveState() {}
	// Attackstate
	void UEnemyFSM::AttackState() {}
	// Damagestate
	void UEnemyFSM::DamageState() {}
	// Diestate
	void UEnemyFSM::DieState() {}