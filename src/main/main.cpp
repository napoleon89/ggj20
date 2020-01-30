#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <variant>
#include <atomic>


// #define STB_RECT_PACK_IMPLEMENTATION
// #include <core/stb_rect_pack.h>
// #define STB_TRUETYPE_IMPLEMENTATION
// #include <core/stb_truetype.h>

#include <imgui/imgui.h>

#include <core/std.h>
#include <core/math.h>
#include <core/platform.h>
#include <core/render_context.h>
#include <core/collections/dynamic_array.h>
#include <core/logger.h>
#include <core/utils/args.h>
#include <core/utils/event.h>
#include <core/memory.h>
#include <core/collections/buffer.h>
#include <core/stb_image.h>

#include <engine/timer.h>
#include <engine/input.h>
#include <engine/audio.h>
#include <engine/imgui_impl.h>
#include <engine/audio.h>
#include <engine/profiler.h>
#include <engine/renderer.h>
#include <engine/debug_renderer.h>
#include <engine/config.h>
#include <engine/allocators.h>
#include <engine/shaders.h>
#include <engine/animation2d.h>
#include <engine/json.h>
#include <engine/reflection.h>

#include <game/game.h>
#include <game/assets.h>
#include <game/memory.h>
#ifdef PP_EDITOR
#include <game/editor.h>
#else
#endif // PP_EDITOR

static void applyCorporateTheme() {
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border]                 = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator]              = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	// colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	// colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_DockingEmptyBg]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_Tab]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_TabActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_TabUnfocused]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_DockingPreview]     = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

}

StackAllocator g_frame_stack;
std::atomic<bool> g_running = false;

int main(int arg_count, char* args[]) {
	if(!Platform::init()) {
		Platform::error("Couldn't init platform");
	}

	ConfigFile config;
	config.parseFromFile("data/settings.cfg");
	config.fillStaticStructFromMemory<Settings>();

	u32 window_width = Settings::window_size.x;
	u32 window_height = Settings::window_size.y;

	PlatformWindow* window = Platform::createWindow("ggj20", window_width, window_height, true, true);

	Platform::setWindowFullscreen(window, Settings::fullscreen);

	if(window->handle == 0) {
		Platform::error("Couldn't create window");
	}
	s32 refresh_rate = 60;
	RenderContext::init(window_width, window_height, refresh_rate, window);

	AudioEngine audio_engine = {};
	audio_engine.init();

	Timer frame_timer = Timer();
	g_running = true;

	Platform::quit_event.addCallback([]()
	{
		g_running = false; 
	});

	MemoryStore mem_store = {};
	mem_store.game_memory = {0, Megabytes(8)};
	mem_store.asset_memory = {0, Megabytes(8)};
	mem_store.frame_memory = {0, Megabytes(2)};
	mem_store.debug_memory = {0, Megabytes(8)};

	u64 total_memory_size = mem_store.calcTotalSize();
	mem_store.memory = (void *)Memory_alloc(total_memory_size);
	memset(mem_store.memory, 0, total_memory_size); // NOTE(nathan): this might need removing for performance??
	u8* byte_walker = (u8 *)mem_store.memory;
	for(int i = 0; i < MEMORY_STORE_COUNT; i++) {
		mem_store.blocks[i].memory = (void *)byte_walker;
		byte_walker += mem_store.blocks[i].size;
	}

	g_frame_stack.initialize(mem_store.frame_memory.memory, mem_store.frame_memory.size);

	f32 target_seconds_per_frame = 1.0f / (f32)refresh_rate;
	f32 delta = target_seconds_per_frame;

	Assets::shaders.sprite = loadShaderFromFile("sprite");
	Assets::shaders.imgui = loadShaderFromFile("imgui");
	Assets::shaders.editor = loadShaderFromFile("editor");
	
	ImGui::CreateContext();
	ImGuiImpl::init(window, &Assets::shaders.imgui);

	const std::string base_path = Platform::getBasePath();

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	const std::string imgui_path = base_path + "imgui.ini";
	const char* imgui_path_c = imgui_path.c_str();
	io.IniFilename = imgui_path_c;
	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 0.0f;
	
	style.FramePadding = ImVec2(12.0f, 5.0f);
	style.ScrollbarSize = 20.0f;
	style.GrabMinSize = 16.0f;
	style.FrameBorderSize = 1.0f;
	style.TabBorderSize = 1.0f;
	
	applyCorporateTheme();

	io.ConfigDockingWithShift = false;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	GameState* game = new(mem_store.game_memory.memory) GameState();

#ifdef PP_EDITOR
	EditorState *editor = new(mem_store.debug_memory.memory) EditorState();
	editor->init();
#endif PP_EDITOR

	Renderer renderer = {};
	renderer.initResources();
	
	game->init(window, &audio_engine);
	Input::init();
	
	while(g_running) {
		frame_timer.start();
		Profiler::startFrame();

		g_frame_stack.clear();

		{
			ProfileScope("Pump Events");
			Platform::processEvents(window);	
		}

		
		DebugRenderQueue::startframe();
		Input::processKeys();
		ImGuiImpl::newFrame(window, delta);

		u32 width, height;
		Platform::getWindowSize(window, width, height);
		RenderContext::setViewport(0, 0, (s32)width, (s32)height, 0.0f, 1.0f);

		if(Input::isKeyDownOnce(Key::Escape)) {
			g_running = false;
		}

		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::SetNextWindowPos(ImVec2(20, 20));
    	ImGui::Begin("Debug Stuff", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | 
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | 
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		{
			ImGui::Text("Frame time: %.3f", delta * 1000.0f);
		}
		ImGui::End();

		if(Input::isKeyDownOnce(Key::F11)) {
			Platform::setWindowFullscreen(window, !Platform::isWindowFullscreen(window));
		}

#ifdef PP_EDITOR
		if(Input::isKeyDownOnce(Key::F4)) {
			editor->enabled = !editor->enabled;
		}
		
		Profiler::paused = editor->enabled;
		
		if(editor->enabled) {
			if(!editor->was_enabled) {
				editor->onShow(game);
			}
			editor->update(delta, window);
		} else {
			if(editor->was_enabled) {
				editor->onHide();
			}
#endif PP_EDITOR
		game->update(delta, window);

#ifdef PP_EDITOR
		}

		editor->was_enabled = editor->enabled;
#endif PP_EDITOR


		RenderFrame* new_frame = renderer.getNewRenderFrame();

		game->render(new_frame, window, delta);
		
#ifdef PP_EDITOR
		if(editor->enabled) {
			editor->render(window, delta);
		}
#endif PP_EDITOR
		
		renderer.processRenderFrame(new_frame);
		
		{
			ProfileScope("Frame Sleep");
			u64 work_counter = frame_timer.getWallClock();
			f32 work_seconds_elapsed = frame_timer.getSecondsElapsed();
			
			f32 seconds_elapsed_for_frame = work_seconds_elapsed;
			if(seconds_elapsed_for_frame < target_seconds_per_frame) {
				u32 sleep_ms = (u32)((1000.0f * (target_seconds_per_frame - seconds_elapsed_for_frame)) - 1);
				if(sleep_ms > 0)
					Platform::sleepMS(sleep_ms);
				while(seconds_elapsed_for_frame < target_seconds_per_frame) {
					seconds_elapsed_for_frame = frame_timer.getSecondsElapsed();
				}
			} else {
				// missed frame
				Log::warning("Missed frame");
			}
		}
		
		u64 end_counter = frame_timer.getWallClock();
		delta = frame_timer.getSecondsElapsed();
		Input::endFrame();
		Profiler::finishFrame();
	}

	// Platform::waitThread(hotload_thread);
	
	audio_engine.uninit();
	ImGuiImpl::shutdown();
	ImGui::DestroyContext();
	RenderContext::uninit();
	Platform::destroyWindow(window);


	Platform::uninit();
	return 0;
}
