#include "Engine.h"
#include <iostream>
#include <Windows.h>

#include "Level/Level.h"
#include "Utils/Utils.h"
#include "Input.h"
#include "Render/ScreenBuffer.h"

// 정적 변수 초기화.
Engine* Engine::instance = nullptr;

BOOL WINAPI ConsoleMessageProcedure(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		// Engine의 메모리 해제.
		//Engine::Get().~Engine();
		Engine::Get().CleanUp();
		return false;
	}

	return false;
}

Engine::Engine()
{
	instance = this;

	// 콘솔 커서 끄기.
	CONSOLE_CURSOR_INFO info;
	info.bVisible = false;
	info.dwSize = 1;

	SetConsoleCursorInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&info
	);
	
	// 엔진 설정 로드.
	LoadEngineSettings();

	// 랜덤 종자값(seed) 설정.
	srand(static_cast<unsigned int>(time(nullptr)));

	// 이미지 버퍼 생성 / 콘솔에 보낼 버퍼 생성.
	Vector2 screenSize(settings.width, settings.height);

	imageBuffer = new ImageBuffer(screenSize.x * screenSize.y);
	bgBuffer = new ImageBuffer(screenSize.x * screenSize.y);

	// 버퍼 초기화 (문자 버퍼).
	//ClearImageBuffer();
	LoadBgBuffer();

	// 두 개의 버퍼 생성.
	renderTargets[0] = new ScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE), screenSize);
	renderTargets[1] = new ScreenBuffer(screenSize);

	// 버퍼 교환.
	Present();

	// 콘솔 창 이벤트 등록.
	SetConsoleCtrlHandler(ConsoleMessageProcedure, TRUE);

	// cls 호출.
	system("cls");
}

Engine::~Engine()
{
	CleanUp();
}

void Engine::Run()
{
	// 밀리 세컨드 단위로 현재 시간을 알려줌.
	//float currentTime = timeGetTime();
	LARGE_INTEGER currentTime;
	LARGE_INTEGER previousTime;
	QueryPerformanceCounter(&currentTime);
	previousTime = currentTime;

	// 하드웨어 시계의 정밀도(주파수) 가져오기.
	// 나중에 초로 변환하기 위해.
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	// 타겟 프레임.
	float targetFrameRate 
		= settings.framerate == 0.0f ? 60.0f : settings.framerate;

	// 타겟 한 프레임 시간.
	float oneFrameTime = 1.0f / targetFrameRate;

	// GameLoop.
	while (true)
	{
		// 엔진 종료 여부 확인.
		if (isQuit)
		{
			// 루프 종료.
			break;
		}

		// 프레임 시간 계산.
		// (현재 시간 - 이전 시간) / 주파수 = 초단위.
		QueryPerformanceCounter(&currentTime);

		// 프레임 시간.
		float deltaTime = 
			(currentTime.QuadPart - previousTime.QuadPart)
			/ (float)frequency.QuadPart;

		// 입력은 최대한 빨리.
		input.ProcessInput();

		// 고정 프레임.
		if (deltaTime >= oneFrameTime)
		{
			BeginPlay();
			input.DispatchCallbacks();
			Tick(deltaTime);
			Render();

			// 시간 업데이트.
			previousTime = currentTime;

			// 현재 프레임의 입력을 기록.
			input.SavePreviouseKeyStates();

			// 이전 프레임에 추가 및 삭제 요청된 액터 처리.
			if (mainLevel)
			{
				mainLevel->ProcessAddAndDestroyActors();
			}
		}
	}

	// 정리(텍스트 색상 원래대로 돌려놓기).
	Utils::SetConsoleTextColor(Color::White);
}

//pos -> 왼쪽 위 시작 개행 만나면 늘 아래로 내리기
void Engine::WriteToBuffer(
	const Vector2& position,
	const char* image,
	int width, int height,
	Color bg,
	Color color,
	int sortingOrder)
{
	//int length = static_cast<int>(strlen(image));

	// 1. 소스(image)에서 읽기 시작할 인덱스 계산
	// position.x가 -3이라면, image의 0, 1, 2번 인덱스는 화면 밖에 있으므로 3번부터 읽어야 함.
	int sourceStartIndex = 0;
	if (position.x < 0)
	{
		sourceStartIndex = -position.x;
	}

	// 2. 목적지(버퍼)에 쓰기 시작할 X좌표 계산
	// position.x가 -3이라면, 화면의 0번 위치부터 그려야 함.
	// position.x가 양수라면, 해당 위치부터 그리면 됨.
	int destStartX = (position.x < 0) ? 0 : position.x;

	// 3. 실제로 그려야 할 문자의 개수 계산
	// (전체 길이) - (왼쪽에서 잘리는 개수) - (오른쪽에서 잘리는 개수)
	int charsCutFromLeft = sourceStartIndex;
	int charsCutFromRight = 0;

	// 오른쪽으로 짤림
	if (position.x + width > settings.width)
	{
		charsCutFromRight = (position.x + width) - settings.width;
	}

	// todo: 양쪽 다 짤리면 중간만 그려지는지?
	int charsToDraw = width - charsCutFromLeft -charsCutFromRight;

	// 그릴 문자가 하나도 없으면 함수 종료
	if (charsToDraw <= 0)
	{
		return;
	}

	// 4. 계산된 값들을 이용해 버퍼에 직접 기록
	//int y{ 0 };
	for (int y{ 0 }; y < height; ++y)
	{
		for (int i = 0; i < charsToDraw; ++i)
		{
			// 버퍼에 기록할 최종 인덱스
			int destIndex = ((position.y + y) * settings.width) + destStartX + i;

			// @Incomplete: 화면 버퍼 크기 안 넘어가게 예외처리 필요함.
			// 추가로 y좌표가 화면 밖을 벗어나는 경우도 체크하면 더 안정적입니다.
			if (destIndex < 0 || destIndex >= (settings.width * settings.height))
			{
				continue;
			}

			// 뎁스 테스트
			if (imageBuffer->sortingOrderArray[destIndex] > sortingOrder)
			{
				continue;
			}

			// 원본 이미지에서 올바른 문자를 직접 읽어와 버퍼에 기록
			imageBuffer->charInfoArray[destIndex].Char.AsciiChar = image[(width + 1) * y + sourceStartIndex + i];
			imageBuffer->charInfoArray[destIndex].Attributes = Utils::SetColor(bg, color);
			imageBuffer->sortingOrderArray[destIndex] = sortingOrder;
		}
	}
}

void Engine::AddLevel(Level* newLevel)
{
	// 기존에 있던 레벨은 제거.
	if (mainLevel)
	{
		delete mainLevel;
	}

	mainLevel = newLevel;
}

void Engine::CleanUp()
{
	// 레벨 삭제.
	SafeDelete(mainLevel);

	// 문자 버퍼 삭제.
	//SafeDeleteArray(imageBuffer);
	//SafeDeleteArray(charInfo);
	SafeDelete(imageBuffer);
	SafeDelete(bgBuffer);

	// 렌더 타겟 삭제.
	SafeDelete(renderTargets[0]);
	SafeDelete(renderTargets[1]);
}

void Engine::Quit()
{
	// 종료 플래그 설정.
	isQuit = true;
}

Engine& Engine::Get()
{
	return *instance;
}

int Engine::Width() const
{
	return settings.width;
}

int Engine::Height() const
{
	return settings.height;
}

void Engine::OnInitialized()
{

}

void Engine::BeginPlay()
{
	if (mainLevel)
	{
		mainLevel->BeginPlay();
	}
}

void Engine::Tick(float deltaTime)
{
	// 레벨 업데이트.
	if (mainLevel)
	{
		mainLevel->Tick(deltaTime);
	}
}

void Engine::Clear()
{
	SetImageBuffer();
	GetRenderer()->Clear();
}

void Engine::Render()
{
	// 화면 지우기.
	Clear();

	// 레벨 그리기.
	if (mainLevel)
	{
		mainLevel->Render();
	}

	// 백버퍼에 데이터 쓰기.
	//GetRenderer()->Render(imageBuffer);
	GetRenderer()->Render(imageBuffer->charInfoArray);

	// 버퍼 교환.
	Present();
}

void Engine::Present()
{
	// 버퍼 교환.
	SetConsoleActiveScreenBuffer(GetRenderer()->buffer);

	// 인덱스 뒤집기. 1->0, 0->1.
	currentRenderTargetIndex = 1 - currentRenderTargetIndex;
}

void Engine::PresentImmediately()
{
	// 백버퍼에 데이터 쓰기.
	//GetRenderer()->Render(imageBuffer);
	GetRenderer()->Render(imageBuffer->charInfoArray);

	Present();
}

void Engine::LoadEngineSettings()
{
	// 엔진 설정 파일 열기.
	FILE* file = nullptr;
	fopen_s(&file, "../Settings/EngineSettings.txt", "rt");
	if (file == nullptr)
	{
		std::cout << "Failed to load engine settings.\n";
		__debugbreak();
		return;
	}

	// 로드.

	// FP(File Position) 포인터를 가장 뒤로 옮기기.
	fseek(file, 0, SEEK_END);

	// 이 위치 구하기.
	size_t fileSize = ftell(file);

	// 다시 첫 위치로 되돌리기.
	//fseek(file, 0, SEEK_SET);
	rewind(file);

	// 파일 내용을 저장할 버퍼 할당.
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);

	// 내용 읽기.
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// 파싱(Parcing, 구문 해석->필요한 정보를 얻는 과정).
	char* context = nullptr;
	char* token = nullptr;

	token = strtok_s(buffer, "\n", &context);

	// 구문 분석.
	while (token != nullptr)
	{
		// 키/값 분리.
		char header[10] = { };

		// 아래 구문이 제대로 동작하려면 키와 값 사이의 빈칸이 있어야 함.
		sscanf_s(token, "%s", header, 10);

		// 헤더 문자열 비교.
		if (strcmp(header, "framerate") == 0)
		{
			sscanf_s(token, "framerate = %f", &settings.framerate);
		}
		else if (strcmp(header, "width") == 0)
		{
			sscanf_s(token, "width = %d", &settings.width);
		}
		else if (strcmp(header, "height") == 0)
		{
			sscanf_s(token, "height = %d", &settings.height);
		}

		// 그 다음줄 분리.
		token = strtok_s(nullptr, "\n", &context);
	}


	// 버퍼 해제.
	SafeDeleteArray(buffer);

	// 파일 닫기.
	fclose(file);
}

ScreenBuffer* Engine::GetRenderer() const
{
	return renderTargets[currentRenderTargetIndex];
}

void Engine::LoadBgBuffer()
{
	FILE* file = nullptr;

	fopen_s(&file, "../Assets/BG.txt", "rt");
	if (file == nullptr)
	{
		std::cout << "BG Open Failed\n";
		__debugbreak();
		return;
	}

	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* fileBuffer = new char[fileSize];
	size_t readSize = fread(fileBuffer, 1, fileSize, file);

	// i for file traverse, index for bgBuffer traverse(follows i execept if i meets \n, index remains)
	int size = settings.width * settings.height;
	for (int i{ 0 }, index{ 0 }; index < size; ++i, ++index)
	{
		if (fileBuffer[i] == '\n')
		{
			--index;
			continue;
		}

		CHAR_INFO& buffer = bgBuffer->charInfoArray[index];

		// bgBuffer에 문자와 색 입력
		switch (fileBuffer[i])
		{
		case '-':
			buffer.Char.AsciiChar = '-';
			buffer.Attributes = Utils::SetColor(Color::White, Color::IGray);
			break;

		case '$':
			buffer.Char.AsciiChar = ' ';
			buffer.Attributes = Utils::SetColor(Color::IGreen);
			break;

		case '^':
			buffer.Char.AsciiChar = '=';
			buffer.Attributes = Utils::SetColor(Color::Intensity, Color::Green);
			break;

		case '/':
			buffer.Char.AsciiChar = '/';
			buffer.Attributes = 
				Utils::SetColor(Color::IGreen);
			break;
			//todo: ( 대체하기
		case '(':
			buffer.Char.AsciiChar = '(';
			buffer.Attributes = Utils::SetColor(Color::IRed,Color::Red);
			break;

		case '#':
			buffer.Char.AsciiChar = '/';
			buffer.Attributes = Utils::SetColor(Color::Green);
			break;

		case '.':
			buffer.Char.AsciiChar = '.';
			buffer.Attributes = Utils::SetColor(Color::Yellow, Color::IYellow);
			break;

		default:
			buffer.Char.AsciiChar = ' ';
			buffer.Attributes = Utils::SetColor(Color::ICyan, Color::White);
		}
		bgBuffer->sortingOrderArray[index] = -1;
	}

	SafeDeleteArray(fileBuffer);
	fclose(file);
}

//todo: timer for bg
void Engine::SetImageBuffer()
{
	// 1. imageBuffer의 모든 픽셀(x, y)을 순회합니다.
	for (int y = 0; y < settings.height; ++y)
	{
		for (int x = 0; x < settings.width; ++x)
		{
			int sourceX; // 원본 bgBuffer에서 가져올 X좌표

			// 2. 현재 y좌표가 35인지 확인합니다.
			if (y == 35)
			{
				// y가 35일 때만 스크롤 효과를 적용합니다.
				sourceX = (x + offSet) % settings.width;
			}
			else
			{
				// 나머지 줄은 스크롤 없이 그대로 복사합니다.
				sourceX = x;
			}

			// 3. 목적지와 소스의 1차원 인덱스를 계산합니다.
			int destIndex = (y * settings.width) + x;
			int sourceIndex = (y * settings.width) + sourceX;

			// 4. 소스(bgBuffer)에서 목적지(imageBuffer)로 데이터를 복사합니다.
			imageBuffer->charInfoArray[destIndex] = bgBuffer->charInfoArray[sourceIndex];
			imageBuffer->sortingOrderArray[destIndex] = bgBuffer->sortingOrderArray[sourceIndex];
		}
	}

	//5. 다음 프레임을 위해 오프셋을 1 증가시킵니다.
	offSet = (offSet + 1) % settings.width;
}
