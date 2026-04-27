// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "InputMappingContext.h"
#include "InputAction.h"

#include "EnhancedInputSubsystems.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	// 기본 설정.
	// 컨트롤러의 회전 값을 받아서 설정하는 옵션 비활성화.
	bUseControllerRotationPitch = false; // y축 회전
	bUseControllerRotationYaw = false;   // z축 회전
	bUseControllerRotationRoll = false;  // x축 회전

	// 무브먼트 설정. 전방선언되어서 헤더 추가 해야함.
	// 캐릭터가 이동하는 방향에 맞게 회전을 해주는 옵션.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	// 메시 컴포넌트 설정.
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// 메시 애셋 지정 (검색 필요함).
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple")
	);

	// 로드 성공했으면 설정.
	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// 애님 블루프린트 클래스 정보 지정.
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(
		TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed.ABP_Unarmed_C")
	);

	if (CharacterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnim.Class);
	}

	// 컴포넌트 생성.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	// 계층 설정 (루트 컴포넌트 아래로).
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	// 컨트롤러의 회전 값을 사용할 지 여부.
	SpringArm->bUsePawnControlRotation = true;

	// 카메라 컴포넌트.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// 입력 관련 애셋 로드 및 설정.
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextRef(
			TEXT("/Game/ArenaBattle/Input/IMC_Default.IMC_Default")
	);

	if (DefaultMappingContextRef.Succeeded())
	{
		DefaultMappingContext = DefaultMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(
		TEXT("/Game/ArenaBattle/Input/Actions/IA_Move.IA_Move")
	);

	if (MoveActionRef.Succeeded())
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(
		TEXT("/Game/ArenaBattle/Input/Actions/IA_Look.IA_Look")
	);

	if (LookActionRef.Succeeded())
	{
		LookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(
		TEXT("/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump")
	);

	if (JumpActionRef.Succeeded())
	{
		JumpAction = JumpActionRef.Object;
	}

}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// 사용할 입력 매핑 컨텍스트 지정.
	// 다운캐스팅
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) 
	{
		// 향상된 입력 시스템의 서브 시스템 가져오기.
		UEnhancedInputLocalPlayerSubsystem* InputSystem 
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()
		);

		// 향상된 입력 서브 시스템 얻어온 후에 사용할 매핑 컨텍스트 설정.
		if (InputSystem)
		{
			InputSystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}
