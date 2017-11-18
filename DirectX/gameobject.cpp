#include "gameobject.h"

GameObject::GameObject()
{
	m_pos = Vector3::Zero;
	m_scale = Vector3::One;
	m_PhysicsOn = false;
}

GameObject::~GameObject()
{
}

void GameObject::Tick(GameData * _GD)
{
	if (m_PhysicsOn)
	{


	}

	Matrix scaleMat = Matrix::CreateScale(m_scale);
	m_rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll);
	Matrix transMat = Matrix::CreateTranslation(m_pos);

	m_worldMat = m_rotMat*scaleMat * transMat;
}