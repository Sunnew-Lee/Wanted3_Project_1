#include "PlayerDestroyEffect.h"
#include "Engine.h"

PlayerDestroyEffect::PlayerDestroyEffect(const Vector2& position) : Actor("E  >\nE  x\n d x", 4, 3, Color::IRed, Color::None, 5, position)
{
	moveTimer.SetTargetTime(0.05f);
}

void PlayerDestroyEffect::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (position.y == 32)
		return;

	// 떨어짐을 위해 타이머 업데이트.
	moveTimer.Tick(deltaTime);
	if (!moveTimer.IsTimeout())
	{
		return;
	}
	
	position.y++;

	moveTimer.Reset();
};