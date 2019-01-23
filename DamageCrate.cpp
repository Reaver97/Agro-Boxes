#include "DamageCrate.h"




DamageCrate::DamageCrate(ID3D11Device * device, ID3D11DeviceContext * context)
{
	//Creates new model,loads object,sets scale,loads texture
	damageCrate = new Model(device, context);
	damageCrate->LoadObjModel((char*)"bunny.obj");
	damageCrate->setScale(scale);
	damageCrate->AddTexture((char*)"assets/texture.bmp");
}

void DamageCrate::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	damageCrate->Draw(view, projection);
}

//Clean up
DamageCrate::~DamageCrate()
{
	delete damageCrate;
	damageCrate = nullptr;
}
