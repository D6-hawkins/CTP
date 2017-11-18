#include "light.h"
#include "gamedata.h"

Light::Light(Vector3 _pos, Color _colour, Color _ambientColour)
{
	m_pos = _pos;
	m_colour = _colour;
	m_ambientColour = _ambientColour;
}

Light::~Light()
{
}

void Light::Tick(GameData* _GD)
{
}