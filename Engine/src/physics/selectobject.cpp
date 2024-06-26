//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "imgui.h"

//engine
#include "selectobject.hpp"
#include "render.hpp"
#include "collision.hpp"

using glm::inverse;
using glm::normalize;
using glm::vec4;
using std::numeric_limits;

using Graphics::Render;
using Physics::Collision;
using Type = Graphics::Shape::Mesh::MeshType;

namespace Physics
{
	Select::Ray Select::RayFromMouse(double mouseX, double mouseY, const mat4& viewMatrix, const mat4& projectionMatrix)
	{
		int width, height;
		glfwGetWindowSize(Render::window, &width, &height);
		float x = (2.0f * static_cast<float>(mouseX)) / width - 1.0f;
		float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / height;

		vec4 rayClip = vec4(x, y, -1.0, 1.0);
		vec4 rayEye = inverse(projectionMatrix) * rayClip;
		rayEye = vec4(rayEye.x, rayEye.y, -1.0, 0.0);

		vec4 rayWorld = inverse(viewMatrix) * rayEye;
		vec3 rayDirection = normalize(vec3(rayWorld));

		return Ray(Render::camera.GetCameraPosition(), rayDirection);
	}

	int Select::CheckRayObjectIntersections(const Ray& ray, const vector<shared_ptr<GameObject>>& objects)
	{
		//if user pressed left mouse button over any imgui window
		if (ImGui::GetIO().WantCaptureMouse)
		{
			return -2;
		}
		else
		{
			for (int i = 0; i < objects.size(); i++)
			{
				Type objType = objects[i]->GetMesh()->GetMeshType();
				if ((objType == Type::model
					|| objType == Type::point_light
					|| objType == Type::spot_light)
					&& Collision::IsRayIntersectingCube(ray, objects[i]))
				{
					return i;
				}
			}

			//if user did not press any valid gameobject
			return -1;
		}
	}
}