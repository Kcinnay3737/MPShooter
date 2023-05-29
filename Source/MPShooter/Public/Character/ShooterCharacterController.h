// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ShooterCharacterController.generated.h"

class UInputMappingContext;
class UInputAction;
class AShooterCharacter;

/**
 * 
 */
UCLASS()
class MPSHOOTER_API AShooterCharacterController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void SetupInputComponent() override;

	AShooterCharacter* GetShooterCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* m_InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* m_MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* m_LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* m_JumpAction;

private:
	void InitializeMappingContext();

	UFUNCTION()
	void OnMove(const FInputActionValue& value);
	UFUNCTION()
	void OnLook(const FInputActionValue& value);
	UFUNCTION()
	void OnJump(const FInputActionValue& value);
};
