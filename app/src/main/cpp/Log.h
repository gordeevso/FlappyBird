#pragma once


class Log
{
public:
    static void error(const char* pMessage, ...);
    static void warn(const char* pMessage, ...);
    static void info(const char* pMessage, ...);
    static void debug(const char* pMessage, ...);
};

#ifndef NDEBUG
#define Log_debug(...) Log::debug(__VA_ARGS__)
#else
#define Log_debug(...)
#endif

