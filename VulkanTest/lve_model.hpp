#pragma once
#include "lve_device.hpp"
#include "lve_buffer.hpp"

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

				bool operator==(const vertex& other)const {
					return position == other.position && color == other.color && normal == other.normal && UV == other.UV;
				};
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
		std::unique_ptr<LveBuffer> vertexBuffer;
		uint32_t vertexCount;

		//Index components
		std::unique_ptr<LveBuffer> indexBuffer;
		uint32_t indexCount;
		bool hasIndexBuffer = false;
	};
}