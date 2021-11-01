#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

extern "C"
{
#include "Deps/wgpu.h"
}

class WGPUContext final
{
public:
	static bool init();

	static WGPUSwapChain createWindowSwapChain(void* hInstance, void* hWnd, uint32_t width, uint32_t height, WGPUTextureFormat* format);

	inline static void presentSwapChain(WGPUSwapChain swapChain)
	{
		wgpuSwapChainPresent(swapChain);
	}

	inline static WGPUTextureView getSwapChainTextureView(WGPUSwapChain swapChain)
	{
		return wgpuSwapChainGetCurrentTextureView(swapChain);
	}

	inline static void writeBuffer(WGPUBuffer buffer, uint64_t bufferOffset, void const* data, size_t size)
	{
		wgpuQueueWriteBuffer(queue, buffer, bufferOffset, data, size);
	}

	inline static WGPUAdapter getAdapter()
	{
		return adapter;
	}

	inline static WGPUDevice getDevice()
	{
		return device;
	}

	inline static WGPUQueue getQueue()
	{
		return queue;
	}

private:
	static WGPUAdapter adapter;
	static WGPUDevice device;
	static WGPUQueue queue;
};

struct RasterRenderTarget
{
	RasterRenderTarget(int colorAttachCount = 1);
	std::vector<WGPURenderPassColorAttachment> colorAttachs{ 1 };
};

class RasterPipeline final
{
public:
	friend class RasterCommandList;

	struct ResourceBindDesc
	{
		WGPUBufferBindingType type;
		WGPUShaderStageFlags visibility;
		uint32_t binding;
		WGPUBuffer buffer;
		uint64_t size;
	};

	struct FragmentOutputLayoutDesc
	{
		WGPUTextureFormat format;
		WGPUBlendFactor blendSrc;
		WGPUBlendFactor blendDst;
		WGPUBlendOperation blendOpera;
	};

	RasterPipeline() = default;

	bool shaderFromString(const std::string& src);
	bool shaderFromFile(const std::string& filename);

	void bindResource(const std::vector<ResourceBindDesc>& descs);

	void vertexInputLayout(uint64_t stride, const std::vector<WGPUVertexAttribute>& vertexAttributes);
	void fragmentOutputLayout(const std::vector<FragmentOutputLayoutDesc>& descs);

	bool compile();

	inline WGPURenderPipeline getWGPURenderPipeline()
	{
		return pipeline;
	}

	inline WGPUBindGroup getWGPUBindGroup(int index)
	{
		return bindGroups[index];
	}

	WGPUPrimitiveTopology primitiveTopology = WGPUPrimitiveTopology_TriangleList;
	WGPUFrontFace frontFace = WGPUFrontFace_CCW;
	WGPUCullMode cullMode = WGPUCullMode_None;
	WGPUPolygonMode polygonMode = WGPUPolygonMode_Fill;
	int viewport[4] = { 0, 0, 0, 0 };
	int scissorBox[4] = { 0, 0, 0, 0 };

private:
	RasterPipeline(const RasterPipeline&) = delete;
	RasterPipeline& operator=(const RasterPipeline&) = delete;

	std::vector<ResourceBindDesc> resourceBindDescs;
	uint64_t vertexArrayStride = 0;
	std::vector<WGPUVertexAttribute> vertexInputLayouts;
	std::vector<FragmentOutputLayoutDesc> fragmentOutputLayouts;

	WGPUShaderModule shaderModule = nullptr;
	WGPURenderPipeline pipeline = nullptr;
	std::vector<WGPUBindGroup> bindGroups;
};

class RasterCommandList final
{
public:
	friend class RenderGraph;

	RasterCommandList(WGPUCommandEncoder encoder);

	void setRenderTarget(const RasterRenderTarget& renderTarget);

	void setPipeline(std::shared_ptr<RasterPipeline> pipeline);

	void setVertexBuffer(uint32_t slot, WGPUBuffer buffer, uint64_t offset, uint64_t size);
	void draw(uint32_t vertexCount, uint32_t instanceCount = 1);

private:
	RasterCommandList(const RasterCommandList&) = delete;
	RasterCommandList& operator=(const RasterCommandList&) = delete;

	WGPUCommandEncoder commandEncoder = nullptr;
	WGPURenderPassEncoder renderPassEncoder = nullptr;
};

class RenderGraphBuilder final
{
public:
	RenderGraphBuilder() = default;
private:
	RenderGraphBuilder(const RenderGraphBuilder&) = delete;
	RenderGraphBuilder& operator=(const RenderGraphBuilder&) = delete;
};

class RenderGraph final
{
public:
	RenderGraph();
	~RenderGraph();

	void addRasterPass(const std::string& key, 
		std::function<std::function<void(RasterCommandList&)>(RenderGraphBuilder&)> setupFunc);

	void compile();

	void execute();

private:
	RenderGraph(const RenderGraph&) = delete;
	RenderGraph& operator=(const RenderGraph&) = delete;

	std::map<std::string, std::function<std::function<void(RasterCommandList&)>(RenderGraphBuilder&)>> rasterSetupFuncs;
	std::map<std::string, std::function<void(RasterCommandList&)>> rasterRenderFuncs;
};
