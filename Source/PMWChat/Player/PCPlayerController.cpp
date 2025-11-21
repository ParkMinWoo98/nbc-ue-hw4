#include "Player/PCPlayerController.h"
#include "UI/PCChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PMWChat.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/PCGameModeBase.h"
#include "Player/PCPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "UI/UIPlayerTurn.h"

APCPlayerController::APCPlayerController()
{
	bReplicates = true;
}

void APCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() == false)
		return;

	SetInputMode(FInputModeUIOnly());

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UPCChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(UIPlayerTurnClass) == true)
	{
		UIPlayerTurnInstance = CreateWidget<UUIPlayerTurn>(this, UIPlayerTurnClass);
		if (IsValid(UIPlayerTurnInstance) == true)
		{
			UIPlayerTurnInstance->AddToViewport();
		}
	}

	ServerRPCReadyToPlay();
}

void APCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void APCPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalPlayerController() == true)
	{
		APCPlayerState* PS = GetPlayerState<APCPlayerState>();
		if (IsValid(PS) == true)
		{
			ServerRPCPrintChatMessageString(InChatMessageString);
		}
	}
}

void APCPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	PMWChatFunctionLibrary::MyPrintString(this, InChatMessageString, 10.0f);
}

void APCPlayerController::ClientRPCSetPlayerTurn_Implementation(const FString& NameString, float Timer)
{
	if (IsValid(UIPlayerTurnInstance) == true)
	{
		UIPlayerTurnInstance->SetInfo(NameString, Timer);
	}
}

void APCPlayerController::ClientRPCEndGame_Implementation()
{
	if (IsValid(UIPlayerTurnInstance) == true)
	{
		UIPlayerTurnInstance->ClearInfo();
	}
}

void APCPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void APCPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	APCGameModeBase* GM = Cast<APCGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(GM) == true)
	{
		GM->PrintChatMessageString(this, InChatMessageString);
	}
}

void APCPlayerController::ServerRPCReadyToPlay_Implementation()
{
	APCGameModeBase* GM = Cast<APCGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(GM) == true)
	{
		GM->ReadyToPlay(this);
	}
}
