#include "Actor.h"
#include "Utils/Utils.h"
#include "Engine.h"
#include "Level/Level.h"

#include <Windows.h>
#include <iostream>

Actor::Actor(const char* image, int width, int height, Color bg, Color color, unsigned int order, const Vector2& position)
	: width(width), height(height), bg(bg), color(color), sortingOrder(order), position(position)
{
	if (image == nullptr)
	{
		return;
	}

	// 문자열 길이.
	int originalLength = (int)strlen(image);

	// 메모리 할당.
	this->image = new char[originalLength + 1];

	// 문자열 복사.
	strcpy_s(this->image, originalLength + 1, image);
}

Actor::~Actor()
{
	// 메모리 해제.
	SafeDeleteArray(image);
}

// 객체 생애주기(Lifetime)에 1번만 호출됨 (초기화 목적).
void Actor::BeginPlay()
{
	hasBeganPlay = true;
}

// 프레임 마다 호출 (반복성 작업/지속성이 필요한 작업).
void Actor::Tick(float deltaTime)
{
}

// 그리기 함수.
void Actor::Render()
{
	// 커서 이동.
	//Utils::SetConsolePosition(position);

	// 색상 설정.
	//Utils::SetConsoleTextColor(color);

	// 그리기.
	//std::cout << image;

	// 엔진이 관리하는 이미지 버퍼에 액터의 문자열/색상 기록.
	Engine::Get().WriteToBuffer(position, image, width, height, bg, color, sortingOrder);
}

void Actor::SetPosition(const Vector2& newPosition)
{
	// 같으면 업데이트 안함.
	if (position == newPosition)
	{
		return;
	}

	position = newPosition;
}

Vector2 Actor::Position() const
{
	return position;
}

int Actor::Width() const
{
	return width;
}

int Actor::Height() const
{
	return height;
}

void Actor::SetSortingOrder(unsigned int sortingOrder)
{
	this->sortingOrder = sortingOrder;
}

void Actor::SetOwner(Level* newOwner)
{
	owner = newOwner;
}

Level* Actor::GetOwner()
{
	return owner;
}

bool Actor::TestIntersect(const Actor* const other, Vector2& pos)
{
	// AABB(Axis Aligned Bounding Box).

	// 이 액터의 x 좌표 정보.
	int left = position.x;
	int right = position.x + width - 1;
	int top = position.y;
	int bottom = position.y + height - 1;

	// 충돌 비교할 다른 액터의 x 좌표 정보.
	int other_left = other->position.x;
	int other_right = other->position.x + other->width - 1;
	int other_top = other->position.y;
	int other_bottom = other->position.y + other->height - 1;

	//위로 넘어가는거 방지
	if (other_top == 0 && bottom < other_top && !(right < other_left || left > other_right))
	{
		pos = Vector2(right - 3, top);
		return true;
	}

	// 안겹치는 조건 확인.
	if (right < other_left || left > other_right ||
		bottom < other_top || top > other_bottom)
	{
		return false;
	}

	// y 좌표가 같은지 최종 확인.
	if (left < other_left)	//좌 충돌
	{
		if (other_top == 0)
			pos = Vector2(right-3, top);

		else
			pos = Vector2(right-3, bottom);
	}
	else if (right > other_right)	//우 충돌
	{
		if (other_top == 0)
			pos = Vector2(left, top);
		else
			pos = Vector2(left, bottom);
	}
	else
	{
		if (other_top == 0)
			pos = Vector2(right - width / 2 , top);
		else
			pos = Vector2(right - width / 2 , bottom);
	}
	return true;
}

void Actor::ChangeImage(const char* newImage)
{
	// 기존 이미지 메모리 해제.
	SafeDelete(image);

	// 새로운 문자열 복사.
	size_t length = strlen(newImage) + 1;
	image = new char[length];
	strcpy_s(image, length, newImage);
}

void Actor::ChangeColor(Color bg, Color color)
{
	this->bg = bg;
	this->color = color;
}

void Actor::Destroy()
{
	// 중복 삭제 방지 처리.
	if (isExpired)
	{
		return;
	}

	// 삭제 요청 되었다고 설정.
	isExpired = true;

	// 레벨에 삭제 요청.
	owner->DestroyActor(this);

	// 죽음 요청됐다고 알림.
	OnDestroy();
}

void Actor::OnDestroy()
{

}

void Actor::SetLifetime(float newLifetime)
{
	// 입력값 확인.
	if (newLifetime <= 0.0f)
	{
		return;
	}

	// 수명 주기 설정.
	lifetime = newLifetime;

	// 자동 제거 옵션 활성화.
	autoDestroy = true;
}

void Actor::QuitGame()
{
	Engine::Get().Quit();
}
