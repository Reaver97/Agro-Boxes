#pragma once
#include "Model.h"
#include "Player.h"

class Enemy
{
public:
	//Initialize enemy
	Enemy(ID3D11Device* device, ID3D11DeviceContext* context);

	//Executes the AI functions
	void EnemyUpdate(XMMATRIX* view, XMMATRIX* projection, Player* player, float deltaTime);

	//Checks for collision with other objects in world space
	bool CheckCollision(Model* obj) {return enemy->CheckCollision(obj); }

	//Gets and sets for the position
	float getX() { return enemy->getXPos(); }
	float getY() { return enemy->getYPos(); }
	float getZ() { return enemy->getZPos(); }

	void setX(float position) { enemy->setXPos(position); }
	void setY(float position) { enemy->setYPos(position); }
	void setZ(float position) { enemy->setZPos(position); }

	//Checks if the enemy has been killed
	bool WasDestroyed() { return hasBeenDestroyed; }

	//Returns true if the enemy has been killed
	void Destroyed() { hasBeenDestroyed = true; }

	//Gets the model pointer of the enemy (used mostly for collision detection)
	Model* GetEnemy() { return enemy; }

	~Enemy();
private:
	//constant scale and offset (they never change their values)
	const float scale = 0.3f;
	const float offset = 0.005f;


	bool hasBeenDestroyed = false;
	
	//pointer to the model class
	Model* enemy;
};

