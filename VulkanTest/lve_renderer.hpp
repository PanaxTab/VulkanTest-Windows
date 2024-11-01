#pragma once

#include "lve_device.hpp"
#include "lve_window.hpp"
#include "lve_swap_chain.hpp"
#include "lve_game_object.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace lve {
	class LveRenderer
	{
	public:
		LveRenderer(LveWindow& window, LveDevice& device);
		~LveRenderer();

		LveRenderer(const LveRenderer&) = delete;
		LveRenderer& operator = (const LveRenderer&) = delete;

		
		VkRenderPass getSwapChainRenderPass()const { return lveSwapChain->getRenderPass(); }
		float getAspectRatio() const { return lveSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; };
		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentframeIndex];
		}
		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentframeIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void begiSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		uint32_t currentImageIndex;
		int currentframeIndex;
		bool isFrameStarted;
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		LveWindow& lveWindow;
		LveDevice& lveDevice;
		std::unique_ptr<LveSwapChain> lveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<LveGameObject> gameObjects;
	};
}
