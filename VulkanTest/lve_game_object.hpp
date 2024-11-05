#pragma once

#include <memory>
#include "lve_model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace lve {
	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f,1.f,1.f };
		glm::vec3 rotation{};
		//Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
		// Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
		//slow mat4:
		//glm::mat4 mat4() {
		//	auto transform = glm::translate(glm::mat4{ 1.1f }, translation);
		//	transform = glm::rotate(transform, rotation.y, { 0.f,1.f,0.f });
		//	transform = glm::rotate(transform, rotation.x, { 1.f,0.f,0.f });
		//	transform = glm::rotate(transform, rotation.z, { 0.f,0.f,1.f });
		//	transform = glm::scale(transform, scale);
		//	return transform;
		//}
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};


	class LveGameObject {
	public:
		using id_t = unsigned int;

		static LveGameObject createGameObject() {
			static id_t currentId = 0;
			return LveGameObject(currentId++);
		};
		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;
		id_t getId() { return id; }

		std::shared_ptr<LveModel> model{};
		glm::vec3 color;

		TransformComponent transform;
	private:
		LveGameObject(id_t objId): id{ objId } {}

		id_t id;
	};
}