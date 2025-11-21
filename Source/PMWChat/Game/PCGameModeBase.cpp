#include "Game/PCGameModeBase.h"
#include "Game/PCGameStateBase.h"
#include "Player/PCPlayerController.h"
#include "EngineUtils.h"
#include "Player/PCPlayerState.h"

void APCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

}

void APCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void APCGameModeBase::ReadyToPlay(APCPlayerController* PlayerController)
{
	if (IsValid(PlayerController) == true)
	{
		ReadyPlayerControllers.Add(PlayerController);
	}

	if (bIsInGame == false)
	{
		CheckCanStartGame();
	}
	else
	{
		for (auto& ReadyPlayerController : ReadyPlayerControllers)
		{
			ReadyPlayerController->NotificationText = FText::FromString(FString::Printf(TEXT("%d Players are Waiting for next game."), ReadyPlayerControllers.Num()));
		}
	}
}

void APCGameModeBase::CheckCanStartGame()
{
	if (ReadyPlayerControllers.Num() >= NeedPlayerCountToStart)
	{
		StartGame();
	}
	else
	{
		FText Text = FText::FromString(FString::Printf(TEXT("Players: %d / %d"), ReadyPlayerControllers.Num(), NeedPlayerCountToStart));
		for (auto& ReadyPlayerController : ReadyPlayerControllers)
		{
			ReadyPlayerController->NotificationText = Text;
		}
	}
}

void APCGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < SecretNumberLen; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	SecretNumberString = Result;
}

bool APCGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;
	do
	{
		if (InNumberString.Len() != SecretNumberLen)
			break;

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0' || UniqueDigits.Contains(C) == true)
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
			break;

		bCanPlay = true;
	} while (false);

	return bCanPlay;
}

FString APCGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < SecretNumberLen; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			++StrikeCount;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				++BallCount;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
		return TEXT("OUT");

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void APCGameModeBase::PrintChatMessageString(APCPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	if (bIsInGame == false)
		return;

	PrintChatInGame(InChattingPlayerController, InChatMessageString);

	/*if (bIsInGame == true)
	{
	}
	else
	{
		FString ChatMessageString = InChatMessageString;
		APCPlayerState* PS = InChattingPlayerController->GetPlayerState<APCPlayerState>();
		if (IsValid(PS) == true)
		{
			ChatMessageString = PS->GetPlayerName() + TEXT(": ") + ChatMessageString;
		}

		for (TActorIterator<APCPlayerController> It(GetWorld()); It; ++It)
		{
			APCPlayerController* PlayerController = *It;
			if (IsValid(PlayerController) == true)
			{
				PlayerController->ClientRPCPrintChatMessageString(ChatMessageString);
			}
		}
	}*/
}

void APCGameModeBase::PrintChatInGame(APCPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - SecretNumberLen;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	FString ChatMessageString = InChatMessageString;

	auto ChatResult =
		[&]()
		{
			APCPlayerState* PS = InChattingPlayerController->GetPlayerState<APCPlayerState>();
			if (IsValid(PS) == true)
			{
				ChatMessageString = PS->GetPlayerInfoString() + TEXT(": ") + ChatMessageString;
			}

			for (TActorIterator<APCPlayerController> It(GetWorld()); It; ++It)
			{
				APCPlayerController* PlayerController = *It;
				if (IsValid(PlayerController) == true)
				{
					PlayerController->ClientRPCPrintChatMessageString(ChatMessageString);
				}
			}
		};

	if (IsGuessNumberString(GuessNumberString) == true)
	{
		if (InChattingPlayerController != InGamePlayerControllers[CurrPlayerIdx])
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("Other Player's turn"));
			return;
		}

		if (CheckGuessChance(InChattingPlayerController) == false)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("You have used every chance to guess."));
			return;
		}

		IncreaseGuessCount(InChattingPlayerController);
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		ChatMessageString += TEXT(" -> ") + JudgeResultString;
		ChatResult();

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InChattingPlayerController, StrikeCount);
	}
	else
	{
		ChatResult();
	}
}

bool APCGameModeBase::CheckGuessChance(APCPlayerController* InChattingPlayerController)
{
	APCPlayerState* PS = InChattingPlayerController->GetPlayerState<APCPlayerState>();
	if (IsValid(PS) == true)
	{
		return PS->CurrentGuessCount < PS->MaxGuessCount;
	}
	return false;
}

void APCGameModeBase::IncreaseGuessCount(APCPlayerController* InChattingPlayerController)
{
	APCPlayerState* PS = InChattingPlayerController->GetPlayerState<APCPlayerState>();
	if (IsValid(PS) == true)
	{
		++PS->CurrentGuessCount;
	}
}

void APCGameModeBase::StartGame()
{
	bIsInGame = true;

	InGamePlayerControllers = ReadyPlayerControllers;
	ReadyPlayerControllers.Empty();
	for (int Idx = 0; Idx < InGamePlayerControllers.Num(); ++Idx)
	{
		InGamePlayerControllers[Idx]->NotificationText = FText::GetEmpty();

		APCPlayerState* PS = InGamePlayerControllers[Idx]->GetPlayerState<APCPlayerState>();
		if (IsValid(PS) == true)
		{
			FString NameString = TEXT("Player") + FString::FromInt(Idx + 1);

			PS->PlayerNameString = NameString;
			InGamePlayerControllers[Idx]->NotificationText = FText::FromString(FString::Printf(TEXT("My Name: %s"), *NameString));
		}
	}

	GenerateSecretNumber();
	//UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);

	for (const auto& PlayerController : InGamePlayerControllers)
	{
		APCPlayerState* PS = PlayerController->GetPlayerState<APCPlayerState>();
		if (IsValid(PS) == true)
		{
			PS->CurrentGuessCount = 0;
		}
	}

	SetTurn(0);
}

void APCGameModeBase::JudgeGame(APCPlayerController* InChattingPlayerController, int InStrikeCount)
{
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);

	if (InStrikeCount == SecretNumberLen)
	{
		APCPlayerState* PS = InChattingPlayerController->GetPlayerState<APCPlayerState>();
		for (const auto& PlayerController : InGamePlayerControllers)
		{
			if (IsValid(PS) == true)
			{
				FString CombinedMessageString = PS->PlayerNameString + TEXT(" has won the game");
				PlayerController->NotificationText = FText::FromString(CombinedMessageString);
			}
		}

		EndGame();
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& PlayerController : InGamePlayerControllers)
		{
			APCPlayerState* PS = PlayerController->GetPlayerState<APCPlayerState>();
			if (IsValid(PS) == true)
			{
				if (PS->CurrentGuessCount < PS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (bIsDraw == true)
		{
			for (const auto& PlayerController : InGamePlayerControllers)
			{
				PlayerController->NotificationText = FText::FromString(TEXT("Draw..."));
			}
			EndGame();
		}
		else
		{
			PassTurnToNext();
		}
	}
}

void APCGameModeBase::EndGame()
{
	for (const auto& PlayerController : InGamePlayerControllers)
	{
		PlayerController->ClientRPCEndGame();
	}

	GetWorldTimerManager().SetTimer(
		EndGameTimerHandle,
		this,
		&ThisClass::ResetGame,
		5.0f);
}

void APCGameModeBase::ResetGame()
{
	bIsInGame = false;
	for (auto& PlayerController : InGamePlayerControllers)
	{
		ReadyPlayerControllers.Add(PlayerController);
	}
	InGamePlayerControllers.Empty();
	CheckCanStartGame();
}

void APCGameModeBase::SetTurn(int32 PlayerIdx)
{
	if (bIsInGame == false || 
		PlayerIdx < 0 || PlayerIdx >= InGamePlayerControllers.Num())
		return;

	CurrPlayerIdx = PlayerIdx;

	if (PlayerIdx >= 0)
	{
		if (APCPlayerState* PS = InGamePlayerControllers[PlayerIdx]->GetPlayerState<APCPlayerState>())
		{
			for (const auto& PlayerController : InGamePlayerControllers)
			{
				PlayerController->ClientRPCSetPlayerTurn(PS->PlayerNameString, TurnDuration);
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ThisClass::TurnTimeOut,
		TurnDuration);
}

void APCGameModeBase::PassTurnToNext()
{
	SetTurn((CurrPlayerIdx + 1) % InGamePlayerControllers.Num());
}

void APCGameModeBase::TurnTimeOut()
{
	IncreaseGuessCount(InGamePlayerControllers[CurrPlayerIdx]);
	JudgeGame(InGamePlayerControllers[CurrPlayerIdx], 0);
}
