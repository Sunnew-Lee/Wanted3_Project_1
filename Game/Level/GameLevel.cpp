#include "GameLevel.h"
#include "Actor/Player.h"
#include "Actor/Pipe.h"
#include "Actor/PlayerBullet.h"
#include "Actor/EnemyBullet.h"

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

GameLevel::GameLevel()
{
	// todo: 플레이어 저장해 둘 필요가 있는가?
	Player* player = new Player(Vector2(21,19));
	//playerWidth = player->Width();
	AddActor(player);

	// 타이머 설정.
	enemySpawnTimer.SetTargetTime(0.1f);
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
	super::Tick(deltaTime);

	// 적 생성.
	SpawnEnemies(deltaTime);

	// 플레이어 탄약과 적의 충돌 처리.
	//ProcessCollisionPlayerBulletAndEnemy();

	// 적의 탄약과 플레이어의 충돌 처리.
	//ProcessCollisionPlayerAndEnemyBullet();
}

void GameLevel::SpawnEnemies(float deltaTime)
{
	// 적 생성.
	enemySpawnTimer.Tick(deltaTime);

	// 타임 아웃 확인.
	if (!enemySpawnTimer.IsTimeout())
	{
		return;
	}

	// 타이머 정리.


	// todo: 점수 더하기 로직
	++score;
	if (score == 100)
	{
		score = 99;
	}

	// 기둥 길이 랜덤으로 구하기. 최소 기둥길이는 2, 최대는 18
	int ranLen = Utils::Random(2, 18);

	// 기둥 생성.
	AddActor(new Pipe(ranLen, 0));	//윗기둥
	AddActor(new Pipe(20 - ranLen, 1));	//아랫기둥

	enemySpawnTimer.SetTargetTime(5.f);
	enemySpawnTimer.Reset();
	//enemySpawnTimer.SetTargetTime(1.f);
}

void GameLevel::ProcessCollisionPlayerBulletAndEnemy()
{
	// 플레이어 탄약 액터 배열.
	std::vector<PlayerBullet*> bullets;
	std::vector<Pipe*> enemies;

	// 두 타입의 액터를 검색해서 배열 채우기.
	for (Actor* const actor : actors)
	{
		PlayerBullet* bullet = actor->As<PlayerBullet>();
		if (bullet)
		{
			bullets.emplace_back(bullet);
			continue;
		}

		Pipe* enemy = actor->As<Pipe>();
		if (enemy)
		{
			enemies.emplace_back(enemy);
		}
	}

	// 예외처리 (안해도 상황 확인).
	if (bullets.size() == 0 || enemies.size() == 0)
	{
		return;
	}

	// 충돌 처리.
	for (auto* bullet : bullets)
	{
		for (auto* enemy : enemies)
		{
			// 두 액터가 서로 겹쳤는지 확인.
			if (bullet->TestIntersect(enemy))
			{
				//enemy->Destroy();

				// 적에게 대미지 받았다고 알림.
				//enemy->OnDamaged();

				// 플레이어 탄약 제거.
				bullet->Destroy();

				// 점수를 획득했기 때문에 점수 증가 처리해야 함.
				score = score + 1;
				continue;
			}
		}
	}
}

void GameLevel::ProcessCollisionPlayerAndEnemyBullet()
{
	Player* player = nullptr;
	std::vector<EnemyBullet*> bullets;

	for (Actor* const actor : actors)
	{
		// 적 탄약인지 확인 후 배열에 추가.
		EnemyBullet* bullet = actor->As<EnemyBullet>();
		if (bullet)
		{
			bullets.emplace_back(bullet);
			continue;
		}

		// 플레이어 확인.
		if (!player)
		{
			player = actor->As<Player>();
		}
	}

	// 안해도 되는 상황 확인.
	if (bullets.size() == 0 || !player)
	{
		return;
	}

	// 충돌 확인.
	for (auto* bullet : bullets)
	{
		if (player->TestIntersect(bullet))
		{
			// 죽음 처리 (게임 종료).
			isPlayerDead = true;
			playerDeadPosition.x = player->Position().x + player->Width() / 2;
			playerDeadPosition.y = player->Position().y;

			player->Destroy();
			bullet->Destroy();
			break;
		}
	}
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
		Engine::Get().WriteToBuffer(Vector2(26, 5), numberType[first], DIGIT_WIDTH, DIGIT_HEIGHT, Color::White, Color::None, 100);
		Engine::Get().WriteToBuffer(Vector2(32, 5), numberType[second], DIGIT_WIDTH, DIGIT_HEIGHT, Color::White, Color::None, 100);
	}
}

void GameLevel::Render()
{
	super::Render();

	//PrintMenu();

	// todo: 게임 종료 시 처리.
	if (isPlayerDead)
	{
		// 다음에 그릴 위치가 화면 밖을 넘어가지 않도록 보정.
		// 아래 문자열 중에서 길이가 가장 긴 문자열을 기준으로 위치 보정.
		int longestStringLength = (int)strlen("Game Over!");
		int x = playerDeadPosition.x + longestStringLength > Engine::Get().Width() ?
			Engine::Get().Width() - longestStringLength : playerDeadPosition.x;
		int y = playerDeadPosition.y;
		//Engine::Get().WriteToBuffer(Vector2(x, y - 3), "   .   ", Color::Red);
		//Engine::Get().WriteToBuffer(Vector2(x, y - 2), " .  .  .", Color::Red);
		//Engine::Get().WriteToBuffer(Vector2(x, y - 1), "..:V:..", Color::Red);
		//Engine::Get().WriteToBuffer(Vector2(x, y), "Game Over!", Color::Red);


		// 위에서 출력 요청한 글자를 바로 화면에 보이도록 함수 호출.
		Engine::Get().PresentImmediately();

		// 잠깐 정지(대략 2초).
		Sleep(2000);
		Engine::Get().Quit();
	}

	ShowGameScore();
}