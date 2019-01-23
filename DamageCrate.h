#pragma once
#include "Model.h"
class DamageCrate
{
public:
	//initialise crate
	DamageCrate(ID3D11Device* device, ID3D11DeviceContext* context);

	//draw in world space
	void Draw(XMMATRIX* view, XMMATRIX* projection);

	//returns pointer to model
	Model* GetDamageCrate() { return damageCrate; }

	//checks if it has been destroyed
	bool WasDestroyed() { return hasBeenDestroyed; }

	//returns the boolean to true when it has been destroyed
	void Destroyed() { hasBeenDestroyed = true; }
	~DamageCrate();
private:
	const float scale = 15.0f;
	bool hasBeenDestroyed = false;
	Model* damageCrate;
};

