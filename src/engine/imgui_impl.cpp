#include <engine/imgui_impl.h>
#include <core/platform.h>
#include <core/render_context.h>
#include <core/std.h>
#include <imgui/imgui.h>
#include <engine/profiler.h>

static ShaderConstant im_matrix;
static Shader *im_shader;
static VertexArray im_layout;
static Texture2D im_font_texture;
static Sampler im_font_sampler;
static ImGuiMouseCursor previous_cursor;

struct VERTEX_CONSTANT_BUFFER
{
	float mvp[4][4];
};

void ImGuiImpl::render() {
	ProfileFunc;
	RenderContext::pushDebugGroup("Render ImGui");
	
	ImDrawData *draw_data = ImGui::GetDrawData();

	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
        return;


	// int index_count = 0;
	// int vertex_count = 0;
	// for (int n = 0; n < draw_data->CmdListsCount; n++)
	// {
	// 	const ImDrawList* cmd_list = draw_data->CmdLists[n];
		
	// 	vertex_count += cmd_list->VtxBuffer.Size;
	// 	index_count += cmd_list->IdxBuffer.Size;
	// }
	
	// // ImDrawVert *vertices = (ImDrawVert *)malloc(vertex_count * sizeof(ImDrawVert));
	// // ImDrawIdx *indices = (ImDrawIdx *)malloc(index_count * sizeof(ImDrawIdx));
	
	// // ImDrawVert* vtx_dst = (ImDrawVert*)vertices;
	// // ImDrawIdx* idx_dst = (ImDrawIdx*)indices;
	// // for (int n = 0; n < draw_data->CmdListsCount; n++) {
	// // 	const ImDrawList* cmd_list = draw_data->CmdLists[n];
	// // 	memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
	// // 	memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
	// // 	vtx_dst += cmd_list->VtxBuffer.Size;
	// // 	idx_dst += cmd_list->IdxBuffer.Size;
	// // }
	

	RenderContext::bindShader(im_shader);

	// Setup orthographic projection matrix into our constant buffer
	{
		
		float L = 0.0f;
		float R = ImGui::GetIO().DisplaySize.x;
		float B = ImGui::GetIO().DisplaySize.y;
		float T = 0.0f;
		float mvp[4][4] =
		{
			{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
		};
		
		RenderContext::updateShaderConstant(&im_matrix, mvp);
		RenderContext::bindShaderConstant(&im_matrix, 0);
	}
	
	RenderContext::setViewport(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0.0f, 1.0f);

	// Bind shader and vertex buffers
	unsigned int stride = sizeof(ImDrawVert);
	unsigned int offset = 0;
	
	RenderContext::bindVertexArray(&im_layout);
	
	
	
	RenderContext::bindSampler(&im_font_sampler, 0);
	
	// Setup render state
	RenderContext::setBlendMode(BlendMode::AlphaBlend);
	RenderContext::enableClipRect();
	RenderContext::setDepthMode(DepthMode::Disabled);
	RenderContext::setCullMode(CullMode::Disabled);

	ImVec2 clip_off = draw_data->DisplayPos;
	ImVec2 clip_scale = draw_data->FramebufferScale;

	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];


		VertexBuffer vb = RenderContext::createVertexBuffer(cmd_list->VtxBuffer.Data, sizeof(ImDrawVert), cmd_list->VtxBuffer.Size);
		VertexBuffer ib = RenderContext::createVertexBuffer(cmd_list->IdxBuffer.Data, sizeof(ImDrawIdx), cmd_list->IdxBuffer.Size);

		RenderContext::bindVertexBufferToVertexArray(&im_layout, &vb);
		RenderContext::bindIndexBufferToVertexArray(&im_layout, &ib);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				ImVec4 clip_rect;
				clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
				clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
				clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
				clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f) {
					Texture2D *tex = (Texture2D *)pcmd->TextureId;
					if(tex != 0)
						RenderContext::bindTexture2D(tex, 0);
					
					
					RenderContext::setClipRect((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
					// RenderContext::setClipRect(clip_rect.x, clip_rect.y, clip_rect.z, clip_rect.w);
					// VertexBuffer vb = RenderContext::createVertexBuffer(cmd_list->VtxBuffer.Data, sizeof(ImDrawVert), cmd_list->VtxBuffer.Size);
					// VertexBuffer ib = RenderContext::createVertexBuffer(cmd_list->IdxBuffer.Data, sizeof(ImDrawIdx), cmd_list->IdxBuffer.Size, RenderContext::BufferType::Index);
					
					
					// RenderContext::bindVertexBuffer(&vb, 0);
					// RenderContext::bindIndexBuffer(&ib, sizeof(ImDrawIdx) == 2 ? RenderContext::Format::u16 : RenderContext::Format::u32);
					RenderContext::sendDrawIndexed(
						Topology::TriangleList, 
						sizeof(ImDrawIdx) == 2 ? RenderContext::Format::u16 : RenderContext::Format::u32,
						pcmd->ElemCount, 
						pcmd->IdxOffset
					);
				}
				
			}
			
		}

		RenderContext::destroyVertexBuffer(&vb);
		RenderContext::destroyVertexBuffer(&ib);
		
	}
	
// ??	free(vertices);
	// free(indices);
	
	// RenderContext::destroyVertexBuffer(&vb);
	// RenderContext::destroyVertexBuffer(&ib);

	RenderContext::disableClipRect();
	RenderContext::setBlendMode(BlendMode::Disabled);
	RenderContext::bindVertexArray(0);
	RenderContext::unbindShader();
	RenderContext::bindSampler(0, 0);
	
	RenderContext::popDebugGroup();
}

static void createFontsTexture() {
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	
	im_font_texture = RenderContext::createTexture2D(pixels, width, height, RenderContext::Format::u32_unorm);
	im_font_sampler = RenderContext::createSampler(WrapMode::Clamp, FilterType::Linear);
	io.Fonts->TexID = &im_font_texture;
}

static void invalidateDeviceObjects() {
	
}

static void createDeviceObjects() {
	if (im_font_sampler.handle)
		invalidateDeviceObjects();

	// By using D3DCompile() from <d3dcompiler.h> / d3dcompiler.lib, we introduce a dependency to a given version of d3dcompiler_XX.dll (see D3DCOMPILER_DLL_A)
	// If you would like to use this DX11 sample code but remove this dependency you can: 
	//  1) compile once, save the compiled shader blobs into a file or source code and pass them to CreateVertexShader()/CreatePixelShader() [preferred solution]
	//  2) use code to detect any version of the DLL and grab a pointer to D3DCompile from the DLL. 
	// See https://github.com/ocornut/imgui/pull/638 for sources and details.

	// Create the vertex shader
	
	im_layout = RenderContext::createVertexArray({
		{"POSITION", RenderContext::Format::Vec2},
		{"TEXCOORD", RenderContext::Format::Vec2},
		{"COLOR", RenderContext::Format::u32_unorm},
	});

	im_matrix = RenderContext::createShaderConstant(sizeof(VERTEX_CONSTANT_BUFFER));

	createFontsTexture();
}


void ImGuiImpl::shutdown() {
	invalidateDeviceObjects();
}

static const char *GetClipboardText(void *data) {
	return Platform::getClipboardText();
}

static void setClipboardText(void *data, const char *text) {
	Platform::setClipboardText(text);
}

static void onTextInput(const char *text) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharactersUTF8(text);	
}

void ImGuiImpl::init(PlatformWindow *window, Shader *shader) {
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = (int)Key::Tab;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = (int)Key::Left;
	io.KeyMap[ImGuiKey_RightArrow] = (int)Key::Right;
	io.KeyMap[ImGuiKey_UpArrow] = (int)Key::Up;
	io.KeyMap[ImGuiKey_DownArrow] = (int)Key::Down;
	io.KeyMap[ImGuiKey_PageUp] = (int)Key::PageUp;
	io.KeyMap[ImGuiKey_PageDown] = (int)Key::PageDown;
	io.KeyMap[ImGuiKey_Home] = (int)Key::Home;
	io.KeyMap[ImGuiKey_End] = (int)Key::End;
	io.KeyMap[ImGuiKey_Delete] = (int)Key::Delete;
	io.KeyMap[ImGuiKey_Backspace] = (int)Key::Backspace;
	io.KeyMap[ImGuiKey_Enter] = (int)Key::Enter;
	io.KeyMap[ImGuiKey_Escape] = (int)Key::Escape;
	io.KeyMap[ImGuiKey_A] = (int)Key::A;
	io.KeyMap[ImGuiKey_C] = (int)Key::C;
	io.KeyMap[ImGuiKey_V] = (int)Key::V;
	io.KeyMap[ImGuiKey_X] = (int)Key::X;
	io.KeyMap[ImGuiKey_Y] = (int)Key::Y;
	io.KeyMap[ImGuiKey_Z] = (int)Key::Z;
	im_shader = shader;
	// io.ImeWindowHandle = window->platform_handle;
	
	io.SetClipboardTextFn = setClipboardText;
	io.GetClipboardTextFn = GetClipboardText;
	
	io.MouseDrawCursor = false;
	Platform::text_input_event.addCallback(onTextInput);
}

void ImGuiImpl::newFrame(PlatformWindow *window, float delta) {
	ProfileFunc;
	if (!im_font_sampler.handle) createDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	u32 w, h;
	int display_w, display_h;
	Platform::getWindowSize(window, w, h);
	display_w = w;
	display_h = h;
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	io.DeltaTime = delta;

	for(int i = 0; i < (int)Key::KeyCount; i++) {
		io.KeysDown[i] = Platform::getKeyDown((Key)i);
	}
	
	io.KeyShift = Platform::getKeyDown(Key::LShift) || Platform::getKeyDown(Key::RShift);
	io.KeyCtrl = Platform::getKeyDown(Key::LCtrl) || Platform::getKeyDown(Key::RCtrl);
	io.KeyAlt = Platform::getKeyDown(Key::LAlt) || Platform::getKeyDown(Key::RAlt);
	io.KeySuper = Platform::getKeyDown(Key::LGui) || Platform::getKeyDown(Key::RGui);

	s32 mouse_x, mouse_y;
	Platform::getMousePosition(mouse_x, mouse_y);
	io.MousePos = ImVec2(mouse_x, mouse_y);


	// for(int i = 0; i < 3; i++) io.MouseDown[i] = Platform::getMouseDown((MouseButton)i);
	io.MouseDown[0] = Platform::getMouseDown(MouseButton::Left);
	io.MouseDown[1] = Platform::getMouseDown(MouseButton::Right);
	io.MouseDown[2] = Platform::getMouseDown(MouseButton::Middle);

	io.MouseWheel = Platform::getMouseWheel();

	ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
	if(cursor != previous_cursor) {
		Platform::setCursorType((CursorType)cursor);
		previous_cursor = cursor;
	}

	// Hide OS mouse cursor if ImGui is drawing it
	// Platform::setCursorVisible(io.MouseDrawCursor);

	// Start the frame
	ImGui::NewFrame();
}