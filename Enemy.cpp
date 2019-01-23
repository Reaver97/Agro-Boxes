#include "Enemy.h"


Enemy::Enemy(ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Creates new model
	enemy = new Model(device, context);

	//Assigns a model
	enemy->LoadObjModel((char*)"sphere.obj");

	//Sets the scale
	enemy->setScale(scale);

	//Assigns a texture
	enemy->AddTexture((char*)"assets/enemy.bmp");
}

void Enemy::EnemyUpdate(XMMATRIX* view,XMMATRIX* projection,Player * player,float deltaTime)
{
	//Looks at the player position in world space
	enemy->LookAt_XZ(player->GetXPos(), player->GetZPos());

	//Goes towards where is looking
	enemy->MoveForward(offset * deltaTime);

	//Draws the object in world space
	enemy->Draw(view,projection);
}



Enemy::~Enemy()
{
	//clean up
	delete enemy;
	enemy = nullptr;
}
