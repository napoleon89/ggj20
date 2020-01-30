#ifndef IMGUI_IMPL_H
#define IMGUI_IMPL_H

struct RenderContext;
struct Platform;
struct PlatformWindow;
struct Shader;

struct ImGuiImpl {
	static void init(PlatformWindow *window, Shader *shader);
	static void shutdown();
	static void render();
	static void newFrame(PlatformWindow *window, float delta);
	
};

#endif // IMGUI_IMPL_H