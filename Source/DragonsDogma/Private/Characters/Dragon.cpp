// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Dragon.h"
#include "AbilitySystemComponent.h"

// Sets default values
ADragon::ADragon()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADragon::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAttributes();
	AddCharacterAbilities();
}

// Called every frame
void ADragon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADragon::InitializeAttributes()
{
	if (DefaultAttributes)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void ADragon::AddCharacterAbilities()
{
	for (TSubclassOf<UGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
	}
}

void ADragon::Flame()
{
	const FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Dragon.Flame"));
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTag.GetSingleTagContainer(), AbilitiesToActivate);

	for (auto GameplayAbilitySpec : AbilitiesToActivate)
	{
		if (!GameplayAbilitySpec->IsActive())
		{
			AbilitySystemComponent->TryActivateAbility(GameplayAbilitySpec->Handle);
		}
	}
}
