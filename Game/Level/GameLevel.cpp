#include "GameLevel.h"
#include "Actor/Player.h"
#include "Actor/Pipe.h"

#include "Engine.h"

#include "Utils/Utils.h"

#include <iostream>

// 점수 표시에 사용할 글자 값.
// 여기에서 static은 private임.
static const char* numberType[] =
{
	// 0
	"XXXXX\nX   X\nX   X\nX   X\nXXXXX",

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

GameLevel::GameLevel(): FLOOR_Y(Engine::Get().Height() - 6),scorePositionFirst(Vector2(26, 5)), scorePositionSecond(Vector2(32, 5))
{
	SpawnPlayer();

	// 타이머 설정.
	pipeSpawnTimer.SetTargetTime(0.1f);
	scoreMoveTimer.SetTargetTime(0.25f);
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
	if (isCleared == true)
	{
		MoveScore(deltaTime);

		if (player == nullptr)
		{
			return;
		}
		player = nullptr;
		return;
	}
	
	super::Tick(deltaTime);

	// 적 생성.
	SpawnEnemies(deltaTime);

	// 플레이어와 파이프 충돌 처리.
	ProcessCollisionPlayer_Pipe();
	ProcessCollisionPlayer_Floor();
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
	int ranLen = Utils::Random(2, 18);

	// 기둥 생성.
	AddActor(new Pipe(ranLen, 0));	//윗기둥
	AddActor(new Pipe(20 - ranLen, 1));	//아랫기둥


	// 타이머 정리.
	pipeSpawnTimer.SetTargetTime(5.f);
	pipeSpawnTimer.Reset();
	//enemySpawnTimer.SetTargetTime(1.f);
}

void GameLevel::MoveScore(float deltaTime)
{
	if (scorePositionFirst.y > 13)
	{
		return;
	}
	scoreMoveTimer.Tick(deltaTime);

	// 타임 아웃 확인.
	if (!scoreMoveTimer.IsTimeout())
	{
		return;
	}

	++scorePositionFirst.y;
	++scorePositionSecond.y;

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
		if (player->TestIntersect(pipe))
		{
			// 적에게 대미지 받았다고 알림.
			player->OnDamaged();

			// 게임 끝
			isCleared = true;
			break;
		}

		// todo: 한 쌍이 와서 중복해서 처리될텐데..
		if (pipe->GetPassed() == false)
		{
			// player의 중앙이 파이프를 만나거나 지나면 점수 추가
			if (player->Position().x + player->Width() / 2 < pipe->Position().x)
			{
				continue;
			}

			// 점수 추가
			++score;
			if (score == 100)
			{
				//최대 99점
				score = 99;
			}

			pipe->SetPassed(true);
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
		Engine::Get().WriteToBuffer(scorePositionFirst, numberType[first], DIGIT_WIDTH, DIGIT_HEIGHT, Color::White, Color::None, 100);
		Engine::Get().WriteToBuffer(scorePositionSecond, numberType[second], DIGIT_WIDTH, DIGIT_HEIGHT, Color::White, Color::None, 100);
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
	scorePositionFirst = Vector2(26, 5);
	scorePositionSecond = Vector2(32, 5);
	score = 0;
	SpawnPlayer();
}
