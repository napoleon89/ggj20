#ifdef _WIN32
#include <windows.h>
#include <stdio.h>

#include <filesystem>
#include <core/collections/dynamic_array.h>
#include <core/utils/event.h>

void Platform::initColoredConsole() {
	HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(h_out, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(h_out, dwMode);
}

bool Platform::copyFile(char *a, char *b) {
	bool result = CopyFile(a, b, FALSE);
	return result;
}

FileTime Platform::getLastWriteTime(char *filename) {
	FileTime last_write_time = {0};
	WIN32_FIND_DATA find_data;
	HANDLE find_handle = FindFirstFileA(filename, &find_data);
	if(find_handle != INVALID_HANDLE_VALUE) {
		last_write_time.low_date_time = find_data.ftLastWriteTime.dwLowDateTime;
		last_write_time.high_date_time = find_data.ftLastWriteTime.dwHighDateTime;
		FindClose(find_handle);
	}
	
	return last_write_time;
}

u32 Platform::compareFileTime(FileTime *a, FileTime *b) {
	FILETIME f;
	f.dwLowDateTime = a->low_date_time;
	f.dwHighDateTime = a->high_date_time;
	FILETIME g;
	g.dwLowDateTime = b->low_date_time;
	g.dwHighDateTime = b->high_date_time;
	return CompareFileTime(&f, &g);
}

const DynamicArray<FileInfo> Platform::getDirectoryContents(const std::string &folder_path, const std::string &search_pattern /* = "*.*" */) {

	WIN32_FIND_DATAA find_data;
	HANDLE file_handle = FindFirstFile((folder_path + "/" + search_pattern).c_str(), &find_data);

	DynamicArray<FileInfo> result;
	do {
		if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			FileInfo info = {};
			info.filename = find_data.cFileName;
			info.full_path = folder_path + "/" + info.filename;
			info.last_write_time.low_date_time = find_data.ftLastWriteTime.dwLowDateTime;
			info.last_write_time.high_date_time = find_data.ftLastWriteTime.dwHighDateTime;
			result.add(info);
		}
	} while(FindNextFile(file_handle, &find_data));

	FindClose(file_handle);
	return result;
}
#endif _WIN32