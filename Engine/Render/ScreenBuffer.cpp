#include "ScreenBuffer.h"
#include <iostream>

ScreenBuffer::ScreenBuffer(const Vector2& screenSize)
	: screenSize(screenSize)
{
	// 접근 모드 설정 (읽기/쓰기).
	unsigned long accessMode = GENERIC_READ | GENERIC_WRITE;

	// 공유 모드 설정 (파일 입출력).
	unsigned long shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// 콘솔 버퍼 생성.
	buffer = CreateConsoleScreenBuffer(accessMode, shareMode, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	// 예외처리: 버퍼 생성 실패.
	if (buffer == INVALID_HANDLE_VALUE)
	{
		//__debugbreak();
		std::cout << "버퍼 생성 실패\n";
		return;
	}

	// 콘솔 창 크기 설정. -1씩 한 이유는 화면 버퍼보다 -1이 최대이기 때문
	SMALL_RECT rect{ 0, 0, (short)screenSize.x - 1, (short)screenSize.y - 1 };
	BOOL setWindowsInfoResult = SetConsoleWindowInfo(buffer, true, &rect);
	if (!setWindowsInfoResult)
	{
		std::cerr << "Failed to set console window size\n";
		__debugbreak();
	}

	// 콘솔 버퍼 크기 설정.
	BOOL setScreenBufferSizeResult = SetConsoleScreenBufferSize(buffer, Vector2(screenSize.x, screenSize.y));
	if (!setScreenBufferSizeResult)
	{
		std::cerr << "Failed to set screen buffer size\n";
		__debugbreak();
	}

	// 커서 안보이게 설정.
	CONSOLE_CURSOR_INFO cursorInfo{ 1, FALSE };
	SetConsoleCursorInfo(buffer, &cursorInfo);
}

ScreenBuffer::ScreenBuffer(HANDLE console, const Vector2& screenSize)
	: screenSize(screenSize), buffer(console)
{
	// 콘솔 창 크기 설정. -1씩 한 이유는 화면 버퍼보다 -1이 최대이기 때문
	SMALL_RECT rect{ 0, 0, (short)screenSize.x - 1, (short)screenSize.y - 1 };
	BOOL setWindowsInfoResult = SetConsoleWindowInfo(buffer, true, &rect);
	if (!setWindowsInfoResult)
	{
		std::cerr << "Failed to set console window size\n";
		__debugbreak();
	}

	// 콘솔 버퍼 크기 설정.
	BOOL setScreenBufferSizeResult = SetConsoleScreenBufferSize(buffer, Vector2(screenSize.x, screenSize.y));
	if (!setScreenBufferSizeResult)
	{
		std::cerr << "Failed to set screen buffer size\n";
		__debugbreak();
	}

	// 커서 안보이게 설정.
	CONSOLE_CURSOR_INFO cursorInfo{ 1, FALSE };
	SetConsoleCursorInfo(buffer, &cursorInfo);

	//CONSOLE_FONT_INFOEX cfi;
	//BOOL b = GetCurrentConsoleFontEx(buffer, FALSE, &cfi);
	////cfi.dwFontSize.X/=2;
	//cfi.dwFontSize.Y/=2;
	//BOOL a = SetCurrentConsoleFontEx(buffer, FALSE, &cfi);
	//if (!a)
	//{
	//	std::cout << "폰트 정보 설정 실패!" << std::endl;
	//	return ;
	//}
	

	// 1. 콘솔 윈도우 핸들(HWND)을 가져옵니다.
//    (버퍼를 제어하는 HANDLE과 다른, 창 자체를 가리키는 핸들입니다.)
	HWND consoleWindow = GetConsoleWindow();

	// 2. 현재 윈도우 스타일 값을 가져옵니다.
	LONG_PTR style = GetWindowLongPtr(consoleWindow, GWL_STYLE);

	// 3. 제목 표시줄, 크기 조절 테두리, 시스템 메뉴 등의 스타일을 제거합니다.
	style &= ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_VSCROLL | WS_HSCROLL);

	// 4. 변경된 스타일을 다시 적용합니다.
	SetWindowLongPtr(consoleWindow, GWL_STYLE, style);
}

ScreenBuffer::~ScreenBuffer()
{
	// 생성한 핸들 해제.
	if (buffer)
	{
		CloseHandle(buffer);
	}
}

void ScreenBuffer::Clear()
{
	// 커서 위치.
	Vector2 position;

	// 콘솔에 기록한 글자 수를 반환받기 위한 변수.
	DWORD writtenCount = 0;

	// 화면 버퍼 전체에 빈 문자열 기록.
	// 콘솔 전체에 한 글자를 입력할 때 사용 (memset과 비슷).
	FillConsoleOutputCharacter(buffer, ' ', screenSize.x * screenSize.y, position, &writtenCount);
}

void ScreenBuffer::Render(CHAR_INFO* charInfo)
{
	// 버퍼 위치 초기화.
	Vector2 bufferPosition;

	// 기록할 영역의 크기.
	//todo: why -1 -1??
	SMALL_RECT writeRegion{ 0, 0, (short)screenSize.x - 1, (short)screenSize.y - 1 };

	// 전달된 문자 정보를 버퍼에 기록.
	WriteConsoleOutputA(buffer, charInfo, screenSize, bufferPosition, &writeRegion);
}