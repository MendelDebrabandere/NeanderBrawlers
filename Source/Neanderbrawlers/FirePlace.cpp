// Fill out your copyright notice in the Description page of Project Settings.


#include "FirePlace.h"
#include "Components/BoxComponent.h"
#include "Meat.h"
#include "NeanderbrawlersGameModeBase.h"
#include "Components/ArrowComponent.h"
#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

// Sets default values
AFirePlace::AFirePlace()
	:CookingMeat{}
	,CookPercentagePerSecond{0.2f}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init of root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	//Init box component
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComp->SetupAttachment(Root);
	BoxComp->SetCanEverAffectNavigation(true);

	//Init box component
	WalkingHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("WalkingHitbox"));
	WalkingHitbox->SetupAttachment(Root);
	WalkingHitbox->SetCanEverAffectNavigation(true);

	//Init static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMesh->SetupAttachment(Root);

	// Create the ArrowComponent and attach it to the root component
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(Root);
	ArrowComponent->ArrowColor = FColor::Red;
	ArrowComponent->bTreatAsASprite = true;
	ArrowComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	// Create and attach the Niagara component to the RootComponent.
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	// Create the Widget Component and attach it to the actor's root component
	LoadingBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LoadingBarWidgetComponent"));
	LoadingBarWidgetComponent->SetupAttachment(RootComponent);

	// Set the widget class for the component
	// Replace 'ULoadingBarWidgetClass' with the actual class of your UMG Blueprint
	static ConstructorHelpers::FClassFinder<UUserWidget> LoadingBarWidgetClass(TEXT("/Game/UserInterface/MeatBarWidget"));
	if (LoadingBarWidgetClass.Succeeded())
	{
		LoadingBarWidgetComponent->SetWidgetClass(LoadingBarWidgetClass.Class);
	}

	// Create and attach a new AudioComponent
	CookingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CookingAudioComponent"));
	CookingAudioComponent->SetupAttachment(RootComponent);
	CookingAudioComponent->bAutoActivate = false; // Prevent sound from starting automatically

	// Create and attach a new AudioComponent
	FireAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudioComponent"));
	FireAudioComponent->SetupAttachment(RootComponent);
	FireAudioComponent->bAutoActivate = false; // Prevent sound from starting automatically

}

// Called when the game starts or when spawned
void AFirePlace::BeginPlay()
{
	Super::BeginPlay();

	if (NiagaraSystem)
	{
		// Assign the NiagaraSystem to the NiagaraComponent and activate it.
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->Activate(true);
	}


	if (FireSound != nullptr)
	{
		FireAudioComponent->SetSound(FireSound);
		FireAudioComponent->Play();
	}
}

// Called every frame
void AFirePlace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	//only check if nothing is cooking
	if (CookingMeat.IsValid())
	{
		CookMeat(DeltaTime);
		CheckIfSomeonePickedUpItem();
	}
	
}

void AFirePlace::SkewerMeat(AMeat* Meat)
{
	//Sound logic
	if (CookingSound != nullptr)
	{
		CookingAudioComponent->SetSound(CookingSound);
		CookingAudioComponent->Play();
	}
	Meat->DisablePhysicsSimulation();
	Meat->DisableCollision();
	Meat->AttachToComponent(ArrowComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CookingMeat = Meat;
}

float AFirePlace::GetCookPercentage()
{
	if (CookingMeat.IsValid())
	{
		return CookingMeat->GetCookedPercentage();
	}
	return 0.f;
}

void AFirePlace::CookMeat(float deltaTime)
{
	// If it has meat cooking
	if (CookingMeat.IsValid())
	{
		//If it isnt fully cooked yet
		const float cookPercentage{ CookingMeat->GetCookedPercentage() };
		//Cook meat
		CookingMeat->SetCookedPercentage(cookPercentage + CookPercentagePerSecond * deltaTime);
	}
}

void AFirePlace::CheckIfSomeonePickedUpItem()
{
	//Getting the parent of the CookingMeat
	if (CookingMeat.IsValid())
	{
		const USceneComponent* RootComponentMeat = CookingMeat->GetRootComponent();
		if (RootComponentMeat != nullptr)
		{
			const USceneComponent* ParentComponentMeat = RootComponentMeat->GetAttachParent();
			if (ParentComponentMeat != nullptr)
			{
				const AActor* ParentActorMeat = ParentComponentMeat->GetOwner();
				//if true: this means that it is attached to something else than the campfire
				if (ParentActorMeat != this)
				{
					CookingMeat = nullptr;

					// stop sound effect
					CookingAudioComponent->Stop();
				}
			}
		}
	}
}
