#pragma once
#include <dinput.h>
#include <xinput.h>
#include <math.h>
#include <cmath>
class Input
{
public:
	//Default constructor
	Input();

	//Initialize input (the instance of the window taken as parameter)
	HRESULT InitInput(HINSTANCE hInst,HWND hWnd);

	//Read the input
	void ReadInputStates();

	//Check if the key is pressed
	bool IsKeyPressed(unsigned char DI_keycode);

	//Check if the controller button is pressed
	bool IsButtonPressed(int XI_keycode);

	//Mouse X and Y position
	auto Mouse_X() { return mouse_state.lX; }
	auto Mouse_Y() { return mouse_state.lY; }

	//Gamepad sticks position
	float Gamepad_LeftStick_X();
	float Gamepad_RightStick_Y();
	float Gamepad_LeftStick_Y();
	float Gamepad_RightStick_X();

	~Input();
private:
	//Direct input
	IDirectInput8* direct_input;

	//Pointer to keyboard and mouse
	IDirectInputDevice8* keyboard_device;
	IDirectInputDevice8* mouse_device;
	
	//Mouse and controller state
	DIMOUSESTATE mouse_state;
	XINPUT_STATE controller_state;

	//number of controllers ( 0 = 1 controller , 1 = 2 controllers etc.)
	int num_of_controllers = 0;

	//bool to check if the controller is connected
	bool controller_connected = false;

	//number of keys
	const static int num_of_states = 256;
	unsigned char keyboard_keys_state[num_of_states];

	//the minimum and maximum value of the L and R sticks
	const float stick_min_value = -1.0f;
	const float stick_max_value = 32767.0f;

	//The whole zone of the stick as a whole
	const int stick_zone = 1;
};

