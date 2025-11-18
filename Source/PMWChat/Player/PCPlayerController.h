#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCPlayerController.generated.h"

class UPCChatInput;
class UUserWidget;

UCLASS()
class PMWCHAT_API APCPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	APCPlayerController();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPCChatInput> ChatInputWidgetClass;
	UPROPERTY()
	TObjectPtr<UPCChatInput> ChatInputWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

	FString ChatMessageString;
};
