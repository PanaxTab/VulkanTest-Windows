#pragma once

#include "lve_camera.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_frame_info.hpp"
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
		void renderGameObjects(FrameInfo &frameInfo,std::vector<LveGameObject> &gameObjects);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		LveDevice& lveDevice;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
	};
}