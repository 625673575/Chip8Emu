#ifndef _CHIP8_TYPES_
#define _CHIP8_TYPES_

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <locale>
#include <codecvt>
#include <iomanip>
using u8 = uint8_t;
using u16 = uint16_t;
using s8 = int8_t;
using s16 = int16_t;

#define FONT_COUNT 0x10
#define FULL_RAM_SIZE 0x1000
#define MID_MEM_SIZE 0x600
#define PROGRAM_START_RAM_ADDR 0x200
struct string_util {
	static u16 hex2int(const std::string& hexStr)
	{
		char* offset;
		if (hexStr.length() > 2)
		{
			if (hexStr[0] == '0' && hexStr[1] == 'x')
			{
				return (u16)strtol(hexStr.c_str(), &offset, 0);
			}
		}
		return 0;
	}
	/** Converts a size in bytes to a human readable string:
		- prints bytes (B) if size < 512 bytes
		- prints kilobytes (KB) if size < 512 kilobytes
		- prints megabytes (MB) if size < 512 megabytes
		- otherwise prints gigabytes (GB)
		\param[in] size Size in bytes
		\return Returns a human readable string.
	*/
	static std::string formatByteSize(size_t size)
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(1);

		if (size < 512) oss << size << "B";
		else if (size < 512 * 1024) oss << (size / 1024.0) << "KB";
		else if (size < 512 * 1024 * 1024) oss << (size / (1024.0 * 1024.0)) << "MB";
		else oss << (size / (1024.0 * 1024.0 * 1024.0)) << "GB";

		return oss.str();
	}

	/** Convert an ASCII string to a UTF-8 wstring
	*/
	static std::wstring string_2_wstring(const std::string& s)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
		std::wstring ws = cvt.from_bytes(s);
		return ws;
	}

	/** Convert a UTF-8 wstring to an ASCII string
	*/
	static std::string wstring_2_string(const std::wstring& ws)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
		std::string s = cvt.to_bytes(ws);
		return s;
	}
};
#ifndef		CHECK
#define CHECK(condition)								\
    if(!(condition))Log::Fatal("Check failed: " #condition \
    " at %s, line %d ,\n",__FILE__,__LINE__);
#endif

#ifndef CHECK_NOTNULL
#define CHECK_NOTNULL (pointer)\
    if ((pointer) == nullptr) Log::Fatal( #pointer " Can't be NULL");
#endif
#ifndef LOGLEVEL_H
#define LOGLEVEL_H
enum class LogLevel :int64_t {
	Fatal = -1,
	Warning = 0,
	Info = 1,
	Debug = 2,
};
#endif
class log {
private:
	static LogLevel& GetLevel() { static LogLevel level = LogLevel::Debug; return level; }
	static void Write(LogLevel level, const char* level_str, const char* format, va_list val) {
		if (level <= GetLevel()) {
			printf("[%s] ", level_str);
			vprintf(format, val);
			printf("\n");
			fflush(stdout);
		}
	}
public:
	static void Fatal(const char* format, ...) {
		va_list val;
		va_start(val, format);
		fprintf(stderr, "Fatal");
		vfprintf(stderr, format, val);
		fprintf(stderr, "\n");
		fflush(stderr);
		va_end(val);
		exit(1);
	}

	static void Debug(const char* format, ...) {
		va_list val;
		va_start(val, format);
		Write(LogLevel::Debug, "Debug", format, val);
		va_end(val);
	}

	static void Info(const char* format, ...) {
		va_list val;
		va_start(val, format);
		Write(LogLevel::Info, "Info", format, val);
		va_end(val);
	}

	static void Warning(const char* format, ...) {
		va_list val;
		va_start(val, format);
		Write(LogLevel::Warning, "Warning", format, val);
		va_end(val);
	}
};

#endif