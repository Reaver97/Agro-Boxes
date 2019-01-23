#pragma once
#include "Camera.h"
#include "Input.h"
#include "Model.h"
class Player
{
public:
	//Initialises player
	Player(HINSTANCE hInst, HWND hWnd);

	//Reads the input
	void PlayerInput(float deltaTime, HWND hWnd,int negative_number);

	//returns if it's alive or not
	bool isAlive() { return PlayerHP > 0; }

	//Checks collision between player and object
	bool CheckCollision(Model* obj);

	//gets the view matrix
	XMMATRIX GetViewMatrix() { return playerCamera->GetViewMatrix(); }

	//Gets of the xyz position
	float GetXPos() { return playerCamera->GetX(); }
	float GetYPos() { return playerCamera->GetY(); }
	float GetZPos() { return playerCamera->GetZ(); }

	//Sets of the player HP
	void setPlayerHP(int offset) { PlayerHP = offset; }

	//Heal the player
	void addPlayerHP() { PlayerHP += HealAmount; }

	//Damage the palyer
	void DamagePlayer() { PlayerHP -= DamageAmount; }

	//Return the player's hp
	int  GetPlayerHP() { return PlayerHP; }
	~Player();
private:

	XMVECTOR GetBoundingSphereWorldSpacePosition();

	//Offsets and transforms
	const float movement_offset = 0.01f;
	const float camera_sensitivity = 0.05f;
	const float scale = 1.0f;
	const float position_offset = 0.0f;
	const float radius = 1.0f;
	const float crouch = -0.5f;
	const float not_crouch = 0.0f;

	//Player's hp
	int PlayerHP;

	//Heal amount and damage amount as consts because their value never changes
	const int HealAmount = 50;
	const int DamageAmount = 1;

	//xyz position
	float x;
	float y;
	float z;
	//rotation
	float rotation;

	//pointers to input and camera
	Input* playerInput;
	Camera* playerCamera;
};

