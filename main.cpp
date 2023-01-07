#include <iostream>
#include <vector>
#include <string>

#include <windows.h>

using namespace std;

//HHOOK SetWindowsHookEx(int idHook, HOOKPPROC lpfn, HINSTANCE hMod, DWORD dwThreadId); // 钩子安装(钩子类型,钩子过程的指针, 应用程序实例的曲柄, 要安装的钩子线程id)
//LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam); //钩子过程(钩子标识码 决定下一步操作, 依赖nCode参数的内容, 依赖nCode参数的内容)
//UnhookWindowsHookEx(hhk: HHOOK {钩子曲柄}): BOOL; {True/False} //钩子卸载

int main(int argc, char* argv[])
{
	cout << "hello world!\n";	
	Sleep(1000);
	cout << "typing TEST\n";	
	keybd_event(84,0,0,0);
	Sleep(1000);
	cout << "release TEST\n";	
	keybd_event(84,0,0,1);
	system("pause");
	return 0;
}
