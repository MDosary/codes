// D3DWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "d3dDemo.h"
#include "D3DWnd.h"

//定义FVF的顶点结构
struct CUSTOMVERTEX
{
	float x, y, z; //顶点坐标
	DWORD color; //顶点颜色
};
//定义FVF用到的数据项：坐标颜色
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



// CD3DWnd 消息处理程序

//初始化
void CD3DWnd::InitD3D()
{
	//创建Direct3D对象，并获取接口IDirect3D9的指针，
	//我们将通过该指针操作Direct3D对象。
	m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	::ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE; //创建窗口模式的Direct3D程序
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; //生成16 bit的Z缓存
	
	//调用方法IDirect3D9::CreateDevice创建设备对象，并获取
	//接口IDirect3DDevice9的指针，我们将通过该指针操作设备对象
	m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,					//使用缺省的显卡
		D3DDEVTYPE_HAL,						//指定设备类型为HAL
		m_hWnd,								//Direct3D窗口的句柄
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,//软件顶点处理
		&d3dpp, &m_pDevice);

	//设置渲染
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

//渲染部分
void CD3DWnd::Render()
{
	//用指定颜色清除后备缓存区
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
		//绑定索引缓存区

			m_pDevice->SetIndices( m_pIB );
		//从索引缓存区绘制图元，参数1为图元格式，参数4为顶点数，参数6为三角形数
		m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 4 );
		m_pDevice->EndScene();
		
	}
	m_pDevice->Present(NULL,NULL,NULL,NULL);
}


void CD3DWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//Render();
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	InitD3D();
}


void CD3DWnd::OnDestroy()
{
	CWnd::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	CleanUp();
}

//设置顶点缓存和索引
void CD3DWnd::InitGeometry()
{
	//三角形实体的数学模型
	CUSTOMVERTEX vertices[] =									//FVF顶点数据
	{{ 0.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,255,0) },				//点A，绿色
	{ -1.0f, -1.0f, -0.577f, D3DCOLOR_XRGB(255,0,0) },			//点B，红色
	{ 1.0f, -1.0f, -0.577f, D3DCOLOR_XRGB(0,255,255) },			//点C，浅蓝
	{ 0.0f, -1.0f, 1.155f, D3DCOLOR_XRGB(255,0,255) }};			//点D，粉红
	WORD indices[] = { 0, 2, 1, 0, 3, 2, 0, 1, 3, 1, 2, 3 };	//索引序列
	
	m_pDevice->CreateVertexBuffer(
		sizeof(vertices), //缓存区尺寸
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &m_pVB, NULL );
	
	//把顶点数据填入顶点缓存区
	void* pVertices;
	m_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 );
	memcpy( pVertices, vertices, sizeof(vertices) );
	m_pVB->Unlock();
	
	//创建索引缓存区，并获取接口LPDIRECT3DINDEXBUFFER9的指针
	m_pDevice->CreateIndexBuffer(
		sizeof(indices), //缓存区尺寸
		0, D3DFMT_INDEX16, //使用16 bit的索引值
		D3DPOOL_DEFAULT, &m_pIB, NULL );
	
	//把索引值填入索引缓存区
	void *pIndices;
	m_pIB->Lock( 0, sizeof(indices), (void**)&pIndices, 0 );
	memcpy( pIndices, indices, sizeof(indices) );
	m_pIB->Unlock();
}

//坐标变换
void CD3DWnd::SetupMatrices()
{
	float angle = m_nRotateY * D3DX_PI / 180; //把旋转角换算成弧度
	D3DXMATRIX matWorld;
	
	//计算世界变换矩阵
	::D3DXMatrixRotationY( &matWorld, angle );
	
	//把世界变换矩阵设置到渲染环境
	m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXVECTOR3 eye( 0.0f, 3.0f,-5.0f ); //观察点
	D3DXVECTOR3 lookat( 0.0f, 0.0f, 0.0f ); //视线目标点
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f ); //上方向
	D3DXMATRIX matView;
	
	//计算视角变换矩阵
	::D3DXMatrixLookAtLH( &matView, &eye, &lookat, &up );
	
	//把视角变换矩阵设置到渲染环境
	m_pDevice->SetTransform( D3DTS_VIEW, &matView );
	D3DXMATRIXA16 matProj;
	
	//计算透视投影变换矩阵
	::D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
	
	//把投影变换矩阵设置到渲染环境
	m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

//初始化
int CD3DWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	// TODO:  在此添加您专用的创建代码
	InitD3D();
	InitGeometry();
	m_nRotateY = 0;

	SetTimer(1,40,NULL);
	//Render();
		return 0;
}

//时间相应函数，改变旋转角度
void CD3DWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Render();
	m_nRotateY += 2;
	CWnd::OnTimer(nIDEvent);
}
