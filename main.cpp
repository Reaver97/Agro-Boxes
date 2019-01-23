
#include "GameLoop.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	GameLoop* gl;
	gl = new GameLoop(hInstance,hPrevInstance, lpCmdLine,nCmdShow);
	
	gl->Update();
	delete gl;
	gl = nullptr;
	return 0;

}


