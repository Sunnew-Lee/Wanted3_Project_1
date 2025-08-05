#include "Pipe.h"
#include "Utils/Utils.h"
#include "Engine.h"
#include "Level/Level.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemyDestroyEffect.h"

//todo: Up기둥 Down기둥. 생성할때 지정하면 여기서 처리.
// 점수 표시에 사용할 글자 값.
// 여기에서 static은 private임.
static const char* pipeConstants[] =
{
	// 기둥 한칸
	"|||||||||\n",	//10
	// 마무리
	"|||||||||",	//9

	// 기둥 머리 반드시 두칸.
	"---------\n---------\n",	//20
	//마무리
	"---------\n---------"		//19
};

Pipe::Pipe(int length, int dir) :Actor("i",PIPE_WIDTH,length+2, Color::IRed, Color::Red, 1)
{
	MakePipe(length, dir);
	
	// 기둥은 늘 오른쪽에서 출력
	position.x = Engine::Get().Width() - 1;
	position.y = (dir == 0 ? 0 : Engine::Get().Height() - 6 - length - 2);	// 진디 위부터 시작(6), 기둥길이(length), 머리 길이(2)

	// 타이머 설정.
	moveTimer.SetTargetTime(0.1f);
}

void Pipe::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	moveTimer.Tick(deltaTime);

	if (!moveTimer.IsTimeout())
	{
		return;
	}

	// 한칸 왼쪽으로 움직임
	--position.x;

	// 화면 왼쪽으로 완전히 벗어났는지 확인.
	if (position.x + width <= 0)
	{
		// 화면에서 벗어나면 액터 제거.
		Destroy();
		return;
	}

	// 안 벗어났으면 위치 설정.
	SetPosition(position);

	moveTimer.Reset();
}

// todo: ???
void Pipe::OnDestroy()
{
	super::OnDestroy();
}

void Pipe::MakePipe(int len, int dir)
{
	char temp[500];

	if (dir == 0)	//facing down
	{
		strcpy_s(temp, PIPE_WIDTH + 2, pipeConstants[0]);	// 9 + \n + \0
		//temp[PIPE_WIDTH - 1] = '\n';
		for (int i{ 1 }; i < len; i++)
		{
			strcat_s(temp, PIPE_WIDTH * (i + 1) + (2+i), pipeConstants[0]);
		}

		strcat_s(temp, PIPE_WIDTH * (len + 2) + PIPE_WIDTH*len + 2, pipeConstants[3]);
	}
	else //facing up
	{
		strcpy_s(temp, (PIPE_WIDTH+1)*2 + 2, pipeConstants[2]);	// 9 + \n + \0
		//temp[PIPE_WIDTH - 1] = '\n';
		for (int i{ 0 }; i < len-1; i++)
		{
			strcat_s(temp, (PIPE_WIDTH+1) * (i + 3) + 1, pipeConstants[0]);
		}

		strcat_s(temp, (PIPE_WIDTH+1) * (len + 1) + PIPE_WIDTH + 1, pipeConstants[1]);
	}

	//super::image
	SafeDeleteArray(image);
	int originalLength = (int)strlen(temp);

	// 메모리 할당.
	image = new char[originalLength + 1];

	// 문자열 복사.
	strcpy_s(image, originalLength + 1, temp);

	//Actor::Actor(temp, PIPE_WIDTH, len + 2, Color::IRed, Color::Red);
}

//void Pipe::OnDamaged()
//{
//	// todo: 이거 처리할꺼임? 기둥은 데미지 안입는거 아닐까?
//}