// Name:
// Window.cpp
// Description:
// Implementation file for Window class
// Notes:
// OS-Aware
// The Window class uses the pimpl idiom to avoid polution of windows header files into the rest of the project.

#include "CustomMemory.h"

#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include "Window.h"
#include "KeyEnum.h"
#include "PlayerView.h"
#include "Format.h"

//Includes for graphics engine testing
#include "Globals.h"
#include "GraphicsEngine.h"
#include "ShaderProgram.h"
#include "MasterDirectoryResourceSource.h"
#include "ResourceCache.h"
#include "ResourceHandle.h"
#include "StringResourceProcessor.h"
#include "Scene.h"
#include "CubeModel.h"
#include "SceneNode.h"
#include "Logger.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD     ((USHORT) 0x06)
#endif

KeyEnum translations[256] = { KeyEnum::Invalid };

LRESULT CALLBACK staticHandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
Window* appWindow;

struct Window::Pimpl
{
	//Windows specific data, kept out of class definition
	HWND windowHandle;
	HDC deviceContext;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	WNDCLASSEX windowClass;
};

Window::Window(string title, string className, bool fullScreen, int width, int height, int bits) : fullScreen(fullScreen), width(width), height(height), bits(bits), userInterface(nullptr)
{
	//Initialize key translations
	translations[16] = KeyEnum::Leftshift;
	translations[65] = KeyEnum::A;
	translations[68] = KeyEnum::D;
	translations[69] = KeyEnum::E;
	translations[81] = KeyEnum::Q;
	translations[83] = KeyEnum::S;
	translations[87] = KeyEnum::W;

	//All keys are up.
	for (int I = 0; I < (unsigned short)KeyEnum::TotalKeys; I++)
	{
		keyStates[I] = false;
	}

	//Application only supports single window. If there's a need for multiwindow display, this can be upgraded to support it.
	if (appWindow != nullptr)
		throw exception("Attempt to create second window!");
	else
		appWindow = this;

	//Setup Pimpl
	pimpl = new Pimpl;

	//Prepare pixelFormatDescriptor
	pimpl->pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pimpl->pixelFormatDescriptor.nVersion = 1;
	pimpl->pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pimpl->pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pimpl->pixelFormatDescriptor.cColorBits = 32;
	pimpl->pixelFormatDescriptor.cRedBits = 0;
	pimpl->pixelFormatDescriptor.cRedShift = 0;
	pimpl->pixelFormatDescriptor.cGreenBits = 0;
	pimpl->pixelFormatDescriptor.cGreenShift = 0;
	pimpl->pixelFormatDescriptor.cBlueBits = 0;
	pimpl->pixelFormatDescriptor.cBlueShift = 0;
	pimpl->pixelFormatDescriptor.cAlphaBits = 0;
	pimpl->pixelFormatDescriptor.cAlphaShift = 0;
	pimpl->pixelFormatDescriptor.cAccumBits = 0;
	pimpl->pixelFormatDescriptor.cAccumRedBits = 0;
	pimpl->pixelFormatDescriptor.cAccumGreenBits = 0;
	pimpl->pixelFormatDescriptor.cAccumBlueBits = 0;
	pimpl->pixelFormatDescriptor.cAccumAlphaBits = 0;
	pimpl->pixelFormatDescriptor.cDepthBits = 32;
	pimpl->pixelFormatDescriptor.cStencilBits = 0;
	pimpl->pixelFormatDescriptor.cAuxBuffers = 0;
	pimpl->pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
	pimpl->pixelFormatDescriptor.bReserved = 0;
	pimpl->pixelFormatDescriptor.dwLayerMask = 0;
	pimpl->pixelFormatDescriptor.dwVisibleMask = 0;
	pimpl->pixelFormatDescriptor.dwDamageMask = 0;

	//Prepare windowClass
	pimpl->windowClass.cbSize = sizeof(WNDCLASSEX);
	pimpl->windowClass.style = CS_HREDRAW | CS_VREDRAW;
	pimpl->windowClass.lpfnWndProc = staticHandleMessage;
	pimpl->windowClass.cbClsExtra = 0;
	pimpl->windowClass.cbWndExtra = 0;
	pimpl->windowClass.hInstance = GetModuleHandle(nullptr);
	pimpl->windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	pimpl->windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	pimpl->windowClass.hbrBackground = NULL;
	pimpl->windowClass.lpszMenuName = NULL;
	pimpl->windowClass.lpszClassName = className.c_str();
	pimpl->windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	//Register window class
	if (!RegisterClassEx(&pimpl->windowClass))
	{
		throw exception("Failed to register Window class");
	}

	//Get handle
	HINSTANCE temp = GetModuleHandle(nullptr);

	//Prep windowRect
	RECT    windowRect;
	windowRect.left = 0;               //set left value to 0
	windowRect.right = width;  //set right value to requested width
	windowRect.top = 0;                //set top value to 0
	windowRect.bottom = height;//set bottom value to requested height

	/*      Check if fullscreen is on*/
	if (fullScreen)
	{
		DEVMODE dmScreenSettings; //Holds data for screen settings change
		//Clear the structure to all 0
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		//Populate required elements
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;   //screen width
		dmScreenSettings.dmPelsHeight = height; //screen height
		dmScreenSettings.dmBitsPerPel = bits;   //bits per pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Attempt settings change
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) !=
			DISP_CHANGE_SUCCESSFUL)
		{
			/*      Setting display mode failed, switch to windowed*/
			throw exception("Failed to activate full-screen mode");
		}
	}

	//Fill out window style information based on whether the window is in fullscreen or not.
	DWORD   dwExStyle;              //window extended style
	DWORD   dwStyle;                //window style
	if (fullScreen)
	{
		dwExStyle = WS_EX_APPWINDOW;    //window extended style
		dwStyle = WS_POPUP;             //windows style
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //window extended style
		dwStyle = WS_OVERLAPPEDWINDOW;                  //windows style
	}

	//Adjusts window size for style information, so ultimately based on whether we're in fullscreen.
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	/*      Class registerd, so now create our window*/
	//This will trigger a WM_CREATE message for the windows message handler routine, which in turn creates the graphics engine.
	pimpl->windowHandle = CreateWindowEx(NULL, className.c_str(),  //class name
		title.c_str(),       //app name
		dwStyle |
		WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS,
		0, 0,                         //x and y coords
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,//width, height
		NULL,                 //handle to parent
		NULL,                 //handle to menu
		GetModuleHandle(nullptr),    //application instance
		NULL);                //no xtra params

	/*      Check if window creation failed (hwnd = null ?)*/
	if (!pimpl->windowHandle)
	{
		throw exception("Window Creation Failed");
	}

	//Display the window
	show();

	//Register us to receive raw mouse and keyboard data.
	RAWINPUTDEVICE Rid[2];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = pimpl->windowHandle;
	Rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	Rid[1].dwFlags = RIDEV_INPUTSINK;
	Rid[1].hwndTarget = pimpl->windowHandle;
	RegisterRawInputDevices(Rid, 2, sizeof(Rid[0]));

	//Set the graphicsEngine's viewport.
	graphicsEngine->setViewport(width, height);
}

Window::~Window()
{
	//Reset windows screen
	if (fullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	//Relinquish and delete graphics engine
	graphicsEngine->relinquish();

	delete graphicsEngine;

	//Tell windows to destroy our window
	DestroyWindow(pimpl->windowHandle);

	//Set the application window handle to null
	appWindow = nullptr;

	//Delete Pimpl
	delete pimpl;
}

//Windows message handler, forwards messages onto the window object.
LRESULT CALLBACK staticHandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (appWindow != nullptr)
	{
		return appWindow->handleMessage(hwnd, message, wParam, lParam);
	}
	else
	{
		globalLogger->eWriteLog(formatToString("Message received after window destruction: \\{0}", message), LogLevel::Warning, { "General" });
		return (DefWindowProc(hwnd, message, wParam, lParam));
	}
}

int Window::handleMessage(void* windowHandle, unsigned int message, unsigned int wParam, long lParam)
{
	try
	{
		int width, height;  // For resizing the window when necessary.

		if (message == WM_CREATE)
		{
			pimpl->deviceContext = GetDC((HWND)windowHandle);
			/*      Pixel format index*/
			int nPixelFormat;

			/*      Choose best matching format*/
			nPixelFormat = ChoosePixelFormat(pimpl->deviceContext, &(pimpl->pixelFormatDescriptor));

			/*      Set the pixel format to the device context*/
			SetPixelFormat(pimpl->deviceContext, nPixelFormat, &(pimpl->pixelFormatDescriptor));

			/*      Create rendering context and make it current
			*/
			graphicsEngine = new GraphicsEngine(pimpl->deviceContext);

			return 0;
		}
		//Window closing
		else if (message == WM_CLOSE)  //window is closing
		{
			if (userInterface)
				userInterface->windowClose();

			return 0;
		}
		//Window resizing
		else if (message == WM_SIZE)
		{
			/*      Retrieve width and height*/
			height = HIWORD(lParam);
			width = LOWORD(lParam);

			/*      Don't want a divide by 0*/
			if (height == 0)
			{
				height = 1;
			}

			/*      Reset the viewport to new dimensions*/
			graphicsEngine->setViewport(width, height);

			return 0;
		}
		else if (message == WM_ACTIVATEAPP)
		{
			return 0;
		}
		//If the left(main) mouse button is pressed
		else if (message == WM_INPUT && GetFocus() == windowHandle)
		{
			UINT dwSize;
			RAWINPUT* raw;

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
				nullptr, &dwSize, sizeof(RAWINPUTHEADER));

			raw = (RAWINPUT *)new char[dwSize];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
				raw, &dwSize, sizeof(RAWINPUTHEADER));

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				if (userInterface)
				{
					userInterface->mouseMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN)
						userInterface->mouseDown(1);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN)
						userInterface->mouseDown(2);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN)
						userInterface->mouseDown(3);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
						userInterface->mouseDown(4);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
						userInterface->mouseDown(5);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP)
						userInterface->mouseUp(1);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP)
						userInterface->mouseUp(2);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP)
						userInterface->mouseUp(3);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
						userInterface->mouseUp(4);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
						userInterface->mouseUp(5);
				}
			}
			else if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				if (raw->data.keyboard.Flags == RI_KEY_MAKE && !keyStates[(unsigned int)translations[raw->data.keyboard.VKey]])
				{
					//KeyDown event
					if (userInterface)
						userInterface->keyDown(translations[raw->data.keyboard.VKey]);
					keyStates[(unsigned int)translations[raw->data.keyboard.VKey]] = true;
				}
				else if(raw->data.keyboard.Flags == RI_KEY_BREAK && keyStates[(unsigned int)translations[raw->data.keyboard.VKey]])
				{
					//KeyUp event
					if (userInterface)
						userInterface->keyUp(translations[raw->data.keyboard.VKey]);
					keyStates[(unsigned int)translations[raw->data.keyboard.VKey]] = false;
				}
				
			}
			delete (char*)raw;
		}
		else
		{
		}
	}
	catch (exception e)
	{
		globalLogger->eWriteLog(formatToString("ERROR: Error occured in Windows Message Handler. \\{0}", e.what()), LogLevel::Warning, { "Graphics" });
	}
	catch (...)
	{
		globalLogger->eWriteLog("ERROR: Error occured in Windows Message Handler. Unknown Error.", LogLevel::Warning, { "Graphics" });
	}

	//Apply any other processing windows will do for messages
	return (DefWindowProc((HWND)windowHandle, message, wParam, lParam));
}

void Window::show()
{
	ShowWindow(pimpl->windowHandle, SW_SHOW);      //display window
	UpdateWindow(pimpl->windowHandle);             //update window
}

GraphicsEngine* Window::getGraphicsEngine()
{
	return graphicsEngine;
}

void Window::setUserInterface(UserInterface *userInterface)
{
	this->userInterface = userInterface;
}