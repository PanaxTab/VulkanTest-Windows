#pragma once

#include "lve_camera.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
// std
#include <memory>
#include <vector>

namespace lve {
	class LveSimpleRenderSystem
	{
	public:
		LveSimpleRenderSystem(LveDevice& device	,VkRenderPass renderPass);
		~LveSimpleRenderSystem();

		LveSimpleRenderSystem(const LveWindow&) = delete;
		LveSimpleRenderSystem& operator=(const LveWindow&) = delete;
		void renderGameObjects(VkCommandBuffer commandBuffer,std::vector<LveGameObject> &gameObjects,const LveCamera &camera);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		LveDevice& lveDevice;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
	};
}