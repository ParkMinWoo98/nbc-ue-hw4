#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PCPlayerState.generated.h"

UCLASS()
class PMWCHAT_API APCPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	APCPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	FString GetPlayerInfoString() const;

public:
	UPROPERTY(Replicated)
	FString PlayerNameString;
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;
	UPROPERTY(Replicated)
	int32 MaxGuessCount;

protected:
};
