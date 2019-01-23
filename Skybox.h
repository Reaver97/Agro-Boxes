#pragma once
#include "objfilemodel.h"
#include "Camera.h"
#include "Player.h"

struct SKYBOX_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; //64 B
};

class Skybox
{
public:

	//Initialises skybox
	Skybox(ID3D11Device* device, ID3D11DeviceContext* context, Player* camera);

	//Loads object
	HRESULT LoadObjModel(char* filename);

	//Sets raster state
	void RasterState(ID3D11Device* device);
	
	//Makes the objects in the world visible through the skybox
	void DisableDepthBuffer(ID3D11Device* device);

	//Draws the skybox
	void Draw(XMMATRIX* view, XMMATRIX* projection, Player* camera);
	~Skybox();

private:
	//Transforms
	float xAngle, yAngle, zAngle;
	float x, y, z;
	float scale;

	//Object loader pointer
	ObjFileModel*			  skybox;

	//Pointer to D3D11 classes
	ID3D11Device*             pDevice;
	ID3D11DeviceContext*      pContext;
	ID3D11RasterizerState*	  pRasterSolid = 0;
	ID3D11RasterizerState*	  pRasterSkyBox = 0;
	ID3D11DepthStencilState*  pDepthWriteSolid = 0;
	ID3D11DepthStencilState*  pDepthWriteSkyBox = 0;
	ID3DBlob				  *s_VS, *s_PS, *error;
	ID3D11VertexShader*		  s_pVShader;
	ID3D11PixelShader*		  s_pPShader;
	ID3D11InputLayout*		  s_pInputLayout;
	ID3D11Buffer*			  s_pConstantBuffer;
	ID3D11ShaderResourceView* pTexture0;
	ID3D11SamplerState*       pSampler;
};

