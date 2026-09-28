// Graphics Programming CUP688.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <d3d11.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#pragma comment (lib, "d3d11.lib")

HINSTANCE g_hInst = NULL; // stores the handle to the application instance
HWND g_hWnd = NULL; // stores the handle to the created window
const wchar_t* windowName = L"DirectX Hello World!"; //  the actual name for the window, L is used to inform the compiler that this is a wide character array 

IDXGISwapChain* g_swapchain = NULL; // the pointer to the swap chain interface - this is a chain of screen buffers that we draw to and "flip" to display the image on the display with
ID3D11Device* g_dev = NULL; // The pointer to our Direct3D device interface - A pointer to the graphics card driver, this object is used to create and allocate resources on the adapter
ID3D11DeviceContext* g_devcon = NULL; // The pointer to our Direct3D device context - Pointer to a device context, used to control the settings and states of the device and issue rendering commands


// using global variables for this is bad practice, should be creating classes to encapsulate this data.

HRESULT InitWindow(HINSTANCE instanceHandle, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitD3D(HWND hWnd);
void CleanD3D();

#include <iostream>

// program entry point 

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
		MessageBox(NULL, L"Failed to create window", L"Critical Error!", MB_ICONERROR | MB_OK);
	}
	if (FAILED(InitD3D(g_hWnd)))
	{
		MessageBox(NULL, L"Failed to create swapchain and device", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	// Used to hold windows event messages
	MSG msg;

	// Enter the main loop
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate certain messages into correct formats
			TranslateMessage(&msg);

			// Send the message to the WindowProc function
			DispatchMessage(&msg);

			// Break out of the loop if a quit message is detected
			if (msg.message == WM_QUIT)
				break;
		}
		else {
			// rah
		}
	}

	CleanD3D();
}
void CleanD3D()
{
	if (g_swapchain) g_swapchain->Release();
	if (g_dev) g_dev->Release();
	if (g_devcon) g_devcon->Release();
	// Typical rule of thumb - cleanup should be done in reverse order of initialisation just in case there are dependencies between objects,
	// as we created the objects all at once, no need to worry on this

}
HRESULT InitD3D(HWND hWnd)
{
	// Create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};
	// Fill the swap chain description struct
	scd.BufferCount = 1; // One back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32-bit colour
	scd.BufferDesc.Width = SCREEN_WIDTH; // Set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT; // Set the back buffer height
	scd.BufferDesc.RefreshRate.Numerator = 60; // 60 FPS
	scd.BufferDesc.RefreshRate.Denominator = 1; // Divides the numerator, so 60/1 = 60
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Intended swapchain use
	scd.OutputWindow = hWnd; // Which window to use
	scd.SampleDesc.Count = 1; // Number of samples for anti-aliasing
	scd.Windowed = TRUE; // Windowed/fullscreen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Allow full-screen switching

	HRESULT hr;

	hr = D3D11CreateDeviceAndSwapChain(NULL, // Use default graphics adaptor
		D3D_DRIVER_TYPE_HARDWARE, // Use hardware acceleration, can also use software or WARP renderers
		NULL, // This is used for software driver types, as none to be placed here, NULL
		D3D11_CREATE_DEVICE_DEBUG, // Flags can be OR'd together, enabling debug for better warnings and errors
		NULL, // Direct3D feature levels - as using D3D11.0 / older, putting NULL
		NULL, // Size of array passed to above member, as we're using NULL above, using NULL here
		D3D11_SDK_VERSION, // Always set to D3D11_SDK_VERSION
		&scd, // Pointer to our swap chain description
		&g_swapchain, // Pointer to our swap chain COM object
		&g_dev, // Pointer to our device
		NULL, // Out param, will be set to chosen feature level
		&g_devcon); // Pointer to our immediate device context
	
	if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT InitWindow(HINSTANCE instanceHandle, int nCmdShow)
{
	g_hInst = instanceHandle; // stores the app handle / app memory location

	WNDCLASSEX wc = {}; // "= {}" sets all the values to 0, fill in the structure as needed as below

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc; // Window procedure function, handles window creation or calls DefWindowProc
	wc.hInstance = instanceHandle; // gives app handle
	wc.lpszClassName = L"WindowClass1"; // Windows will store data for our window class under this name
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; // background colour for the win32 app, tthough not needed for Direct3D apps
	if (!RegisterClassEx(&wc))
	{
		return E_FAIL; // returns the fail code if the class doesn't register properly 
	}

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; // adjusts the window dimensions so that the top window bar isn't taking pixels away from the app
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWnd = CreateWindowEx(NULL,
		L"WindowClass1", // Name of the window class
		windowName, // Title of the window
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, // Window style with no reizing or maxmimising, WS_OVERLAPPEDWINDOW allows it however
		100, // x-position of the window
		100, // y-position of the window
		wr.right - wr.left, // Width of the window
		wr.bottom - wr.top, // Height of the window
		NULL, // If it had a parent window it'd go here, otherwise NULL
		NULL, // If it had any menus it'd go here, otherwise NULL
		instanceHandle, // Application Handle
		NULL); // If it used multiple windows they'd go here, otherwise NULL
	
	if (g_hWnd == NULL) return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow); // Display the window on screen

	return S_OK;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		/// This is the message that is sent when the user closes the window
	case WM_DESTROY:
	{
		// Send a quit message to the app
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(g_hWnd); // Destroying the window is not the same as closing the app, destroying the window will post a WM_DESTROY which with the above handling actually closes app
			break;
		}
	default:
	{
		// Let windows handle anything else with default handling
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
}