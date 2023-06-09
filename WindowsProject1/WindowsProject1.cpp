﻿// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WindowsProject1.h"
#include "Canvas.h"
#include <math.h>
#include "Image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前程序ID
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HDC hDC;  // 显示器直接获取每个像素的来源
HDC hMem; // hDC的备用
GT::Canvas* _canvas = nullptr;
GT::Image* _image = NULL;
GT::Image* _bkImage = NULL;
GT::Image* _zoomImage = NULL;

// 窗口的基本性质
HWND hWnd;
int wWidth = 1200;
int wHeight = 800;

float speed = 0.01f;
float angle = 0;

// 全局处理函数
void Render();

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);  // 注册一个窗体类型，方便设置需要的属性
BOOL                InitInstance(HINSTANCE, int); // 程序的初始化端口
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM); // 事件循环的回调
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 主循环
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    // 构建画布
    void* buffer;

    hDC = GetDC(hWnd);  // 显示器直接获取每个像素的来源
    hMem = CreateCompatibleDC(hDC); // 为hDC进行预计算，计算好后的值全部给hDC，避免卡顿

    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = wWidth;
    bmpInfo.bmiHeader.biHeight = wHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32; // RGBA
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biSizeImage = 0;
    bmpInfo.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biClrImportant = 0;

    // 实际创建位图(画布)，buffer指定到内存上，相当于可直接操控的一块内存画布
    HBITMAP hBmp = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&buffer, 0, 0);
    SelectObject(hMem, hBmp); // 将hMem和hBmp绑定起来

    memset(buffer, 0, wWidth * wHeight * 4); // 清空buffer为0

    // canvas和buffer的概念应该是一样的
    _canvas = new GT::Canvas(wWidth, wHeight, buffer);
    _bkImage = GT::Image::readFromFile("resource/bk.jpg");

    MSG msg;

    // 主消息循环:
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Render();
    }

    return (int) msg.wParam;
}

void Render()
{
    _canvas->clear();
    
    GT::Point ptArray[] =
    {
        {0, 0, 0,  GT::RGBA(255, 0 , 0), GT::floatV2(0, 0)},
        {300, 0, 0, GT::RGBA(0, 255 , 0), GT::floatV2(1.0, 0)},
        {300, 300, 0, GT::RGBA(0, 0 , 255), GT::floatV2(1.0, 1.0)},

        {300, 0, 0,  GT::RGBA(255, 0 , 0), GT::floatV2(0, 0)},
        {300, 300, 0, GT::RGBA(0, 255 , 0), GT::floatV2(1.0, 0)},
        {300, 0, -500, GT::RGBA(0, 0 , 255), GT::floatV2(1.0, 1.0)}
    };

    for (int i = 0; i < 6; i++)
    {
        glm::vec4 ptv4(ptArray[i].m_x, ptArray[i].m_y, ptArray[i].m_z, 1);
        
        // 构建旋转矩阵
        glm::mat4 rMat(1.0f);
        rMat = glm::rotate(rMat, glm::radians(angle), glm::vec3(0, 1.0f, 0));
        rMat = glm::rotate(rMat, glm::radians(50.0f), glm::vec3(1, 0.0f, 0));
        // 构建平移矩阵
        glm::mat4 tMat(1.0f);
        tMat = glm::translate(tMat, glm::vec3(-300, 0, 0));

        // 构建观察矩阵
        glm::mat4 vMat(1.0f);
        vMat = glm::lookAt(glm::vec3(0, 0, 1000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // 构建投影矩阵
        glm::mat4 pMat(1.0f);
        pMat = glm::perspective(glm::radians(60.0f), (float)wWidth / (float)wHeight, 1.0f, 1000.0f);

        ptv4 = pMat * vMat * rMat * tMat * ptv4;

        ptArray[i].m_x = (ptv4.x / ptv4.w + 1.0) * (float)wWidth / 2.0;
        ptArray[i].m_y = (ptv4.y / ptv4.w + 1.0) * (float)wHeight / 2.0;
        ptArray[i].m_z = ptv4.z / ptv4.w;
    }

    angle += 0.2;

    _canvas->gtVertexPointer(2, GT::GT_FlOAT, sizeof(GT::Point), (GT::byte*)ptArray);
    _canvas->gtColorPointer(1, GT::GT_FlOAT, sizeof(GT::Point), (GT::byte*)&ptArray[0].m_color);
    _canvas->gtTexCoordPointer(1, GT::GT_FlOAT, sizeof(GT::Point), (GT::byte*)&ptArray[0].m_uv);
    
    _canvas->enableTexture(true);
    _canvas->setTextureType(GT::Image::TX_CLAMP_TO_EDGE);
    _canvas->bindTexture(_bkImage);

    _canvas->gtDrawArray(GT::GT_TRIANGLE, 0, 6);
    // 画到设备上，hMem相当于缓冲区
    BitBlt(hDC, 0, 0, wWidth, wHeight, hMem, 0, 0, SRCCOPY);
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex; // 窗口的结构体

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   // 生成窗口实例
   hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP || WS_OVERLAPPED,
      CW_USEDEFAULT, 0, wWidth, wHeight, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow); // 展示
   UpdateWindow(hWnd); // 刷新

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
