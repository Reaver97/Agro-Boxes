#pragma once
#include "objfilemodel.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; //64 B
	XMVECTOR dir_light_vector;    //16 B
	XMVECTOR dir_light_colour;	  //16 B
	XMVECTOR ambient_light_colour;//16 B
	XMVECTOR point_light_pos;	  //16 B
	XMVECTOR point_light_colour;  //16 B
};//TOTAL: 144Bytes

class Model
{
public:

	//Constructor
	Model(ID3D11Device* device, ID3D11DeviceContext* context);

	//Load object from file
	HRESULT LoadObjModel(char* filename);


	//Sets of the object transforms
	void setXPos(float x);
	void setYPos(float y);
	void setZPos(float z);
	void setXAngle(float x);
	void setYAngle(float y);
	void setZAngle(float z);
	void setScale(float scale);

	//Gets of the object transforms
	float getXPos()		{ return m_x; }
	float getYPos()		{ return m_y; }
	float getZPos()		{ return m_z; }
	float getXAngle()   { return m_xAngle; }
	float getYAngle()	{ return m_yAngle; }
	float getZAngle()	{ return m_zAngle; }
	float getScale()	{ return m_scale; }

	//Increment the object transforms
	void moveXPos(float move_by);
	void moveYPos(float move_by);
	void moveZPos(float move_by);
	void rotateXAngle(float rotate_by);
	void rotateYAngle(float rotate_by);
	void rotateZAngle(float rotate_by);
	void scaleObject(float scale_by);


	//Look at the XZ of an object
	void LookAt_XZ(float x, float z);

	//Go towards facing direction
	void MoveForward(float distance);

	//Check collision
	bool CheckCollision(Model* obj);

	//Add light
	void AddLight();

	//Set Position and colour of lights
	void setDirectionalLightVector(float x, float y, float z, float w);
	void setDirectionalLightColour(float x, float y, float z, float w);
	void setAmbientLightColour(float x, float y, float z, float w);
	void setPointLightPos(float x, float y, float z, float w);
	void setPointLightColour(float x, float y, float z, float w);

	//Draw object on screen
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	float GetBoundingSphereRadius();
	//Add texture to object
	void AddTexture(char* filename);

	//Destructor
	~Model();

private:
	//D3D11 elements
	ID3D11Device*			  m_pD3DDevice;
	ID3D11DeviceContext*	  m_pImmediateContext;
	ID3DBlob				  *m_VS, *m_PS, *error;
	ID3D11VertexShader*		  m_pVShader;
	ID3D11PixelShader*		  m_pPShader;
	ID3D11InputLayout*		  m_pInputLayout;
	ID3D11Buffer*			  m_pConstantBuffer;
	ID3D11ShaderResourceView* m_pTexture;
	ID3D11SamplerState*		  m_pSampler;

	//pointer to object file loader
	ObjFileModel*			m_pObject;
	
	//Calculates the values for sphere collision
	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();

	//conversion to radians
	const float radian_conversion = 180.0f;
	//Camera and Object distance
	float			m_dz, m_dx;
	//Object position
	float			m_x, m_y, m_z;
	//Object rotation
	float			m_xAngle, m_yAngle, m_zAngle;
	//Object scale
	float			m_scale;

	//Sphere centre coords
	float m_bounding_sphere_centre_x, 
		  m_bounding_sphere_centre_y, 
		  m_bounding_sphere_centre_z, 
		  m_bounding_sphere_radius;
	
	//Lighting colour and position
	const float ambient_light_colour = 0.1f;
	const float pointlight_r = 1.0f;
	const float pointlight_g = 1.0f;
	const float pointlight_b = 1.0f;
	const float light_alpha = 1.0f;
	const float pointlight_x = -20.0f;
	const float pointlight_y = 10.0f;
	const float pointlight_z = 20.0f;
	const float pointlight_w = 0.0f;

	//Light vectors (color,position and direction)
	XMVECTOR m_dir_light_shines_from;
	XMVECTOR m_dir_light_colour;
	XMVECTOR m_ambient_light_colour;
	XMVECTOR m_point_light_pos;
	XMVECTOR m_point_light_colour;

};

