#include "Player.h"
#include "Input.h"
#include "Utils/Utils.h"
#include "Level/Level.h"
#include "Engine.h"

#include <Windows.h>
#include <iostream>

//Player::Player(float fireInterval, FireMode fireMode)
//	: Actor("<<A>>", Color::Red),
//	fireInterval(fireInterval), fireMode(fireMode)
//{
//	// 시작 위치 (화면의 가운데, 가장 아래쪽).
//	int xPosition = Engine::Get().Width() / 2 - width / 2 - 1;
//	int yPosition = Engine::Get().Height() - 2;
//	SetPosition(Vector2(xPosition, yPosition));
//
//	// 타이머 시간 설정.
//	fireIntervaltimer.SetTargetTime(fireInterval);
//
//	// keydown 콜백 등록.
//	Input::Get().RegisterKeydownEvent<Player, &Player::Quit>(VK_ESCAPE, this);
//	Input::Get().RegisterKeydownEvent<Player, &Player::Fire>(VK_SPACE, this);
//	Input::Get().RegisterKeydownEvent<Player, &Player::ChangeFireMode>('R', this);
//
//	// keyrepeat 콜백 등록.
//	Input::Get().RegisterKeyrepeatEvent<Player, &Player::FireInterval>(VK_SPACE, this);
//	Input::Get().RegisterKeyrepeatEvent<Player, &Player::MoveLeft>(VK_LEFT, this);
//	Input::Get().RegisterKeyrepeatEvent<Player, &Player::MoveRight>(VK_RIGHT, this);
//}

// Actor("( > <\n/  A >", 5, 2, Color::IRed, Color::None, 5, pos)
Player::Player(Vector2 pos) : Actor("^ @ @\n/ *V*>", 5, 2, Color::IYellow, Color::None, 5, pos)
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

	if (yVelocity > 8.f)
	{
		yVelocity = 8.f;
	}

	//todo: 속도에 따른 ChangeImage

	yPosition += yVelocity * deltaTime*5.f;

	SetPosition(Vector2(position.x, (int)yPosition)); 
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

void Player::OnDamaged()
{
	isDead = true;
	//image change
	ChangeImage("( > <\n/  A >");
	ChangeColor(Color::IRed, Color::None);
}