// cloudWallpaper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <myincludes/shader.h>
#include <myincludes/camera.h>
#include <myincludes/model.h>
#include <myincludes/SeaVertices.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


// settings
const unsigned int APP_WIDTH = 200;
const unsigned int APP_HEIGHT = 120;
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const unsigned int headWindowWidth = 400;
const unsigned int headWindowHeight = 400;
const unsigned int FRAME_LIMIT = 45;

HWND workerw;
SeaVertices mySeaVertices;
bool flagRender = false;
bool flagThread = true;
HWND headWindow;
HINSTANCE hInst = NULL; //全局实例句柄
LRESULT CALLBACK WindProc(HWND, UINT, WPARAM, LPARAM);//主窗口过程
LRESULT CALLBACK BtnProc1(HWND, UINT, WPARAM, LPARAM); //按钮1窗口过程
LRESULT CALLBACK BtnProc2(HWND, UINT, WPARAM, LPARAM);//按钮2窗口过程

BOOL CALLBACK EnumWindowsFindDesktopWindow(HWND hwnd,LPARAM lParam) {
	//if this hwnd has the child window "SHELLDLL_DefView",then set workerw as the next window.
	HWND p = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
	if (p != NULL) {
		workerw = FindWindowEx(NULL, hwnd, "WorkerW", NULL);
	}
	return true;
}

LRESULT CALLBACK headWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		ValidateRect(hwnd, NULL);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
			break;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
int testThread();
int renderDesk();

int main(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	thread myThread(renderDesk);
	
	hInst = hInstance;
	TCHAR szAppName[] = TEXT("MyApp");
	char szBtnName1[] = "MyBtn1";
	char szBtnName2[] = "MyBtn2";

	//窗口创建四部曲之一：设计一个完整的窗口类
	WNDCLASS wndclass = { 0 };
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(COLOR_BACKGROUND);
	wndclass.hCursor = NULL;
	wndclass.hIcon = NULL;
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WindProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	//窗口创建四部曲之二：注册窗口类
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("RegisterClass wndclass failed"), TEXT("ERROR"), 0);
		return 0;
	}
	//窗口创建四部曲之三：创建窗口
	HWND hwnd = CreateWindow(szAppName, TEXT("MyWindow"), WS_OVERLAPPEDWINDOW,
		100, 100, APP_WIDTH, APP_HEIGHT, NULL, NULL, hInstance, 0);
	//窗口创建四部曲之四：窗口的移动、显示与更新
	//MoveWindow(hwnd, 250, 80, headWindowWidth, headWindowHeight, true);
	ShowWindow(hwnd, SW_SHOW);//显示窗口
	UpdateWindow(hwnd);

	//自定义按钮类
	WNDCLASS wndbtn1 = { 0 };
	wndbtn1.cbClsExtra = 0;
	wndbtn1.cbWndExtra = 0;  //这个是位图背景，需要自己添加位图资源
	//wndbtn1.hbrBackground = CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP)));
	wndbtn1.hbrBackground = CreateSolidBrush(RGB(187, 255, 255));
	wndbtn1.hCursor = NULL;
	wndbtn1.hIcon = NULL;
	wndbtn1.hInstance = hInstance;
	wndbtn1.lpfnWndProc = BtnProc1;
	wndbtn1.lpszClassName = szBtnName1;
	wndbtn1.lpszMenuName = NULL;
	wndbtn1.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndbtn1))
	{
		MessageBox(NULL, TEXT("RegisterClass wndbtn failed"), TEXT("ERROR"), 0);
		return 0;
	}

	//自绘按钮
	HWND hButton1 = CreateWindow(szBtnName1, TEXT("START"),
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hInstance, 0);

	ShowWindow(hButton1, SW_SHOW);
	UpdateWindow(hButton1);

	WNDCLASS wndbtn2 = { 0 };
	wndbtn2.cbClsExtra = 0;
	wndbtn2.cbWndExtra = 0;
	wndbtn2.hbrBackground = CreateSolidBrush(RGB(187, 255, 255));
	wndbtn2.hCursor = NULL;
	wndbtn2.hIcon = NULL;
	wndbtn2.hInstance = hInstance;
	wndbtn2.lpfnWndProc = BtnProc2;
	wndbtn2.lpszClassName = szBtnName2;
	wndbtn2.lpszMenuName = NULL;
	wndbtn2.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndbtn2))
	{
		MessageBox(NULL, TEXT("RegisterClass wndbtn failed"), TEXT("ERROR"), 0);
		return 0;
	}
	HWND hButton2 = CreateWindow(szBtnName2, TEXT("STOP"),
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hInstance, 0);
	ShowWindow(hButton2, SW_SHOW);
	UpdateWindow(hButton2);

	//消息类
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//窗口类注销
	UnregisterClass("MyWindow", wndclass.hInstance);

	flagThread = false;
	myThread.join();
	//int renderMsg=renderDesk();
	return 0;
}

int testThread()
{
	while (flagThread) {
		if (flagRender) {
			std::cout << "thread working" << endl;
		}
		Sleep(100);
	}
	return 0;
}

int renderDesk()
{

	//int a = mySeaVertices.test();
	//std::cout << a << endl;

	DWORD_PTR result = 0;
	float lastFrame = 0.0f;
	float deltFrame = 0.0f;

	//find th progman window, then send 0x052c message,which 
	//create a WorkerW behind icons(if already exist,do nothing)
	HWND hwnd_Progman = FindWindow("Progman", NULL);
	if (hwnd_Progman == NULL) {
		std::cout << "cannot get handle of progman" << std::endl;
	}
	else {
		std::cout << "handle of progman get" << std::endl;
	}
	SendMessageTimeout(hwnd_Progman, 0x052c, NULL, NULL, SMTO_NORMAL, 1000, &result);

	EnumWindows(EnumWindowsFindDesktopWindow, NULL);
	//enum all the windows to find workerw.
	//std::cout << workerw << std::endl;

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//creat a new window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//create ketCallback
	glfwSetKeyCallback(window, key_callback);

	// configure global opengl state
	// -----------------------------
	//glEnable(GL_DEPTH_TEST);

	glfwSetWindowPos(window, 0, 0);
	glfwMakeContextCurrent(window);
	HWND HWND_childWindow = GetActiveWindow();
	//replace the desktop window with child window
	SetParent(HWND_childWindow, workerw);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	float baseFrame = glfwGetTime();
	float currentFrame = glfwGetTime();
	float trueFrame = 0.0f;

	while (flagThread) {
		currentFrame = glfwGetTime();
		deltFrame = currentFrame - lastFrame;

		if (deltFrame > 1 / FRAME_LIMIT) {
			if (flagRender) {
				trueFrame = currentFrame - baseFrame;
				//std::cout << currentFrame << endl;
				glClearColor(0.1f * trueFrame, 0.15f * trueFrame, 0.15f * trueFrame, 1.0f);
				//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
				// -------------------------------------------------------------------------------
				glfwSwapBuffers(window);
				glfwPollEvents();
				lastFrame = currentFrame;
			}
			else {
				//deltFrame -= (1 / FRAME_LIMIT);
				lastFrame += deltFrame;
				baseFrame += deltFrame;
			}
		}

	}
	return 0;

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
		std::cout << "escape pressed" << endl;
}

LRESULT CALLBACK WindProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps = { 0 };
	HDC DC = NULL;
	switch (message)
	{
	case WM_CREATE:

		break;
	case WM_PAINT:
		DC = BeginPaint(hwnd, &ps);

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);

}

LRESULT CALLBACK BtnProc1(HWND hBtn1, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps = { 0 };
	HDC DC = NULL;
	switch (message)
	{
	case WM_CREATE:
	{
		MoveWindow(hBtn1, 0, 0, 100, 100, TRUE);
		//画椭圆区域，还可以画圆角，多边形等等，可以自行百度
		HRGN hRgn = CreateRoundRectRgn(10, 30, 80, 60, 20, 20);
		SetWindowRgn(hBtn1, hRgn, TRUE);
	}
	break;
	case WM_LBUTTONDOWN:   //测试是否成功响应消息
		MessageBox(NULL, TEXT("START"), TEXT("提示"), 0);
		flagRender = true;
		break;
	case WM_PAINT:
	{
		DC = BeginPaint(hBtn1, &ps);
		SetBkColor(DC, RGB(0, 255, 255)); //按钮背景颜色
		SetTextColor(DC, RGB(255, 0, 0)); //按钮上的文字颜色
		TextOut(DC, 30, 40, TEXT("START"),5);//按钮上的文字
		EndPaint(hBtn1, &ps);
	}
	break;
	default:
		break;
	}
	return DefWindowProc(hBtn1, message, wParam, lParam);

}

LRESULT CALLBACK BtnProc2(HWND hBtn2, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps = { 0 };
	HDC DC = NULL;
	switch (message)
	{
	case WM_CREATE:
	{
		MoveWindow(hBtn2, 100, 0, 100, 100, TRUE);
		//画椭圆区域，还可以画圆角，多边形等等，可以自行百度
		HRGN hRgn = CreateRoundRectRgn(10, 30, 80, 60, 20, 20);
		SetWindowRgn(hBtn2, hRgn, TRUE);
	}
	break;
	case WM_LBUTTONDOWN:
		MessageBox(NULL, TEXT("STOP"), TEXT("提示"), 0);
		flagRender = false;
		break;
	case WM_PAINT:
	{
		DC = BeginPaint(hBtn2, &ps);
		SetBkColor(DC, RGB(0, 255, 255)); //按钮背景颜色
		SetTextColor(DC, RGB(255, 0, 0)); //按钮上的文字颜色
		TextOut(DC, 30, 40, TEXT("STOP"), 4); //按钮上的文字
		EndPaint(hBtn2, &ps);
	}
	break;
	default:
		break;
	}
	return DefWindowProc(hBtn2, message, wParam, lParam);

}