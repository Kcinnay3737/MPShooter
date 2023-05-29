// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/ShooterCharacter.h"
#include "Character/ShooterCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void AShooterCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		enhancedInputComponent->BindAction(m_MoveAction, ETriggerEvent::Triggered, this, &ThisClass::OnMove);
		enhancedInputComponent->BindAction(m_LookAction, ETriggerEvent::Triggered, this, &ThisClass::OnLook);
		enhancedInputComponent->BindAction(m_JumpAction, ETriggerEvent::Triggered, this, &ThisClass::OnJump);
	}
}

AShooterCharacter* AShooterCharacterController::GetShooterCharacter()
{
	APawn* pawn = GetPawn();
	if (!pawn) return nullptr;
	AShooterCharacter* shooterCharacter = Cast<AShooterCharacter>(pawn);
	if (!shooterCharacter) return nullptr;
	return shooterCharacter;
}

void AShooterCharacterController::BeginPlay()
{
	Super::BeginPlay();
	InitializeMappingContext();
}

void AShooterCharacterController::InitializeMappingContext()
{
	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer()))
	{
		if (m_InputMappingContext)
		{
			subsystem->ClearAllMappings();
			subsystem->AddMappingContext(m_InputMappingContext, 0);
		}
	}
}

void AShooterCharacterController::OnMove(const FInputActionValue& value)
{
	FVector2D currentValue = value.Get<FVector2D>();
	AShooterCharacter* shooterCharacter = GetShooterCharacter();
	if (currentValue.Equals(FVector2D::ZeroVector) || shooterCharacter == nullptr) return;

	if (currentValue.Y != 0.0f)
	{
		const FRotator yawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		shooterCharacter->AddMovementInput(direction, currentValue.Y);
	}
	if (currentValue.X != 0.0f)
	{
		const FRotator yawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		shooterCharacter->AddMovementInput(direction, currentValue.X);
	}
}

void AShooterCharacterController::OnLook(const FInputActionValue& value)
{
	FVector2D currentValue = value.Get<FVector2D>();
	AShooterCharacter* shooterCharacter = GetShooterCharacter();
	if (currentValue.Equals(FVector2D::ZeroVector) || shooterCharacter == nullptr) return;

	if (currentValue.X != 0.0f)
	{
		shooterCharacter->AddControllerYawInput(currentValue.X);
	}
	if (currentValue.Y != 0.0f)
	{
		shooterCharacter->AddControllerPitchInput(currentValue.Y);
	}
}

void AShooterCharacterController::OnJump(const FInputActionValue& value)
{
	bool currentValue = value.Get<bool>();
	AShooterCharacter* shooterCharacter = GetShooterCharacter();
	if (!currentValue || shooterCharacter == nullptr || shooterCharacter->GetCharacterMovement()->IsFalling()) return;

	shooterCharacter->Jump();
	UShooterCharacterAnimInstance* shooterCharacterAnimInstance = Cast<UShooterCharacterAnimInstance>(shooterCharacter->GetMesh()->GetAnimInstance());
	if (shooterCharacterAnimInstance)
	{
		shooterCharacterAnimInstance->StartJumping();
	}
}
