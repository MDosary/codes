// d3d9hook.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "d3d9hook.h"

HHOOK m_hook = NULL;

//���ӻص�����
LRESULT CALLBACK CBTProc(int nCode,
						 WPARAM wParam,
						 LPARAM lParam)
{
	return CallNextHookEx(m_hook, nCode, wParam, lParam);
}



D3D9HOOK_API void SetHook()
{
	HMODULE hmod = GetModuleHandle(L"d3d9hook.dll");

	m_hook = SetWindowsHookEx(WH_CBT, CBTProc, hmod, 0);

	OutputDebugString(L"begin hook...\n");
}

D3D9HOOK_API void UnHook()
{
	if (m_hook)
	{
		UnhookWindowsHookEx(m_hook);
	}
	OutputDebugString(L"unhook...\n");
	
}

//��ⴰ���Ƿ�Ϊ��Ϸ����
D3D9HOOK_API void CheckWindow()
{
	HWND hWnd = FindWindow(L"DEVIL_MAY_CRY_4",NULL);
	//HWND hWnd = FindWindow(NULL,L"D3DWND");
	
	if (hWnd)
	{
		OutputDebugString(L"find  window\n");
	}
	DWORD dwpid;
	GetWindowThreadProcessId(hWnd, &dwpid);
	if (dwpid == GetCurrentProcessId())
	{
		//hook successfully
		OutputDebugString(L"find process \n");
		HookDrawIndexedPrimitive();
	}
	return;
}

//ԭ����
DWORD g_dwjmpto = 0;
__declspec(naked) HRESULT WINAPI Old_DrawIndexedPrimitive(
	LPDIRECT3DDEVICE9 m_pDevice,
	D3DPRIMITIVETYPE Type,
	INT              BaseVertexIndex,
	UINT             MinIndex,
	UINT             NumVertices,
	UINT             StartIndex,
	UINT             PrimitiveCount
	)
{
	_asm
	{
		mov edi,edi
		push ebp
		mov ebp, esp
		mov eax, g_dwjmpto
		jmp eax
	}
}

//���캯��
HRESULT WINAPI New_DrawIndexedPrimitive(
	LPDIRECT3DDEVICE9 m_pDevice,
	D3DPRIMITIVETYPE Type,
	INT              BaseVertexIndex,
	UINT             MinIndex,
	UINT             NumVertices,
	UINT             StartIndex,
	UINT             PrimitiveCount
	)
{
	//Sleep(1);
	//͸�ӹ���
	/*
	HRESULT result = S_OK;
	IDirect3DVertexBuffer9* pStreamData = NULL;
	UINT iOffsetInByte, iStride;
	//�õ���Ⱦ��Դ
	if (m_pDevice->GetStreamSource(0, &pStreamData, &iOffsetInByte, &iStride) == D3D_OK)
	{
		pStreamData->Release();
	}
	//����ģ���ж�
	if (iStride == ����)
	{
		result = Old_DrawIndexedPrimitive(m_pDevice, Type,BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	}
	//OutputDebugString(L"d3d hook \n");
	//return Old_DrawIndexedPrimitive(m_pDevice, Type,BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	*/
	
	//����z�Ỻ��
	//m_pDevice->SetRenderState( D3DRS_ZENABLE,false );
	
	
	return Old_DrawIndexedPrimitive(m_pDevice, Type,BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	//return S_OK;
}

ULONG_PTR g_uladdr ;

//hook drawIndexedPrimitive
BOOL HookDrawIndexedPrimitive()
{
	g_uladdr = GetDrawIndexedPrimitiveAddress();
	g_dwjmpto = g_uladdr + 5;
	DWORD dwoldprotect;
	if(VirtualProtect((LPVOID)g_uladdr, 5, PAGE_EXECUTE_READWRITE, &dwoldprotect))
	{
		DWORD dwjmp = (DWORD)New_DrawIndexedPrimitive - g_uladdr - 5;
		_asm
		{
			mov eax, g_uladdr	
			mov byte ptr[eax], 0xe9
			add eax, 1
			mov ebx, dwjmp
			mov dword ptr[eax], ebx
		}
		
	}
	VirtualProtect((LPVOID)g_uladdr, 5, dwoldprotect, &dwoldprotect);
	
	return TRUE;
}

//find addr
ULONG_PTR GetDrawIndexedPrimitiveAddress()
{
	HANDLE handle = GetModuleHandle(L"d3d9.dll");
	if (handle == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(L"get d3d9.dll failed\n");
		return NULL;
	}
	
	return (ULONG_PTR)handle + 0x2B6B1;

}


