#include <windows.h>
#include <iostream>

LRESULT CALLBACK InputProc(
	int nCode,		// 规定钩子如何处理消息，小于 0 则直接 CallNextHookEx
	WPARAM wParam,	// 消息类型
	LPARAM lParam	// 指向某个结构体的指针，这里是 KBDLLHOOKSTRUCT（低级键盘输入事件）
)
{
	KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT*)lParam;		// 包含低级键盘输入事件信息
	if(ks->flags == 0)
	{
		// 监控键盘
		switch(ks->vkCode)
		{
			case int('0'): std::cout << "0"; break;
			case int('1'): std::cout << "1"; break;
			case int('2'): std::cout << "2"; break;
			case int('3'): std::cout << "3"; break;
			case int('4'): std::cout << "4"; break;
			case int('5'): std::cout << "5"; break;
			case int('6'): std::cout << "6"; break;
			case int('7'): std::cout << "7"; break;
			case int('8'): std::cout << "8"; break;
			case int('9'): std::cout << "9"; break;
			case int('\r'): std::cout << "\n"; break;
			case int('\b'): std::cout << "\b \b"; break;
		}
	}
	// 将消息传递给钩子链中的下一个钩子
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
