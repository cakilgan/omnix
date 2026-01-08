#include "window_module.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

namespace {
    MSG msg{};
}

ox::result window_module::init() {
	Logger = dynamic_cast<logger*>(this->Managers[0]);
	Controller = dynamic_cast<controller*>(this->Managers[1]);
	OX_ASSERT(Logger);
	OX_ASSERT(Controller);

    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    const wchar_t* CLASS_NAME = L"OmnixWindow";

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Omnix (main)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1280, 720,
        nullptr, nullptr,
        hInstance,
        nullptr
    );
    ShowWindow(hwnd, SW_SHOW);
	return ox::ok;
};
ox::result window_module::update(double dt) {
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return 0;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	return ox::ok;
};
ox::result window_module::shutdown() {
	return ox::ok;
};