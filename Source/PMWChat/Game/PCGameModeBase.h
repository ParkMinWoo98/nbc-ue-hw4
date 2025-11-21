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

	void ReadyToPlay(APCPlayerController* PlayerController);
	void CheckCanStartGame();

	void GenerateSecretNumber();
	bool IsGuessNumberString(const FString& InNumberString);
	FString JudgeResult(const FString& InScretNumberString, const FString& InGuessNumberString);
	void PrintChatMessageString(APCPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	void PrintChatInGame(APCPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	bool CheckGuessChance(APCPlayerController* InChattingPlayerController);
	void IncreaseGuessCount(APCPlayerController* InChattingPlayerController);
	void StartGame();
	void JudgeGame(APCPlayerController* InChattingPlayerController, int InStrikeCount);
	void EndGame();
	void ResetGame();

	void SetTurn(int32 PlayerIdx);
	void PassTurnToNext();
	void TurnTimeOut();

protected:
	UPROPERTY(EditAnywhere)
	int32 SecretNumberLen = 3;
	FString SecretNumberString;

	bool bIsInGame = false;

	UPROPERTY(EditAnywhere)
	float TurnDuration = 10.0f;
	int32 CurrPlayerIdx = -1;

	FTimerHandle TurnTimerHandle;
	FTimerHandle EndGameTimerHandle;

	UPROPERTY(EditAnywhere)
	int32 NeedPlayerCountToStart = 4;

	TArray<TObjectPtr<APCPlayerController>> ReadyPlayerControllers;
	TArray<TObjectPtr<APCPlayerController>> InGamePlayerControllers;
};
