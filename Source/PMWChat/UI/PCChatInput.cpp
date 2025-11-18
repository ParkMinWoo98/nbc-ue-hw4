#include "UI/PCChatInput.h"
#include "Components/EditableTextBox.h"
#include "Player/PCPlayerController.h"

void UPCChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (TextChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextComitted) == false)
	{
		TextChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextComitted);
	}
}

void UPCChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (TextChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextComitted) == true)
	{
		TextChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextComitted);
	}
}

void UPCChatInput::OnChatInputTextComitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APCPlayerController* PlayerController = Cast<APCPlayerController>(GetOwningPlayer());
		if (IsValid(PlayerController) == true)
		{
			PlayerController->SetChatMessageString(Text.ToString());
			TextChatInput->SetText(FText());
		}
	}
}
