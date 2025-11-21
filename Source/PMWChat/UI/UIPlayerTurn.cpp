#include "UIPlayerTurn.h"
#include "UI/UIPlayerTurn.h"
#include "Components/TextBlock.h"

void UUIPlayerTurn::SetInfo(const FString& NameString, float InTimer)
{
	if (IsValid(TextName) == true)
	{
		TextName->SetText(FText::FromString(NameString));
	}

	Timer = InTimer;
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
		return;

	World->GetTimerManager().ClearTimer(TimerHandle);
	World->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ThisClass::UpdateTimer,
		0.1f,
		true,
		0.0f);
}

void UUIPlayerTurn::ClearInfo()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	if (IsValid(TextName) == true)
	{
		TextName->SetText(FText::GetEmpty());
	}

	if (IsValid(TextTimer) == true)
	{
		TextTimer->SetText(FText::GetEmpty());
	}
}

void UUIPlayerTurn::UpdateTimer()
{
	Timer = FMath::Max(0.0f, Timer - 0.1f);
	if (IsValid(TextTimer) == true)
	{
		TextTimer->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), Timer)));
	}
}
