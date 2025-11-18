#include "PCPawn.h"
#include "PMWChat.h"

void APCPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = PMWChatFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *PMWChatFunctionLibrary::GetNetModeString(this), *NetRoleString);
	//PMWChatFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void APCPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = PMWChatFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *PMWChatFunctionLibrary::GetNetModeString(this), *NetRoleString);
	//PMWChatFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
