#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "gameobject.h"
//Base camera

struct GameData;

class Camera : public GameObject
{
public:
	Camera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up , Vector3 _target = Vector3::Zero);
	~Camera();

	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;
	//Getters
	Matrix getProj() { return m_projMat; }
	Matrix getView() { return m_viewMat; }


private:
	Matrix m_projMat;
	Matrix m_viewMat;

	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlaneDistance;
	float m_FarPlaneDistance;


	Vector3 m_target;
	Vector3 m_up;
};
#endif