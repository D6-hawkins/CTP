#ifndef _CONSTBUFFER_H_
#define _CONSTBUFFER_H_

//Constant buffer to render VBGOs

#include <Windows.h>
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace SimpleMath;

//_declspec(align(16))

struct ConstantBuffer
{
	Matrix world;
	Matrix view;
	Matrix Projection;
	Matrix rot;
	Color lightCol;
	Color ambientCol;
	Vector3 lightPos;
};

#endif