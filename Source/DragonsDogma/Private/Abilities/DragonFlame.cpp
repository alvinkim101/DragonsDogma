// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/DragonFlame.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" 
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h" 
#include "GameplayCueNotify_Actor.h" 

UDragonFlame::UDragonFlame()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(AbilityName));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(AbilityName));
}

void UDragonFlame::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, AbilityName, AbilityMontage);
	PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this, &UDragonFlame::OnMontageCompleted);
	PlayMontageAndWait->OnBlendOut.AddUniqueDynamic(this, &UDragonFlame::OnMontageBlendOut);
	PlayMontageAndWait->OnInterrupted.AddUniqueDynamic(this, &UDragonFlame::OnMontageInterrupted);
	PlayMontageAndWait->OnCancelled.AddUniqueDynamic(this, &UDragonFlame::OnMontageCancelled);
	PlayMontageAndWait->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* FlameBeginGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.Dragon.FlameBegin")));
	UAbilityTask_WaitGameplayEvent* FlameEndGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.Dragon.FlameEnd")));
	FlameBeginGameplayEvent->EventReceived.AddUniqueDynamic(this, &UDragonFlame::OnFlameBegin);
	FlameEndGameplayEvent->EventReceived.AddUniqueDynamic(this, &UDragonFlame::OnFlameEnd);
	FlameBeginGameplayEvent->ReadyForActivation();
	FlameEndGameplayEvent->ReadyForActivation();
}

void UDragonFlame::OnFlameBegin(FGameplayEventData Data)
{
	K2_AddGameplayCue(FGameplayTag::RequestGameplayTag(CueName), FGameplayEffectContextHandle());
}

void UDragonFlame::OnFlameEnd(FGameplayEventData Data)
{
	K2_RemoveGameplayCue(FGameplayTag::RequestGameplayTag(CueName));
}

void UDragonFlame::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UDragonFlame::OnMontageBlendOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UDragonFlame::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UDragonFlame::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}
