#include "Game/PCGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PCPlayerController.h"

void APCGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APCPlayerController* PC = Cast<APCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (IsValid(PC) == true)
		{
			FString NotificationString = InNameString + TEXT(" has joined the game.");
			PC->PrintChatMessageString(NotificationString);
		}
	}
}
