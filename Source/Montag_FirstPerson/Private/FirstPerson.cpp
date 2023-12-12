// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPerson.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

DECLARE_LOG_CATEGORY_CLASS(LogMontag, Log, All);

// Sets default values for this component's properties
UFirstPerson::UFirstPerson()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UFirstPerson::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}
	
	if (!InputMapping)
	{
		UE_LOG(LogMontag, Error, TEXT("InputMapping is not set!"));
		return;
	}
	
	if (Pawn = Cast<APawn>(GetOwner()); !IsValid(Pawn))
	{
		UE_LOG(LogMontag, Error, TEXT("Owner is not valid or it is not pawn!"));
		return;
	} 

	if (const auto& PC = Cast<APlayerController>(Pawn->GetController()))
	{
		if (const auto& Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputMapping, InputPriority);
		}
		else
		{
			UE_LOG(LogMontag, Error, TEXT("Input Subsystem is not valid or not UEnhancedInputLocalPlayerSubsystem!"));
		}
	}
	else
	{
		UE_LOG(LogMontag, Error, TEXT("Owner is not valid or it is not pawn!"));
	}
}

void UFirstPerson::OnPossessed_Implementation(AController* NewController)
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}
	
	if (!IsValid(NewController))
	{
		UE_LOG(LogMontag,Error, TEXT("OnPossessed NewController is not Valid"));
		return;
	}
	
	if (!NewController->IsA<APlayerController>())
	{
		UE_LOG(LogMontag,Error, TEXT("NewController is not APlayerController!"));
		return;
	}

	SetupPlayerInputComponent(NewController->InputComponent);
}

void UFirstPerson::OnUnPossessed_Implementation(AController* OldController)
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}
	
	if (!IsValid(OldController))
	{
		UE_LOG(LogMontag,Error, TEXT("OnUnPossessed OldController is not Valid"));
		return;
	}
	
	if (const auto& PEI = Cast<UEnhancedInputComponent>(OldController->InputComponent))
	{
		if(InputMoveHandle.IsValid())
		{
			PEI->RemoveBinding(*InputMoveHandle.Get());
		}
		if(InputLookHandle.IsValid())
		{
			PEI->RemoveBinding(*InputLookHandle.Get());
		}
	}
}

void UFirstPerson::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	if (!IsValid(InputComponent))
	{
		UE_LOG(LogMontag, Error, TEXT("On SetupPlayerInputComponent InputComponent is not valid!"));
		return;
	}
	
	if (const auto& PEI = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (InputMove)
		{
			InputMoveHandle = MakeShared<FInputBindingHandle>(PEI->BindAction(InputMove, ETriggerEvent::Triggered, this, &UFirstPerson::Move));
		}
		if (InputLook)
		{
			InputLookHandle = MakeShared<FInputBindingHandle>( PEI->BindAction(InputLook, ETriggerEvent::Triggered, this, &UFirstPerson::Look));
		}
	}
	else
	{
		UE_LOG(LogMontag, Error, TEXT("On SetupPlayerInputComponent InputComponent is not UEnhancedInputComponent!"));
	}
}

void UFirstPerson::Move(const FInputActionValue& Value)
{
	if (!Pawn)
	{
		UE_LOG(LogMontag, Error, TEXT("Pawn is not valid on move!"));
		return;
	}
	
	if (const auto& Controller = Pawn->GetController())
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);
	
		if (MoveValue.Y != 0.f)
		{
			const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(Direction, MoveValue.Y);
		}
	
		if (MoveValue.X != 0.f)
		{
			const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(Direction, MoveValue.X);
		}
	}
	else
	{
		UE_LOG(LogMontag, Error, TEXT("Controller is not valid on move!"));
	}
}

void UFirstPerson::Look(const FInputActionValue& Value)
{
	if (!IsValid(Pawn))
	{
		UE_LOG(LogMontag, Error, TEXT("Pawn is not valid on Look!"));
		return;
	}
	
	const FVector2D LookValue = Value.Get<FVector2D>();

	if (LookValue.X != 0.f)
	{
		Pawn->AddControllerYawInput(LookValue.X);
	}

	if (LookValue.Y != 0.f)
	{
		Pawn->AddControllerPitchInput(LookValue.Y);
	}
}

void UFirstPerson::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( UFirstPerson, Pawn );
}
