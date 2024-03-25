// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Camera/CameraComponent.h>
#include <Blueprint/UserWidget.h>
#include "Bullet.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(0, 70, 90));
	SpringArmComp->TargetArmLength = 400;
	SpringArmComp->bUsePawnControlRotation = true;

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(SpringArmComp);
	tpsCamComp->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;

	// double jump
	JumpMaxCount = 2;

	// gun skeletalMeshComp
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// inherit to Mesh Component
	gunMeshComp->SetupAttachment(GetMesh());
	// data load skeletalMesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	// if data load has succeess
	if (TempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
	}

	// Sniper Component
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	// inherit to parent component
	sniperGunComp->SetupAttachment(GetMesh());
	// StaticMesh data load
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// if data load succeed
	if (TempSniperMesh.Succeeded()) 
	{
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		sniperGunComp->SetRelativeLocation(FVector(-22, 55, 120));
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// generate sniper UI widget
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// Generate General Crosshair instance
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// General corsshair UI
	_crosshairUI->AddToViewport();

	// default to sniperGun
	ChangeToSniperGun();
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(IA_Forward, ETriggerEvent::Triggered, this, &ATPSPlayer::MoveForward);
		EnhancedInputComponent->BindAction(IA_Backward, ETriggerEvent::Triggered, this, &ATPSPlayer::MoveBackward);
		EnhancedInputComponent->BindAction(IA_Right, ETriggerEvent::Triggered, this, &ATPSPlayer::MoveRight);
		EnhancedInputComponent->BindAction(IA_Left, ETriggerEvent::Triggered, this, &ATPSPlayer::MoveLeft);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPSPlayer::Look);
		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATPSPlayer::Fire);
		// Change Weapon
		EnhancedInputComponent->BindAction(OneAction, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		EnhancedInputComponent->BindAction(TwoAction, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);
		// Change Weapon
		EnhancedInputComponent->BindAction(SniperAction, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		EnhancedInputComponent->BindAction(SniperAction, ETriggerEvent::Completed, this, &ATPSPlayer::SniperAim);
	}

}

void ATPSPlayer::MoveForward(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	}
}

void ATPSPlayer::MoveBackward(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	}
}

void ATPSPlayer::MoveRight(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ATPSPlayer::MoveLeft(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ATPSPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void ATPSPlayer::Fire(const FInputActionValue& Value)
{
	// if use grenadeGun
	if (bUsingGrenadeGun)
	{
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	// if use sniperGun
	else
	{
		// LineTrace Start point
		FVector startPos = tpsCamComp->GetComponentLocation();
		// LineTrace End point
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// float for Collision data to LineTrace
		FHitResult hitInfo;
		// variation for collision option.
		FCollisionQueryParams params;
		// except Player
		params.AddIgnoredActor(this);
		// using with Channel filter for collision
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		// if Collision with LineTrace
		if (bHit)
		{
			// transform for bullet effect
			FTransform bulletTrans;
			// Impact location
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// Generate instance for bullet effect
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			auto hitComp = hitInfo.GetComponent();
			// if component has Physics
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// need force and direction
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;
				// Force that direction
				hitComp->AddForce(force);
			}
		}
	}
}

void ATPSPlayer::ChangeToGrenadeGun()
{
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

void ATPSPlayer::ChangeToSniperGun()
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

void ATPSPlayer::SniperAim()
{
	// if not snipergun mode don't do it.
	if (bUsingGrenadeGun)
	{
		return;
	}
	// Started
	if (bSniperAim == false)
	{
		// snipergun aiming mode
		bSniperAim = true;
		// Sniper UI
		_sniperUI->AddToViewport();
		// Setting Field Of View in Camera sight
		tpsCamComp->SetFieldOfView(45.0f);
		// Remove General crosshair
		_crosshairUI->RemoveFromParent();
	}
	// completed
	else
	{
		// deactivate snipergun aiming mode
		bSniperAim = false;
		// remove sniper UI
		_sniperUI->RemoveFromParent();
		// Restore Camera Sight
		tpsCamComp->SetFieldOfView(90.0f);
		// Generate General crosshair
		_crosshairUI->AddToViewport();
	}
}
