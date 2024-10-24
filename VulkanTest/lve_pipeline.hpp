#pragma once
#include <string>
#include <vector>
#include "lve_device.hpp"

namespace lve {
	struct PipelineConfigInfo {};

	class LvePipeline {
	public:
		LvePipeline(
			LveDevice& device, 
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo
		);
		~LvePipeline() {};
		LvePipeline(const LvePipeline&) = delete;
		void operator = (const LvePipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		// Could be unsafe if device is released before pipeline, we could dereference dangling pointer causing a crash
		// Only use when there's a implicit variable where a member variable will outlive any instance of a class that depend on it
		// In UML this is referred to as aggregation
		LveDevice& lveDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
	
}