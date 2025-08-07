#pragma once

#include "Level/Level.h"
#include "Timer.h"
#include "Math/Vector2.h"

class Player;
class PlayerDestroyEffect;

class GameLevel : public Level
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	~GameLevel();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;
	virtual void Reset() override;
	
private:
	void SpawnPlayer();
	void SpawnEnemies(float deltaTime);
	void MoveScore(float deltaTime);
	
	void ProcessCollisionPlayer_Pipe();
	void ProcessCollisionPlayer_Floor();

	void ShowGameScore();

	//void PrintMenu();

	//protected으로 바꿔서 player가 isPlayerDead 접근하도록 해?
private:
	Player* player{ nullptr };
	PlayerDestroyEffect* PDE{ nullptr };

	// 파이프 생성 시 시간 계산을 위한 타이머.
	Timer pipeSpawnTimer;

	// 스코어보드 움직임 타이머
	Timer scoreMoveTimer;

	// 점수 변수.
	int score = 0;	//00~99

	// 플레이어 죽은 위치(다음 액터 생성 위치)
	Vector2 deadPosition;

	// 스코어 출력 위치.
	Vector2 scorePosition[2];

	int checkDir = 0;	// check only downwards

	static constexpr int DIGIT_WIDTH = 5; // 각 숫자의 가로 크기
	static constexpr int DIGIT_HEIGHT = 5; // 각 숫자의 세로 크기

	int FLOOR_Y = 34;
};
