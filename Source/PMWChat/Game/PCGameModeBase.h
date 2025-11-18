#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PCGameModeBase.generated.h"

class APCPlayerController;

UCLASS()
class PMWCHAT_API APCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void BeginPlay() override;

	FString GenerateSecretNumber();
	bool IsGuessNumberString(const FString& InNumberString);
	FString JudgeResult(const FString& InScretNumberString, const FString& InGuessNumberString);
	void PrintChatMessageString(APCPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	bool CheckGuessChance(APCPlayerController* InChattingPlayerController);
	void IncreaseGuessCount(APCPlayerController* InChattingPlayerController);
	void ResetGame();
	void JudgeGame(APCPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	UPROPERTY(EditAnywhere)
	int32 SecretNumberLen = 3;
	FString SecretNumberString;

	TArray<TObjectPtr<APCPlayerController>> AllPlayerControllers;
};
