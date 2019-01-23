#include "Input.h"



Input::Input()
{
}

HRESULT Input::InitInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	//Allocate memory for the keyboard
	ZeroMemory(keyboard_keys_state, sizeof(keyboard_keys_state));
	

	//Create the direct input
	hr = DirectInput8Create(hInst,DIRECTINPUT_VERSION,IID_IDirectInput8, (void**)&direct_input,NULL);
	if (FAILED(hr)) return hr;

	//Create the keyboard device
	hr = direct_input->CreateDevice(GUID_SysKeyboard, &keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	//Set the data format of the keyboard
	hr = keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	//Set the cooperative level (executed only in foreground and it's not exclusive to the window)
	hr = keyboard_device->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	//acquires the keyboard device
	hr = keyboard_device->Acquire();
	if (FAILED(hr)) return hr;

	//creates the mouse device
	hr = direct_input->CreateDevice(GUID_SysMouse,&mouse_device, NULL );
	if (FAILED(hr)) return hr;

	//sets the data format of the mouse
	hr = mouse_device->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) return hr;

	//Set the cooperative level (executed only in foreground and it's exclusive to the window)
	hr = mouse_device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(hr)) return hr;

	//acquire the mouse device
	hr = mouse_device->Acquire();
	if (FAILED(hr)) return hr;

	return S_OK;
}

void Input::ReadInputStates()
{
	HRESULT hr;
	hr = keyboard_device->GetDeviceState(sizeof(keyboard_keys_state), (LPVOID)&keyboard_keys_state);

	//if the keyboard was not acquired of the input has been lost acquire
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			keyboard_device->Acquire();
		}
	}
	//if the mouse was not acquired of the input has been lost acquire
	if (mouse_device) 
	{
		hr = mouse_device->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);
		if (FAILED(hr))
		{
			if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			{
				mouse_device->Acquire();
			}
		}
	}
	//Allocate memory to the controller state
	ZeroMemory(&controller_state, sizeof(controller_state));

	//Get the number of controllers
	controller_connected = (XInputGetState(num_of_controllers, &controller_state) == ERROR_SUCCESS);
}

bool Input::IsKeyPressed(unsigned char DI_keycode)
{
	return keyboard_keys_state[DI_keycode] & 0x80;
}


bool Input::IsButtonPressed(int XI_keycode)
{
	return ((controller_state.Gamepad.wButtons & XI_keycode) !=0);
}


float Input::Gamepad_LeftStick_X()
{
	float LX = controller_state.Gamepad.sThumbLX;

	//normalize the dravel distance from min value to max value
	float normalizedLX = fmaxf( stick_min_value, (float)LX / stick_max_value);
	
	//assume a deadzone of 0.05
	float deadzoneX = 0.05f;

	float leftStickX = (abs(normalizedLX) < deadzoneX ? 0 : normalizedLX);

	//if deadzone, substract from the total zone of the stick
	if (deadzoneX > 0) leftStickX *= stick_zone - deadzoneX;

	return leftStickX;

}

float Input::Gamepad_LeftStick_Y()
{

	float LY = controller_state.Gamepad.sThumbLY;

	float normalizedLY = fmaxf(stick_min_value, (float)LY / stick_max_value);

	float deadzoneY = 0.05f;

	float leftStickY = (abs(normalizedLY) < deadzoneY ? 0 : normalizedLY);
	
	if (deadzoneY > 0) leftStickY *= stick_zone - deadzoneY;

	return leftStickY;
}

float Input::Gamepad_RightStick_X()
{
	float RX = controller_state.Gamepad.sThumbRX;

	float normalizedRX = fmaxf(stick_min_value, (float)RX / stick_max_value);

	float deadzoneX = 0.05f;

	float rightStickX = (abs(normalizedRX) < deadzoneX ? 0 : normalizedRX);

	if (deadzoneX > 0) rightStickX *= stick_zone - deadzoneX;

	return rightStickX;

}

float Input::Gamepad_RightStick_Y()
{

	float RY = controller_state.Gamepad.sThumbRY;

	float normalizedRY = fmaxf(stick_min_value, (float)RY / stick_max_value);

	float deadzoneY = 0.05f;

	float rightStickY = (abs(normalizedRY) < deadzoneY ? 0 : normalizedRY);

	if (deadzoneY > 0) rightStickY *= stick_zone - deadzoneY;

	return rightStickY;
}

Input::~Input()
{
	//Clean up
	if (keyboard_device)
	{
		keyboard_device->Unacquire();
		keyboard_device->Release();
	}
	if (mouse_device)
	{
		mouse_device->Unacquire();
		mouse_device->Release();
	}
	if (direct_input) direct_input->Release();
}
