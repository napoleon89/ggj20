#pragma once

#include <stdio.h>
#include <string>

struct Log {

	enum class TerminalColor {
		Default = 0,
		ForegroundBlack = 30,
		ForegroundRed = 31,
		ForegroundGreen = 32,
		ForegroundYellow = 33,
		ForegroundBlue = 34,
		ForegroundMagenta = 35,
		ForegroundCyan = 36,
		ForegroundWhite = 37,
		ForegroundExtended = 38,
		ForegroundDefault = 39,
		BackgroundBlack = 40,
		BackgroundRed = 41,
		BackgroundGreen = 42,
		BackgroundYellow = 43,
		BackgroundBlue = 44,
		BackgroundMagenta = 45,
		BackgroundCyan = 46,
		BackgroundWhite = 47,
		BackgroundExtended = 48,
		BackgroundDefault = 49,
		BrightForegroundBlack = 90,
		BrightForegroundRed = 91,
		BrightForegroundGreen = 92,
		BrightForegroundYellow = 93,
		BrightForegroundBlue = 94,
		BrightForegroundMagenta = 95,
		BrightForegroundCyan = 96,
		BrightForegroundWhite = 97,
		BrightBackgroundBlack = 100,
		BrightBackgroundRed = 101,
		BrightBackgroundGreen = 102,
		BrightBackgroundYellow = 103,
		BrightBackgroundBlue = 104,
		BrightBackgroundMagenta = 105,
		BrightBackgroundCyan = 106,
		BrightBackgroundWhite = 107,
	};

	template<typename ... Args>
	inline static void info(const char *format, const Args &...args) {
		std::string log_line = "Info: " + std::string(format);
		print(log_line.c_str(), TerminalColor::Default, args...);
	}

	template<typename ... Args>
	inline static void success(const char *format, const Args &...args) {
		std::string log_line = "Success: " + std::string(format);
		print(log_line.c_str(), TerminalColor::ForegroundGreen, args...);
	}

	template<typename ... Args>
	inline static void warning(const char *format, const Args &...args) {
		std::string log_line = "Warning: " + std::string(format);
		print(log_line.c_str(), TerminalColor::ForegroundYellow, args...);
	}

	template<typename ... Args>
	inline static void error(const char *format, const Args &...args) {
		std::string log_line = "Error: " + std::string(format);
		print(log_line.c_str(), TerminalColor::ForegroundRed, args...);
	}

	template<typename ... Args>
	inline static void print(const char *format, const TerminalColor &color, const Args &...args) {
		std::string log_line = "\x1b[" + std::to_string((int)color) + "m" + std::string(format) + "\n";
		printf(log_line.c_str(), args...);
		printf("\x1b[0m");
	}
};