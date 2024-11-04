#pragma once
#include "lve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace lve {
	class LveModel {
		public:
			struct vertex {
				glm::vec3 position{};
				glm::vec3 color{};
				glm::vec3 normal{};
				glm::vec2 UV{};

				static std::vector<VkVertexInputBindingDescription>getBindingDescriptions();
				static std::vector<VkVertexInputAttributeDescription>getAttributeDescriptions();
			};
			//Temporary helper object
			struct builder {
				std::vector<vertex> vertices{};
				std::vector<uint32_t>indices{};

				void loadModel(const std::string &filepath);
			};
		LveModel(LveDevice &device, const LveModel::builder &builder);
		~LveModel();

		LveModel(const LveModel&) = delete;
		LveModel& operator=(const LveModel&) = delete;

		static std::unique_ptr<LveModel> createModelFromFile(LveDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<vertex>&vertices);
		void createIndexBuffers(const std::vector<uint32_t>&indices);
		LveDevice& lveDevice;
		
		//Vertex components
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		//Index components
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
		bool hasIndexBuffer = false;
	};
}