// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTER_API UShooterCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaTime) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartJumping();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void LoopJumping();


private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* m_ShooterCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float m_Speed;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool m_bIsInAir;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool m_bIsJumping;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool m_bIsAccelerating;

public:
	UFUNCTION(BlueprintCallable)
	void SetIsJumping(bool value);
};
