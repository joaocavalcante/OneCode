/****************************** 模块头 ******************************\
模块名:  CppReceiveWM_COPYDATA.cpp
项目名:      CppReceiveWM_COPYDATA
版权 (c) Microsoft Corporation.

基于windows 消息 WM_COPYDATA 进程间通信(IPC) 是一种在本地机器上windows应用程序交换数据机制。

接受程序必须是一个windows应用程序。数据被传递必须不包含指针或者不能被应用程序接受的指向对象的引用。

当发送WM_COPYDATA消息时，引用数据不能被发送进程别的线程改变。 接受应用程序应该只考虑只读数据。

如果接受应用程序想要在SendMessage返回之后进入数据， 它必须拷贝数据到本地缓存。

这个代码例子示范了从发送程序(CppSendWM_COPYDATA)通过处理消息WM_COPYDATA 接受
一个客户数据结构(MY_STRUCT) 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Manifest Dependencies
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

// 使可见风格有效
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma endregion


// 携带数据的结构
struct MY_STRUCT
{
    int Number;

    wchar_t Message[256];
};


//
//   函数: OnCopyData(HWND, HWND, PCOPYDATASTRUCT)
//
//   目的: 处理WM_COPYDATA消息
//
//   参数:
//   * hWnd - 当前窗口句柄
//   * hwndFrom - 传数据的窗口句柄 
//   * pcds - 指向COPYDATASTRUCT 结构体的指针，这个结构体包含要传的数据
//
BOOL OnCopyData(HWND hWnd, HWND hwndFrom, PCOPYDATASTRUCT pcds)
{
    MY_STRUCT myStruct;

    // 如果大小匹配
    if (pcds->cbData == sizeof(myStruct))
    {
        // 接受程序应该考虑数据只读 (pcds->lpData) 
        // 在消息处理过程中pcds 参数是有效的
        // 接受程序不应该释放被pcds引用的内存
		// 如果接受程序在SendMessage返回前必须访问数据，它必须拷贝数据到本地缓存
        memcpy_s(&myStruct, sizeof(myStruct), pcds->lpData, pcds->cbData);

        // 在窗口中显示MY_STRUCT值
        SetDlgItemInt(hWnd, IDC_NUMBER_STATIC, myStruct.Number, TRUE);
        SetDlgItemText(hWnd, IDC_MESSAGE_STATIC, myStruct.Message);
    }

    return TRUE;
}


//
//   函数: OnClose(HWND)
//
//   目的 处理WM_CLOSE消息
//
void OnClose(HWND hWnd)
{
    EndDialog(hWnd, 0);
}


//
//  函数: DialogProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  处理主对话框消息
//
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		// 在OnCopyData中处理WM_COPYDATA消息
        HANDLE_MSG (hWnd, WM_COPYDATA, OnCopyData);

        // 在OnClose中处理WM_CLOSE消息
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;
    }
    return 0;
}


//
//  函数: wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
//
//  目的:  应用程序入口点
//
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow)
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
}