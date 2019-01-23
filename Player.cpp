#include "Player.h"



Player::Player(HINSTANCE hInst, HWND hWnd)
{
	//Default value assigned to the player
	PlayerHP = 100;
	x = -20.0f;
	y = 0.0f;
	z = 20.0f;
	rotation = 0.0f;

	//Create the player camera and initialize the input
	playerCamera = new Camera(x,y,z,rotation,rotation);
	playerInput = new Input();
	playerInput->InitInput(hInst, hWnd);
}

void Player::PlayerInput(float deltaTime, HWND hWnd,int negative_number)
{
	//Update input
	playerInput->ReadInputStates();

	//Reset the camera Y
	playerCamera->SetY(not_crouch);
	//Keyboard buttons and Controller buttons input

	//Player presses escape or start exits the game
	if (playerInput->IsKeyPressed(DIK_ESCAPE) || playerInput->IsKeyPressed(XINPUT_GAMEPAD_START))
		DestroyWindow(hWnd);

	//Player presses WASD or arrow keys, moves towards that direction
	if (playerInput->IsKeyPressed(DIK_W) || playerInput->IsKeyPressed(DIK_UP))
		playerCamera->Forward(negative_number * movement_offset * deltaTime);

	if (playerInput->IsKeyPressed(DIK_S) || playerInput->IsKeyPressed(DIK_DOWN))
		playerCamera->Forward(negative_number * -movement_offset * deltaTime);

	if (playerInput->IsKeyPressed(DIK_A) || playerInput->IsKeyPressed(DIK_LEFT))
		playerCamera->Strafe(negative_number * movement_offset * deltaTime);

	if (playerInput->IsKeyPressed(DIK_D) || playerInput->IsKeyPressed(DIK_RIGHT))
		playerCamera->Strafe(negative_number * -movement_offset * deltaTime);

	/*if (playerInput->IsKeyPressed(DIK_SPACE) || playerInput->IsButtonPressed(XINPUT_GAMEPAD_A))
		playerCamera->Up(movement_offset*deltaTime);*/

	if (playerInput->IsKeyPressed(DIK_LCONTROL) || playerInput->IsButtonPressed(XINPUT_GAMEPAD_B))
	{
		//Set the camera Y to crouch
		playerCamera->SetY(crouch);
	}


	//Mouse and sticks input

	//Rotates the camera based on the mouse input or controller sticks input
	//(controller also moves the player with one of the sticks)
	playerCamera->Rotate(playerInput->Mouse_X() * camera_sensitivity * deltaTime);

	playerCamera->RotateY(playerInput->Mouse_Y() *-camera_sensitivity * deltaTime);

	playerCamera->Forward(playerInput->Gamepad_LeftStick_Y()* movement_offset * deltaTime);

	playerCamera->Strafe(-playerInput->Gamepad_LeftStick_X() * movement_offset * deltaTime);

	playerCamera->Rotate(playerInput->Gamepad_RightStick_X() * movement_offset * deltaTime);

	playerCamera->RotateY(playerInput->Gamepad_RightStick_Y() * movement_offset * deltaTime);
}
//Similar to Model collision
bool Player::CheckCollision(Model * obj)
{
	XMVECTOR current_model_position;
	XMVECTOR passed_model_position;
	current_model_position = GetBoundingSphereWorldSpacePosition();
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

	float distance = sqrt(x + y + z);
	float sumOfRadius = obj->GetBoundingSphereRadius() + radius;

	if (sumOfRadius > distance)
	{
		return true;
	}

	return false;
}

//Similar to model method
XMVECTOR Player::GetBoundingSphereWorldSpacePosition()
{
	XMMATRIX world;

	world = XMMatrixScaling(scale, scale, scale);
	world *= XMMatrixRotationX(XMConvertToRadians(0));
	world *= XMMatrixRotationY(XMConvertToRadians(0));
	world *= XMMatrixRotationZ(XMConvertToRadians(0));
	world *= XMMatrixTranslation(playerCamera->GetX(), playerCamera->GetY(), playerCamera->GetZ());

	XMVECTOR offset;
	offset = XMVectorSet(position_offset, position_offset, position_offset, position_offset);

	offset = XMVector3Transform(offset, world);


	return offset;
}

//Clean up
Player::~Player()
{
	if (playerInput)
	{
		delete playerInput;
		playerInput = nullptr;
	}
	if (playerCamera)
	{
		delete playerCamera;
		playerCamera = nullptr;
	}
}
