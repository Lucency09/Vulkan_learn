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

	//�������(��ͼ�������)
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);//�������
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);//���Ŀ��
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);//���Ϸ���
	glm::vec3 cameraFront = this->cameraTarget - this->cameraPos;//�������
	float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
	float lastFrame = 0.0f; // ��һ֡��ʱ��

	//�������(ͶӰ�������)
	float fov = 45.0f; // ��Ұ�Ƕ�
	float aspect = (float)WINDOWS_WIDTH / (float)WINDOWS_HIGHT; // ��߱�
	float nearPlane = 0.1f; // ���ü���
	float farPlane = 100.0f; // Զ�ü���

	bool firstMouse = true;
	float yaw = -90.0f;	// ������
	float pitch = 0.0f;//ƫ����
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;

	void viewLookAt();//������ͼ����
	void projectionPerspective();//����ͶӰ����

};
}
#endif // !CAMERA