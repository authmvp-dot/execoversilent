#pragma once
#include <windows.h>

namespace adb {
    inline void KillEmulatorAndAdbOnExit() {}
    namespace MemoryUtils {
        inline uintptr_t ogPhysRead = 0;
    }
}
