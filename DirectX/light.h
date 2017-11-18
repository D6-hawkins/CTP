#ifndef  _LIGHT_H_
#define _LIGHT_H_
#include "gameobject.h"


struct DrawData;

class Light : public GameObject
{
public:
	Light(Vector3 _pos, Color _colour, Color _ambientColour);
	~Light();

	virtual void Draw(DrawData* _DD) override { _DD; };

	virtual void Tick(GameData* _GD) override;

	Color GetColour() { return m_colour; }
	Color GetAmbCol() { return m_ambientColour; }
	void SetColor(Color _colour) { m_colour = _colour; }
	void SetAmbCol(Color _colour) { m_ambientColour = _colour; }

protected:
	Color m_colour;
	Color m_ambientColour;



};

#endif // ! _LIGHT_H_
