#pragma once
#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_

//#include "SimpleMath.h"
//Data file to be passed to all game objects in their tick function
//using namespace DirectX;

#include <dinput.h>
#include "GameState.h"

using namespace DirectX;

struct GameData
{
	float m_dt; //Delta time
	GameState m_GS; //global game state

	//Player input tracker
	unsigned char* m_keyboardState; //Current state of the keyboard
	unsigned char* m_prevkKeyboardState; //Previous state of the keyboard
	DIMOUSESTATE *m_mouseState; //Mouse State
};


#endif