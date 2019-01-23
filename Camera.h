#pragma once
#define _XM_NO_INTRINSICS_
#define _XM_NO_ALIGNMENT

#include <d3d11.h>
#include <xnamath.h>
#include <math.h>
#include <cmath>
using namespace std;
class Camera
{
public:
	//Camera constructor, creates camera at position and rotation
	Camera(float x,float y, float z, float rotation,float rotation_y);

	//Rotate camera left and right
	void Rotate(float rotate_by);

	//Rotate camera up and down
	void RotateY(float rotate_by);

	//Move forward towards camera direction
	void Forward(float move_by);

	//Go up or down
	void Up(float move_by);

	//Strafe left and right based on camera position
	void Strafe(float strafe_by);

	//Get the XYZ pos methods
	float GetX() { return m_x; }
	float GetY() { return m_y; }
	float GetZ() { return m_z; }

	//Set the XYZ pos methods;
	void SetX(float offset) { m_x = offset; }
	void SetY(float offset) { m_y = offset; }
	void SetZ(float offset) { m_z = offset; }

	//Get the view matrix
	XMMATRIX GetViewMatrix();

	//Default destructor
	~Camera();

private:
	//Prevent the camera from doing a 360 on the Y axis
	const float camera_min_lerp = -90.0f;
	const float camera_max_lerp = 90.0f;

	//Float used for radian conversion
	const float radian_conversion = 180.0f;

	//Values for the strafe direction calculation
	const float strafe_z = 0.0f;
	const float strafe_y = 1.0f;
	const float strafe_x = 0.0f;

	// default w value
	const float w = 0.0f;

	//default Up values
	const float m_up_y = 1.0f;
	const float m_up_z = 0.0f;
	const float m_up_x = 0.0f;

	//Camera rotation on the X axis
	float m_camera_rotation;

	//XYZ pos and XYZ directions
	float m_x, m_y, m_z, m_dx, m_dz, m_dy;

	//Camera rotation on the Y axis
	float m_camera_rotation_y;

	//Vectors containing the positions, the look at of the camera and the up
	XMVECTOR m_position, m_lookat, m_up;
};

