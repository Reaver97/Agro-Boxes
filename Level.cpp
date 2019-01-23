#include "Level.h"



Level::Level(ID3D11Device* device, ID3D11DeviceContext* context, HINSTANCE hInst, HWND hWnd)
{
	player = new Player(hInst, hWnd);
	for (int x = 0; x < arraySize; x++)
	{
		for (int z = 0; z < arraySize; z++)
		{
			//Create the ground floor if found in the array
			if (groundFloor[x][z] == wall) 
			{
				floor[x][z] = new Model(device, context);
				floor[x][z]->LoadObjModel((char*)"cube.obj");
				floor[x][z]->AddTexture((char*)"assets/texture.bmp");

				floor[x][z]->setYPos(ground_y_offset);
				floor[x][z]->setXPos(-x * xz_offset);
				floor[x][z]->setZPos(z * xz_offset);
			}
			//create the walls at the 2nd floor if found in the array
			if (level1[x][z] == wall)
			{
				walls1[x][z] = new Model(device, context);
				walls1[x][z]->LoadObjModel((char*)"cube.obj");
				walls1[x][z]->AddTexture((char*)"assets/texture.bmp");
				walls1[x][z]->setYPos(level1_y_offset);
				walls1[x][z]->setXPos(-x * xz_offset);
				walls1[x][z]->setZPos(z * xz_offset);
			}
			//create the health packs at the 2nd floor if found in the array
			if (level1[x][z] == hpkit)
			{
				collectable[x][z] = new Collectable(device, context);
				collectable[x][z]->GetCollectable()->setYPos(level1_y_offset);
				collectable[x][z]->GetCollectable()->setXPos(-x * xz_offset);
				collectable[x][z]->GetCollectable()->setZPos( z * xz_offset);

			}
			//create the crates at the 2nd floor if found in the array
			if (level1[x][z] == crate)
			{
				damageCrate[x][z] = new DamageCrate(device, context);
				damageCrate[x][z]->GetDamageCrate()->setYPos(bunny_y_offset);
				damageCrate[x][z]->GetDamageCrate()->setXPos(-x * xz_offset);
				damageCrate[x][z]->GetDamageCrate()->setZPos( z * xz_offset);
			}
			//create the enemies at the 2nd floor if found in the array
			if (level1[x][z] == enemy)
			{
				enemies[x][z] = new Enemy(device, context);
				enemies[x][z]->setY(level1_y_offset);
				enemies[x][z]->setX(-x * xz_offset);
				enemies[x][z]->setZ(z * xz_offset);
			}
			//create the walls at the 3rd floor if found in the array
			if (level2[x][z] == wall)
			{
				walls2[x][z] = new Model(device, context);
				walls2[x][z]->LoadObjModel((char*)"cube.obj");
				walls2[x][z]->AddTexture((char*)"assets/texture.bmp");
				walls2[x][z]->setYPos(level2_y_offset);
				walls2[x][z]->setXPos(-x * xz_offset);
				walls2[x][z]->setZPos(z * xz_offset);
			}

			
		}
	}
}

void Level::Draw(XMMATRIX* view, XMMATRIX* projection, HWND hWnd, float deltaTime)
{
	//Initialise player input
	player->PlayerInput(deltaTime,hWnd,number);

	//Player collision with world bounds
	if (player->GetXPos() <= -max_offset || player->GetZPos() >= max_offset || 
		player->GetXPos() >= -min_offset || player->GetZPos() <= min_offset)
	{
		//Send back the player if collides
		player->PlayerInput(deltaTime, hWnd,-number);
	}

//Array to draw the objects
for (int x = 0; x < arraySize; x++)
{
	for (int z = 0; z < arraySize; z++)
	{
		if (groundFloor[x][z] == wall)
		{
			floor[x][z]->Draw(view, projection);
		}
		//Draw the enemy if he hasn't died
		if (level1[x][z] == enemy && !enemies[x][z]->WasDestroyed())
		{
			//Move the enemy
			enemies[x][z]->EnemyUpdate(view, projection, player, deltaTime);
			//Check collision between the player and the enemy
			if (player->CheckCollision(enemies[x][z]->GetEnemy()))
			{
				//Damage the player
				player->DamagePlayer();
				//Collide with player
				enemies[x][z]->EnemyUpdate(view, projection, player, -deltaTime);
			}

			//Check collision between player and enemy
			if (player->CheckCollision(enemies[x][z]->GetEnemy()))
			{
				//Player collides with enemy
				player->PlayerInput(deltaTime, hWnd, -number);
			}
			//third and fourth for loop to loop through enemies that are not self
			for (int x1 = 0; x1 < arraySize; x1++)
			{
				for (int z1 = 0; z1 < arraySize; z1++)
				{
					if (level1[x][z] == enemy && level1[x1][z1] == enemy)
					{
						//If there is a collision between enemies and the enemy is not destroyed collide
						if (enemies[x][z]->CheckCollision(enemies[x1][z1]->GetEnemy()) && !enemies[x1][z1]->WasDestroyed())
						{
							enemies[x][z]->EnemyUpdate(view, projection, player, -deltaTime);
						}
					}
				}
			}
		}
		//Draw the walls
		if (level1[x][z] == wall)
		{
			walls1[x][z]->Draw(view, projection);
		}
		//Loop created to check collision between enemy and wall
		for (int x1 = 0; x1 < arraySize; x1++)
		{
			for (int z1 = 0; z1 < arraySize; z1++)
			{
				if (level1[x][z] == wall && level1[x1][z1] == enemy)
				{
					if (walls1[x][z]->CheckCollision(enemies[x1][z1]->GetEnemy()))
					{
						enemies[x1][z1]->EnemyUpdate(view, projection, player, -deltaTime);
					}
				}
			}
		}

		//Loop created to check collision between enemy and damaging crate
		for (int x1 = 0; x1 < arraySize; x1++)
		{
			for (int z1 = 0; z1 < arraySize; z1++)
			{
				if (level1[x][z] == crate && level1[x1][z1] == enemy 
					&& !enemies[x1][z1]->WasDestroyed() && !damageCrate[x][z]->WasDestroyed())
				{
					if (damageCrate[x][z]->GetDamageCrate()->CheckCollision(enemies[x1][z1]->GetEnemy()) 
						&& !enemies[x1][z1]->WasDestroyed() && !damageCrate[x][z]->WasDestroyed())
					{
						enemies[x1][z1]->Destroyed();
						damageCrate[x][z]->Destroyed();
					}
				}
			}
		}

		//Draw 3rd floor walls
		if (level2[x][z] == wall)
		{
			walls2[x][z]->Draw(view, projection);
		}

		//Draw the health pack if it hasn't been picked by the player
		if (level1[x][z] == hpkit)
		{
			if(!collectable[x][z]->WasPicked())
			collectable[x][z]->Draw(view, projection);
			//if picked by the player, assign as picked and heal player
			if (player->CheckCollision(collectable[x][z]->GetCollectable()) && !collectable[x][z] ->WasPicked())
			{
				player->addPlayerHP();
				collectable[x][z]->PickedUp();
			}
		}
		//create crates in 2nd floor if it wasn't destroyed
		if (level1[x][z] == crate)
		{
			if (!damageCrate[x][z]->WasDestroyed())
				damageCrate[x][z]->Draw(view, projection);
			//if player collides with the crate, collide
			if (player->CheckCollision(damageCrate[x][z]->GetDamageCrate()) && !damageCrate[x][z]->WasDestroyed())
			{
				player->PlayerInput(deltaTime, hWnd, -number);
			}
		}
	}
}
}

Level::~Level()
{
	//clean up
	delete player;
	player = nullptr;
	for (int x = 0; x < arraySize; x++)
	{
		for (int z = 0; z < arraySize; z++)
		{
			
			if (groundFloor[x][z] == wall)
			{
				delete floor[x][z];
				floor[x][z] = nullptr;
			}
			if (level1[x][z] == wall)
			{
				delete walls1[x][z];
				walls1[x][z] = nullptr;
			}
			if (level2[x][z] == wall)
			{
				delete walls2[x][z];
				walls2[x][z] = nullptr;
			}
			if (level1[x][z] == enemy)
			{
				delete enemies[x][z];
				enemies[x][z] = nullptr;
			}
			if (level1[x][z] == hpkit)
			{
				delete collectable[x][z];
				collectable[x][z] = nullptr;
			}
			if (level1[x][z] == crate)
			{
				delete damageCrate[x][z];
				damageCrate[x][z] = nullptr;
			}
		}
	}

}
