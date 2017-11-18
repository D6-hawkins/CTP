#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

//Base Game Object Class
#include "CommonStates.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera;
struct ID3D111DeviceContext;
struct GameData;
struct DrawData;

class GameObject
{
public:
	//Base functions
	GameObject();
	virtual ~GameObject();

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD) = 0;
	//-----

	//getters
	Vector3 getPos() { return m_pos; }
	Vector3 getScale() { return m_scale; }

	float getDrag() { return m_drag; }

	//setters
	void setPos(Vector3 _pos) { m_pos = _pos; }
	void setPitch(float _pitch) { m_pitch = _pitch; }
	void setScale(Vector3 _scale) { m_scale = _scale * Vector3::One; }

protected:

	Vector3 m_pos;
	Vector3 m_scale;
	float m_pitch, m_yaw, m_roll;

	bool m_PhysicsOn;
	Matrix m_worldMat;

	Matrix m_rotMat;
	//Physics - for water implementation
	float m_drag = 0.0f;
	Vector3 m_vel = Vector3::Zero;
	Vector3 m_acc = Vector3::Zero;

};
#endif