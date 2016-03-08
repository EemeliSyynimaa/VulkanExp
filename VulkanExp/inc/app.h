#pragma once

#include <string>
#include "vulkan/vulkan.h"
#include "SDL2/SDL.h"

class App
{
public:
	App();
	~App();

	void init();
	void run();
	void deinit();
private:
	void update();

	void handleEvents();

	void initSDL();
	void deinitSDL();

	void initVulkan();
	void deinitVulkan();

	void checkResult(const VkResult& result, const std::string& action);

	VkInstance instance;
	VkResult result;
	VkDevice device;
	VkQueue queue;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	const std::string name;
	const int width;
	const int height;
	bool running;

	SDL_Window* window;
};