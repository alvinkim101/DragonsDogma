// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GreatswordCombo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" 
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h" 
#include "Characters/DragonsDogmaCharacter.h"
#include "AbilitySystemComponent.h"

void UGreatswordCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{			
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, CurrentAbilityTag, Montage);
	PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this, &UGreatswordCombo::OnMontageCompleted);
	PlayMontageAndWait->OnBlendOut.AddUniqueDynamic(this, &UGreatswordCombo::OnMontageBlendOut);
	PlayMontageAndWait->OnInterrupted.AddUniqueDynamic(this, &UGreatswordCombo::OnMontageInterrupted);
	PlayMontageAndWait->OnCancelled.AddUniqueDynamic(this, &UGreatswordCombo::OnMontageCancelled);
	PlayMontageAndWait->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.Greatsword.Comboable")));
	WaitGameplayEvent->EventReceived.AddUniqueDynamic(this, &UGreatswordCombo::OnEventReceived);
	WaitGameplayEvent->ReadyForActivation();

	UE_LOG(LogTemp, Display, TEXT("%s ActivateAbility"), *CurrentAbilityTag.ToString());
}

void UGreatswordCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Comboable")));
}

void UGreatswordCombo::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	UE_LOG(LogTemp, Display, TEXT("%s OnMontageCompleted"), *CurrentAbilityTag.ToString());
}

void UGreatswordCombo::OnMontageBlendOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	UE_LOG(LogTemp, Display, TEXT("%s OnMontageBlendOut"), *CurrentAbilityTag.ToString());
}

void UGreatswordCombo::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);

	UE_LOG(LogTemp, Display, TEXT("%s OnMontageInterrupted"), *CurrentAbilityTag.ToString());
}

void UGreatswordCombo::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);

	UE_LOG(LogTemp, Display, TEXT("%s OnMontageCancelled"), *CurrentAbilityTag.ToString());
}

void UGreatswordCombo::OnEventReceived(FGameplayEventData Data)
{
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Comboable")));

	UE_LOG(LogTemp, Display, TEXT("%s OnEventReceived"), *CurrentAbilityTag.ToString());
}
