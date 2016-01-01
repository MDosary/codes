// D3DWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "d3dDemo.h"
#include "D3DWnd.h"

//����FVF�Ķ���ṹ
struct CUSTOMVERTEX
{
	float x, y, z; //��������
	DWORD color; //������ɫ
};
//����FVF�õ��������������ɫ
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

// CD3DWnd

IMPLEMENT_DYNAMIC(CD3DWnd, CWnd)

CD3DWnd::CD3DWnd()
{

}

CD3DWnd::~CD3DWnd()
{
}


BEGIN_MESSAGE_MAP(CD3DWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CD3DWnd ��Ϣ�������

//��ʼ��
void CD3DWnd::InitD3D()
{
	//����Direct3D���󣬲���ȡ�ӿ�IDirect3D9��ָ�룬
	//���ǽ�ͨ����ָ�����Direct3D����
	m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	::ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE; //��������ģʽ��Direct3D����
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; //����16 bit��Z����
	
	//���÷���IDirect3D9::CreateDevice�����豸���󣬲���ȡ
	//�ӿ�IDirect3DDevice9��ָ�룬���ǽ�ͨ����ָ������豸����
	m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,					//ʹ��ȱʡ���Կ�
		D3DDEVTYPE_HAL,						//ָ���豸����ΪHAL
		m_hWnd,								//Direct3D���ڵľ��
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,//������㴦��
		&d3dpp, &m_pDevice);

	//������Ⱦ
	m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
}


void CD3DWnd::CleanUp()
{
	if(m_pVB!=NULL)
	{
		m_pVB->Release();
	}
	if (m_pIB != NULL)
	{
		m_pIB->Release();
	}
	if(m_pDevice!=NULL)
	{
		m_pDevice->Release();
	}
	if(m_pD3D!=NULL)
	{
		m_pD3D->Release();
	}
}

//��Ⱦ����
void CD3DWnd::Render()
{
	//��ָ����ɫ����󱸻�����
	if(m_pDevice==NULL)
	{
		return;
	}
	m_pDevice->Clear(
		0, NULL, D3DCLEAR_TARGET| D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0,0,255),
		1.0f, 0);
	if(m_pDevice->BeginScene())
	{

		SetupMatrices();
		m_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		m_pDevice->SetStreamSource(0,m_pVB,0,
			sizeof(CUSTOMVERTEX));

		m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
		//������������

			m_pDevice->SetIndices( m_pIB );
		//����������������ͼԪ������1ΪͼԪ��ʽ������4Ϊ������������6Ϊ��������
		m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 4 );
		m_pDevice->EndScene();
		
	}
	m_pDevice->Present(NULL,NULL,NULL,NULL);
}


void CD3DWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//Render();
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CWnd::OnPaint()
	InitD3D();
}


void CD3DWnd::OnDestroy()
{
	CWnd::OnDestroy();
	// TODO: �ڴ˴������Ϣ����������
	CleanUp();
}

//���ö��㻺�������
void CD3DWnd::InitGeometry()
{
	//������ʵ�����ѧģ��
	CUSTOMVERTEX vertices[] =									//FVF��������
	{{ 0.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,255,0) },				//��A����ɫ
	{ -1.0f, -1.0f, -0.577f, D3DCOLOR_XRGB(255,0,0) },			//��B����ɫ
	{ 1.0f, -1.0f, -0.577f, D3DCOLOR_XRGB(0,255,255) },			//��C��ǳ��
	{ 0.0f, -1.0f, 1.155f, D3DCOLOR_XRGB(255,0,255) }};			//��D���ۺ�
	WORD indices[] = { 0, 2, 1, 0, 3, 2, 0, 1, 3, 1, 2, 3 };	//��������
	
	m_pDevice->CreateVertexBuffer(
		sizeof(vertices), //�������ߴ�
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &m_pVB, NULL );
	
	//�Ѷ����������붥�㻺����
	void* pVertices;
	m_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 );
	memcpy( pVertices, vertices, sizeof(vertices) );
	m_pVB->Unlock();
	
	//��������������������ȡ�ӿ�LPDIRECT3DINDEXBUFFER9��ָ��
	m_pDevice->CreateIndexBuffer(
		sizeof(indices), //�������ߴ�
		0, D3DFMT_INDEX16, //ʹ��16 bit������ֵ
		D3DPOOL_DEFAULT, &m_pIB, NULL );
	
	//������ֵ��������������
	void *pIndices;
	m_pIB->Lock( 0, sizeof(indices), (void**)&pIndices, 0 );
	memcpy( pIndices, indices, sizeof(indices) );
	m_pIB->Unlock();
}

//����任
void CD3DWnd::SetupMatrices()
{
	float angle = m_nRotateY * D3DX_PI / 180; //����ת�ǻ���ɻ���
	D3DXMATRIX matWorld;
	
	//��������任����
	::D3DXMatrixRotationY( &matWorld, angle );
	
	//������任�������õ���Ⱦ����
	m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXVECTOR3 eye( 0.0f, 3.0f,-5.0f ); //�۲��
	D3DXVECTOR3 lookat( 0.0f, 0.0f, 0.0f ); //����Ŀ���
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f ); //�Ϸ���
	D3DXMATRIX matView;
	
	//�����ӽǱ任����
	::D3DXMatrixLookAtLH( &matView, &eye, &lookat, &up );
	
	//���ӽǱ任�������õ���Ⱦ����
	m_pDevice->SetTransform( D3DTS_VIEW, &matView );
	D3DXMATRIXA16 matProj;
	
	//����͸��ͶӰ�任����
	::D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
	
	//��ͶӰ�任�������õ���Ⱦ����
	m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

//��ʼ��
int CD3DWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	// TODO:  �ڴ������ר�õĴ�������
	InitD3D();
	InitGeometry();
	m_nRotateY = 0;

	SetTimer(1,40,NULL);
	//Render();
		return 0;
}

//ʱ����Ӧ�������ı���ת�Ƕ�
void CD3DWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	Render();
	m_nRotateY += 2;
	CWnd::OnTimer(nIDEvent);
}
