// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

// setting
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	EEnemyState mState = EEnemyState::Idle;

	// Idle
	void IdleState();
	// move
	void MoveState();
	// attack
	void AttackState();
	// damage
	void DamageState();
	// die
	void DieState();

	// delay time
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 2;

	// time pass
	float currentTime = 0;

	// target
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	class ATPSPlayer* target;

	// owner Actor
	UPROPERTY()
	class AEnemy* me;

	// attack range
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 150.0f;
};
