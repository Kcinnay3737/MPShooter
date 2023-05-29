// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	m_ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterCharacterAnimInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);

	if (m_ShooterCharacter == nullptr)
	{
		m_ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (m_ShooterCharacter == nullptr) return;

	FVector velocity = m_ShooterCharacter->GetVelocity();
	velocity.Z = 0.0f;
	m_Speed = velocity.Size();

	m_bIsInAir = m_ShooterCharacter->GetCharacterMovement()->IsFalling();

	m_bIsAccelerating = m_ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
}

void UShooterCharacterAnimInstance::SetIsJumping(bool value)
{
	m_bIsJumping = value;
}
