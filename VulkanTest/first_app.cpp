#include "lve_simple_render_system.hpp"
#include "first_app.hpp"
#include "lve_camera.hpp"
#include "lve_renderer.hpp"
#include "lve_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

namespace lve {
	struct GlobalUbo
	{
		glm::mat4 projectionView{1.f};
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f,-3.f,-1.f });
	};

	struct SimplePushData {
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	FirstApp::FirstApp() {
		globalPool =
			LveDescriptorPool::Builder(lveDevice)
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
		//createPipelineLayout();
		//createPipeline();
	}

	FirstApp::~FirstApp() {}//vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);

	void FirstApp::run() {
		std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<LveBuffer>(
				lveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}
		//LveSimpleRenderSystem simpleRenderSystem{ lveDevice,lveRenderer.getSwapChainRenderPass() };
		auto globalSetLayout =
			LveDescriptorSetLayout::Builder(lveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();
		std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		LveSimpleRenderSystem simpleRenderSystem{
			lveDevice,
			lveRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout()
		};

		LveCamera camera{};
		//camera.setViewDirection(glm::vec3 (0.f), glm::vec3(0.5f,0.f,1.f));// Cube appears on the left cause we look to the right
		//camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f)); // Cube appears centered

		auto viewerObject = LveGameObject::createGameObject();
		KeyboardMovementController cameraController{};
		
		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
			
			float aspect = lveRenderer.getAspectRatio();
			//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.f), aspect,.1f,10.f); // Controls how far we can see with near and far
			//drawFrame();
			if (auto commandBuffer = lveRenderer.beginFrame()) {
				int frameIndex = lveRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};
				//update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjectionMatrix() * camera.getViewMatrix();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass
				//render
				lveRenderer.begiSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(lveDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj"); //createCubeModel(lveDevice, { .0f,.0f,.0f });

		auto gameObj = LveGameObject::createGameObject();
		gameObj.model = lveModel;
		gameObj.transform.translation = { .0f,.0f,2.5f };
		gameObj.transform.scale = glm::vec3(3.f);
		gameObjects.push_back(std::move(gameObj));
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

			SimplePushData push{};
			push.offset = obj.transform.translation;
			push.color = obj.color;
			push.transform = obj.transform.mat4();

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
}