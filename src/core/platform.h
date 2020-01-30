#pragma once

#include <core/std.h>
#include <string>
#include <core/utils/event.h>

template<typename T>
struct DynamicArray;

struct FileData {
	char *contents;
	u64 size;	
};

struct FileTime {
	u32 low_date_time;
	u32 high_date_time;

	u64 getAsU64() const {
		u64 result = high_date_time;
		result = result << 32;
		result |= low_date_time;
		return result;
	}
	void setFromU64(const u64 input) {
		low_date_time = (u32)(input & 0xFFFFFFFF);
		high_date_time = (u32)((input >> 32) & 0xFFFFFFFF);
	}
};

struct FileInfo {
	std::string filename;
	std::string full_path;
	FileTime last_write_time;
};

struct PlatformWindow {
	void *handle;
	bool open;
};

enum class MouseButton {
	Left,
	Middle,
	Right,	
};

enum class Key {
	Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	NumPad0, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
	Left, Right, Up, Down, RCtrl, LCtrl, LShift, RShift, Escape, Enter, Tab, LAlt, RAlt,
	PageUp, PageDown, Home, End, Delete, Backspace, LGui, RGui, Space,
	KeyCount
};

enum class ControllerAxis {
	LeftX,
	LeftY,
	RightX,
	RightY,
	Count,
};

enum class ControllerButton {
	A,
	B,
	X,
	Y,
	Count
};

enum class CursorType {
    Arrow,
    TextInput,
    ResizeAll,
    ResizeNS,
    ResizeEW,
    ResizeNESW,
    ResizeNWSE,
    Hand,
    NotAllowed,
};

typedef void (TextInputFunc)(const char *);
typedef void* ControllerHandle;
typedef void* ThreadHandle;
typedef s32 (ThreadFunction)(void* data);

struct Platform {
	static Event<u32, u32> window_resize_event;
	static Event<const char *> text_input_event;
	static Event<> quit_event;
	static Event<const u32> controller_added_event;
	
	static bool init();
	static void initColoredConsole();
	static void uninit();

	static std::string getBasePath();

	static bool copyFile(char *a, char *b);
	static FileTime getLastWriteTime(char *file);
	static u32 compareFileTime(FileTime *a, FileTime *b);
	
	static const DynamicArray<FileInfo> getDirectoryContents(const std::string &folder_path, const std::string &search_pattern = "*.*");
	static FileData readEntireFile(const char *filename);
	static void writeStructureToFile(const char *filename, void *structure, s32 size);
	static void *openFileForWriting(const char *filename);
	static void closeOpenFile(void *file);
	static void writeToFile(void *file, void *structure, s32 size);
	static void writeToFile(void *file, const std::string &str);
	static void writeLineToFile(void *file, const std::string &str);
	
	static void *reAlloc(void *data, u64 size);
	static void *alloc(u64 size);
	static void free(void *data);
	static void memCopy(void* src, void* dest, u64 size);
	static void memSet(void* dest, u8 value, u64 size);
	
	// NOTE(nathan): an error message box
	static void error(char *err);
		
	static PlatformWindow *createWindow(const char *title, u32 width, u32 height, bool centerd, bool resizable);
	static void destroyWindow(PlatformWindow *window);
	static void processEvents(PlatformWindow *window);
	static void getWindowSize(PlatformWindow *window, u32 &x, u32 &y);
	static bool isWindowFullscreen(PlatformWindow *window);
	static void setWindowFullscreen(PlatformWindow *window, bool fullscreen);
	static void setWindowTitle(PlatformWindow *window, const char *title);
	
	static void sleepMS(u32 ms);
	
	static void *loadLibrary(const char *name);
	static void unloadLibrary(void *library);
	static void *loadFunction(void *library, const char *name);
	
	static char *getExePath();
	
	static u64 getPerformanceCounter();
	static u64 getPerformanceFrequency();
	
	// static u32 getControllerCount();
	static bool getKeyDown(Key key);
	static bool getMouseDown(MouseButton button);
	static void getMousePosition(s32 &x, s32 &y); // NOTE(nathan): relative to window
	static void setMousePosition(s32 x, s32 y); // NOTE(nathan): relative to window
	static f32 getMouseWheel();
	static void setCursorVisible(bool visible);
	static void setCursorType(const CursorType type);

	static ControllerHandle openController(const u32 index);
	static f32 getControllerAxis(const ControllerHandle controller, const ControllerAxis axis);
	static bool getControllerButton(const ControllerHandle controller, const ControllerButton button);
	
	static const char *getClipboardText();
	static void setClipboardText(const char *text);

	static PlatformWindow *getCurrentWindow();

	static ThreadHandle createThread(ThreadFunction* function, const std::string name, void* data);
	static s32 waitThread(ThreadHandle thread);
};
