// Fill out your copyright notice in the Description page of Project Settings.


#include "DragonsDogmaBaseCharacter.h"
#include "DragonsDogmaAttributeSet.h"

// Sets default values
ADragonsDogmaBaseCharacter::ADragonsDogmaBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UDragonsDogmaAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ADragonsDogmaBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
