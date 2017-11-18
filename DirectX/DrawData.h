#ifndef  _DRAW_DATA_H_
#define _DRAW_DATA_H_
#include "CommonStates.h"
using namespace DirectX;
class Camera;
class Light;

//Data to be passed to all gameobjects through the draw function
struct DrawData
{
	ID3D11DeviceContext* m_pd3dImmediateContext;
	CommonStates * m_states;
	Camera* m_cam;
	Light* m_light;
};


#endif // ! _DRAW_DATA_H_
