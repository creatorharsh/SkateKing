#include "SkateKingController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../SkateboardCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASkateKingController::Look);
    }
}

void ASkateKingController::Move(const FInputActionValue& Value)
{
    if (SkateboardCharacter)
    {
        FVector2D MovementVector = Value.Get<FVector2D>();

        // Update the MovementInput based on the current input
        if (MovementVector.X != 0 || MovementVector.Y != 0)
        {
            SkateboardCharacter->MovementInput.X += MovementVector.X;
            SkateboardCharacter->MovementInput.Y += MovementVector.Y;
        }
        else
        {
            // Reset the MovementInput if no input is given
            SkateboardCharacter->MovementInput = FVector2D::ZeroVector;
        }

        // Normalize the MovementInput to ensure diagonal movement is smooth
        SkateboardCharacter->MovementInput = SkateboardCharacter->MovementInput.GetSafeNormal();
    }

    //UE_LOG(LogTemp, Log, TEXT("Move Function Called"));
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
    if (Value.Get<bool>() && SkateboardCharacter && SkateboardCharacter->GetCharacterMovement()->IsMovingOnGround())
    {
        SkateboardCharacter->LaunchCharacter(FVector(0, 0, SkateboardCharacter->JumpImpulse), false, true);
    }
}

void ASkateKingController::Look(const FInputActionValue& Value)
{
    if (SkateboardCharacter)
    {
        FVector2D LookVector = Value.Get<FVector2D>();
        SkateboardCharacter->AddControllerYawInput(LookVector.X * SkateboardCharacter->BaseTurnRate * GetWorld()->GetDeltaSeconds());
        SkateboardCharacter->AddControllerPitchInput(LookVector.Y * -1.0f * SkateboardCharacter->BaseLookUpRate * GetWorld()->GetDeltaSeconds());
    }
}
