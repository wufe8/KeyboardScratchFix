#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <windows.h>

using namespace std;

#define DEBUG

void press();
void release();
template<typename T>
void setBit(T& value, int bit, int set);

//HHOOK SetWindowsHookEx(int idHook, HOOKPPROC lpfn, HINSTANCE hMod, DWORD dwThreadId); // 钩子安装(钩子类型,钩子过程的指针, 应用程序实例的曲柄, 要安装的钩子线程id)
//LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam); //钩子过程(钩子标识码 决定下一步操作, 依赖nCode参数的内容, 依赖nCode参数的内容)
//UnhookWindowsHookEx(hhk: HHOOK {钩子曲柄}): BOOL; {True/False} //钩子卸载

//int main(int argc, char* argv[])
//{
	//cout << "hello world!\n";	
	//Sleep(1000);
	//cout << "typing TEST\n";	
	//keybd_event(84,0,0,0);
	//Sleep(1000);
	//cout << "release TEST\n";	
	//keybd_event(84,0,0,1);
	//system("pause");
	//return 0;
//}

//代码改自: https://www.cnblogs.com/LyShark/p/15019694.html

HHOOK keyboardHook = 0;		// 钩子句柄
int holdingState = 0; //0x01 按下A, 0x02 按下L, 0x04 先按下A, 0x08 先按下L
bool isClockwise = true;

LRESULT CALLBACK LowLevelKeyboardProc(
	int nCode,		// 规定钩子如何处理消息，小于 0 则直接 CallNextHookEx
	WPARAM wParam,	// 消息类型
	LPARAM lParam	// 指向某个结构体的指针，这里是 KBDLLHOOKSTRUCT（低级键盘输入事件）
	)
{
    KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT*)lParam;		// 包含低级键盘输入事件信息
	/*
	typedef struct tagKBDLLHOOKSTRUCT {
		DWORD     vkCode;		// 按键代号
		DWORD     scanCode;		// 硬件扫描代号，同 vkCode 也可以作为按键的代号。
		DWORD     flags;		// 事件类型，一般按键按下为 0 抬起为 128。
		DWORD     time;			// 消息时间戳
		ULONG_PTR dwExtraInfo;	// 消息附加信息，一般为 0。
	}KBDLLHOOKSTRUCT,*LPKBDLLHOOKSTRUCT,*PKBDLLHOOKSTRUCT;
	*/
    if(ks->flags == 0)
    {
		// 监控键盘
		switch(ks->vkCode)
		{
		case 0x41:
			//cout << "Detect input: " << "A" << endl;
			setBit(holdingState, 0x01, 1);
			if(holdingState & 0x08)
			{
				Sleep(1);
				release();
				isClockwise = !isClockwise;
				Sleep(1);
				press();
				//cout << "L pressed earlier, change state" << endl;
				setBit(holdingState, 0x08, 0);
				setBit(holdingState, 0x04, 1);
			}
			else
			{
				setBit(holdingState, 0x04, 1);
				press();
			}
			return 1;		// 使按键失效
			break;
		case 0x4C:
			//cout << "Detect input: " << "L" << endl;
			setBit(holdingState, 0x02, 1);
			if(holdingState & 0x04)
			{
				Sleep(1);
				release();
				isClockwise = !isClockwise;
				Sleep(1);
				press();
				//cout << "A pressed earlier, change state" << endl;
				setBit(holdingState, 0x04, 0);
				setBit(holdingState, 0x08, 1);
			}
			else
			{
				setBit(holdingState, 0x08, 1);
				press();
			}
			return 1;		// 使按键失效
			break;
		//case 0x52: //debug line
			//cout << "--------" << endl;
			//break;
    	}
	}
    if(ks->flags == 128 || ks->flags == 129)
    {
		// 监控键盘
		switch(ks->vkCode)
		{
		case 0x41:
			//cout << "Release input: " << "A" << endl;
			setBit(holdingState, 0x01, 0);
			setBit(holdingState, 0x04, 0);
			release();
			isClockwise = !isClockwise; //注意由于bool实际存储仍然为1Byte 即bool true为0x00000001 因此不可用位取反
			break;
		case 0x4C:
			//cout << "Release input: " << "L" << endl;
			setBit(holdingState, 0x02, 0);
			setBit(holdingState, 0x08, 0);
			release();
			isClockwise = !isClockwise; //注意由于bool实际存储仍然为1Byte 即bool true为0x00000001 因此不可用位取反
			break;
        //return 1;		// 使按键失效
    	}
	}
	// 将消息传递给钩子链中的下一个钩子
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(int argc, char* argv[])
{
	// 读取按键配置
	fstream cfgFile;
	cfgFile.open("bind.ini");
	cfgFile.close();
	
	// 安装钩子
	keyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,			// 钩子类型，WH_KEYBOARD_LL 为键盘钩子
		LowLevelKeyboardProc,	// 指向钩子函数的指针
		GetModuleHandleA(NULL),	// Dll 句柄
		0					
		);
    if (keyboardHook == 0){cout << "挂钩键盘失败" << endl; return -1;}

    //不可漏掉消息处理，不然程序会卡死
    MSG msg;
    while(1)
    {
		// 如果消息队列中有消息
        if (PeekMessageA(
			&msg,		// MSG 接收这个消息
			NULL,		// 检测消息的窗口句柄，NULL: 检索当前线程所有窗口消息
			0,		// 检查消息范围中第一个消息的值，NULL: 检查所有消息（必须和下面的同时为NULL）
			0,		// 检查消息范围中最后一个消息的值，NULL: 检查所有消息（必须和上面的同时为NULL）
			PM_REMOVE	// 处理消息的方式，PM_REMOVE: 处理后将消息从队列中删除
			))
		{
				// 把按键消息传递给字符消息
				TranslateMessage(&msg);

				// 将消息分派给窗口程序
				DispatchMessageW(&msg);
		}
        else
            Sleep(0);    //避免CPU全负载运行
    }
	// 删除钩子
    UnhookWindowsHookEx(keyboardHook);

	return 0;
}

void press()
{
	if(isClockwise == true)
	{
		keybd_event(0x4C,0,0,0);
	}
	else
	{
		keybd_event(0x4C,0,0,0);
	}
#ifdef DEBUG
	cout << "hit" << endl;
#endif
}

void release()
{
	keybd_event(0x4C,0,KEYEVENTF_KEYUP,0);
	//keybd_event(0x4C,0,KEYEVENTF_KEYUP,0);
	//if(isClockwise == true)
	//{
		//keybd_event(0x41,0,KEYEVENTF_KEYUP,0);
	//}
	//else
	//{
		//keybd_event(0x4C,0,KEYEVENTF_KEYUP,0);
	//}
}

template<typename T>
void setBit(T& value, int bit, int set)
{
	if(set == 1) //设value的第bit位为1
	{
		value = value | bit;
	}
	else //设value的第bit位为0
	{
		value = value & ~bit;
	}
#ifdef DEBUG
	//cout << value << endl;
#endif
}
