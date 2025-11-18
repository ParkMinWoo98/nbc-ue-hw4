#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PCPawn.generated.h"

UCLASS()
class PMWCHAT_API APCPawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
};
