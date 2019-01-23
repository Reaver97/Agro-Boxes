#pragma once

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>
#include <xnamath.h>
#include <string>
#include "Camera.h"
#include "Model.h"
#include "text2D.h"
#include "Timer.h"
#include "Input.h"
#include "Skybox.h"
#include "Enemy.h"
#include "Level.h"
using namespace std;


class GameLoop
{
public:
	//Initialises the Game
	GameLoop(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

	//Creates the window process
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//Initialises the window
	HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);

	//Initialises the D3D elements
	HRESULT InitialiseD3D();

	//Initialises the elements in the game world
	HRESULT InitialiseGraphics();

	//Calculates the fps
	void CalculateFPS();

	//Creates the loop
	void Update();

	//The Game loop
	void RenderFrame();

	// Show text on screen (FPS,Delta Time, Player information, Death screen)
	void ShowText();

	//Clean up
	void Release();
	~GameLoop();

private:
	//DirectX variables
	HINSTANCE				  g_hInst = NULL;
	HWND					  g_hWnd = NULL;
	D3D_DRIVER_TYPE           g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL         g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*             g_pD3DDevice = NULL;
	ID3D11DeviceContext*      g_pImmediateContext = NULL;
	IDXGISwapChain*           g_pSwapChain = NULL;
	ID3D11RenderTargetView*   g_pBackBufferRTView = NULL;
	ID3D11DepthStencilView*   g_pZBuffer;

	//Pointers to includes
	Level*					  level;
	Text2D*					  g_2DText;
	Skybox*					  g_pSkybox;
	Timer                     timer;
	Timer					  fpsTimer;

	//Check if everything was drawn on the screen before code is executed
	bool drawn = false;

	//Is assigned the fps value
	int FPS;

	char g_AppName[100] = "Agro-Boxes\0";

	//Is assigned the delta time value
	float deltaTime;
	float x, y, z;
	
	//clear back buffer with a colour
	const float red = 0.1f;
	const float green = 0.1f;
	const float blue = 0.5f;
	const float alpha = 1.0f;

	//Width and Height of the window
	const int Width = 1280;
	const int Height = 720;

	//used for conversion to radian
	const float degrees = 45.0f;

	//Camera clipping planes
	const float nearPlane = 1.0f;
	const float farPlane = 100.0f;

	//Millisecond value
	const double ms = 1000.0;

	//Coordinates of text on screen
	float text_x, text_y, text_z;
};

