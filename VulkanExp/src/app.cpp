#include <iostream>
#include <vector>

#include "app.h"

App::App() : 
	title("Vulkan example"),
	width(1280), 
	height(720),
	running(true),
	window(nullptr)
{
	initSDL();
	renderer.init(window, title);
}

App::~App()
{
	deinitSDL();
	renderer.deinit();
}

void App::run()
{
	while (running)
	{
		handleEvents();
		update();
		render();
	}
}

void App::handleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			running = false; break;
		}
	}
}

void App::update()
{

}

void App::render()
{
	renderer.begin();
	renderer.clear();
	renderer.end();
}

void App::initSDL()
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
}

void App::deinitSDL()
{
	SDL_DestroyWindow(window);

	SDL_Quit();
}