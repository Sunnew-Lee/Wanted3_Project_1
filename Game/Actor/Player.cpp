#include "Player.h"
#include "Input.h"
#include "Utils/Utils.h"
#include "Level/Level.h"
#include "Engine.h"

#include <Windows.h>
#include <iostream>

// 플레이어 표정 표현에 사용할 글자 값.
// 여기에서 static은 private임.
static const char* playerImage[] =
{
	// IDLE
	"^ ~ ~\nU  - >",

	// UP
	"^ @ @\n< *V*>",

	// DOWN
	"^ > <\nU  O >",

	// DEAD
	"^ > <\nU  A >"	
};

// Actor("( > <\n/  A >", 5, 2, Color::IRed, Color::None, 5, pos)
Player::Player(Vector2 pos) : Actor(playerImage[static_cast<int>(State::IDLE)], 5, 2, Color::IYellow, Color::None, 5, pos)
{
	yPosition = static_cast<float>(pos.y);
	Input::Get().RegisterKeydownEvent<Player, &Player::MoveUp>(VK_SPACE, this);
	Input::Get().RegisterKeydownEvent<Player, &Player::Quit>(VK_ESCAPE, this);
}

void Player::Tick(float deltaTime)
{
	if (didStart == false)
		return;
	
	super::Tick(deltaTime);

	// 속도 : 중력을 두고 Cap을 만들자.
	yVelocity += GRAVITY * deltaTime;

	if (yVelocity > 10.f)
	{
		yVelocity = 10.f;
	}

	yPosition += yVelocity * deltaTime*5.f;

	SetPosition(Vector2(position.x, (int)yPosition));
	
	// 속도에 따른 ChangeImage
	SetState(yVelocity);

}

void Player::MoveUp()
{
	if (didStart == false)
	{
		didStart = true;
	}

	if (isDead)
	{
		Destroy();
		return;
	}
		

	yVelocity = -JUMP_STRENGTH;
}

void Player::Quit()
{
	QuitGame();
}

void Player::SetState(float vel)
{
	//if (vel > 0.f && vel < 3.f)
	//{
	//	currState = State::IDLE;
	//}
	if (vel > 5.f && vel < 11.f)
	{
		currState = State::DOWN;
	}
	else 
	{
		currState = State::UP;
	}

	if (prevState != currState)
	{
		ChangeImage(playerImage[static_cast<int>(currState)]);
	}

	prevState = currState;
}

void Player::OnDamaged()
{
	isDead = true;
	//image change
	currState = State::DEAD;
	ChangeImage(playerImage[static_cast<int>(currState)]);
	ChangeColor(Color::IRed, Color::None);
}

bool Player::DidStart()
{
	return didStart;
}
