#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>
namespace lve {
	class LveCamera
	{
	public:
		//LveCamera(LveCamera& lveCamera);
		//~LveCamera();
		//
		//LveCamera(const LveCamera&) = delete;
		//LveCamera& operator=(const LveCamera&) = delete;
		
		void setOrthographicProjection(float left,float right,float top, float bottom, float near, float far);
		void setPerspectiveProjection(float fovy, float aspect, float near, float far);
		const glm::mat4& getProjectionMatrix() const { return projectionMatrix; };
		const glm::mat4& getViewMatrix() const { return viewMatrix; };
		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f,-1.f,0.f });
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f,-1.f,0.f });
		void setViewYXZ(glm::vec3 position, glm::vec3 rotation);
	private:
		glm::mat4 projectionMatrix{1.f};
		glm::mat4 viewMatrix{1.f};
		//LveCamera& lveCamera;
	};
}