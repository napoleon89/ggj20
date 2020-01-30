#pragma once

#include <core/std.h>
#include <string>
#include <core/platform.h>
#include <core/math.h>

struct PlatformRenderState;

struct Shader {
	u32 program;
	void* vertex_shader;
	void* pixel_shader;
	char* vertex_src;
	char* pixel_src;
	u64 vertex_src_size;
	u64 pixel_src_size;
};

struct VertexArray {
	u32 handle;
};

struct ShaderConstant {
	u32 buffer;
	u32 size;
};

struct VertexBuffer {
	u32 handle;
	u32 vertex_size;
};

struct Texture2D {
	u32 handle;
	u32 width;
	u32 height;
};

struct Sampler {
	u32 handle;
};

enum class BlendMode {
	Disabled,
	AlphaBlend,
	Additive,
};

enum class CullMode {
	Disabled,
	Front,
	Back,
};

enum class DepthMode {
	Disabled,
	LEqual,
};

enum class AlphaTestMode {
	Disabled,
	Clip,
};

enum class FilterType {
	Point,
	Linear,
};

enum class WrapMode {
	Clamp,
	Repeat,
};

enum class Topology {
	TriangleList,
	TriangleStrip,
	MAX
};

struct RenderContext {

	enum class Format {
		Vec2,
		Vec3,
		Vec4,
		u32_unorm,
		u16,
		u32,
		F32,
		MAX
	};

	struct LayoutElement {
		std::string name;
		Format format;
		u32 buffer_binding = 0;
		u32 instance_divisor = 0;
	};

	static u32 topologies[(u32)Topology::MAX];

	static void resizeBuffer(s32 width, s32 height);
	static void init(s32 width, s32 height, s32 refresh_rate, PlatformWindow* window);
	static void uninit();
	static void clear(const Color& color);
	static void present();

	static Texture2D createTexture2D(void* data, u32 width, u32 height, Format format);
	static void bindTexture2D(Texture2D* texture, u32 slot);
	static void destroyTexture2D(Texture2D* texture);

	static Sampler createSampler(const WrapMode wrap_mode, const FilterType filter);
	static void bindSampler(Sampler* sampler, u32 slot);
	static void destroySampler(Sampler* sampler);

	static Shader createShader(const std::string& name, char* vertex_ptr, u64 vertex_size, char* pixel_ptr, u64 pixel_size);
	static void destroyShader(Shader* shader);
	static void bindShader(Shader* shader);
	static void unbindShader();

	static VertexArray createVertexArray(const DynamicArray<LayoutElement>&& elements);
	static void bindVertexArray(VertexArray* layout);

	static ShaderConstant createShaderConstant(u32 buffer_size);
	static void updateShaderConstant(ShaderConstant* constant, void* data);
	static void bindShaderConstant(ShaderConstant* constant, u32 location);

	static VertexBuffer createVertexBuffer(void* vertices, u32 vertex_size, u32 num_vertices);
	static void destroyVertexBuffer(VertexBuffer* vb);

	static void bindVertexBufferToVertexArray(VertexArray* vao, VertexBuffer* buffer, u32 slot = 0);
	static void bindIndexBufferToVertexArray(VertexArray* vao, VertexBuffer* buffer);

	// =========== Raster State ===========================

	static void setBlendMode(const BlendMode mode);
	static void setCullMode(const CullMode mode);

	static void enableClipRect();
	static void disableClipRect();
	static void setClipRect(s32 x, s32 y, s32 w, s32 h);

	static void setDepthMode(const DepthMode mode);

	static void setViewport(s32 x, s32 y, s32 width, s32 height, f32 min_depth, f32 max_depth);

	static void setMultisampleEnabled(const bool enabled);
	static void setAlphaToCoverageEnabled(const bool enabled);

	// ============= Draw Commands =========================

	static void sendDraw(Topology topology, u32 num_vertices);
	static void sendDrawIndexed(Topology topology, Format format, u32 num_indices, int index_offset = 0, int vertex_offset = 0);
	static void sendDrawInstanced(Topology topology, u32 num_vertices, u32 num_instances);

	// ============= Debug Commands =========================
	static void pushDebugGroup(const std::string& name);
	static void popDebugGroup();
};
