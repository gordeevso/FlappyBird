cmake_minimum_required(VERSION 3.4.1)

add_library( FlappyPelican SHARED
             src/main/cpp/GLContextWrapper.cpp
             src/main/cpp/LogWrapper.cpp
             src/main/cpp/TouchDetector.cpp
             src/main/cpp/FlappyEngine.cpp
             src/main/cpp/Main.cpp )

add_library(app-glue STATIC
            libs/native_app_glue/android_native_app_glue.c)

add_library(libpng STATIC
            libs/lpng1629/png.c
            libs/lpng1629/pngerror.c
            libs/lpng1629/pngget.c
            libs/lpng1629/pngmem.c
            libs/lpng1629/pngpread.c
            libs/lpng1629/pngread.c
            libs/lpng1629/pngrio.c
            libs/lpng1629/pngrtran.c
            libs/lpng1629/pngrutil.c
            libs/lpng1629/pngset.c
            libs/lpng1629/pngtrans.c
            libs/lpng1629/pngwio.c
            libs/lpng1629/pngwrite.c
            libs/lpng1629/pngwtran.c
            libs/lpng1629/pngwutil.c)

find_library(android
             app-glue
             libpng
             EGL
             GLESv3
             log)

target_include_directories(FlappyPelican PRIVATE
                           libs/native_app_glue
                           libs/lpng1629
                           libs/glm)

target_link_libraries(FlappyPelican
                      android
                      app-glue
                      libpng
                      EGL
                      GLESv3
                      log)