# KeyboardScratchFix
use two key to do as one single key

Tested to work on OSU! and ______

## Instructions
1. Open KeyboardScratchFix.exe
2. Bind output key in the game what you want and play it
---
1. 启动KeyboardScratchFix.exe
2. 在游戏中绑定输出键 然后按输入的按键就可以了

## Precautions
1. Some app which run as admin. must run this program as admin first. Otherwise
2. To change bind key, edit 'bind.ini' if not exist, create one. first two char means input and third chopen bind.ini.
3. **Although it only uses the keyboard reading and writing of windows native api, it has no any effect on the game, but I'm not sure if it can be detected**
4. After testing, _____ may re-register the hook when switching interfaces or screens, causing the program to fail. Therefore, the function of automatically re-registering the hook with no operation for 5 seconds has been added. It cannot be disable temporarily, but there is no visible performance impact.(at least on my computer)
---
1. 需要管理员身份运行的应用程序 必须以管理员身份运行此程序 否则钩子抓不到键盘事件
2. 关于修改按键: 创建'bind.ini' 里面前两个字母表示输入 第三个字母表示输出
3. **尽管只使用了windows原生api的键盘读写 对游戏没有任何影响 但我不确定能否被检测到**
4. 经测试 _____ 在切换界面或屏幕时可能重新注册钩子 导致程序失效 故添加了5秒无操作自动重新注册钩子的功能 暂时无法关闭 但(至少在我的电脑里)没有可见性能影响
