#include "lve_renderer.hpp"
#include "lve_simple_render_system.hpp"
#include "first_app.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <array>
#include <iostream>

namespace lve {
	struct SimplePushData {
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	FirstApp::FirstApp() {
		loadGameObjects();
		//createPipelineLayout();
		//createPipeline();

	}

	FirstApp::~FirstApp() {}//vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);

	void FirstApp::run() {
		LveSimpleRenderSystem simpleRenderSystem{ lveDevice,lveRenderer.getSwapChainRenderPass() };
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();
			//drawFrame();
			if (auto commandBuffer = lveRenderer.beginFrame()) {
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass
				lveRenderer.begiSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(lveDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::vector<LveModel::vertex>vertices
		{
			{{0.0f,-0.5f},{0.0f,1.0f,0.0f}}, //Top
			{{0.5f,0.5f},{0.0f,0.0f,1.0f}},	//Right
			{{-0.5f,0.5f},{1.0f,0.0f,0.0f}} //Left
		};
		std::vector<glm::vec3> colors
		{
			{1.f, .7f, .73f},
			{1.f, .87f, .73f},
			{1.f, 1.f, .73f},
			{.73f, 1.f, .8f},
			{.73, .88f, 1.f}  //
		};
		for (auto& color : colors) {
			color = glm::pow(color, glm::vec3{ 2.2f });
		}
		//sierpinski(vertices, 5, vertices[0].position, vertices[1].position, vertices[2].position);
		auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
		for (int i = 0; i < 20; i++) 
		{
			auto triangle = LveGameObject::createGameObject();
			triangle.model = lveModel;
			triangle.Transform2D.scale = glm::vec2(.5f) + i * 0.050f;
			triangle.Transform2D.rotation = i /glm::pi<float>() * .025f;
			triangle.color = colors[i % colors.size()];
			gameObjects.push_back(std::move(triangle));
		}
	}

	void FirstApp::createPipelineLayout() {
		VkPushConstantRange pushContstantRange{};
		pushContstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushContstantRange.offset = 0;
		pushContstantRange.size = sizeof(SimplePushData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushContstantRange;
		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void FirstApp::createPipeline() {
		//assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = lveRenderer.getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {		
		lvePipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			obj.Transform2D.rotation = glm::mod(obj.Transform2D.rotation + 0.01f, glm::two_over_pi<float>());
			SimplePushData push{};
			push.offset = obj.Transform2D.translation;
			push.color = obj.color;
			push.transform = obj.Transform2D.mat2();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
				sizeof(SimplePushData),
				&push);
			obj.model->bind(commandBuffer); 
			obj.model->draw(commandBuffer);
		}
	}

	void FirstApp::sierpinski(std::vector<LveModel::vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top) {
		if (depth <= 0) {
			vertices.push_back({ top });
			vertices.push_back({ right });
			vertices.push_back({ left });
		} else {
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	};
}