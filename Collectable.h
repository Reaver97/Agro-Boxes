#pragma once
#include "Model.h"
class Collectable
{
public:
	//Initialise collectable
	Collectable(ID3D11Device* device, ID3D11DeviceContext* context);

	//Draw the object in world space
	void Draw(XMMATRIX* view, XMMATRIX* projection);

	//Returns pointer to the model class (used for collision detection mostly)
	Model* GetCollectable() { return collectable; }

	//Checks if the collectable has been picked
	bool WasPicked() { return hasBeenPicked; }

	//Sets the bool to true when object has been picked
	void PickedUp() { hasBeenPicked = true; }

	//Destructor
	~Collectable();

private:
	//scale of object (constant because it never changes values)
	const float scale = 0.3f;

	bool hasBeenPicked = false;

	//pointer to the model class
	Model* collectable;
};

