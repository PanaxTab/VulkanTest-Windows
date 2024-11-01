#include "lve_simple_render_system.hpp"
//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
//std
#include <stdexcept>
#include <array>
#include <cassert>
#include <iostream>

namespace lve {
	struct SimplePushData {
		glm::mat4 transform{ 1.f };
		alignas(16) glm::vec3 color;
	};

	LveSimpleRenderSystem::LveSimpleRenderSystem(LveDevice& device, VkRenderPass renderPass) : lveDevice{ device }{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	LveSimpleRenderSystem::~LveSimpleRenderSystem() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}


	void LveSimpleRenderSystem::createPipelineLayout() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void LveSimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}
	void LveSimpleRenderSystem::renderGameObjects(
		VkCommandBuffer commandBuffer, 
		std::vector<LveGameObject>& gameObjects, 
		const LveCamera& camera)
	{
		lvePipeline->bind(commandBuffer);

		auto projectionView = camera.getProjectionMatrix() * camera.getViewMatrix();

		for (auto& obj : gameObjects) {
			obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
			obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());

			SimplePushData push{};
			push.color = obj.color;
			push.transform = projectionView * obj.transform.mat4();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
				sizeof(SimplePushData),
				&push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	};
}