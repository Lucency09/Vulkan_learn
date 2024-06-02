#pragma once
#include <GLFW/glfw3.h>

#include "Control/include/camera.hpp"

#ifndef INPUT_HANDLER
#define INPUT_HANDLER

namespace toy2d
{
	class InputHandler
	{
	public:
		InputHandler(GLFWwindow* window, Camera& camera) : window_(window), camera_(camera) {}
		virtual ~InputHandler();

		virtual void Update();

	private:
		GLFWwindow* window_;
		Camera& camera_;
	};
}

#endif // INPUT_HANDLER