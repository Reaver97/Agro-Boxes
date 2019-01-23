#include "Camera.h"



Camera::Camera(float x, float y, float z, float rotation,float rotation_y)
{
	//Initialize the values of the camera
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation = rotation;
	m_camera_rotation_y = rotation_y;

	// calculate the direction of the camera on each axis
	m_dx = std::sin(m_camera_rotation * (XM_PI / radian_conversion));
	m_dz = std::cos(m_camera_rotation * (XM_PI / radian_conversion));
	m_dy = std::tan(m_camera_rotation_y * (XM_PI / radian_conversion));
}

void Camera::Rotate(float rotate_by)
{
	m_camera_rotation += rotate_by;
	//Direction of the camera
	m_dx = std::sin(m_camera_rotation * (XM_PI / radian_conversion));
	m_dz = std::cos(m_camera_rotation * (XM_PI / radian_conversion));
}
void Camera::RotateY(float rotate_by)
{

		m_camera_rotation_y += rotate_by;
		//Lerp the camera rotation on Y axis
		if (m_camera_rotation_y >= camera_max_lerp)
		{
			m_camera_rotation_y -= rotate_by;
		}
		if (m_camera_rotation_y <= camera_min_lerp)
		{
			m_camera_rotation_y -= rotate_by;
		}
		//Direction of the camera on Y axis (only done for spectate navigation)
		m_dy = tan(m_camera_rotation_y * (XM_PI / radian_conversion));

}
void Camera::Forward(float move_by)
{
	//move towards where camera is pointing
	m_x += move_by * m_dx;
	m_z += move_by * m_dz;
	//use only if moving towards y axis as well
	//m_y += move_by * m_dy;
}
void Camera::Up(float move_by)
{
	//move up
	m_y += move_by;
}
void Camera::Strafe(float strafe_by)
{
	XMVECTOR direction;

	//dot product for strafing
	direction.x = m_y * strafe_z - m_dz * strafe_y;
	direction.z = m_dx * strafe_y - m_y * strafe_x;

	//add the calculated direction multiplied by the offset
	m_x += direction.x *strafe_by;
	m_z += direction.z * strafe_by;
}
XMMATRIX Camera::GetViewMatrix()
{
	//Set the position,look at, and up of the camera with previously calculated values
	m_position = XMVectorSet(m_x , m_y , m_z, w);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, w);
	m_up = XMVectorSet(m_up_x, m_up_y, m_up_z, w);

	//Set the camera matrix to look at the above vectors
	XMMATRIX camera = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return camera;
}
Camera::~Camera()
{

}
