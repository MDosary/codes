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
	LPDIRECT3D9 m_pD3D; //Direct3D����Ľӿ�ָ��
	LPDIRECT3DDEVICE9 m_pDevice; //�豸����ָ��
	LPDIRECT3DINDEXBUFFER9 m_pIB; //�����������Ľӿ�ָ��
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	void InitD3D(); //�ú������ڳ�ʼ��Direct3D
	void CleanUp();
	void Render();

	LPDIRECT3DVERTEXBUFFER9 m_pVB; //���㻺�����ĵĽӿ�ָ��
	void InitGeometry(); //�ú������ڽ�ģ

	int m_nRotateY; //ʵ�����ת�Ƕȣ���λ���ȣ�
	void SetupMatrices(); //�ú����������������任����
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


