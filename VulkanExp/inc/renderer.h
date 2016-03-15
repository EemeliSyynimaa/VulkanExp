#pragma once

#include <string>
#include "vulkan/vulkan.h"

struct SDL_Window;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void init(SDL_Window* window, const std::string& title);
	void deinit();

	void begin();
	void end();

	void clear();

private:
	void flush();

	void checkResult(const VkResult& result, const std::string& action);

	void createInstance(const std::string& title, const char* const* layerNames, uint32_t layerCount);
	void createDevice(const char* const* layerNames, uint32_t layerCount);

	VkInstance instance;
	VkResult result;
	VkDevice device;
	VkQueue queue;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	bool initialized;
};