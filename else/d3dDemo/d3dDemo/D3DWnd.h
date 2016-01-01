#pragma once
#include <d3d9.h>
#include <d3dx9math.h>
#pragma comment(lib,"d3d9.lib")

#pragma comment(lib,"d3dx9.lib")
// CD3DWnd



class CD3DWnd : public CWnd
{
	DECLARE_DYNAMIC(CD3DWnd)

public:
	CD3DWnd();
	virtual ~CD3DWnd();
	

protected:
	LPDIRECT3D9 m_pD3D; //Direct3D对象的接口指针
	LPDIRECT3DDEVICE9 m_pDevice; //设备对象指针
	LPDIRECT3DINDEXBUFFER9 m_pIB; //索引缓存区的接口指针
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	void InitD3D(); //该函数用于初始化Direct3D
	void CleanUp();
	void Render();

	LPDIRECT3DVERTEXBUFFER9 m_pVB; //顶点缓存区的的接口指针
	void InitGeometry(); //该函数用于建模

	int m_nRotateY; //实体的旋转角度（单位：度）
	void SetupMatrices(); //该函数用于设置三个变换矩阵
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


