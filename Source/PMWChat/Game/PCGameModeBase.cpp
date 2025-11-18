#include "Game/PCGameModeBase.h"
#include "Game/PCGameStateBase.h"
#include "Player/PCPlayerController.h"
#include "EngineUtils.h"
#include "Player/PCPlayerState.h"

void APCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	APCPlayerController* PlayerController = Cast<APCPlayerController>(NewPlayer);
	if (IsValid(PlayerController) == true)
	{
		PlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(PlayerController);

		APCPlayerState* PS = PlayerController->GetPlayerState<APCPlayerState>();
		if (IsValid(PS) == true)
		{
			PS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		APCGameStateBase* GameStateBase = GetGameState<APCGameStateBase>();
		if (IsValid(GameStateBase) == true)
		{
			GameStateBase->MulticastRPCBroadcastLoginMessage(PS->PlayerNameString);
		}
	}
}

void APCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ResetGame();
}

FString APCGameModeBase::GenerateSecretNumber()
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

	return Result;
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

void APCGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();
	//UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
	
	for (const auto& PlayerController : AllPlayerControllers)
	{
		APCPlayerState* PS = PlayerController->GetPlayerState<APCPlayerState>();
		if (IsValid(PS) == true)
		{
			PS->CurrentGuessCount = 0;
		}
	}
}

void APCGameModeBase::JudgeGame(APCPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (InStrikeCount == SecretNumberLen)
	{
		APCPlayerState* PS = InChattingPlayerController->GetPlayerState<APCPlayerState>();
		for (const auto& PlayerController : AllPlayerControllers)
		{
			if (IsValid(PS) == true)
			{
				FString CombinedMessageString = PS->PlayerNameString + TEXT(" has won the game");
				PlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& PlayerController : AllPlayerControllers)
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
			for (const auto& PlayerController : AllPlayerControllers)
			{
				PlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}
