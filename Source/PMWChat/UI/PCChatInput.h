#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PCChatInput.generated.h"

class UEditableTextBox;

UCLASS()
class PMWCHAT_API UPCChatInput : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void OnChatInputTextComitted(const FText& Text, ETextCommit::Type CommitMethod);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextChatInput;
};
