#pragma once

#include "Actor/Actor.h"
#include "Timer.h"

/*
* 작성자: 장세윤.
* EnemyDestroyEffect 클래스.
* Enemy가 죽었을 때 죽음 효과를 재생할 때 생성하는 액터.
*/

class PlayerDestroyEffect : public Actor
{
	RTTI_DECLARATIONS(PlayerDestroyEffect, Actor)

public:

	PlayerDestroyEffect(const Vector2& position);

	virtual void Tick(float deltaTime) override;

private:

	Timer moveTimer;
};