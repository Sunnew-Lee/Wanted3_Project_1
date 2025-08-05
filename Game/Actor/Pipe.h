#pragma once

#include "Actor/Actor.h"
#include "Utils/Timer.h"

/*
* 이동 패턴 (좌우).
* - 오른쪽에서 태어났으면 왼쪽으로 이동.
* - 왼쪽에서 태어났으면 오른쪽으로 이동.
*/
class Pipe : public Actor
{
	RTTI_DECLARATIONS(Pipe, Actor)

public:
	// 적 캐릭터가 보여줄 이미지(문자열)와 생성 y위치.
	Pipe(int length, int dir);

	virtual void Tick(float deltaTime) override;

	virtual void OnDestroy() override;

	// 플레이어 탄약에 맞았을 때 호출할 함수.
	//void OnDamaged();

private:
	void MakePipe(int len, int dir);

	// 속력을 고려한 이동 위치 계산을 위한 float 변수.
	//float xPosition = 0.0f;

	// 이동 빠르기.
	//float moveSpeed = 5.0f;

	// 타이머.
	Timer moveTimer;
	static constexpr int PIPE_WIDTH{ 9 };
	// 타이머 계산을 위한 변수.
	// 경과 시간 계산 변수.
	//float elapsedTime = 0.0f;

	// 목표 시간 값. (setTime, targetTime).
	//float targetTime = 0.0f;
};