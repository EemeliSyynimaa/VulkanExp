#include <iostream>
#include <string>
#include <vector>

#include "vulkan/vulkan.h"

int main(int argc, char**argv)
{
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = "Vulkan example";
	appInfo.applicationVersion = 1;
	appInfo.engineVersion = 1;
	appInfo.pEngineName = "Vulkan example";
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

	VkInstance instance;
	VkResult result;
	VkDevice device;
	VkQueue queue;

	result = vkCreateInstance(&instInfo, NULL, &instance);

	switch (result)
	{
	case VK_SUCCESS: 
		std::cout << "SUCCESS" << std::endl; break;
	default: 
		std::cout << "SUPER ERROR" << std::endl; break;
	}

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

					// Is this correct? :D
					if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) std::cout << "IS GRAPHICS" << std::endl;
					if (queue.queueFlags & VK_QUEUE_COMPUTE_BIT) std::cout << "IS COMPUTE" << std::endl;
					if (queue.queueFlags & VK_QUEUE_TRANSFER_BIT) std::cout << "IS TRANSFER" << std::endl;
					if (queue.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) std::cout << "IS SPARSE BINDING" << std::endl;
				}
			}
		}
	}

	// Create logical device.
	// We just take the first physical device in the list.

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

	switch (result)
	{
	case VK_SUCCESS:
		std::cout << "SUCCESS" << std::endl; break;
	default:
		std::cout << "SUPER ERROR" << std::endl; break;
	}

	vkGetDeviceQueue(device, 0, 0, &queue);

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);

	return 0;
}