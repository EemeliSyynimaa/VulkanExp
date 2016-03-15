#pragma once

#include <string>

#include "SDL2/SDL.h"
#include "renderer.h"

class App
{
public:
	App();
	~App();

	void init();
	void run();
	void deinit();
private:
	void handleEvents();
	void update();
	void render();

	void initSDL();
	void deinitSDL();

	Renderer renderer;

	const std::string title;
	const int width;
	const int height;
	bool running;

	SDL_Window* window;
};