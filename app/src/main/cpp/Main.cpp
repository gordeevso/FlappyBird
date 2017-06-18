#include <android_native_app_glue.h>

#include "Android.h"
#include "FlappyEngine.h"
#include "LogWrapper.h"

void android_main(struct android_app* androidApp) {

    LogWrapper::debug("path = %s",androidApp->activity->internalDataPath);
    LogWrapper::debug("path = %s",androidApp->activity->externalDataPath);
    Android::GetInstance().Init(androidApp);
    FlappyEngine::GetInstance().Run();
}
