// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DragonFlame.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDOGMA_API UDragonFlame : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDragonFlame();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnFlameBegin(FGameplayEventData Data);

	UFUNCTION()
	void OnFlameEnd(FGameplayEventData Data);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageBlendOut();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AbilityMontage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle;

	FName AbilityName{ "Ability.Dragon.Flame" };
	FName CueName{ "GameplayCue.Dragon.Flame"};
};
