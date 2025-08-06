#include "Game.h"
#include "Level/GameLevel.h"

Game::Game()
{
	AddLevel(new GameLevel());
	Input::Get().RegisterKeydownEvent<Game, &Game::Restart>(VK_SPACE, this);
}

void Game::Restart()
{
	if (mainLevel->GetisCleared())
	{
		Engine::Restart();
		Input::Get().RegisterKeydownEvent<Game, &Game::Restart>(VK_SPACE, this);
	}
}