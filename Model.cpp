#include "Model.h"



Model::Model(ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Assign the device and context variables
	m_pD3DDevice = device;
	m_pImmediateContext = context;

	//XYZ position
	m_x		 = 0.0f;
	m_y		 = 0.0f;
	m_z		 = 0.0f;
	//XYZ rotation
	m_xAngle = 0.0f;
	m_yAngle = 0.0f;
	m_zAngle = 0.0f;
	//scale
	m_scale  = 1.0f;
	

}


Model::~Model()
{
	//Clean up memory
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pTexture) m_pTexture->Release();
	if (m_pSampler) m_pSampler->Release();

	//Delete the object
	delete m_pObject;
}

void Model::CalculateModelCentrePoint()
{

	//take the first element of the array and make it the comparable element
	//to the others remaining in the array
	auto max_x = m_pObject->vertices[0].Pos.x;
	auto max_y = m_pObject->vertices[0].Pos.y;
	auto max_z = m_pObject->vertices[0].Pos.z;

	auto min_x = m_pObject->vertices[0].Pos.x;
	auto min_y = m_pObject->vertices[0].Pos.y;
	auto min_z = m_pObject->vertices[0].Pos.z;

	//loop through the vertices and find the max value xyz and the min value xyz of the vertices
	for (size_t i = 0; i < m_pObject->numverts; i++)
	{
		if (m_pObject->vertices[i].Pos.x > max_x)
		{
			max_x = m_pObject->vertices[i].Pos.x;
		}
		if (m_pObject->vertices[i].Pos.y > max_y)
		{
			max_y = m_pObject->vertices[i].Pos.y;
		}
		if (m_pObject->vertices[i].Pos.z > max_z)
		{
			max_z = m_pObject->vertices[i].Pos.z;
		}

		if (m_pObject->vertices[i].Pos.x < min_x)
		{
			min_x = m_pObject->vertices[i].Pos.x;
		}
		if (m_pObject->vertices[i].Pos.y < min_y)
		{
			min_y = m_pObject->vertices[i].Pos.y;
		}
		if (m_pObject->vertices[i].Pos.z < min_z)
		{
			min_z = m_pObject->vertices[i].Pos.z;
		}
	}
	//with the information above calculate the average between the two to get the center point
	m_bounding_sphere_centre_x = (min_x + max_x) / 2;
	m_bounding_sphere_centre_y = (min_y + max_y) / 2;
	m_bounding_sphere_centre_z = (min_z + max_z) / 2;
}

void Model::CalculateBoundingSphereRadius()
{
	//Calculate the distance between the max value xyz position and the centre position (pythagoras theorem)
	float x = pow((m_pObject->vertices[0].Pos.x - m_bounding_sphere_centre_x), 2);
	float y = pow((m_pObject->vertices[0].Pos.y - m_bounding_sphere_centre_y), 2);
	float z = pow((m_pObject->vertices[0].Pos.z - m_bounding_sphere_centre_z), 2);

	float max_value = sqrt(x + y + z);
	float current_x;
	float current_y;
	float current_z;
	float current_value;

	//loop through vertices to find the max value xyz and calculate the distance between the centre point and max point
	for (size_t i = 0; i < m_pObject->numverts; i++)
	{
		current_x = pow((m_pObject->vertices[i].Pos.x - m_bounding_sphere_centre_x), 2);
		current_y = pow((m_pObject->vertices[i].Pos.y - m_bounding_sphere_centre_y), 2);
		current_z = pow((m_pObject->vertices[i].Pos.z - m_bounding_sphere_centre_z), 2);
		current_value = sqrt(current_x + current_y + current_z);

		if (current_value > max_value)
		{
			max_value = current_value;
		}
	}

	//the max value is the sphere radius
	m_bounding_sphere_radius = max_value;
}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
{
	//Transform to world coordinates
	XMMATRIX world;

	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	//set the offset to the centre points
	XMVECTOR offset;
	offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0f);

	//Transform it to the world position
	offset = XMVector3Transform(offset, world);


	return offset;
}

float Model::GetBoundingSphereRadius()
{
	//Calculate the radius no matter the size of the object
	return m_bounding_sphere_radius * m_scale;
}

HRESULT Model::LoadObjModel(char * filename)
{
	HRESULT hr = S_OK;

	//Create object
	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);

	if (m_pObject->filename == "FILE NOT LOADED") return S_FALSE;

	//Compile the Model Vertex Shader
	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &m_VS, &error, 0);
	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}
	//Compile the Model Pixel Shader
	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &m_PS, &error, 0);
	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}
	//Create the Model Vertex shader
	hr = m_pD3DDevice->CreateVertexShader(m_VS->GetBufferPointer(), m_VS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr))//Don't fail if error is just a warning
	{
		return hr;
	}
	hr =m_pD3DDevice->CreatePixelShader(m_PS->GetBufferPointer(), m_PS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr))//Don't fail if error is just a warning
	{
		return hr;
	}
	//Set Model Shaders as active ones
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//NOTE the spelling of COLOR. Again, be careful setting the correct dxgi format (using A32) and correct D3D version
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	//Create input layout for buffer
	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), m_VS->GetBufferPointer(), m_VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_BUFFER_DESC const_buffer_desc;
	ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));
	//Constant buffer is uses GPU
	const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;

	//Constant Buffer Byte width
	const_buffer_desc.ByteWidth = 144;

	//Set Constant buffer bind flag
	const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	//Create constant buffer
	hr = m_pD3DDevice->CreateBuffer(&const_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}
	CalculateModelCentrePoint();
	CalculateBoundingSphereRadius();
	return S_OK;
}


void Model::setXPos(float x)
{
	m_x = x;
}

void Model::setYPos(float y)
{
	m_y = y;
}

void Model::setZPos(float z)
{
	m_z = z;
}

void Model::setXAngle(float x)
{
	m_xAngle = x;
}

void Model::setYAngle(float y)
{
	m_yAngle = y;
}

void Model::setZAngle(float z)
{
	m_zAngle = z;
}

void Model::setScale(float scale)
{
	m_scale = scale;
}

void Model::LookAt_XZ(float x, float z)
{
	m_dx = x - m_x;
	m_dz = z - m_z;
	m_yAngle = atan2(m_dx, m_dz) * (radian_conversion / XM_PI);
}

void Model::MoveForward(float offset)
{
	m_x += sin(m_yAngle * (XM_PI / radian_conversion)) * offset;
	m_z += cos(m_yAngle * (XM_PI / radian_conversion)) * offset;


}

bool Model::CheckCollision(Model * obj)
{
	//prevent collision with self
	if (obj == this)
	{
		return false;
	}
	//get the position of current model and the model that is checked for collision
	XMVECTOR current_model_position;
	XMVECTOR passed_model_position;
	current_model_position = this->GetBoundingSphereWorldSpacePosition();
	passed_model_position = obj->GetBoundingSphereWorldSpacePosition();

	float x1 = XMVectorGetX(current_model_position);
	float x2 = XMVectorGetX(passed_model_position);

	float y1 = XMVectorGetY(current_model_position);
	float y2 = XMVectorGetY(passed_model_position);

	float z1 = XMVectorGetZ(current_model_position);
	float z2 = XMVectorGetZ(passed_model_position);

	float x = pow((x2 - x1), 2);
	float y = pow((y2 - y1), 2);
	float z = pow((z2 - z1), 2);

	//Calculate the distance (pythagoras theorem)
	float distance = sqrt(x + y + z);

	//Calculate the sum of radiuses
	float sumOfRadius = obj->GetBoundingSphereRadius() + this->GetBoundingSphereRadius();

	//if the sum of radius is greater than the distance then collision has occured
	if (sumOfRadius > distance)
	{
		return true;
	}

	return false;
}

void Model::AddLight()
{
	setAmbientLightColour(ambient_light_colour, ambient_light_colour, ambient_light_colour, light_alpha);
	setPointLightColour(pointlight_r, pointlight_g, pointlight_b, light_alpha);
	setPointLightPos(pointlight_x, pointlight_y, pointlight_z, pointlight_w);
}

void Model::Draw(XMMATRIX * view, XMMATRIX * projection)
{
	AddLight();
	XMMATRIX world;
	XMMATRIX transpose;
	XMVECTOR determinant;
	XMMATRIX inverse;
	
	//Declare constant buffer structure to use the shaders
	MODEL_CONSTANT_BUFFER model_cb_values;


	//Scale, Rotate and move the object
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	//Set the world view projection matrix
	model_cb_values.WorldViewProjection = world * (*view) * (*projection);

	inverse = XMMatrixInverse(&determinant, world);
	transpose = XMMatrixTranspose(world);

	//Pass the values to the constant buffer
	model_cb_values.dir_light_colour = m_dir_light_colour;
	model_cb_values.ambient_light_colour = m_ambient_light_colour;
	model_cb_values.point_light_colour = m_point_light_colour;
	model_cb_values.dir_light_vector = XMVector3Transform(m_dir_light_shines_from, transpose);
	model_cb_values.point_light_pos = XMVector3Transform(m_point_light_pos, inverse);
	model_cb_values.dir_light_vector = XMVector3Normalize(model_cb_values.dir_light_vector);
	
	//Set constant buffers
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//Update subresources
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

	//Set Shaders
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	
	//Set Input Layout
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	//Assign primitive topology
	m_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Assign sampler
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler);

	//Assign ShaderResources to our texture
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);




	m_pObject->Draw();
	

}

void Model::AddTexture(char * filename)
{
	//Create Texture
	D3DX11CreateShaderResourceViewFromFile
		(m_pD3DDevice, filename, 
		NULL, NULL, &m_pTexture, NULL);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler);
}

void Model::moveXPos(float move_by)
{
	m_x += move_by;
}

void Model::moveYPos(float move_by)
{
	m_y += move_by;
}

void Model::moveZPos(float move_by)
{
	m_z += move_by;
}

void Model::rotateXAngle(float rotate_by)
{
	m_xAngle += rotate_by;
}

void Model::rotateYAngle(float rotate_by)
{
	m_yAngle += rotate_by;
}

void Model::rotateZAngle(float rotate_by)
{
	m_zAngle += rotate_by;
}

void Model::scaleObject(float scale_by)
{
	m_scale += scale_by;
}

void Model::setDirectionalLightVector(float x, float y, float z, float w)
{
	m_dir_light_shines_from = XMVectorSet(x, y, z, w);
}

void Model::setDirectionalLightColour(float x, float y, float z, float w)
{
	m_dir_light_colour = XMVectorSet(x, y, z, w);
}

void Model::setAmbientLightColour(float x, float y, float z, float w)
{
	m_ambient_light_colour = XMVectorSet(x, y, z, w);
}

void Model::setPointLightPos(float x, float y, float z, float w)
{
	m_point_light_pos = XMVectorSet(x, y, z, w);
}

void Model::setPointLightColour(float x, float y, float z, float w)
{
	m_point_light_colour = XMVectorSet(x, y, z, w);
}
