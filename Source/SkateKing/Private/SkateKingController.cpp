#include "SkateKingController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../SkateboardCharacter.h"

void ASkateKingController::BeginPlay()
{
    Super::BeginPlay();

    // Apply the default input mapping context
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 1);
        }
    }

    // Cache the reference to the character
    SkateboardCharacter = Cast<ASkateboardCharacter>(GetPawn());
}

void ASkateKingController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASkateKingController::Move);
        EnhancedInputComponent->BindAction(SpeedUpAction, ETriggerEvent::Triggered, this, &ASkateKingController::SpeedUp);
        EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Triggered, this, &ASkateKingController::SlowDown);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASkateKingController::Jump);
    }
}

void ASkateKingController::Move(const FInputActionValue& Value)
{
    if (SkateboardCharacter)
    {
        FVector2D MovementVector = Value.Get<FVector2D>();

        // Check if MovementVector is zero, which indicates key release
        if (MovementVector.IsNearlyZero())
        {
            SkateboardCharacter->MovementInput = FVector2D::ZeroVector;
        }
        else
        {
            SkateboardCharacter->MovementInput = MovementVector;
        }

        //UE_LOG(LogTemp, Log, TEXT("Move: MovementInput = %s"), *SkateboardCharacter->MovementInput.ToString());
    }
}

void ASkateKingController::SpeedUp(const FInputActionValue& Value)
{
    if (SkateboardCharacter)
    {
        SkateboardCharacter->bIsSpeedingUp = Value.Get<bool>();
    }
}

void ASkateKingController::SlowDown(const FInputActionValue& Value)
{
    if (SkateboardCharacter)
    {
        SkateboardCharacter->bIsSlowingDown = Value.Get<bool>();
    }
}

void ASkateKingController::Jump(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && SkateboardCharacter)
    {
        SkateboardCharacter->LaunchCharacter(FVector(0, 0, SkateboardCharacter->JumpImpulse), false, true);
    }
}
