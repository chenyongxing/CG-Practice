#include "RenderGraph.h"
#include <cassert>
#include <fstream>
#include <sstream>

WGPUAdapter WGPUContext::adapter = nullptr;
WGPUDevice WGPUContext::device = nullptr;
WGPUQueue WGPUContext::queue = nullptr;

bool WGPUContext::init()
{
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

	WGPURequestAdapterOptions adapterOption = { 0 };
	auto request_adapter_callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void* userdata)
	{
		*(WGPUAdapter*)userdata = adapter;
	};
	wgpuInstanceRequestAdapter(nullptr, &adapterOption, request_adapter_callback, &adapter);

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

	queue = wgpuDeviceGetQueue(device);

	return device;
}

WGPUSwapChain WGPUContext::createWindowSwapChain(void* hInstance, void* hWnd, uint32_t width, uint32_t height, WGPUTextureFormat* format)
{
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
	*format = swapChainFormat;

	WGPUSwapChainDescriptor swapChainDescriptor = { 0 };
	swapChainDescriptor.usage = WGPUTextureUsage_RenderAttachment;
	swapChainDescriptor.format = swapChainFormat;
	swapChainDescriptor.width = width;
	swapChainDescriptor.height = height;
	swapChainDescriptor.presentMode = WGPUPresentMode_Fifo;
	return wgpuDeviceCreateSwapChain(device, surface, &swapChainDescriptor);
}

RasterRenderTarget::RasterRenderTarget(int colorAttachCount)
{
	colorAttachs.resize(colorAttachCount);
	for (int i = 0; i < colorAttachCount; ++i)
	{
		WGPURenderPassColorAttachment colorAttachment = { 0 };
		colorAttachment.view = nullptr;
		colorAttachment.resolveTarget = nullptr;
		colorAttachment.loadOp = WGPULoadOp_Clear;
		colorAttachment.storeOp = WGPUStoreOp_Store;
		colorAttachment.clearColor = { 0.0, 0.0, 0.0, 1.0 };
		colorAttachs[i] = colorAttachment;
	}
}

bool RasterPipeline::shaderFromString(const std::string& src)
{
	assert(shaderModule == nullptr);

	WGPUShaderModuleWGSLDescriptor wgslDescriptor;
	wgslDescriptor.chain.next = nullptr;
	wgslDescriptor.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
	wgslDescriptor.source = src.c_str();
	WGPUShaderModuleDescriptor shaderModuleDescriptor = { 0 };
	shaderModuleDescriptor.nextInChain = (WGPUChainedStruct*)&wgslDescriptor;
	shaderModule = wgpuDeviceCreateShaderModule(WGPUContext::getDevice(), &shaderModuleDescriptor);

	return shaderModule != nullptr;
}

bool RasterPipeline::shaderFromFile(const std::string& filename)
{
	assert(shaderModule == nullptr);

	using namespace std;
	ifstream ifile;
	stringstream sstream;
	ifile.open(filename);
	if (!ifile.good())	return false;

	sstream << ifile.rdbuf();
	ifile.close();
	return shaderFromString(sstream.str());
}

void RasterPipeline::bindResource(const std::vector<ResourceBindDesc>& descs)
{
	resourceBindDescs = descs;
}

void RasterPipeline::vertexInputLayout(uint64_t stride, const std::vector<WGPUVertexAttribute>& vertexAttributes)
{
	vertexArrayStride = stride;
	vertexInputLayouts = vertexAttributes;
}

void RasterPipeline::fragmentOutputLayout(const std::vector<FragmentOutputLayoutDesc>& descs)
{
	fragmentOutputLayouts = descs;
}

bool RasterPipeline::compile()
{
	assert(pipeline == nullptr);
	assert(shaderModule != nullptr);
	assert(vertexInputLayouts.size() > 0);
	
	WGPUDevice device = WGPUContext::getDevice();

	std::vector<WGPUBindGroupLayout> bindGroupLayouts;
	for (auto& desc : resourceBindDescs)
	{
		WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = { 0 };
		WGPUBindGroupLayoutEntry bindGroupLayoutEntry = { 0 };
		bindGroupLayoutEntry.binding = 0;
		bindGroupLayoutEntry.visibility = desc.visibility;
		WGPUBufferBindingLayout bufferBindingLayout = { 0 };
		bufferBindingLayout.type = desc.type;
		bindGroupLayoutEntry.buffer = bufferBindingLayout;
		bindGroupLayoutDescriptor.entries = &bindGroupLayoutEntry;
		bindGroupLayoutDescriptor.entryCount = 1;
		WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDescriptor);

		WGPUBindGroupDescriptor bindGroupDescriptor = { 0 };
		bindGroupDescriptor.layout = bindGroupLayout;
		WGPUBindGroupEntry bindGroupEntry = { 0 };
		bindGroupEntry.binding = desc.binding;
		bindGroupEntry.buffer = desc.buffer;
		bindGroupEntry.size = desc.size;
		bindGroupDescriptor.entries = &bindGroupEntry;
		bindGroupDescriptor.entryCount = 1;
		WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);
		bindGroups.push_back(bindGroup);

		bindGroupLayouts.push_back(bindGroupLayout);
	}
	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = { 0 };
	pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
	pipelineLayoutDescriptor.bindGroupLayoutCount = (uint32_t)bindGroupLayouts.size();
	WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);

	WGPURenderPipelineDescriptor pipelineDescriptor = { 0 };
	pipelineDescriptor.layout = pipelineLayout;

	WGPUMultisampleState multisampleState = { 0 };
	multisampleState.count = 1;
	multisampleState.mask = ~0;
	pipelineDescriptor.multisample = multisampleState;

	WGPUPrimitiveState primitiveState = { 0 };
	primitiveState.topology = primitiveTopology;
	primitiveState.stripIndexFormat = WGPUIndexFormat_Undefined;
	primitiveState.frontFace = frontFace;
	primitiveState.cullMode = cullMode;
	primitiveState.polygonMode = polygonMode;
	pipelineDescriptor.primitive = primitiveState;

	//Vertex
	WGPUVertexState vertexState = { 0 };
	vertexState.module = shaderModule;
	vertexState.entryPoint = "vs_main";
	WGPUVertexBufferLayout vertexBufferLayout = { 0 };
	vertexBufferLayout.arrayStride = vertexArrayStride;
	vertexBufferLayout.attributes = vertexInputLayouts.data();
	vertexBufferLayout.attributeCount = (uint32_t)vertexInputLayouts.size();
	vertexState.buffers = &vertexBufferLayout;
	vertexState.bufferCount = 1;
	pipelineDescriptor.vertex = vertexState;

	//Fragment
	WGPUFragmentState fragmentState = { 0 };
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";
	std::vector<WGPUColorTargetState> colorTargetStates;
	for (auto fragmentOutput : fragmentOutputLayouts)
	{
		WGPUColorTargetState colorTargetState = { 0 };
		colorTargetState.format = fragmentOutput.format;
		
		WGPUBlendComponent blendComponent;
		blendComponent.srcFactor = fragmentOutput.blendSrc;
		blendComponent.dstFactor = fragmentOutput.blendDst;
		blendComponent.operation = fragmentOutput.blendOpera;
		WGPUBlendState blendState;
		blendState.color = blendComponent;
		blendState.alpha = blendComponent;
		colorTargetState.blend = &blendState;

		colorTargetState.writeMask = WGPUColorWriteMask_All;

		colorTargetStates.push_back(colorTargetState);
	}
	fragmentState.targets = colorTargetStates.data();
	fragmentState.targetCount = (uint32_t)colorTargetStates.size();
	pipelineDescriptor.fragment = &fragmentState;
	
	pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDescriptor);

	return pipeline != nullptr;
}

RasterCommandList::RasterCommandList(WGPUCommandEncoder encoder) :
	commandEncoder(encoder)
{
}

void RasterCommandList::setRenderTarget(const RasterRenderTarget& renderTarget)
{
	WGPURenderPassDescriptor renderPassDesc = { 0 };
	renderPassDesc.colorAttachmentCount = (uint32_t)renderTarget.colorAttachs.size();
	renderPassDesc.colorAttachments = renderTarget.colorAttachs.data();

	renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);
}

void RasterCommandList::setPipeline(std::shared_ptr<RasterPipeline> pipeline)
{
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, pipeline->pipeline);

	for (int i = 0; i < pipeline->bindGroups.size(); ++i)
	{
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, pipeline->resourceBindDescs[i].binding, pipeline->bindGroups[i], 0, 0);
	}
}

void RasterCommandList::setVertexBuffer(uint32_t slot, WGPUBuffer buffer, uint64_t offset, uint64_t size)
{
	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, slot, buffer, offset, size);
}

void RasterCommandList::draw(uint32_t vertexCount, uint32_t instanceCount)
{
	assert(renderPassEncoder != nullptr);
	wgpuRenderPassEncoderDraw(renderPassEncoder, vertexCount, instanceCount, 0, 0);
}

RenderGraph::RenderGraph()
{
}

RenderGraph::~RenderGraph()
{
}

void RenderGraph::addRasterPass(const std::string& key, 
	std::function<std::function<void(RasterCommandList&)>(RenderGraphBuilder&)> setupFunc)
{
	assert(rasterSetupFuncs.find(key) == rasterSetupFuncs.end());
	rasterSetupFuncs[key] = setupFunc;
}

void RenderGraph::compile()
{
	for (auto pair : rasterSetupFuncs)
	{
		RenderGraphBuilder builder;
		rasterRenderFuncs[pair.first] = pair.second(builder);
	}
}

void RenderGraph::execute()
{
	WGPUDevice device = WGPUContext::getDevice();
	WGPUCommandEncoderDescriptor encoderDescriptor = { 0 };
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDescriptor);

	for (auto pair : rasterRenderFuncs)
	{
		RasterCommandList commandList(encoder);
		pair.second(commandList);
		wgpuRenderPassEncoderEndPass(commandList.renderPassEncoder);
	}

	WGPUCommandBufferDescriptor commandBufferDescriptor = { 0 };
	WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(encoder, &commandBufferDescriptor);

	WGPUQueue queue = wgpuDeviceGetQueue(device);
	wgpuQueueSubmit(queue, 1, &cmdBuffer);
}
