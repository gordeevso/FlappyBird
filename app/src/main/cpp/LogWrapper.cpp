#include "LogWrapper.h"
#include <android/log.h>

char const APP_NAME [] = "FlappyPelican";

void LogWrapper::info(const char* pMessage, ...)
{
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_INFO, APP_NAME, pMessage, varArgs);
    __android_log_print(ANDROID_LOG_INFO, APP_NAME, "\n");
    va_end(varArgs);
}

void LogWrapper::error(const char* pMessage, ...)
{
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_ERROR, APP_NAME, pMessage, varArgs);
    __android_log_print(ANDROID_LOG_ERROR, APP_NAME, "\n");
    va_end(varArgs);
}

void LogWrapper::warn(const char* pMessage, ...)
{
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_WARN, APP_NAME, pMessage, varArgs);
    __android_log_print(ANDROID_LOG_WARN, APP_NAME, "\n");
    va_end(varArgs);
}

void LogWrapper::debug(const char* pMessage, ...)
{
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_DEBUG, APP_NAME, pMessage, varArgs);
    __android_log_print(ANDROID_LOG_DEBUG, APP_NAME, "\n");
    va_end(varArgs);
}