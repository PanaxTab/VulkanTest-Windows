#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "first_app.hpp"
#include <stdexcept>
#include <array>
#include <iostream>
#include "lve_renderer.hpp"

namespace lve {
	struct SimplePushData {
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	FirstApp::FirstApp() {
		loadGameObjects();
		createPipelineLayout();
		createPipeline();
		//recreateSwapChain();
		//createCommandBuffers();
	}

	FirstApp::~FirstApp() {
		//vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run() {
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();
			//drawFrame();
			if (auto commandBuffer = lveRenderer.beginFrame()) {
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass
				lveRenderer.begiSwapChainRenderPass(commandBuffer);
				renderGameObjects(commandBuffer);
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

	//void FirstApp::recreateSwapChain() {
	//	auto extent = lveWindow.getExtent();
	//	while (extent.width == 0 || extent.height == 0) {
	//		extent = lveWindow.getExtent();
	//		glfwWaitEvents();
	//	}
	//
	//	vkDeviceWaitIdle(lveDevice.device());
	//	lveSwapChain = nullptr;
	//	if (lveSwapChain == nullptr) {
	//		lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
	//	} else {
	//		lveSwapChain = std::make_unique<LveSwapChain>(lveDevice,extent, std::move(lveSwapChain));
	//		if (lveSwapChain->imageCount() != commandBuffers.size()) {
	//			freeCommandBuffers();
	//			createCommandBuffers();
	//		}
	//	}
	//
	//	
	//	createPipeline();
	//}

	void FirstApp::createPipeline() {
		//assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = lveRenderer.getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}


	//void FirstApp::createCommandBuffers() {
	//	commandBuffers.resize(lveSwapChain->imageCount());
	//	VkCommandBufferAllocateInfo allocInfo{};
	//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	//	allocInfo.commandPool = lveDevice.getCommandPool();
	//	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
	//
	//	if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS
	//		) {
	//		throw std::runtime_error("failed to allocate command buffers!");
	//	}
	//};

	//void FirstApp::freeCommandBuffers() {
	//	vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	//	commandBuffers.clear();
	//}

	//void FirstApp::recordCommandBuffer(int imageIndex) {
	//	//static int frame = 0;
	//	//frame = (frame + 1)%1000;
	//
	//	VkCommandBufferBeginInfo beginInfo{};
	//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//
	//	if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
	//		throw std::runtime_error("command buffer failed to begin recording!");
	//	}
	//
	//	VkRenderPassBeginInfo renderPassInfo{};
	//	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	//	renderPassInfo.renderPass = lveSwapChain->getRenderPass();
	//	renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);
	//
	//	renderPassInfo.renderArea.offset = { 0,0 };
	//	renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();
	//
	//	std::array<VkClearValue, 2> clearValues{};
	//	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	//	clearValues[1].depthStencil = { 1.0f, 0 };
	//	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	//	renderPassInfo.pClearValues = clearValues.data();
	//
	//	vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	//	
	//	VkViewport viewport{};
	//	viewport.x = 0.0f;
	//	viewport.y = 0.0f;
	//	viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
	//	viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
	//	viewport.minDepth = 0.0f;
	//	viewport.maxDepth = 1.0f;
	//	VkRect2D scissor{ {0,0}, lveSwapChain->getSwapChainExtent() };
	//	vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
	//	vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);
	//	
	//	renderGameObjects(commandBuffers[imageIndex]);
	//	//lvePipeline->bind(commandBuffers[imageIndex]);
	//	//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0); //Draw 3 vertices and only 1 instance
	//	//lveModel->bind(commandBuffers[imageIndex]);
	//	//for (int j = 0; j < 4; j++) {
	//	//	SimplePushData push{};
	//	//	push.offset = { -0.5f+frame*0.0002f,-0.4f + j * 0.25f };
	//	//	push.color = { 0.0f,0.0f,0.2f + 0.2f * j };
	//	//
	//	//	vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushData), &push);
	//	//	lveModel->draw(commandBuffers[imageIndex]);
	//	//}
	//	vkCmdEndRenderPass(commandBuffers[imageIndex]);
	//	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
	//		throw std::runtime_error("failed to record command buffer!");
	//	};
	//}

	void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
		//update
		//int i = 0;
		//for (auto& obj : gameObjects) {
		//	i += 1;
		//	obj.Transform2D.rotation = glm::mod<float>(obj.Transform2D.rotation + 0.001f * i, 2.f * glm::pi<float>());
		//}
		
		lvePipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			//obj.Transform2D.rotation = glm::mod(obj.Transform2D.rotation + 0.01f, glm::two_pi<float>());

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

	//void FirstApp::drawFrame() {
	//	uint32_t imageIndex;
	//	auto result = lveSwapChain->acquireNextImage(&imageIndex);
	//
	//	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
	//		recreateSwapChain();
	//		return;
	//	}
	//	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
	//		throw std::runtime_error("failed to aquire swap chain image!");
	//	}
	//	recordCommandBuffer(imageIndex);
	//	result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
	//	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()) {
	//		lveWindow.resetWindowResizedFlag();
	//		recreateSwapChain();
	//		return;
	//	}
	//	if (result != VK_SUCCESS) {
	//		throw std::runtime_error("failed to present swap chaain image!");
	//	}
	//};

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