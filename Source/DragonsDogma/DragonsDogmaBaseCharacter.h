// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "DragonsDogmaBaseCharacter.generated.h"

UCLASS()
class DRAGONSDOGMA_API ADragonsDogmaBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADragonsDogmaBaseCharacter();

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UDragonsDogmaAttributeSet> AttributeSet;
};
