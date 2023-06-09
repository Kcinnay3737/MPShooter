// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class MPSHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* m_CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* m_FollowCamera;

public:

};
