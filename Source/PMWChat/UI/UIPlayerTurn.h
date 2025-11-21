#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIPlayerTurn.generated.h"

class UTextBlock;

UCLASS()
class PMWCHAT_API UUIPlayerTurn : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetInfo(const FString& NameString, float InTimer);
	void ClearInfo();

	void UpdateTimer();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextTimer;

	FTimerHandle TimerHandle;
	float Timer;
};
