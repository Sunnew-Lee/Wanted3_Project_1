#include "GameLevel.h"
#include "Actor/Player.h"
#include "Actor/Pipe.h"
#include "Actor/PlayerDestroyEffect.h"

#include "Engine.h"

#include "Utils/Utils.h"

#include <iostream>

// 점수 표시에 사용할 글자 값.
// 여기에서 static은 private임.
static const char* numberType[] =
{
	// 0
	"#####\n#   #\n#   #\n#   #\n#####",

	// 1
	"  #  \n###  \n  #  \n  #  \n#####",

	// 2
	"#####\n    #\n#####\n#    \n#####",

	// 3
	"#####\n    #\n#####\n    #\n#####",

	// 4
	"#   #\n#   #\n#####\n    #\n    #",

	// 5
	"#####\n#    \n#####\n    #\n#####",

	// 6
	"#####\n#    \n#####\n#   #\n#####",

	// 7
	"#####\n    #\n    #\n    #\n    #",

	// 8
	"#####\n#   #\n#####\n#   #\n#####",

	// 9
	"#####\n#   #\n#####\n    #\n#####"
};

GameLevel::GameLevel() : FLOOR_Y(Engine::Get().Height() - 6), scorePosition{ Vector2(26, 5) , Vector2(32, 5) }
{
	SpawnPlayer();

	// 타이머 설정.
	pipeSpawnTimer.SetTargetTime(0.1f);
	scoreMoveTimer.SetTargetTime(0.2f);
}

GameLevel::~GameLevel()
{
}

void GameLevel::BeginPlay()
{
	super::BeginPlay();
}

void GameLevel::Tick(float deltaTime)
{
	if (player->DidStart() == false)
	{
		return;
	}
	if (isCleared == true)
	{
		MoveScore(deltaTime);
		//tick dead
		if(PDE)
			PDE->Tick(deltaTime);

		//if (player == nullptr)
		//{
		//	return;
		//}

		//player = nullptr;
		return;
	}
	
	super::Tick(deltaTime);

	// 적 생성.
	SpawnEnemies(deltaTime);

	// 플레이어와 파이프 충돌 처리.
	ProcessCollisionPlayer_Floor();
	ProcessCollisionPlayer_Pipe();
}

void GameLevel::SpawnPlayer()
{
	player = new Player(Vector2(21, 19));
	AddActor(player);
}

void GameLevel::SpawnEnemies(float deltaTime)
{
	// 적 생성.
	pipeSpawnTimer.Tick(deltaTime);

	// 타임 아웃 확인.
	if (!pipeSpawnTimer.IsTimeout())
	{
		return;
	}

	// 기둥 길이 랜덤으로 구하기. 최소 기둥길이는 2, 최대는 18
	int ranLen = Utils::Random(2, 20);

	// 기둥 생성.
	AddActor(new Pipe(ranLen, 0));	//윗기둥
	AddActor(new Pipe(22 - ranLen, 1));	//아랫기둥


	// 타이머 정리.
	pipeSpawnTimer.SetTargetTime(1.75f);
	pipeSpawnTimer.Reset();
	//enemySpawnTimer.SetTargetTime(1.f);
}

void GameLevel::MoveScore(float deltaTime)
{
	if (scorePosition[0].y > 12)
	{
		return;
	}
	scoreMoveTimer.Tick(deltaTime);

	// 타임 아웃 확인.
	if (!scoreMoveTimer.IsTimeout())
	{
		return;
	}

	++scorePosition[0].y;
	++scorePosition[1].y;

	scoreMoveTimer.Reset();
}

void GameLevel::ProcessCollisionPlayer_Pipe()
{
	// 파이프 컨테이너
	std::vector<Pipe*> pipes;

	// 모든 파이프 찾기
	for (Actor* const actor : actors)
	{
		Pipe* pipe = actor->As<Pipe>();
		if (pipe)
		{
			pipes.emplace_back(pipe);
		}
	}

	// 예외처리 (안해도 상황 확인).
	if (pipes.size() == 0)
	{
		return;
	}

	// 충돌 처리.
	for (Pipe* pipe : pipes)
	{
		// 두 액터가 서로 겹쳤는지 확인.
		if (player->TestIntersect(pipe, deadPosition))
		{
			player->Destroy();

			// 새로운 액터 생성해서 이 자리에서 주르르륵 떨어지게
			// add dead
			PDE = new PlayerDestroyEffect(deadPosition);
			AddActor(PDE);

			// 게임 끝
			isCleared = true;

			// todo: 꼬름한 슬립
			Sleep(200);
			break;
		}

		if (pipe->GetPassed() == false)
		{
			// player의 중앙이 파이프를 만나거나 지나면 점수 추가
			if (player->Position().x + player->Width() / 2 < pipe->Position().x)
			{
				continue;
			}

			//중앙 지난 상태
			pipe->SetPassed(true);

			// Downward 파이프만 점수 올려준다
			if (checkDir != pipe->GetDir())
			{
				continue;
			}

			// 점수 추가
			if (score < 99)
			{
				++score;
			}			
		}
	}
	
}

void GameLevel::ProcessCollisionPlayer_Floor()
{
	if (player->Position().y + player->Height() - 1 < FLOOR_Y)		// Posy+height-1 < 34
	{
		return;
	}

	// 적에게 대미지 받았다고 알림.
	player->OnDamaged();

	// 게임 끝
	isCleared = true;
}

void GameLevel::ShowGameScore()
{
	// 스코어 보여주기.

	// 출력.
	int first = score / 10;
	int second = score % 10;

	// Score가 모든 애들 위에 그려짐.
	for (int i{ 0 }; i < DIGIT_HEIGHT; ++i)
	{
		Engine::Get().WriteToBuffer(scorePosition[0], numberType[first], DIGIT_WIDTH, DIGIT_HEIGHT, Color::White, Color::None, 100,true);
		Engine::Get().WriteToBuffer(scorePosition[1], numberType[second], DIGIT_WIDTH, DIGIT_HEIGHT, Color::White, Color::None, 100, true);
	}
}

void GameLevel::Render()
{
	super::Render();

	//	// 위에서 출력 요청한 글자를 바로 화면에 보이도록 함수 호출.
	//	Engine::Get().PresentImmediately();

	ShowGameScore();
}

void GameLevel::Reset()
{
	super::Reset();
	pipeSpawnTimer.Reset();
	pipeSpawnTimer.SetTargetTime(0.1f);
	scoreMoveTimer.Reset();
	scorePosition[0] = Vector2(26, 5);
	scorePosition[1] = Vector2(32, 5);
	score = 0;
	SpawnPlayer();
	PDE = nullptr;
}
