#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Render/Common/include/math.hpp"

#ifndef CAMERA
#define CAMERA
namespace toy2d
{ 
class Camera
{
public:
	Camera();
	~Camera() = default;
	MVP get_MVP();

private:
	MVP mvp = {
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f)
	};

	//相机参数(视图矩阵相关)
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);//相机坐标
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);//相机目标
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);//向上方向
	glm::vec3 cameraFront = this->cameraTarget - this->cameraPos;//相机方向
	float deltaTime = 0.0f; // 当前帧与上一帧的时间差
	float lastFrame = 0.0f; // 上一帧的时间

	//相机参数(投影矩阵相关)
	float fov = 45.0f; // 视野角度
	float aspect = (float)WINDOWS_WIDTH / (float)WINDOWS_HIGHT; // 宽高比
	float nearPlane = 0.1f; // 近裁剪面
	float farPlane = 100.0f; // 远裁剪面

	bool firstMouse = true;
	float yaw = -90.0f;	// 俯仰角
	float pitch = 0.0f;//偏航角
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;

	void viewLookAt();//计算视图矩阵
	void projectionPerspective();//计算投影矩阵

};
}
#endif // !CAMERA