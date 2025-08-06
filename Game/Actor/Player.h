#pragma once

#include "Actor/Actor.h"
#include "Timer.h"

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)
public:
	//Player(float fireInterval = 0.15f, FireMode fireMode = FireMode::OneShot);
	Player(Vector2 pos);
	virtual void Tick(float deltaTime) override;
	void OnDamaged();

private:

	// 오른쪽 이동 함수.
	void MoveUp();

	// 게임 종료 함수.
	void Quit();


private:
	// todo: 플레이어 죽으면 주르륵 타이머?
	float yPosition = 0.0f;
	float yVelocity = 0.0f;
	static constexpr float JUMP_STRENGTH{ 6.5f };
	static constexpr float GRAVITY{ 25.f };

	bool didStart{ false };
	bool isDead{ false };
};