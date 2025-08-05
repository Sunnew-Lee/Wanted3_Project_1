#pragma once

#include "Actor/Actor.h"
#include "Utils/Timer.h"

/*
* 기능
* todo: 마우스 좌클
* - 상 이동 (SpaceBar 기반).
*/
class Player : public Actor
{
	// todo: Animation?
	enum class Anim
	{
		None = -1,
		UP,
		DOWN
	};

	RTTI_DECLARATIONS(Player, Actor)
public:
	//Player(float fireInterval = 0.15f, FireMode fireMode = FireMode::OneShot);
	Player(Vector2 pos);
	virtual void Tick(float deltaTime) override;

private:

	// 오른쪽 이동 함수.
	void MoveUp();

	//void OnDestroy() override;

	// 왼쪽 이동 함수.
	//void MoveLeft();

	// 한 발씩 발사하는 함수.
	//void Fire();

	// 연사 모드 함수.
	//void FireInterval();

	// 한 발씩 발사/연사 모드 변경 함수.
	//void ChangeFireMode();

	// 게임 종료 함수.
	void Quit();

	// 연사 모드일 때 발사가 가능한지 확인하는 함수.
	//bool CanShoot();

private:
	// 발사 모드.
	//FireMode fireMode = FireMode::None;

	// 연사 간격 (단위: 초).
	//float fireInterval = 0.15f;

	// 발사 간격 제어 타이머.
	//Timer fireIntervaltimer;

	float yPosition = 0.0f;
	float yVelocity = 0.0f;
	float yAcceleration = 0.0f;
	static constexpr float JUMP_STRENGTH{ 6.5f };
	static constexpr float GRAVITY{ 25.f };
	bool didStart{ false };
};