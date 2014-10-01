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
using namespace std;

#include "Window.h"
#include "KeyEnum.h"
#include "PlayerView.h"

//Includes for graphics engine testing
#include "GraphicsEngine.h"
#include "ShaderProgram.h"
#include "MasterDirectoryResourceSource.h"
#include "ResourceCache.h"
#include "ResourceHandle.h"
#include "StringResourceProcessor.h"
#include "Scene.h"
#include "CubeModel.h"
#include "SceneNode.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

KeyEnum translations[256] = { KeyEnum::Invalid };

LRESULT CALLBACK staticHandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
Window* appWindow;

struct Window::Pimpl
{
	HWND windowHandle;
	HDC deviceContext;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	WNDCLASSEX windowClass;
};

Window::Window(string title, string className, bool fullScreen, int width, int height, int bits, PlayerView* playerView) : fullScreen(fullScreen), width(width), height(height), bits(bits)
{
	//Store the player view to forward messages to.
	this->playerView = playerView;
	//Application only supports single window. If there's a need for multiwindow display, this can be upgraded to support it.
	if (appWindow != nullptr)
		throw exception("Attempt to create second window!");
	else
		appWindow = this;

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

	show();

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = pimpl->windowHandle;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

	pimpl->deviceContext = GetDC(pimpl->windowHandle);

	MasterDirectoryResourceSource mdrs(".");

	mdrs.open();

	ResourceCache resourceCache(1024 * 1024 * 100, &mdrs);
	resourceCache.registerProcessor(shared_ptr<IResourceProcessor>(new StringResourceProcessor));

	shared_ptr<ResourceHandle> vertShaderBase = resourceCache.gethandle("ColorShader.vert");
	shared_ptr<ResourceHandle> fragShaderBase = resourceCache.gethandle("ColorShader.frag");

	graphicsEngine = new GraphicsEngine(pimpl->deviceContext, vertShaderBase, fragShaderBase);
	graphicsEngine->setViewport(width, height);

	shared_ptr<ResourceHandle> vertShader = resourceCache.gethandle("ColorShader.vert");
	shared_ptr<ResourceHandle> fragShader = resourceCache.gethandle("ColorShader.frag");

	glm::dmat4 worldToCamera(1.0);
	worldToCamera = glm::gtc::matrix_transform::translate(worldToCamera, glm::dvec3(0, -3, 0));
	graphicsEngine->setWorldToCamera(worldToCamera);

	ShaderProgram *shader = new ShaderProgram(graphicsEngine, vertShader, fragShader);

	{
		shared_ptr<CubeModel> cubeModel(new CubeModel(graphicsEngine));

		Scene *scene = new Scene(graphicsEngine);

		glm::dmat4 modelToWorld(1.0); 
		modelToWorld = glm::gtc::matrix_transform::translate(glm::dmat4(1.0), glm::dvec3(0, -0.5, 0));
		modelToWorld = glm::gtc::matrix_transform::scale(modelToWorld, glm::dvec3(200.0, 1, 200.0));
		scene->addNode(shared_ptr<SceneNode>(new SceneNode(cubeModel, modelToWorld)));
		modelToWorld = glm::gtc::matrix_transform::translate(glm::dmat4(1.0), glm::dvec3(-1.5, 1, 2));
		scene->addNode(shared_ptr<SceneNode>(new SceneNode(cubeModel, modelToWorld)));
		modelToWorld = glm::gtc::matrix_transform::translate(glm::dmat4(1.0), glm::dvec3(-1.5, 1, 10));
		scene->addNode(shared_ptr<SceneNode>(new SceneNode(cubeModel, modelToWorld)));

		graphicsEngine->clearScreen();
		scene->draw();
		graphicsEngine->swapBuffers();

		delete scene;
	}

	//
	//modelToWorld = ;
	//modelToWorld = modelToWorld, );
	//myModel->draw(modelToWorld);
	//modelToWorld = glm::gtc::matrix_transform::translate(glm::dmat4(1.0), glm::dvec3(-1.5, 1, 2));
	//myModel->draw(modelToWorld);
	//modelToWorld = glm::gtc::matrix_transform::translate(glm::dmat4(1.0), glm::dvec3(-1.5, 1, 10));
	//myModel->draw(modelToWorld);

	delete shader;

	graphicsEngine->relinquish();

}

Window::~Window()
{
	if (fullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	appWindow = nullptr;

	delete pimpl;
	delete graphicsEngine;
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
			nPixelFormat = ChoosePixelFormat(deviceContext, &(pimpl->pixelFormatDescriptor));

			/*      Set the pixel format to the device context*/
			SetPixelFormat(deviceContext, nPixelFormat, &(pimpl->pixelFormatDescriptor));

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
	ShowWindow(pimpl->windowHandle, SW_SHOW);      //display window
	UpdateWindow(pimpl->windowHandle);             //update window
}
