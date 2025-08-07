#pragma once

#include "Actor/Actor.h"
#include "Timer.h"

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)
public:
	enum class State : int
	{
		IDLE,
		UP,
		DOWN,
		DEAD
	};

	Player(Vector2 pos);
	virtual void Tick(float deltaTime) override;
	void OnDamaged();
	bool DidStart();

private:

	// 오른쪽 이동 함수.
	void MoveUp();

	// 게임 종료 함수.
	void Quit();

	void SetState(float vel);

private:
	float yPosition = 0.0f;
	float yVelocity = 0.0f;
	static constexpr float JUMP_STRENGTH{ 6.5f };
	static constexpr float GRAVITY{ 25.f };

	bool didStart{ false };
	bool isDead{ false };
	State prevState{ State::IDLE };
	State currState{ State::IDLE };
};