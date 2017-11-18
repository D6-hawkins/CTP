#ifndef _VBGO_H_
#define _VBGO_H_
#include "gameobject.h"
#include "ConstantBuffer.h"

//base game object to draw objects using the vertex and index buffer


class VBGO : public GameObject
{
public:
	VBGO();
	virtual ~VBGO();

	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;
	//initialise
	static void Init(ID3D11Device* _GD);
	//clean up
	static void CleanUp();
	//update static constant buffer for default rendering
	static void UpdateConstantBuffer(DrawData* _DD);

protected:
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;
	DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT m_numPrims;

	//vertex topology in VB
	D3D_PRIMITIVE_TOPOLOGY m_topology;

	//default vertexshader
	static ID3D11VertexShader*     s_pVertexShader;
	//default vertex layout
	static ID3D11InputLayout*      s_pVertexLayout;
	//default pixelshader
	static ID3D11PixelShader*      s_pPixelShader;
	//default texture (white square)
	static ID3D11ShaderResourceView* s_pTextureRV;
	////deafult const buffer
	static ID3D11Buffer*           s_pConstantBuffer;	//GPU side
	static ConstantBuffer*			s_pCB;				//CPU side
														//default sampler
	static ID3D11SamplerState*		s_pSampler;
	//default raster state
	static ID3D11RasterizerState*  s_pRasterState;

	//my vertexshader
	ID3D11VertexShader*		pVertexShader;
	//my vertex layout
	ID3D11InputLayout*      pVertexLayout;
	//my pixelshader
	ID3D11PixelShader*      pPixelShader;
	//my texture
	ID3D11ShaderResourceView* pTextureRV;
	//my const buffer
	ID3D11Buffer*           pConstantBuffer; //GPU side
	void*					pCB;//CPU side
								  //my sampler
	ID3D11SamplerState*		pSampler;
	//my raster state
	ID3D11RasterizerState*  pRasterState;

	//once populated build an Index Buffer
	void BuildIB(ID3D11Device* _GD, void* _indices);

	//once populated build a Vertex Buffer
	void BuildVB(ID3D11Device* _GD, int _numVerts, void* _vertices);

	static HRESULT CompileShaderFromFile(WCHAR* _szFileName, LPCSTR _szEntryPoint, LPCSTR _szShaderModel, ID3DBlob** _ppBlobOut);
};



#endif // !_VBGO_H_
