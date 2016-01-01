// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� D3D9HOOK_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// D3D9HOOK_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef D3D9HOOK_EXPORTS
#define D3D9HOOK_API __declspec(dllexport)
#else
#define D3D9HOOK_API __declspec(dllimport)
#endif

// �����Ǵ� d3d9hook.dll ������


extern D3D9HOOK_API int nd3d9hook;

D3D9HOOK_API void SetHook();

D3D9HOOK_API void UnHook();

D3D9HOOK_API void CheckWindow();

BOOL HookDrawIndexedPrimitive();

//get func address
ULONG_PTR GetDrawIndexedPrimitiveAddress();