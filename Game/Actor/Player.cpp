#include "Player.h"
#include "Input.h"
#include "Utils/Utils.h"
#include "Level/Level.h"
#include "Engine.h"

#include "Actor/PlayerBullet.h"

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
Player::Player(Vector2 pos) : Actor("&&&&&\n&&&&&", 5, 2, Color::IMagenta, Color::Red, 5)
{
	position = pos;
	yPosition = static_cast<float>(pos.y);
	Input::Get().RegisterKeydownEvent<Player, &Player::MoveUp>(VK_SPACE, this);
	Input::Get().RegisterKeydownEvent<Player, &Player::Quit>(VK_ESCAPE, this);
}

void Player::Tick(float deltaTime)
{
	if (didStart == false)
		return;

	super::Tick(deltaTime);

	// 속도 : (yPosition * -1.f) 중력을 두고 Cap을 만들자.
	float accel = (yAcceleration + GRAVITY);
	yVelocity += accel * deltaTime;

	if (yVelocity > 8.f)
	{
		yVelocity = 8.f;
	}

	yPosition += yVelocity * deltaTime*5.f;
	//todo: end 조건
	if (yPosition > 36.f)
		didStart = false;

	SetPosition(Vector2(position.x, (int)yPosition)); 
	//yAcceleration = 0.f;
	//owner->
	// 연사 타이머 업데이트.
	//fireIntervaltimer.Tick(deltaTime);
}

void Player::MoveUp()
{
	if (didStart == false)
		didStart = true;


	yVelocity = -JUMP_STRENGTH;
	//if (position.x + width <= Engine::Get().Width())
	//{
	//	SetPosition(position);
	//}
}

//void Player::OnDestroy()
//{
//	super::OnDestroy();
//	Destroy();
//}

//void Player::MoveLeft()
//{
//	Vector2 position = Position();
//	position.x -= 1;
//
//	if (position.x >= 0)
//	{
//		SetPosition(position);
//	}
//}

//void Player::Fire()
//{
//	// 플레이어 탄약 객체 생성.
//	// x: 플레이어의 가운데.
//	// y: 플레이어에서 한칸 위.
//	Vector2 bulletPosition(position.x + width / 2, position.y);
//	owner->AddActor(new PlayerBullet(bulletPosition));
//}
//
//void Player::FireInterval()
//{
//	// 발사 가능 여부 확인.
//	// 충분한 시간이 지났는지 & 연사 모드 인지.
//	if (!CanShoot() || fireMode != FireMode::Repeat)
//	{
//		return;
//	}
//
//	// 타이머 리셋.
//	fireIntervaltimer.Reset();
//
//	// 탄약 생성.
//	Vector2 bulletPosition(position.x + width / 2, position.y);
//	owner->AddActor(new PlayerBullet(bulletPosition));
//}
//
//void Player::ChangeFireMode()
//{
//	// 단발 모드 / 연사 모드 전환.
//	int mode = (int)fireMode;
//	mode = 1 - mode;
//
//	fireMode = (FireMode)mode;
//}

void Player::Quit()
{
	QuitGame();
}

//bool Player::CanShoot()
//{
//	// 연사 모드에서 발사가 가능한 충분한 시간이 지났는지 확인.
//	return fireIntervaltimer.IsTimeout();
//}