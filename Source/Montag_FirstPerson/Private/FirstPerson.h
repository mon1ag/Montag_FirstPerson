// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/ActorComponent.h"
#include "FirstPerson.generated.h"

struct FInputBindingHandle;
class UInputAction;
class ACharacter;

DECLARE_LOG_CATEGORY_EXTERN(LogCustom, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UFirstPerson : public UActorComponent
{
	GENERATED_BODY()

protected: //Defaults
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	int32 InputPriority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* InputMove;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* InputLook;

protected: //In Game Variables
	
	UPROPERTY(Replicated)
	APawn* Pawn;
	
	TSharedPtr<FInputBindingHandle> InputMoveHandle;

	TSharedPtr<FInputBindingHandle> InputLookHandle;

public:	
	
	UFirstPerson();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected://Setup Functions

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "MontagTools | Input | FirstPerson")
	void OnPossessed(AController* NewController);

	UFUNCTION(Client, Reliable,BlueprintCallable, Category = "MontagTools | Input | FirstPerson")
	void OnUnPossessed(AController* OldController);
	
	/*Call At Setup Input Component*/
	UFUNCTION(BlueprintCallable, Category = "MontagTools | Input | FirstPerson")
	void SetupPlayerInputComponent(UInputComponent* InputComponent);

protected://In Game Functions

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
};
