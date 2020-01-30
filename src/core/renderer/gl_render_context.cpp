#include <core/render_context.h>
#include <core/logger.h>
#include <core/collections/dynamic_array.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>

#include <nanovg/nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg/nanovg_gl.h>
#include <nanovg/nanovg_gl_utils.h>

global_variable SDL_GLContext gl_context;

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
	}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
	}();

	if(type == GL_DEBUG_TYPE_ERROR) {
		Log::error("%s, %s, %s, %d, %s", src_str, type_str, severity_str, id, message);
	} else {
		Log::warning("%s, %s, %s, %d, %s", src_str, type_str, severity_str, id, message);
	}
	
	// std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

NVGcontext* vg;

void RenderContext::init(s32 width, s32 height, s32 refresh_rate, PlatformWindow* window) {
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    

	gl_context = SDL_GL_CreateContext((SDL_Window*)window->handle);
	if(gl_context == NULL) {
		Log::error("Failed to create GL Context");
	}

	glewExperimental = GL_TRUE;
	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK) {
		Log::error("Failed to init glew");
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);

	Log::info("Renderer: %s", renderer);
	Log::info("Version: %s", version);
	glViewport(0, 0, width, height);

	// glEnable(GL_DEBUG_OUTPUT);
	// glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	// glDebugMessageCallback(message_callback, nullptr);

	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if(vg == 0) {
		Log::error("Failed to init nanovg");
	}
 
	
}

void RenderContext::uninit() {
	SDL_GL_DeleteContext(gl_context);
}

void RenderContext::clear(const Color& color) {
	glClearColor(color.r, color.g, color.b, color.a);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
}

void RenderContext::present() {
	PlatformWindow* window = Platform::getCurrentWindow();	
	
	SDL_Window* sdl_window = (SDL_Window*)window->handle;
	SDL_GL_SwapWindow(sdl_window);
}

void RenderContext::setViewport(s32 x, s32 y, s32 width, s32 height, f32 min_depth, f32 max_depth) {
	glViewport(x, y, width, height);
}

void RenderContext::resizeBuffer(s32 width, s32 height) {
	
}

void RenderContext::setBlendMode(const BlendMode mode) {
	if(mode == BlendMode::Disabled) {
		glDisable(GL_BLEND);
		return;
	}

	glEnable(GL_BLEND);
	switch(mode) {
		case BlendMode::AlphaBlend: {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		} break;

		case BlendMode::Additive: {
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			// glBlendFunc(GL_ONE, GL_ONE);
		} break;
	}
}

void RenderContext::setCullMode(const CullMode mode) {
	if(mode == CullMode::Disabled) {
		glDisable(GL_CULL_FACE);
		return;
	}

	glEnable(GL_CULL_FACE);
	switch(mode) {
		case CullMode::Front: glCullFace(GL_FRONT); break;
		case CullMode::Back: glCullFace(GL_BACK); break;
	}
}

void RenderContext::enableClipRect() {
	glEnable(GL_SCISSOR_TEST);
}

void RenderContext::disableClipRect() {
	glDisable(GL_SCISSOR_TEST);
}

void RenderContext::setClipRect(s32 x, s32 y, s32 w, s32 h) {
	glScissor(x, y, w, h);
}

void RenderContext::setDepthMode(const DepthMode mode) {
	if(mode == DepthMode::Disabled) {
		glDisable(GL_DEPTH_TEST);
		return;
	}

	glEnable(GL_DEPTH_TEST);
	switch(mode) {
		case DepthMode::LEqual: {
			glDepthFunc(GL_LEQUAL);
		} break;
	}
}

void RenderContext::setMultisampleEnabled(const bool enabled) {
	if(enabled) glEnable(GL_MULTISAMPLE);
	else glDisable(GL_MULTISAMPLE);
}

void RenderContext::setAlphaToCoverageEnabled(const bool enabled) {
	if(enabled) {
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		glEnable(GL_SAMPLE_ALPHA_TO_ONE);
	} else {
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		glDisable(GL_SAMPLE_ALPHA_TO_ONE);
	}
}

GLuint compileShader(char* source, s32 length,  GLenum shader_type, const std::string& name) {

	GLuint vs = glCreateShader(shader_type);
	glShaderSource(vs, 1, &source, &length);
	glCompileShader(vs);

	GLint isCompiled = 0;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
		std::string type = shader_type == GL_VERTEX_SHADER ? "vert" : "frag";
		printf("Error: %s:%s: %s\n", name.c_str(), type.c_str(), &errorLog[0]);
	}

	return vs;
}

Shader RenderContext::createShader(const std::string& name, char* vertex_ptr, u64 vertex_size, char* pixel_ptr, u64 pixel_size) {
	GLuint vertex_shader = compileShader(vertex_ptr, (s32)vertex_size, GL_VERTEX_SHADER, name);
	GLuint fragment_shader = compileShader(pixel_ptr, (s32)pixel_size, GL_FRAGMENT_SHADER, name);
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	Shader result = {};
	result.program = shader_program;
	result.vertex_src = vertex_ptr;
	result.pixel_src = pixel_ptr;

	result.vertex_src_size = vertex_size;
	result.pixel_src_size = pixel_size;
	return result;
}

void RenderContext::destroyShader(Shader* shader) {
	glDeleteProgram(shader->program);
}

void RenderContext::bindShader(Shader* shader) {
	glUseProgram(shader->program);
}

void RenderContext::unbindShader() {
	glUseProgram(0);
}

ShaderConstant RenderContext::createShaderConstant(u32 buffer_size) {
	u32 buffer = 0;
	glCreateBuffers(1, &buffer);
	glNamedBufferData(buffer, buffer_size, nullptr, GL_DYNAMIC_DRAW);
	ShaderConstant result = {};
	result.buffer = buffer;
	result.size = buffer_size;
	return result;
}

void RenderContext::updateShaderConstant(ShaderConstant* constant, void* data) {
	glNamedBufferSubData(constant->buffer, 0, constant->size, data);
}

void RenderContext::bindShaderConstant(ShaderConstant* constant, u32 location) {
	glBindBufferBase(GL_UNIFORM_BUFFER, location, constant->buffer);
}

struct ShaderType {
	u32 element_type;
	u32 element_count;
	u32 size_bytes;
	u32 image_format;
	bool normalized;
};

ShaderType shader_types[(u32)RenderContext::Format::MAX] = {
	/* Vec2, */ { GL_FLOAT, 2, 2 * 4, GL_RG, false },
	/* Vec3, */ { GL_FLOAT, 3, 3 * 4, GL_RGB, false },
	/* Vec4, */ { GL_FLOAT, 4, 4 * 4, GL_RGBA, false },
	/* u32_unorm, */ { GL_UNSIGNED_BYTE, 4, 4, GL_RGBA, true },
	/* u16  */ { GL_UNSIGNED_SHORT, 1, 2, false },
	/* u32  */ { GL_UNSIGNED_INT, 1, 4, false },
	/* F32  */ { GL_FLOAT, 1, 4, false },
};

VertexArray RenderContext::createVertexArray(const DynamicArray<RenderContext::LayoutElement>&& elements) {
	u32 vao = 0;
	glCreateVertexArrays(1, &vao);

	u32 offset = 0;
	u32 previous_buffer_binding = 0;
	for(u32 i = 0; i < elements.count; i++) {
		const LayoutElement& element = elements.getRefConst(i);
		ShaderType type = shader_types[(u32)element.format];

		if(element.buffer_binding != previous_buffer_binding) {
			offset = 0;
		}
		
		glEnableVertexArrayAttrib(vao, i);
		glVertexArrayAttribFormat(vao, i, type.element_count, type.element_type, type.normalized, offset);
		glVertexArrayAttribBinding(vao, i, element.buffer_binding);
		glVertexArrayBindingDivisor(vao, i, element.instance_divisor);

		offset += type.size_bytes;

		previous_buffer_binding = element.buffer_binding;
	}

	VertexArray result = { vao };
	return result;
}

void RenderContext::bindVertexArray(VertexArray* layout) {
	if(layout == 0) glBindVertexArray(0);
	else glBindVertexArray(layout->handle);
}

VertexBuffer RenderContext::createVertexBuffer(void* vertices, u32 vertex_size, u32 num_vertices) {
	u32 handle = 0;
	glCreateBuffers(1, &handle);
	glNamedBufferData(handle, vertex_size * num_vertices, vertices, GL_STATIC_DRAW);
	VertexBuffer buffer = { handle, vertex_size };
	return buffer;
}

void RenderContext::destroyVertexBuffer(VertexBuffer* vb) {
	glDeleteBuffers(1, &vb->handle);
}


void RenderContext::bindVertexBufferToVertexArray(VertexArray* vao, VertexBuffer* buffer, u32 slot /* = 0 */ ) {
	glVertexArrayVertexBuffer(vao->handle, slot, buffer->handle, 0, buffer->vertex_size);
}

void RenderContext::bindIndexBufferToVertexArray(VertexArray* vao, VertexBuffer* buffer) {
	glVertexArrayElementBuffer(vao->handle, buffer->handle);
}

Texture2D RenderContext::createTexture2D(void* data, u32 width, u32 height, Format format) {
	u32 handle = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &handle);

	// glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP);
	// glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureStorage2D(handle, 1, GL_RGBA8, width, height);

	const ShaderType type = shader_types[(u32)format];
	glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	Texture2D result = {};
	result.handle = handle;
	result.width = width;
	result.height = height;
	return result;
}

void RenderContext::bindTexture2D(Texture2D* texture, u32 slot) {
	glBindTextureUnit(slot, texture->handle);
}

void RenderContext::destroyTexture2D(Texture2D* texture) {
	glDeleteTextures(1, &texture->handle);
}

Sampler RenderContext::createSampler(const WrapMode wrap_mode, const FilterType filter) {
	u32 handle = 0;
	glCreateSamplers(1, &handle);
	
	GLenum gl_wrap_mode = 0;
	switch(wrap_mode) {
		case WrapMode::Clamp: gl_wrap_mode = GL_CLAMP; break;
		case WrapMode::Repeat: gl_wrap_mode = GL_REPEAT; break;
	}

	GLenum gl_filter_type = 0;
	switch(filter) {
		case FilterType::Point: gl_filter_type = GL_NEAREST; break;
		case FilterType::Linear: gl_filter_type = GL_LINEAR; break;
	}

	glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, gl_wrap_mode);
	glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, gl_wrap_mode);
	glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, gl_filter_type);
	glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, gl_filter_type);

	Sampler result = {handle};
	return result;
}

void RenderContext::bindSampler(Sampler* sampler, u32 slot) {
	if(sampler == 0) glBindSampler(0, 0);
	else glBindSampler(slot, sampler->handle);
}

void RenderContext::destroySampler(Sampler* sampler) {
	glDeleteSamplers(1, &sampler->handle);
}

u32 RenderContext::topologies[(u32)Topology::MAX] = {
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
};

void RenderContext::sendDraw(Topology topology, u32 num_vertices) {
	glDrawArrays(topologies[(u32)topology], 0, num_vertices);
}

void RenderContext::sendDrawIndexed(Topology topology, Format format, u32 num_indices, int index_offset, int vertex_offset) {
	ShaderType shader_type = shader_types[(u32)format];
	GLenum type = shader_type.element_type;
	GLvoid* offset = (GLvoid*)(intptr_t)(index_offset * shader_type.size_bytes);
	// glDrawElements(topologies[(u32)topology], num_indices, type, offset);
	
	glDrawElementsBaseVertex(topologies[(u32)topology], num_indices, type, offset, vertex_offset);
}

void RenderContext::sendDrawInstanced(Topology topology, u32 num_vertices, u32 num_instances) {
	glDrawArraysInstanced(topologies[(u32)topology], 0, num_vertices, num_instances);
}

void RenderContext::pushDebugGroup(const std::string& name) {
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, name.size(), name.c_str());
}

void RenderContext::popDebugGroup() {
	glPopDebugGroup();
}
