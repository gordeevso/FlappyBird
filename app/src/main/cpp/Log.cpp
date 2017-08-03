#include "Log.h"
#include <android/log.h>

char const APP_NAME [] = "FlappyPelican";

void Log::info(const char* pMessage, ...)
{
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_INFO, APP_NAME, pMessage, varArgs);
    __android_log_print(ANDROID_LOG_INFO, APP_NAME, "\n");
    va_end(varArgs);
}

void Log::error(const char* pMessage, ...)
{
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_ERROR, APP_NAME, pMessage, varArgs);
    __android_log_print(ANDROID_LOG_ERROR, APP_NAME, "\n");
    va_end(varArgs);
}

void Log::warn(const char* pMessage, ...)
{
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_WARN, APP_NAME, pMessage, varArgs);
    __android_log_print(ANDROID_LOG_WARN, APP_NAME, "\n");
    va_end(varArgs);
}

void Log::debug(const char* pMessage, ...)
{
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_DEBUG, APP_NAME, pMessage, varArgs);
    __android_log_print(ANDROID_LOG_DEBUG, APP_NAME, "\n");
    va_end(varArgs);
}