#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Canvas.h"
#include "RenderGraph.h"
#include "Camera.h"

static const int WIDTH = 800;
static const int HEIGHT = 600;

const char* SHADER_SRC = R"(
	[[block]] struct Uniforms {
		mvpMatrix : mat4x4<f32>;
	};
	[[binding(0), group(0)]] var<uniform> uniforms : Uniforms;
	
	struct V2F {
		[[builtin(position)]] pos : vec4<f32>;
		[[location(0)]] color : vec3<f32>;
	};
	
	[[stage(vertex)]]
	fn vs_main([[location(0)]] position : vec2<f32>, 
		[[location(1)]] color : vec3<f32>) -> V2F {
		var o : V2F;
		o.pos = uniforms.mvpMatrix * vec4<f32>(position, 0.0, 1.0);
		o.color = color;
		return o;
	}
	
	[[stage(fragment)]]
	fn fs_main([[location(0)]] color : vec3<f32>) -> [[location(0)]] vec4<f32> {
		return vec4<f32>(color, 1.0);
	}
)";

Canvas canvas;

void canvasTestFill()
{
	canvas.beginPath();
	canvas.transform(1, 0, 0, 1, 100, 50);
	canvas.fillStyle = 0xFF0000FF;
	canvas.rect(0, 0, 200, 100);
	canvas.fill();
	canvas.closePath();

	canvas.beginPath();
	canvas.transform(1, 0, 0, 1, 100, 200);
	canvas.fillStyle = 0x00FF00FF;
	canvas.ellipse(100, 50, 100, 50);
	canvas.fill();
	canvas.closePath();

	canvas.beginPath();
	canvas.transform(1, 0, 0, 1, 500, 100);
	canvas.fillStyle = 0x0000FFFF;
	canvas.arc(0, 0, 50, radians(30.f), radians(260.f));
	//canvas.circle(100, 100, 50);
	canvas.fill();
	canvas.closePath();

	//顺时针凹多边形
	canvas.beginPath();
	canvas.transform(1, 0, 0, 1, 500, 200);
	canvas.fillStyle = 0xFFFF00FF;
	canvas.moveTo(100, 0);
	canvas.lineTo(200, 0);
	canvas.lineTo(200, 200);
	canvas.lineTo(0, 200);
	canvas.lineTo(0, 100);
	canvas.lineTo(100, 100);
	canvas.fill();
	canvas.closePath();
}

void canvasTestStroke()
{
	canvas.beginPath();
	canvas.transform(1, 0, 0, 1, 100, 400);
	canvas.strokeStyle = 0x00FFFFFF;
	canvas.lineCap = LineCap::Round;
	canvas.lineJoin = LineJoin::Round;
	canvas.lineWidth = 20;
	canvas.moveTo(0, 0);
	canvas.lineTo(100, 100);
	canvas.lineTo(200, 150);
	canvas.stroke();
	canvas.closePath();

	canvas.beginPath();
	canvas.transform(1, 0, 0, 1, 300, 400);
	canvas.strokeStyle = 0xFF00FFFF;
	canvas.lineCap = LineCap::Butt;
	canvas.lineJoin = LineJoin::Miter;
	canvas.lineWidth = 6;
	canvas.moveTo(0, 0);
	canvas.quadraticCurveTo(0, 100, 200, 0);
	canvas.stroke();
	canvas.closePath();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		float x = (float)LOWORD(lParam);
		float y = (float)HIWORD(lParam);
		bool hit = canvas.isPointInPath(x, y);
		if(hit)	printf("hit canvas\n");
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

int main()
{
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(nullptr);
	WNDCLASSW wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = L"CanvasTestWndClass";
	RegisterClassW(&wc);

	DWORD style = WS_OVERLAPPEDWINDOW & (~WS_SIZEBOX) & (~WS_MAXIMIZEBOX);
	RECT rect = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rect, style, false);
	HWND hWnd = CreateWindowW(wc.lpszClassName, L"", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, 10);
	UpdateWindow(hWnd);

	/////////////////////////vector 2d graph triangulate////////////////////////////
	canvasTestFill();
	canvasTestStroke();
	std::vector<float> vertices;
	canvas.triangulate(&vertices);
	uint32_t verticesSize = uint32_t(sizeof(float) * vertices.size());

	/////////////////////////Draw ////////////////////////////
	wgpuSetLogLevel(WGPULogLevel_Info);
	WGPUContext::init();
	WGPUTextureFormat swapChainFormat;
	WGPUSwapChain swapChain = WGPUContext::createWindowSwapChain(hInstance, hWnd, WIDTH, HEIGHT, &swapChainFormat);
	WGPUDevice device = WGPUContext::getDevice();

	RenderGraph renderGraph;
	renderGraph.addRasterPass("canvasPass", [&](RenderGraphBuilder& builder) 
		{
			WGPUBuffer vertexBuffer;
			{
				WGPUBufferDescriptor bufferDescriptor = { 0 };
				bufferDescriptor.usage = WGPUBufferUsage_Vertex;
				bufferDescriptor.size = verticesSize;
				bufferDescriptor.mappedAtCreation = true;
				vertexBuffer = wgpuDeviceCreateBuffer(device, &bufferDescriptor);
				float* bufferPtr = (float*)wgpuBufferGetMappedRange(vertexBuffer, 0, bufferDescriptor.size);
				memcpy(bufferPtr, vertices.data(), bufferDescriptor.size);
				wgpuBufferUnmap(vertexBuffer);
			}

			WGPUBuffer uniformBuffer;
			{
				WGPUBufferDescriptor bufferDescriptor = { 0 };
				bufferDescriptor.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
				bufferDescriptor.size = sizeof(float) * 16;
				uniformBuffer = wgpuDeviceCreateBuffer(device, &bufferDescriptor);
			}

			auto pipeline = std::make_shared<RasterPipeline>();
			//pipeline->primitiveTopology = WGPUPrimitiveTopology_PointList;
			//pipeline->cullMode = WGPUCullMode_Front;
			//pipeline->polygonMode = WGPUPolygonMode_Line;
			pipeline->shaderFromString(SHADER_SRC);
			pipeline->bindResource({ { WGPUBufferBindingType_Uniform, WGPUShaderStage_Vertex, 0, uniformBuffer, sizeof(float) * 16 } });
			pipeline->vertexInputLayout(20, { { WGPUVertexFormat_Float32x2, 0, 0 }, { WGPUVertexFormat_Float32x3, 8, 1} });
			pipeline->fragmentOutputLayout({ { swapChainFormat, WGPUBlendFactor_One, WGPUBlendFactor_Zero, WGPUBlendOperation_Add } });
			pipeline->compile();

			auto camera = std::make_shared<Camera>();
			camera->setClip(-1, 1);
			camera->setOrthoSize(WIDTH, HEIGHT);
			camera->setPositon(WIDTH * 0.5f, HEIGHT * 0.5f, -0.5);
			camera->lookAt(vec3<f32>(WIDTH * 0.5f, HEIGHT * 0.5f, 0), vec3<f32>(0, -1, 0));
			auto mvp = camera->getProjectionMatrix() * camera->getViewMatrix();
			WGPUContext::writeBuffer(uniformBuffer, 0, &mvp, sizeof(float) * 16);
			return [=](RasterCommandList& commandList)
			{
				RasterRenderTarget renderTarget;
				renderTarget.colorAttachs[0].view = WGPUContext::getSwapChainTextureView(swapChain);
				renderTarget.colorAttachs[0].clearColor = { 0.03, 0.03, 0.03, 1.0 };

				commandList.setRenderTarget(renderTarget);
				commandList.setPipeline(pipeline);
				commandList.setVertexBuffer(0, vertexBuffer, 0, verticesSize);
				commandList.draw(verticesSize / (20));
			};
		});
	renderGraph.compile();

	/////////////////////////////////////////////////////
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		renderGraph.execute();

		WGPUContext::presentSwapChain(swapChain);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
