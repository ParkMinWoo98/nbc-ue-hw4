#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PCGameStateBase.generated.h"

UCLASS()
class PMWCHAT_API APCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXX")));
};
