#include "GameLoop.h"




//Define vertex structure

GameLoop::GameLoop(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Starts the timers for delta time and fps
	timer.Start();
	fpsTimer.Start();
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//initialises the window if everything is ok
	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
	}
	//initialises the d3d element if everything is ok
	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
	}
	//initialises the graphic elements if everything is ok
	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to init graphics");
	}

}

LRESULT GameLoop::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (message)
		{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
}


HRESULT GameLoop::InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	const int MAX_CHAR_NAME = 100;



	// Give your app your own name
	char Name[MAX_CHAR_NAME] = "Agro-Boxes";

	// Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = GameLoop::WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//   wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, Width, Height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(Name, g_AppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}


	
	
HRESULT GameLoop::InitialiseD3D()
{


	HRESULT hr = S_OK;

	int refreshRate = 60;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	//The DirectX version that this app can run on
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//Creates swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = refreshRate;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&g_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	//Create a Z buffer texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D * pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	//Create the Z buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();


	// Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;




	g_pImmediateContext->RSSetViewports(1, &viewport);
	g_2DText = new Text2D("assets/font1.bmp", g_pD3DDevice, g_pImmediateContext);
	return S_OK;
}

HRESULT GameLoop::InitialiseGraphics()
{

	HRESULT hr = S_OK;

	//Initialise the Level and its elements
	level = new Level(g_pD3DDevice, g_pImmediateContext,g_hInst,g_hWnd);

	//Initialise the skybox and attach it to the player
	g_pSkybox = new Skybox(g_pD3DDevice, g_pImmediateContext,level->GetPlayer());

	return S_OK;
}

void GameLoop::CalculateFPS()
{
	//Calculates the number of updates every time a second has passed
	static int fpsCounter = 0;
	fpsCounter += 1;
	if (fpsTimer.GetMiliSecondsElapsed() > ms)
	{
		//Resets the timer
		FPS = fpsCounter;
		fpsCounter = 0;
		fpsTimer.Restart();
	}
}

void GameLoop::Update()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			// do something
			RenderFrame();
		}
	}
	//Clean up
	Release();
}

void GameLoop::RenderFrame()
{
	//Initialise delta time only after drawn (preventing bugs)
	if (drawn) 
	{
		deltaTime = (float)timer.GetMiliSecondsElapsed();
	}
	//Restart the timer to get the right value after everything has been drawn
	timer.Restart();
	
	//Matrix world view projection
	XMMATRIX projection, world, view;

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(degrees), (float)Width/(float)Height, nearPlane, farPlane);

	//Assign the view to the player camera
	view = level->GetPlayer()->GetViewMatrix();

	// Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { red, green, blue, alpha };

	//Clears the back buffer with the colour above
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);

	//Creates the Z buffer
	g_pImmediateContext->ClearDepthStencilView
	(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//if the level has been drawn and the player is still alive draw the skybox and the level
	if (drawn && level->GetPlayer()->isAlive())
	{
		g_pSkybox->Draw(&view, &projection, level->GetPlayer());
		level->Draw(&view, &projection, g_hWnd, deltaTime);
	}

	//Calculate FPS
	CalculateFPS();

	//Show text information on screen
	ShowText();

	// Display what has just been rendered
	g_pSwapChain->Present(0, 0);
	drawn = true;
}

void GameLoop::ShowText()
{
	//Renders the text on screen
	g_2DText->RenderText();

	//Set up position for the text box
	x = -1.0f;
	y = 0.975f;
	z = 0.025f;

	//Shows delta time on screen
	g_2DText->AddText(to_string(deltaTime) + "MS", x, y, z);

	//Set up position for the text box
	x = -1.0f;
	y = 1.0f;
	z = 0.025f;

	//Show FPS on screen
	g_2DText->AddText("FPS " + to_string(FPS), x, y, z);

	//Set up position for the text box
	x = -1.0f;
	y = -0.9f;
	z = 0.05f;

	//Show HP of the player on screen
	g_2DText->AddText("HP " + to_string(level->GetPlayer()->GetPlayerHP()), x, y, z);

	//Prompt the player with the correct messages when he died
	if (!level->GetPlayer()->isAlive())
	{
		//Set up position for the text box
		x = -0.45f;
		y = 0.1f;
		z = 0.1f;


		g_2DText->AddText("YOU DIED", x, y, z);

		//Set up position for the text box
		x = -0.475f;
		y = 0.0f;
		z = 0.05f;


		g_2DText->AddText("Press ESC to exit", x, y,z);
		level->GetPlayer()->PlayerInput(deltaTime, g_hWnd, 0);
	}
}

void GameLoop::Release()
{
	//Clean up
	if (g_2DText)
	{
		delete g_2DText;
		g_2DText = nullptr;
	}
	if (g_pSkybox)
	{
		delete g_pSkybox;
		g_pSkybox = nullptr;
	}
	if (level)
	{
		delete level;
		level = nullptr;
	}

	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();
	if (g_pZBuffer) g_pZBuffer->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
}


GameLoop::~GameLoop()
{

}
