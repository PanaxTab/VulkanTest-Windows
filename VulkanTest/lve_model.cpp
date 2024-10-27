#include "lve_model.hpp"
#include <cassert>

namespace lve {
	LveModel::LveModel(LveDevice &device, const std::vector<vertex> &vertices) :lveDevice{device} {
		createVertexBuffers(vertices);
	}
	LveModel::~LveModel(){
		vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
	}

	void LveModel::createVertexBuffers(const std::vector<vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		lveDevice.createBuffer
		(
			bufferSize, 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory
		);
		void* data;
		vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), vertexBufferMemory);
	}

	void LveModel::draw(VkCommandBuffer commandbuffer) {
		vkCmdDraw(commandbuffer, vertexCount, 1, 0, 0);
	}
	void LveModel::bind(VkCommandBuffer commandbuffer) {
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandbuffer, 0, 1, buffers, offsets);
	}

	std::vector<VkVertexInputBindingDescription> LveModel::vertex::getBindingDescriptions() {
		return { {0,sizeof(vertex),VK_VERTEX_INPUT_RATE_VERTEX} };
	}

	std::vector<VkVertexInputAttributeDescription> LveModel::vertex::getAttributeDescriptions() {
		return{ {0,0,VK_FORMAT_R32G32_SFLOAT,0} };
	};
}