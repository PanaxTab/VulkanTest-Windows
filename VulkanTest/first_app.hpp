#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"
#include "lve_model.hpp"
#include "lve_game_object.hpp"
#include "keyboard_input_controller.hpp"
// std
#include <memory>
#include <vector>

namespace lve {
	class FirstApp
	{
	public:
		static constexpr int WIDTH	= 800;
		static constexpr int HEIGHT	= 600;

		FirstApp();
		~FirstApp();

		FirstApp(const LveWindow&) = delete;
		FirstApp& operator=(const LveWindow&) = delete;

		void run();
		
	private:
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();	
		//void createCommandBuffers();
		//void freeCommandBuffers();
		//void drawFrame();
		//void recreateSwapChain();
		//void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);
		
		//void sierpinski(std::vector<LveModel::vertex> &vertices,int depth,glm::vec2 left, glm::vec2 right,glm::vec2 top); //sierpinski exercise
		LveWindow lveWindow{WIDTH,HEIGHT,"Hello Vulkan!" };
		LveDevice lveDevice{lveWindow};
		LveRenderer lveRenderer{ lveWindow,lveDevice };
		//std::unique_ptr<LveSwapChain> lveSwapChain;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
		//std::vector<VkCommandBuffer> commandBuffers;
		std::vector<LveGameObject> gameObjects;
	};
}	