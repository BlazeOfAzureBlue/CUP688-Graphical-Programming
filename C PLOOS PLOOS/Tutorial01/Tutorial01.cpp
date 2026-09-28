// Tutorial01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>

HINSTANCE g_hInst = NULL; // stores the handle to the application instance
HWND g_hWnd = NULL; // stores the handle to the created window
const wchar_t* windowName = L"DirectX Hello World!"; //  the actual name for the window, L is used to inform the compiler that this is a wide character array 

// using global variables for this is bad practice, should be creating classes to encapsulate this data.

HRESULT InitWindow(HINSTANCE instanceHandle, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM); 

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

	RECT wr = { 0, 0, 640, 480 }; // adjusts the window dimensions so that the top window bar isn't taking pixels away from the app
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