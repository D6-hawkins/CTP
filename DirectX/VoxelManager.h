#pragma once
#include <D3DX11.h>
#include <D3DX10.h>
#include <DirectXMath.h>
#include <vector>

struct VERTEX
{
	FLOAT normx, normy, normz; //normal position
	FLOAT x, y, z; //position
	D3DXCOLOR Colour; // Colour
};
class  VoxelManager
{
public:
	 VoxelManager();
	~ VoxelManager();
	void setUp();
	VERTEX wub();
private:

	float length = 1.0f;
	float height = 1.0f;
	float width = 1.0f;
	VERTEX vertStore[16];
};