#ifndef _MARCHING_CUBES_H_
#define _MARCHING_CUBES_H_
#include "VBGO.h"

using namespace DirectX::SimpleMath;
class MarchingCubes : public VBGO
{
public:
	MarchingCubes() {};
	virtual ~MarchingCubes() {};

	void init(Vector3 _size, float _isolevel, Vector3 _scale, Vector3 _origin, ID3D11Device* _GD);
	void init(Vector3 _min, Vector3 _max, Vector3 _size,float _isolevel , ID3D11Device* _GD);

	void Tick(GameData *_GD) override;
protected:
	float function(Vector3 _pos);
};
#endif