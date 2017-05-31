cmake_minimum_required(VERSION 3.4.1)

add_library( FlappyPelican
             SHARED
             src/main/cpp/GLContextWrapper.cpp
             src/main/cpp/LogWrapper.cpp
             src/main/cpp/SensorManager.cpp
             src/main/cpp/FlappyEngine.cpp
             src/main/cpp/Main.cpp )

add_library(app-glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)


find_library(android
             app-glue
             EGL
             GLESv3
             log)

target_include_directories(FlappyPelican PRIVATE
                           ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(FlappyPelican
                      android
                      app-glue
                      EGL
                      GLESv3
                      log)