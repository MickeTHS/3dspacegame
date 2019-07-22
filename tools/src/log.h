#ifndef __WINGAME_LOG_
#define __WINGAME_LOG_

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <stdarg.h>
#endif


#define loginit() Log::init()
#define logclose() Log::close()
#ifdef WIN32
#define logmsg(msg, ...) Log::write((msg), __VA_ARGS__)
#define vslog(msg, ...) Log::WinOutput((msg), __VA_ARGS__)
#else
#define logmsg(msg, ...) Log::write((msg), ##__VA_ARGS__)
#endif

struct DebugLog {
	static void init(std::string filename) {
		DebugLog::get_instance().debug_file = fopen(filename.c_str(), "w");
	}

	static void close() {
		fclose(DebugLog::get_instance().debug_file);
	}

	static void log(const char *msg, ...) {
		va_list args;
		va_start(args, msg);
		vfprintf(DebugLog::get_instance().debug_file, msg, args);
		va_end(args);
	}

	static DebugLog& get_instance() {
		static DebugLog instance;

		return instance;
	}

	FILE* debug_file;
};

struct Log {
	static void init() {
		log_file = fopen("ee.log", "w");
	}
	static void close() {
		fclose(log_file);
	}
	static void write(const char *msg, ...) {
		va_list args;
		va_start(args, msg);
		vfprintf(log_file, msg, args);
		va_end(args);
	}

	static FILE *log_file;
};

#endif
