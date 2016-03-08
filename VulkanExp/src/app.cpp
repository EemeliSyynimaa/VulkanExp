#include <iostream>
#include <vector>

#include "app.h"

App::App() : 
	name("Vulkan example"), 
	width(1280), 
	height(720),
	running(true),
	window(nullptr)
{

}

App::~App()
{

}

void App::init()
{
	initSDL();
	initVulkan();
}

void App::run()
{
	while (running)
	{
		handleEvents();
		update();
	}
}

void App::deinit()
{
	deinitVulkan();
	deinitSDL();
}

void App::update()
{
	// Begin command buffer.
	VkCommandBufferBeginInfo comBuffBegInfo;
	comBuffBegInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	comBuffBegInfo.pNext = NULL;
	comBuffBegInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	comBuffBegInfo.pInheritanceInfo = NULL;

	result = vkBeginCommandBuffer(commandBuffer, &comBuffBegInfo);

	checkResult(result, "COMMAND BUFFER BEGIN");

	// End command buffer.
	result = vkEndCommandBuffer(commandBuffer);

	checkResult(result, "COMMAND BUFFER END");

	// Submit command buffer to queue.
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

	checkResult(result, "QUEUE SUBMIT");
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

void App::initSDL()
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
}

void App::deinitSDL()
{
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void App::initVulkan()
{
	uint32_t propertyCount = 128;
	VkLayerProperties properties[128];

	result = vkEnumerateInstanceLayerProperties(&propertyCount, properties);

	checkResult(result, "INSTANCE LAYER ENUMERATION");

	for (uint32_t i = 0; i < propertyCount; i++)
	{
		std::cout << properties[i].layerName << ": " << properties[i].description << std::endl;
	}

	// TODO enumerate layers for instance and debug.
	// Info structures for application and instance.
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = name.c_str();
	appInfo.applicationVersion = 1;
	appInfo.engineVersion = 1;
	appInfo.pEngineName = name.c_str();
	appInfo.apiVersion = VK_API_VERSION;

	VkInstanceCreateInfo instInfo;
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = NULL;
	instInfo.flags = 0;
	instInfo.pApplicationInfo = &appInfo;
	instInfo.enabledExtensionCount = 0;
	instInfo.ppEnabledExtensionNames = NULL;
	instInfo.enabledLayerCount = 0;
	instInfo.ppEnabledLayerNames = NULL;

	// Create instance.
	result = vkCreateInstance(&instInfo, NULL, &instance);

	checkResult(result, "INSTANCE");

	// Enumerate physical devices.
	uint32_t deviceCount = 0;
	uint32_t queueCount = 0;
	float* queuePriorities = nullptr;
	std::vector<VkPhysicalDevice> physicalDevices;

	vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

	std::cout << "Found " << deviceCount << " physical devices" << std::endl;

	if (deviceCount > 0)
	{
		physicalDevices.resize(deviceCount);

		vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

		for (auto device : physicalDevices)
		{
			// Read device properties.
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

			std::cout << "Device: " << physicalDeviceProperties.deviceName;

			std::string type;

			switch (physicalDeviceProperties.deviceType)
			{
			case VK_PHYSICAL_DEVICE_TYPE_CPU: type = "CPU"; break;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: type = "INTEGRATED GPU"; break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: type = "DISCRETE GPU"; break;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: type = "VIRTUAL GPU"; break;
			case VK_PHYSICAL_DEVICE_TYPE_OTHER: type = "OTHER"; break;
			default: type = "INVALID??"; break;
			}

			std::cout << ",  " << type << std::endl;

			// Read device queue properties.
			std::vector<VkQueueFamilyProperties> queueFamilyProperties;
			uint32_t queueFamilyPropertyCount = 0;

			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, NULL);

			if (queueFamilyPropertyCount > 0)
			{
				queueFamilyProperties.resize(queueFamilyPropertyCount);

				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, queueFamilyProperties.data());

				for (auto queue : queueFamilyProperties)
				{
					queueCount = queue.queueCount;

					std::cout << "This many queues in family: " << queueCount << std::endl;

					queuePriorities = new float[queueCount];

					for (size_t i = 0; i < queueCount; i++)
					{
						queuePriorities[i] = 1.0f;
					}

					std::cout << "This family has bits: ";

					// Is this correct? :D
					if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) std::cout << " GRAPHICS";
					if (queue.queueFlags & VK_QUEUE_COMPUTE_BIT) std::cout << "  COMPUTE";
					if (queue.queueFlags & VK_QUEUE_TRANSFER_BIT) std::cout << "  TRANSFER";
					if (queue.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) std::cout << "  SPARSE BINDING" << std::endl;
				}
			}
		}
	}

	// Create logical device.
	// We just take the first physical device in the list.
	// Maybe we should take the one with the necessary bits? Graphics etc.
	VkDeviceQueueCreateInfo queInfo;
	queInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queInfo.pNext = NULL;
	queInfo.flags = 0;
	queInfo.queueFamilyIndex = 0;
	queInfo.queueCount = queueCount;
	queInfo.pQueuePriorities = queuePriorities;

	VkPhysicalDeviceFeatures featInfo;

	VkDeviceCreateInfo devInfo;
	devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devInfo.pNext = NULL;
	devInfo.flags = 0;
	devInfo.queueCreateInfoCount = 1;
	devInfo.pQueueCreateInfos = &queInfo;
	devInfo.enabledExtensionCount = 0;
	devInfo.ppEnabledExtensionNames = NULL;
	devInfo.enabledLayerCount = 0;
	devInfo.ppEnabledLayerNames = NULL;
	devInfo.pEnabledFeatures = &featInfo;

	result = vkCreateDevice(physicalDevices[0], &devInfo, NULL, &device);

	checkResult(result, "DEVICE");

	// Get queue 0 from family 0.
	vkGetDeviceQueue(device, 0, 0, &queue);

	// Create command pool.
	VkCommandPoolCreateInfo comPoolInfo;
	comPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	comPoolInfo.pNext = NULL;
	comPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	comPoolInfo.queueFamilyIndex = 0;

	result = vkCreateCommandPool(device, &comPoolInfo, NULL, &commandPool);

	checkResult(result, "COMMAND POOL");

	//// Allocate command buffer.
	VkCommandBufferAllocateInfo comBuffAllocInfo;
	comBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	comBuffAllocInfo.pNext = NULL;
	comBuffAllocInfo.commandPool = commandPool;
	comBuffAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	comBuffAllocInfo.commandBufferCount = 1;

	result = vkAllocateCommandBuffers(device, &comBuffAllocInfo, &commandBuffer);

	checkResult(result, "COMMAND BUFFER ALLOC");
}

void App::deinitVulkan()
{
	// Destroy everything.
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	vkDestroyCommandPool(device, commandPool, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
}

void App::checkResult(const VkResult& result, const std::string& action)
{
	std::string temp;

	switch (result)
	{
	case VK_SUCCESS:
		temp = "SUCCESS"; break;
	case VK_NOT_READY:
		temp = "NOT READY"; break;
	case VK_TIMEOUT:
		temp = "TIMEOUT"; break;
	case VK_EVENT_SET:
		temp = "EVENT SET"; break;
	case VK_EVENT_RESET:
		temp = "EVENT RESET"; break;
	case VK_INCOMPLETE:
		temp = "INCOMPLETE"; break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		temp = "ERROR OUT OF HOST MEMORY"; break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		temp = "ERROR OUT OF DEVICE MEMORY"; break;
	case VK_ERROR_INITIALIZATION_FAILED:
		temp = "ERROR INITIALIZATION FAILED"; break;
	case VK_ERROR_DEVICE_LOST:
		temp = "ERROR DEVICE LOST"; break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		temp = "ERROR MEMORY MAP FAILED"; break;
	case VK_ERROR_LAYER_NOT_PRESENT:
		temp = "ERROR LAYER NOT PRESENT"; break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		temp = "ERROR EXTENSION NOT PRESENT"; break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		temp = "ERROR INCOMPATIBLE DRIVER"; break;
	case VK_ERROR_TOO_MANY_OBJECTS:
		temp = "ERROR TOO MANY OBJECTS"; break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		temp = "ERROR FORMAT NOT SUPPORTED"; break;
	default:
		temp = "SURPRISE ERROR"; break;
	}

	std::cout << "Result of " << action << ": " << temp << std::endl;
}