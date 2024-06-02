#include "Control/include/camera.hpp"
namespace toy2d
{
	Camera::Camera()
	{
		this->viewLookAt();
	}

	void Camera::viewLookAt()
	{
		this->mvp.view = glm::lookAt(this->cameraPos, this->cameraTarget, this->cameraUp);
	}

	void Camera::projectionPerspective()
	{
		this->mvp.proj = glm::perspective(glm::radians(this->fov), this->aspect, this->nearPlane, this->farPlane);
	}

	MVP Camera::get_MVP()
	{
		this->viewLookAt();
		this->projectionPerspective();
		return this->mvp;
	}
	void Camera::set_M(const glm::mat4& model)
	{
		this->mvp.model = model;
	}

	void Camera::set_M(glm::mat4&& model)
	{
		this->mvp.model = model;
	}
}