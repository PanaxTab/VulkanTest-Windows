#include "lve_model.hpp"

//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <cassert>
#include <memory.h>
namespace lve {
	LveModel::LveModel(LveDevice &device, const LveModel::builder &builder) :lveDevice{device} {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}
	LveModel::~LveModel(){
		vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
		
		if (hasIndexBuffer) {
			vkDestroyBuffer(lveDevice.device(), indexBuffer, nullptr);
			vkFreeMemory(lveDevice.device(), indexBufferMemory, nullptr);
		}
	}

	void LveModel::createVertexBuffers(const std::vector<vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);
		void* data;
		vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), stagingBufferMemory);
		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory);

		lveDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
		vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
}



	void LveModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer) {
			return;
		}
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);
		void* data;
		vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), stagingBufferMemory);
		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory);
		lveDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);
		vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
	}

	void LveModel::draw(VkCommandBuffer commandbuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandbuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandbuffer, vertexCount, 1, 0, 0);
		}
	}
	void LveModel::bind(VkCommandBuffer commandbuffer) {
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandbuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandbuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	std::vector<VkVertexInputBindingDescription> LveModel::vertex::getBindingDescriptions() {
		return { {0,sizeof(vertex),VK_VERTEX_INPUT_RATE_VERTEX} };
	}

	std::vector<VkVertexInputAttributeDescription> LveModel::vertex::getAttributeDescriptions() {
		//std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		//attributeDescriptions[0].binding=0;
		//attributeDescriptions[0].location=0;
		//attributeDescriptions[0].format=VK_FORMAT_R32G32_SFLOAT;
		//attributeDescriptions[0].offset = 0;
		//
		//attributeDescriptions[1].binding = 0;
		//attributeDescriptions[1].location = 1;
		//attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		//attributeDescriptions[1].offset = offsetof(vertex, color);
		//
		//return attributeDescriptions;

		return
		{
			{0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(vertex,position)},
			{1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(vertex,color)}
		};
	};
}