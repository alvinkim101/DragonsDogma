// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DragonsDogmaBaseCharacter.h"
#include "Dragon.generated.h"

UCLASS()
class DRAGONSDOGMA_API ADragon : public ADragonsDogmaBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADragon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void Flame();

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> CharacterAbilities;

	void InitializeAttributes();

	void AddCharacterAbilities();
};
