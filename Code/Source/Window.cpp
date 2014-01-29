// Name:
// Window.cpp
// Description:
// Implementation file for Window class
// Notes:
// OS-Aware
// The Window class uses a variant of the pimpl idiom.
// The struct HData(Hidden Data) is the same length as the struct PData(Private Data).
// hData is reinterpret cast into a pData to allow the application to keep data hidden without requiring additional memory allocation, memory deallocation, or pointer dereferencing.

#include <Windows.h>
#include <string>
#include <iostream>
using namespace std;

#include "Window.h"
#include "KeyEnum.h"
#include "PlayerView.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

KeyEnum translations[256] = { KeyEnum::Invalid };

LRESULT CALLBACK staticHandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
Window* appWindow;

//If this structure is modified, you must modify the size of HData in Window.h to match the size of this structure.
//Static asserts in Window constructor can help determine size if you aren't able to determine correct size.
struct PData
{
	HWND windowHandle;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	WNDCLASSEX windowClass;
};

//Macro to assist in casting of hData into PData.
#define pData (*reinterpret_cast<PData*>(&hData))

Window::Window(string title, string className, bool fullScreen, int width, int height, int bits, PlayerView* playerView) : fullScreen(fullScreen), width(width), height(height), bits(bits)
{
	//Make sure that PData and HData are the same size. No memory corruption, thank you.
	static_assert(sizeof(HData) >= sizeof(PData), "HData size to small!");
	static_assert(sizeof(HData) <= sizeof(PData), "HData size to large!");
	//Store the player view to forward messages to.
	this->playerView = playerView;
	//Application only supports single window. If there's a need for multiwindow display, this can be upgraded to support it.
	if (appWindow != nullptr)
		throw exception("Attempt to create second window!");
	else
		appWindow = this;

	//Prepare pixelFormatDescriptor
	pData.pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pData.pixelFormatDescriptor.nVersion = 1;
	pData.pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pData.pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pData.pixelFormatDescriptor.cColorBits = 32;
	pData.pixelFormatDescriptor.cRedBits = 0;
	pData.pixelFormatDescriptor.cRedShift = 0;
	pData.pixelFormatDescriptor.cGreenBits = 0;
	pData.pixelFormatDescriptor.cGreenShift = 0;
	pData.pixelFormatDescriptor.cBlueBits = 0;
	pData.pixelFormatDescriptor.cBlueShift = 0;
	pData.pixelFormatDescriptor.cAlphaBits = 0;
	pData.pixelFormatDescriptor.cAlphaShift = 0;
	pData.pixelFormatDescriptor.cAccumBits = 0;
	pData.pixelFormatDescriptor.cAccumRedBits = 0;
	pData.pixelFormatDescriptor.cAccumGreenBits = 0;
	pData.pixelFormatDescriptor.cAccumBlueBits = 0;
	pData.pixelFormatDescriptor.cAccumAlphaBits = 0;
	pData.pixelFormatDescriptor.cDepthBits = 32;
	pData.pixelFormatDescriptor.cStencilBits = 0;
	pData.pixelFormatDescriptor.cAuxBuffers = 0;
	pData.pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
	pData.pixelFormatDescriptor.bReserved = 0;
	pData.pixelFormatDescriptor.dwLayerMask = 0;
	pData.pixelFormatDescriptor.dwVisibleMask = 0;
	pData.pixelFormatDescriptor.dwDamageMask = 0;

	//Prepare windowClass
	pData.windowClass.cbSize = sizeof(WNDCLASSEX);
	pData.windowClass.style = CS_HREDRAW | CS_VREDRAW;
	pData.windowClass.lpfnWndProc = staticHandleMessage;
	pData.windowClass.cbClsExtra = 0;
	pData.windowClass.cbWndExtra = 0;
	pData.windowClass.hInstance = GetModuleHandle(nullptr);
	pData.windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	pData.windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	pData.windowClass.hbrBackground = NULL;
	pData.windowClass.lpszMenuName = NULL;
	pData.windowClass.lpszClassName = className.c_str();
	pData.windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	//Register window class
	if (!RegisterClassEx(&pData.windowClass))
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
	pData.windowHandle = CreateWindowEx(NULL, className.c_str(),  //class name
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
	if (!pData.windowHandle)
	{
		throw exception("Window Creation Failed");
	}

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = pData.windowHandle;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}

Window::~Window()
{
	if (fullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	appWindow = nullptr;
}

LRESULT CALLBACK staticHandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (appWindow != nullptr)
	{
		return appWindow->handleMessage(hwnd, message, wParam, lParam);
	}
	else
	{
		//TODO: Log error here. It's possible that we might be receiving messages after window destruction. If this is the case, we need to figure out what to do with them.
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
			HDC deviceContext = GetDC((HWND)windowHandle);
			/*      Pixel format index*/
			int nPixelFormat;

			/*      Choose best matching format*/
			nPixelFormat = ChoosePixelFormat(deviceContext, &(pData.pixelFormatDescriptor));

			/*      Set the pixel format to the device context*/
			SetPixelFormat(deviceContext, nPixelFormat, &(pData.pixelFormatDescriptor));

			/*      Create rendering context and make it current
			*/
			//graphicsEngine = new GraphicsEngine(deviceContext);

			return 0;
		}
		//Window closing
		else if (message == WM_CLOSE)  //window is closing
		{
			playerView->windowClosed();

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
			//graphicsEngine->setViewport(width, height);

			return 0;
		}
		else if (message == WM_ACTIVATEAPP)
		{
			return 0;
		}
		//If the left(main) mouse button is pressed
		else if (message == WM_LBUTTONDOWN)
			playerView->mouseClick(1);
		//If the right(secondary) mouse button is pressed
		else if (message == WM_RBUTTONDOWN)
			playerView->mouseClick(2);
		//If the middle(tertiary) mouse button is pressed
		else if (message == WM_MBUTTONDOWN)
			playerView->mouseClick(3);
		else if (message == WM_INPUT)
		{
			UINT dwSize = 40;
			static BYTE lpb[40];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
				lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				playerView->mouseMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
			}
			else if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				if (raw->data.keyboard.Flags == RI_KEY_MAKE)
				{
					//KeyDown event
					playerView->keyDown(translations[raw->data.keyboard.VKey]);
				}
				else
				{
					//KeyUp event
					playerView->keyUp(translations[raw->data.keyboard.VKey]);
				}
				
			}
		}
		else
		{
		}
	}
	catch (exception e)
	{
		//*getLog(LogEnum::ERR) << "ERROR: Error occured in Windows Message Handler.\n" << e.what() << endl;
	}
	catch (...)
	{
		//*getLog(LogEnum::ERR) << "ERROR: Error occured in Windows Message Handler. Unknown Error." << endl;
	}

	//Apply any other processing windows will do for messages
	return (DefWindowProc((HWND)windowHandle, message, wParam, lParam));
}

void Window::show()
{
	ShowWindow(pData.windowHandle, SW_SHOW);      //display window
	UpdateWindow(pData.windowHandle);             //update window
}
