#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
extern "C"
{
#include "Deps/wgpu.h"
}
#include "Camera.h"

static const int WIDTH = 800;
static const int HEIGHT = 600;

const char* SHADER_SRC = R"(
	[[block]] struct Uniforms {
	  mvpMatrix : mat4x4<f32>;
	};
	[[binding(0), group(0)]] var<uniform> uniforms : Uniforms;

	[[stage(vertex)]]
	fn vs_main([[location(0)]] a_position : vec2<f32>) -> [[builtin(position)]] vec4<f32> {
		let final_pos : vec4<f32> = uniforms.mvpMatrix * vec4<f32>(a_position, 0.0, 1.0);
		return final_pos;
	}

	[[stage(fragment)]]
	fn fs_main() -> [[location(0)]] vec4<f32> {
		return vec4<f32>(1.0, 0.0, 0.0, 1.0);
	}
)";

Camera camera;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool mouse_right_down = false;
	static int last_mouse_x = 0;
	static int last_mouse_y = 0;
	switch (message)
	{
	case WM_RBUTTONDOWN:
	{
		mouse_right_down = true;
		last_mouse_x = (int)LOWORD(lParam);
		last_mouse_y = (int)HIWORD(lParam);
		break;
	}
	case WM_RBUTTONUP:
	{
		mouse_right_down = false;
		break;
	}
	case WM_MOUSEMOVE:
	{
		int x = (int)LOWORD(lParam);
		int y = (int)HIWORD(lParam);

		if (mouse_right_down)
		{
			float rotate_speed = 0.1f;
			camera.rotate(rotate_speed * (x - last_mouse_x), rotate_speed * (y - last_mouse_y));
		}
		
		last_mouse_x = x;
		last_mouse_y = y;
		break;
	}
	case WM_CHAR:
	{
		float move_speed = 0.1f;
		switch (wParam)
		{
		case 'w':
		{
			camera.moveFoward(move_speed);
			break;
		}
		case 's':
		{
			camera.moveFoward(-move_speed);
			break;
		}
		case 'a':
		{
			camera.moveRight(-move_speed);
			break;
		}
		case 'd':
		{
			camera.moveRight(move_speed);
			break;
		}
		case 'q':
		{
			camera.moveUp(move_speed);
			break;
		}
		case 'e':
		{
			camera.moveUp(-move_speed);
			break;
		}
		}
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
	wc.lpszClassName = L"WGPUTestWndClass";
	RegisterClassW(&wc);

	DWORD style = WS_OVERLAPPEDWINDOW & (~WS_SIZEBOX) & (~WS_MAXIMIZEBOX);
	RECT rect = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rect, style, false);
	HWND hWnd = CreateWindowW(wc.lpszClassName, L"", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, 10);
	UpdateWindow(hWnd);



	/////////////////////////////////////////////////////
	wgpuSetLogLevel(WGPULogLevel_Info);

	auto log_callback = [](WGPULogLevel level, const char* msg)
	{
		const char* level_str;
		switch (level)
		{
		case WGPULogLevel_Error: level_str = "Error"; break;
		case WGPULogLevel_Warn: level_str = "Warn"; break;
		case WGPULogLevel_Info: level_str = "Info"; break;
		case WGPULogLevel_Debug: level_str = "Debug"; break;
		case WGPULogLevel_Trace: level_str = "Trace"; break;
		default: level_str = "Unknown Level";
		}
		printf("[WGPU][%s] %s\n", level_str, msg);
	};
	wgpuSetLogCallback(log_callback);

	WGPUAdapter adapter;
	WGPURequestAdapterOptions adapterOption = { 0 };
	auto request_adapter_callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void* userdata)
	{
		*(WGPUAdapter*)userdata = adapter;
	};
	wgpuInstanceRequestAdapter(nullptr, &adapterOption, request_adapter_callback, &adapter);

	WGPUDevice device;
	WGPUDeviceDescriptor deviceDescriptor;
	WGPUDeviceExtras deviceExtras;
	deviceExtras.chain.next = nullptr;
	deviceExtras.chain.sType = (WGPUSType)WGPUSType_DeviceExtras;
	deviceExtras.label = "Device";
	deviceExtras.tracePath = nullptr;
	deviceDescriptor.nextInChain = (WGPUChainedStruct*)&deviceExtras;
	WGPURequiredLimits requiredLimits;
	requiredLimits.nextInChain = nullptr;
	requiredLimits.limits = { 0 };
	requiredLimits.limits.maxBindGroups = 1;
	deviceDescriptor.requiredLimits = &requiredLimits;
	auto request_device_callback = [](WGPURequestDeviceStatus status, WGPUDevice device, const char* message, void* userdata)
	{
		*(WGPUDevice*)userdata = device;
	};
	wgpuAdapterRequestDevice(adapter, &deviceDescriptor, request_device_callback, &device);
	
	//...
	WGPUSurfaceDescriptorFromWindowsHWND surfaceDescriptorHWND = { 0 };
	surfaceDescriptorHWND.chain = { 0 };
	surfaceDescriptorHWND.chain.sType = WGPUSType_SurfaceDescriptorFromWindowsHWND;
	surfaceDescriptorHWND.hinstance = hInstance;
	surfaceDescriptorHWND.hwnd = hWnd;
	WGPUSurfaceDescriptor surfaceDescriptor;
	surfaceDescriptor.label = "HWND_Surface";
	surfaceDescriptor.nextInChain = (WGPUChainedStruct*)&surfaceDescriptorHWND;
	WGPUSurface surface = wgpuInstanceCreateSurface(nullptr, &surfaceDescriptor);
	
	WGPUTextureFormat swapChainFormat = wgpuSurfaceGetPreferredFormat(surface, adapter);
	WGPUSwapChainDescriptor swapChainDescriptor = { 0 };
	swapChainDescriptor.usage = WGPUTextureUsage_RenderAttachment;
	swapChainDescriptor.format = swapChainFormat;
	swapChainDescriptor.width = WIDTH;
	swapChainDescriptor.height = HEIGHT;
	swapChainDescriptor.presentMode = WGPUPresentMode_Fifo;
	WGPUSwapChain swapChain = wgpuDeviceCreateSwapChain(device, surface, &swapChainDescriptor);

	WGPUShaderModuleWGSLDescriptor wgslDescriptor;
	wgslDescriptor.chain.next = nullptr;
	wgslDescriptor.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
	wgslDescriptor.source = SHADER_SRC;
	WGPUShaderModuleDescriptor shaderModuleDescriptor = { 0 };
	shaderModuleDescriptor.nextInChain = (WGPUChainedStruct*)&wgslDescriptor;
	WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderModuleDescriptor);

	//uniform buffer
	WGPUBuffer uniformBuffer;
	WGPUBindGroup uniformBindGroup;
	WGPUBindGroupLayout uniformBindGroupLayout;
	{
		WGPUBufferDescriptor bufferDescriptor = { 0 };
		bufferDescriptor.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
		bufferDescriptor.size = sizeof(float) * 16;
		uniformBuffer = wgpuDeviceCreateBuffer(device, &bufferDescriptor);

		WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = { 0 };
		WGPUBindGroupLayoutEntry bindGroupLayoutEntry = { 0 };
		bindGroupLayoutEntry.binding = 0;
		bindGroupLayoutEntry.visibility = WGPUShaderStage_Vertex;
		WGPUBufferBindingLayout bufferBindingLayout = { 0 };
		bufferBindingLayout.type = WGPUBufferBindingType_Uniform;
		bindGroupLayoutEntry.buffer = bufferBindingLayout;
		bindGroupLayoutDescriptor.entries = &bindGroupLayoutEntry;
		bindGroupLayoutDescriptor.entryCount = 1;
		uniformBindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDescriptor);

		WGPUBindGroupDescriptor bindGroupDescriptor = { 0 };
		bindGroupDescriptor.layout = uniformBindGroupLayout;
		WGPUBindGroupEntry bindGroupEntry = { 0 };
		bindGroupEntry.binding = 0;
		bindGroupEntry.buffer = uniformBuffer;
		bindGroupEntry.size = bufferDescriptor.size;
		bindGroupDescriptor.entries = &bindGroupEntry;
		bindGroupDescriptor.entryCount = 1;
		uniformBindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);
	}

	WGPUBindGroupLayout bindGroupLayouts[1] = { uniformBindGroupLayout };
	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = { 0 };
	pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts;
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
	WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);

	WGPURenderPipelineDescriptor pipelineDescriptor = { 0 };
	pipelineDescriptor.layout = pipelineLayout;
	WGPUMultisampleState multisampleState = { 0 };
	multisampleState.count = 1;
	multisampleState.mask = ~0;
	pipelineDescriptor.multisample = multisampleState;
	WGPUPrimitiveState primitiveState = { 0 };
	primitiveState.topology = WGPUPrimitiveTopology_TriangleList;
	primitiveState.stripIndexFormat = WGPUIndexFormat_Undefined;
	primitiveState.frontFace = WGPUFrontFace_CCW;
	primitiveState.cullMode = WGPUCullMode_None;
	primitiveState.polygonMode = WGPUPolygonMode_Fill;
	pipelineDescriptor.primitive = primitiveState;
	//Vertex
	WGPUVertexState vertexState = { 0 };
	vertexState.module = shaderModule;
	vertexState.entryPoint = "vs_main";
	WGPUVertexBufferLayout vertexBufferLayout = { 0 };
	vertexBufferLayout.arrayStride = 4 * 2;
	WGPUVertexAttribute vertexAttribute;
	vertexAttribute.format = WGPUVertexFormat_Float32x2;
	vertexAttribute.offset = 0;
	vertexAttribute.shaderLocation = 0;
	vertexBufferLayout.attributes = &vertexAttribute;
	vertexBufferLayout.attributeCount = 1;
	vertexState.buffers = &vertexBufferLayout;
	vertexState.bufferCount = 1;
	pipelineDescriptor.vertex = vertexState;
	//Fragment
	WGPUFragmentState fragmentState = { 0 };
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";
	fragmentState.targetCount = 1;
	WGPUColorTargetState colorTargetState = { 0 };
	colorTargetState.format = swapChainFormat;
	colorTargetState.writeMask = WGPUColorWriteMask_All;
	WGPUBlendComponent blendComponent;
	blendComponent.srcFactor = WGPUBlendFactor_One;
	blendComponent.dstFactor = WGPUBlendFactor_Zero;
	blendComponent.operation = WGPUBlendOperation_Add;
	WGPUBlendState blendState;
	blendState.color = blendComponent;
	blendState.alpha = blendComponent;
	colorTargetState.blend = &blendState;
	fragmentState.targets = &colorTargetState;
	pipelineDescriptor.fragment = &fragmentState;
	WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDescriptor);
	
	//vertex bufer
	float vertexData[6] = { -0.5, 0.5, -0.5, -0.5, 0.5, -0.5 };
	WGPUBuffer vertexBuffer;
	{
		WGPUBufferDescriptor bufferDescriptor = { 0 };
		bufferDescriptor.usage = WGPUBufferUsage_Vertex;
		bufferDescriptor.size = sizeof(float) * 6;
		bufferDescriptor.mappedAtCreation = true;
		vertexBuffer = wgpuDeviceCreateBuffer(device, &bufferDescriptor);
		float* vertexBufferPtr = (float*)wgpuBufferGetMappedRange(vertexBuffer, 0, bufferDescriptor.size);
		memcpy(vertexBufferPtr, vertexData, bufferDescriptor.size);
		wgpuBufferUnmap(vertexBuffer);
	}

	camera.setFovYAndAspect(45.0f, (float)WIDTH / (float)HEIGHT);
	//camera.setClip(0, 5);
	//camera.orthoSize(8, 6);
	camera.setPositon(0, 0, 5);
	camera.lookAt(vec3<f32>(0, 0, 0));
	
	/////////////////////////////////////////////////////
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		WGPUQueue queue = wgpuDeviceGetQueue(device);

		//mat4x4<f32> mmat = translate(mat4x4<f32>(), 2.0f, 2.f, 0.f);
		auto mvp = camera.getProjectionMatrix() * camera.getViewMatrix();
		wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &mvp, sizeof(float) * 16);

		WGPUTextureView nextTexture = wgpuSwapChainGetCurrentTextureView(swapChain);
		if (!nextTexture) 
		{
			printf("Cannot acquire next swap chain texture\n");
			return 1;
		}

		WGPUCommandEncoderDescriptor encoderDescriptor = { 0 };
		WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDescriptor);
		
		WGPURenderPassDescriptor renderPassDescriptor = { 0 };
		renderPassDescriptor.colorAttachmentCount = 1;
		WGPURenderPassColorAttachment colorAttachment = { 0 };
		colorAttachment.view = nextTexture;
		colorAttachment.loadOp = WGPULoadOp_Clear;
		colorAttachment.storeOp = WGPUStoreOp_Store;
		colorAttachment.clearColor = { 0.0, 1.0, 0.0, 1.0 };
		renderPassDescriptor.colorAttachments = &colorAttachment;
		WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDescriptor);

		wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
		wgpuRenderPassEncoderSetBindGroup(renderPass, 0, uniformBindGroup, 0, 0);
		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, sizeof(float) * 6);
		wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);
		wgpuRenderPassEncoderEndPass(renderPass);

		WGPUCommandBufferDescriptor commandBufferDescriptor = { 0 };
		WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(encoder, &commandBufferDescriptor);

		wgpuQueueSubmit(queue, 1, &cmdBuffer);
		wgpuSwapChainPresent(swapChain);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
