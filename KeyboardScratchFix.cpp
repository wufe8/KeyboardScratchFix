#include <iostream>
#include <fstream>
#include <ctime>


#pragma comment(lib, "winmm.lib") //mingw编译时务必添加'-lwinmm'参数
#include <windows.h>
#include "MMSystem.h"

using namespace std;

//#define DEBUG

void press();
void release();
template<typename T>
void setBit(T& value, int bit, int set);

HHOOK keyboardHook = 0; // 钩子句柄
int holdingState = 0; //0x01 按下A, 0x02 按下L, 0x04 先按下A, 0x08 先按下L
bool isClockwise = true;
string bindKey = "ALLL"; //前两为输入 后两为输出
time_t timerOld = time(0);
time_t timerNew = timerOld;

//HHOOK SetWindowsHookEx(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId); // 钩子安装(钩子类型,钩子过程的指针, 应用程序实例的曲柄, 要安装的钩子线程id)
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

//代码改自: https://blog.51cto.com/wangningyu/3248216
void MSleep(long lTime) //微秒级延时 单位1微秒(1000微秒=1毫秒)
{
	LARGE_INTEGER litmp; 
	LONGLONG QPart1,QPart2;
	double dfMinus, dfFreq, dfTim, dfSpec; 
	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;
	dfSpec = 0.000001*lTime;
		
	do
	{
		QueryPerformanceCounter(&litmp);
		QPart2 = litmp.QuadPart;
		dfMinus = (double)(QPart2-QPart1);
		dfTim = dfMinus / dfFreq;
	}while(dfTim<dfSpec);
}


//代码改自: https://www.cnblogs.com/LyShark/p/15019694.html
LRESULT CALLBACK LowLevelKeyboardProc(
	int nCode,		// 规定钩子如何处理消息，小于 0 则直接 CallNextHookEx
	WPARAM wParam,	// 消息类型
	LPARAM lParam	// 指向某个结构体的指针，这里是 KBDLLHOOKSTRUCT（低级键盘输入事件）
)
{
	if(nCode < 0)
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT*)lParam;		// 包含低级键盘输入事件信息
	/*
	typedef struct tagKBDLLHOOKSTRUCT {
		DWORD	 vkCode;		// 按键代号
		DWORD	 scanCode;		// 硬件扫描代号，同 vkCode 也可以作为按键的代号。
		DWORD	 flags;		// 事件类型，一般按键按下为 0 抬起为 128。
		DWORD	 time;			// 消息时间戳
		ULONG_PTR dwExtraInfo;	// 消息附加信息，一般为 0。
	}KBDLLHOOKSTRUCT,*LPKBDLLHOOKSTRUCT,*PKBDLLHOOKSTRUCT;
	*/
	if(ks->flags == 0)
	{
		// 监控键盘
		if(ks->vkCode == bindKey[0])
		{
			timerOld = timerNew;
			//cout << "Detect input: " << "A" << "\n";
			setBit(holdingState, 0x01, 1);
			if(holdingState & 0x08)
			{
				Sleep(1); //余量防止游戏不识别
				release();
				isClockwise = !isClockwise; //同时按下则立即切换
				Sleep(1);
				press();
				cout << "[V] duplicate state, " << "'" << bindKey[1] << "'" << " earlier" << "\n";
				setBit(holdingState, 0x08, 0);
				setBit(holdingState, 0x04, 1);
			}
			else
			{
				setBit(holdingState, 0x04, 1);
				press();
			}
			return 1;		// 使按键失效
		}
		else if(ks->vkCode == bindKey[1])
		{
			timerOld = timerNew;
			//cout << "Detect input: " << "L" << "\n";
			setBit(holdingState, 0x02, 1);
			if(holdingState & 0x04)
			{
				Sleep(1);
				release();
				isClockwise = !isClockwise;
				Sleep(1);
				press();
				cout << "[V] duplicate state, " << "'" << bindKey[0] << "'" << " earlier" << "\n";
				setBit(holdingState, 0x04, 0);
				setBit(holdingState, 0x08, 1);
			}
			else
			{
				setBit(holdingState, 0x08, 1);
				press();
			}
			return 1;		// 使按键失效
		//case 0x52: //debug line
			//cout << "--------" << "\n";
			//break;
		}
	}
	if(ks->flags == 128 || ks->flags == 129)
	{
		// 监控键盘
		if(ks->vkCode == bindKey[0])
		{
			setBit(holdingState, 0x01, 0);
			setBit(holdingState, 0x04, 0);
			if(not holdingState & 0x01)
			{
				release();
#ifdef DEBUG
				cout << "[V] release" << "'" << bindKey[0] << "'" << "\n";
#endif
				isClockwise = !isClockwise; //注意由于bool实际存储仍然为1Byte 即bool true为0x00000001 因此不可用位取反
			}
		}
		else if(ks->vkCode == bindKey[1])
		{
			setBit(holdingState, 0x02, 0);
			setBit(holdingState, 0x08, 0);
			if(not holdingState & 0x01)
			{
				release();
#ifdef DEBUG
				cout << "[V] release" << "'" << bindKey[1] << "'" << "\n";
#endif
				isClockwise = !isClockwise;
			}
		//return 1;		// 使按键失效
		}
	}
	// 将消息传递给钩子链中的下一个钩子
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

/*
LRESULT CALLBACK InputProc(
	int nCode,		// 规定钩子如何处理消息，小于 0 则直接 CallNextHookEx
	WPARAM wParam,	// 消息类型
	LPARAM lParam	// 指向某个结构体的指针，这里是 KBDLLHOOKSTRUCT（低级键盘输入事件）
);

void CALLBACK TimerProcA(HWND hWnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime)
{
	cout << "Timer: "<< nTimerid << endl;
	if(nTimerid == 1)
	{
		cout << "test timer" << endl;
	}
}
*/

int main(int argc, char* argv[])
{
	cout << "Another: Wufe8; Date: 2023-01-09; Version: 0.5" << "\n"
	<< "Use two key to do as one single key, or trigger alternately without conflict" << "\n"
	<< "Some app which run as admin. must run this program as admin first. Otherwise the hook cannot catch and do keyboard event" << "\n"
	<< "To change bind key, edit 'bind.ini', first two char means input and third fourth char means output(both can be same). If invalid, try capital letters" << "\n"
	<< "--------------" << "\n";
	// 读取按键配置
	fstream cfgFile;
	cfgFile.open("bind.ini");
	if (cfgFile.is_open())
	{
		cfgFile >> bindKey;
		cout << "[I]'" << bindKey[0] << "', '" << bindKey[1] << "'" << " represent '" << bindKey[2] << "', '" << bindKey[3] << "'" << "\n";
		
	}
	else
	{
		cout << "[W] failed to open bind.ini, using default binding: 'A', 'L' represent 'L'" << "\n"; 
	}
	cfgFile.close();

	/*
	cout << "Enter thread id (default: 0 global hook) ";
	int threadId = 0; //default
	//cin >> threadId;
	char* c;
	cin.get(*c);
	if(*c != '\n')
	{
		cin.putback(*c);
		cin >> threadId;
		cin.clear();
	}
	delete c;
	cout << "[D] threadId = " << GetCurrentThreadId() << "\n";
	*/

	// 安装钩子
	 //HMODULE hDll = LoadLibrary(TEXT("hookRun.dll")); //TEXT()兼容Unicode
	 //if(hDll == NULL)
	 //{
		 //cout << "[E] Fail to load inject dll" << "\n";
		 //system("pause");
		 //return -1;
	 //}
	 HMODULE hDll = GetModuleHandleA(NULL);
	 //HMODULE hDll = NULL;

	//FARPROC Proc = (FARPROC)GetProcAddress(hDll, "InputProc");	// 指向钩子函数的指针
	//cout << "&InputProc = " << Proc << endl;
	keyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,			// 钩子类型，WH_KEYBOARD_LL 为键盘钩子
		(HOOKPROC)LowLevelKeyboardProc,	// 指向钩子函数的指针 1为单键输出 其余双键输出
		hDll,	// Dll 句柄
		//threadId); //要安装钩子的线程
		//GetCurrentThreadId());
		0);
	if (keyboardHook == 0)
	{
		cout << "[E] Fail to set keyboard hook (" << keyboardHook << ")" << "\n";
		system("pause");
		return -1;
	}

	/* 搞不懂 万策尽
	char* WindowTitle = new char[128];
	string NewWindowTitle = new char[128];
	GetConsoleTitleA(WindowTitle, 128);
	//cout << WindowTitle << endl;
	NewWindowTitle = WindowTitle;
	NewWindowTitle += " (" + to_string(GetCurrentThreadId()) + ")";
	//cout << NewWindowTitle << endl;
	char* NewWindowTitleC = (char*)NewWindowTitle.c_str();
	HWND WindowHwnd = 0;
	do //用while会直接结束程序 原因不明 但删除while工作正常
	{
		SetConsoleTitle(NewWindowTitleC);
		Sleep(40);
		WindowHwnd = FindWindow(NULL, NewWindowTitleC);
	} while(WindowHwnd == 0);
	cout << "[I] Program hwnd = " << WindowHwnd << "\n";
	UINT_PTR timer1 = SetTimer(WindowHwnd, 1, 1000, TimerProcA); //返回0
	cout << "timer1 set: " << timer1 << endl ;
	delete [] WindowTitle;
	*/
	if(timeBeginPeriod(1) == TIMERR_NOERROR) //申请更改计时器精度
		cout << "[I] Successfully set the sleep period to 1ms" << "\n";
	else
		cout << "[W] Fail set the sleep period. Latency >= 15ms possible" << "\n";
	cout << "[I] Successfully initialized (" << keyboardHook << ")" << "\n";
	//不可漏掉消息处理，不然程序会卡死
	MSG msg;
	while(1)
	{
		// 如果消息队列中有消息
		//GetMessage(&msg, NULL, 0, 0);

		if(PeekMessageA(
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
		{
			if(difftime(timerNew, timerOld) > 5) //一定时间无操作判断为脱钩 重新注册
			{
				timerOld = timerNew;
				cout << "[I] 5s idle, try to reset hook...";
				UnhookWindowsHookEx(keyboardHook);
				keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL,	(HOOKPROC)LowLevelKeyboardProc, hDll, 0);
				if (keyboardHook == 0)
				{
					cout << "\n" << "[E] Fail to reset hook (" << keyboardHook << ")" << "\n";
					system("pause");
					return -1;
				}
				cout << "ok" << "\n";
			}
			timerNew = time(0);
			Sleep(1);	//避免CPU全负载运行 约15ms精度
			//MSleep(1000); //占满单核 1us精度
		}
	}
	// 删除钩子
	UnhookWindowsHookEx(keyboardHook);
	timeEndPeriod(1);
	cin.clear();
	cin.ignore();
	system("pause");
	return 0;
}

void press()
{
	if(isClockwise == true)
	{
		keybd_event(bindKey[2],0,0,0);
#ifdef DEBUG
		cout << "[V] hit0" << "\n";
#endif
	}
	else
	{
		keybd_event(bindKey[3],0,0,0);
#ifdef DEBUG
		cout << "[V] hit1" << "\n";
#endif
	}
}

void release()
{
	//keybd_event(bindKey[2],0,KEYEVENTF_KEYUP,0);
	//keybd_event(bindKey[3],0,KEYEVENTF_KEYUP,0);
	if(isClockwise == true)
	{
		keybd_event(bindKey[2],0,KEYEVENTF_KEYUP,0);
	}
	else
	{
		keybd_event(bindKey[3],0,KEYEVENTF_KEYUP,0);
	}
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
	cout << "[V]" << value << "\n";
#endif
}

