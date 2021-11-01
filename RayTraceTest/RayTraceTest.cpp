#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cassert>
#include <RayTrace/RayTracer.h>

static const int WIDTH = 800;
static const int HEIGHT = 600;
static unsigned int OUTPUT_IMAGE[WIDTH * HEIGHT] = { 0 };

using namespace RayTracer;

void onStart()
{
	//renderOutput = RenderOutput::Normal;

	camera.setFovYAndAspect(45.0f, (float)WIDTH / (float)HEIGHT);
	camera.setPositon(0, 0.5, 2);
	camera.lookAt(vec3<f32>(0, 0.5, 0));

	bvhScene.loadFormObj("../Assets/bunny.obj");
	//bvhScene.loadFormVox("../Assets/chr_sword.vox");
	bvhScene.build();
	//bvhScene.mode = BVHAccelMode::None;
	//RayTracer::samplesPerPixel = 16;

	//renderProgressCallback = [](float progress)
	//{
	//	printf("[Render Progress] %.1f %%\n", progress);
	//};
}

void onRender()
{
	//clear white
	u32* pixel = (u32*)OUTPUT_IMAGE;
	for (int i = 0; i < WIDTH * HEIGHT; ++i)
		*pixel++ = 0xffffff;

	render(WIDTH, HEIGHT, OUTPUT_IMAGE);
}

void onKeyDown(HWND hWnd, WPARAM wParam, bool charcode)
{
	if (!charcode)
	{
		switch (wParam)
		{
		case VK_SPACE:
		{
			printf("RePaint Window\n");
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_LEFT:
		{
			camera.rotate(0.5f, 0.0f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_RIGHT:
		{
			camera.rotate(-0.5f, 0.0f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_UP:
		{
			camera.rotate(0.0f, 0.5f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_DOWN:
		{
			camera.rotate(0.0f, -0.5f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		}
	}
	else
	{
		switch (wParam)
		{
		case '1':
		{
			renderOutput = RenderOutput::Beaut;
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case '2':
		{
			renderOutput = RenderOutput::Albedo;
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case '3':
		{
			renderOutput = RenderOutput::Normal;
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case '4':
		{
			renderOutput = RenderOutput::Barycentric;
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 'w':
		{
			camera.moveFoward(0.5f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 's':
		{
			camera.moveFoward(-0.5f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 'q':
		{
			camera.moveUp(-0.5f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 'e':
		{
			camera.moveUp(0.5f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 'a':
		{
			camera.moveRight(-0.5f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 'd':
		{
			camera.moveRight(0.5f);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		printf("ClientRect: %d, %d, %d, %d\n", rect.left, rect.top, rect.right, rect.bottom);

		onStart();
		break;
	}
	case WM_KEYDOWN:
	{
		onKeyDown(hWnd, wParam, false);
		break;
	}
	case WM_CHAR:
	{
		onKeyDown(hWnd, wParam, true);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		onRender();

		BITMAPINFO bitmap_info;
		bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
		bitmap_info.bmiHeader.biWidth = WIDTH;
		bitmap_info.bmiHeader.biHeight = HEIGHT;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;
		StretchDIBits(hdc, 0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT,
			OUTPUT_IMAGE, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv)
{
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(nullptr);

	WNDCLASSW wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = L"RayTraceTestWndClass";
	RegisterClassW(&wc);

	DWORD style = WS_OVERLAPPEDWINDOW & (~WS_SIZEBOX) & (~WS_MAXIMIZEBOX);
	RECT rect = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rect, style, false);
	HWND hWnd = CreateWindowW(wc.lpszClassName, L"", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, 10);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
