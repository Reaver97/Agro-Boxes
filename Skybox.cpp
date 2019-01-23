#include "Skybox.h"



Skybox::Skybox(ID3D11Device* device, ID3D11DeviceContext* context,Player* camera)
{
	//assign variables
	pDevice = device;
	pContext = context;

	scale = 3.0f;
	x, y, z = 0.0f;
	xAngle, yAngle, zAngle = 0.0f;

	//load object and initialise raster state and disable depth buffer
	LoadObjModel((char*)"cube.obj");
	RasterState(device);
	DisableDepthBuffer(device);

}

HRESULT Skybox::LoadObjModel(char * filename)
{
	HRESULT hr = S_OK;

	//Create object
	skybox = new ObjFileModel(filename, pDevice, pContext);

	if (skybox->filename == "FILE NOT LOADED") return S_FALSE;

	//Compile the Model Vertex Shader
	hr = D3DX11CompileFromFile("sky_shaders.hlsl", 0, 0, "VS", "vs_4_0", 0, 0, 0, &s_VS, &error, 0);
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
	hr = D3DX11CompileFromFile("sky_shaders.hlsl", 0, 0, "PS", "ps_4_0", 0, 0, 0, &s_PS, &error, 0);
	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, "skybox02.dds", NULL, NULL, &pTexture0, NULL);
	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	pDevice->CreateSamplerState(&sampler_desc, &pSampler);
	//Create the Model Vertex shader
	hr = pDevice->CreateVertexShader(s_VS->GetBufferPointer(), s_VS->GetBufferSize(), NULL, &s_pVShader);
	if (FAILED(hr))//Don't fail if error is just a warning
	{
		return hr;
	}
	hr = pDevice->CreatePixelShader(s_PS->GetBufferPointer(), s_PS->GetBufferSize(), NULL, &s_pPShader);
	if (FAILED(hr))//Don't fail if error is just a warning
	{
		return hr;
	}
	//Set Model Shaders as active ones
	pContext->VSSetShader(s_pVShader, 0, 0);
	pContext->PSSetShader(s_pPShader, 0, 0);

	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//Create input layout for buffer
	hr = pDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), s_VS->GetBufferPointer(), s_VS->GetBufferSize(), &s_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_BUFFER_DESC const_buffer_desc;
	ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));
	//Constant buffer is uses GPU
	const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;

	//Constant Buffer Byte width
	const_buffer_desc.ByteWidth = 64;

	//Set Constant buffer bind flag
	const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	//Create constant buffer
	hr = pDevice->CreateBuffer(&const_buffer_desc, NULL, &s_pConstantBuffer);

	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}
	return S_OK;
}

void Skybox::RasterState(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	device->CreateRasterizerState(&rasterizer_desc, &pRasterSolid);

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rasterizer_desc, &pRasterSkyBox);

}

void Skybox::DisableDepthBuffer(ID3D11Device * device)
{
	D3D11_DEPTH_STENCIL_DESC DSDecsc;
	ZeroMemory(&DSDecsc, sizeof(DSDecsc));

	DSDecsc.DepthEnable = true;
	DSDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSDecsc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&DSDecsc, &pDepthWriteSolid);
	DSDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	device->CreateDepthStencilState(&DSDecsc, &pDepthWriteSkyBox);
}

void Skybox::Draw(XMMATRIX * view, XMMATRIX * projection, Player* player)
{
	x = player->GetXPos();
	y = player->GetYPos();
	z = player->GetZPos();
	XMMATRIX world;
	//Declare constant buffer structure to use the shaders
	SKYBOX_CONSTANT_BUFFER skybox_cb_values;


	//Scale, Rotate and move the object
	world = XMMatrixScaling(scale, scale, scale);
	world *= XMMatrixRotationX(XMConvertToRadians(xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(zAngle));
	world *= XMMatrixTranslation(x, y, z);

	//Set the world view projection matrix
	skybox_cb_values.WorldViewProjection = world * (*view) * (*projection);

	//Set constant buffers
	pContext->VSSetConstantBuffers(0, 1, &s_pConstantBuffer);

	//Update subresources
	pContext->UpdateSubresource(s_pConstantBuffer, 0, 0, &skybox_cb_values, 0, 0);

	//Set Shaders
	pContext->VSSetShader(s_pVShader, 0, 0);
	pContext->PSSetShader(s_pPShader, 0, 0);

	pContext->PSSetSamplers(0, 1, &pSampler);
	pContext->PSSetShaderResources(0, 1, &pTexture0);

	//Set Input Layout
	pContext->IASetInputLayout(s_pInputLayout);

	//Assign primitive topology
	pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Rasterize the skybox and set the depth stencil to the skybox
	pContext->RSSetState(pRasterSkyBox);
	pContext->OMSetDepthStencilState(pDepthWriteSkyBox, 1);

	skybox->Draw();

	//Rasterize the solid and set the depth stencil to the solid
	pContext->RSSetState(pRasterSolid);
	pContext->OMSetDepthStencilState(pDepthWriteSolid, 1);
}

//Clean up
Skybox::~Skybox()
{
	delete skybox;
	skybox = nullptr;

	if (s_pVShader) s_pVShader->Release();
	if (s_pPShader) s_pPShader->Release();
	if (s_pInputLayout) s_pInputLayout->Release();
	if (s_pConstantBuffer) s_pConstantBuffer->Release();
	if (pTexture0) pTexture0->Release();
	if (pSampler) pSampler->Release();
	if (pRasterSkyBox) pRasterSkyBox->Release();
	if (pRasterSolid) pRasterSolid->Release();
	if (pDepthWriteSkyBox) pDepthWriteSkyBox->Release();
	if (pDepthWriteSolid) pDepthWriteSolid->Release();
};



