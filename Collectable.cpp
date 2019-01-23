#include "Collectable.h"



Collectable::Collectable(ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Creates new model
	collectable = new Model(device, context);

	//Loads the texture
	collectable->LoadObjModel((char*)"cube.obj");

	//Sets the default scale
	collectable->setScale(scale);

	//Assigns a texture
	collectable->AddTexture((char*)"assets/healthpack.bmp");
}

void Collectable::Draw(XMMATRIX * view, XMMATRIX * projection)
{
	//Draws collectable in world space
	collectable->Draw(view, projection);
}


Collectable::~Collectable()
{
	//deletes the model pointer (clean up)
	delete collectable;
	collectable = nullptr;
}
