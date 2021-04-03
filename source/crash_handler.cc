#if defined(OS_WINDOWS)

    #include <atomic>
    #include <cstring>

    #include <Windows.h>
    #include <tlhelp32.h>

    #include <Memoryapi.h>

    #include <common/auto_release.h>

class CrashHandler {
  private:
    using SetUnhandledExceptionFilterFuncType
        = LPTOP_LEVEL_EXCEPTION_FILTER (*)(LPTOP_LEVEL_EXCEPTION_FILTER);

  private:
    static CrashHandler _instance; ///< Instance.
    SetUnhandledExceptionFilterFuncType
        m_realSetUnhandledExceptionFilter; ///< Real
                                           ///< SetUnhandledExceptionFilter().
    ::std::atomic<LPTOP_LEVEL_EXCEPTION_FILTER>
        m_topLevelExceptionFiler; ///< Top level exception filter.

  public:
    /**
     * @brief	Constructor.
     */
    CrashHandler();

    /**
     * @brief	Destructor.
     */
    virtual ~CrashHandler();

  private:
    /**
     * @brief		Exception handler.
     *
     * @param[in]	exception		Exception pointser.
     */
    static LONG WINAPI onCrash(struct _EXCEPTION_POINTERS *exceptions);

    /**
     * @brief		Fake SetUnhandledExceptionFilter().
     *
     * @param[in]	exception		Exception pointser.
     */
    static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI fakeSetUnhandledExceptionFilter(
        LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

    /**
     * @@brief		Enable EAT hook.
     *
     * @return      On success, the method returns \c true. Otherwise returns
     *              \c false.
     */
    bool enableEATHook();

    /**
     * @@brief		Disable EAT hook.
     */
    void disableEATHook();

    /**
     * @@brief		Enable IAT hook.
     *
     * @return      On success, the method returns \c true. Otherwise returns
     *              \c false.
     */
    bool enableIATHook();
};

CrashHandler CrashHandler::_instance; ///< Instance.

/**
 * @brief	Constructor.
 */
CrashHandler::CrashHandler() :
    m_realSetUnhandledExceptionFilter(nullptr),
    m_topLevelExceptionFiler(nullptr)
{
    m_realSetUnhandledExceptionFilter
        = reinterpret_cast<SetUnhandledExceptionFilterFuncType>(
            ::GetProcAddress(GetModuleHandle("KERNEL32.dll"),
                             "SetUnhandledExceptionFilter"));
    if (m_realSetUnhandledExceptionFilter == nullptr) {
        ::MessageBoxA(NULL, "Failed to find SetUnhandledExceptionFilter().",
                      "Unknow Error", MB_OK | MB_ICONERROR);
    }

    // Enable EAT hook.
    if (! this->enableEATHook()) {
        ::MessageBoxA(NULL,
                      "Failed to make EAT hook, the crash handler is disabled.",
                      "EAT Hook Failed", MB_OK | MB_ICONERROR);
        return;
    }

    // Enable IAT hook.
    if (! this->enableIATHook()) {
        ::MessageBoxA(NULL,
                      "Failed to make IAT hook, the crash handler is disabled.",
                      "IAT Hook Failed", MB_OK | MB_ICONERROR);
        this->disableEATHook();
        return;
    }

    // Register crash handler.
    ::SetUnhandledExceptionFilter(&CrashHandler::onCrash);
}

/**
 * @brief	Destructor.
 */
CrashHandler::~CrashHandler() {}

/**
 * @brief		Exception handler.
 */
LONG CrashHandler::onCrash(struct _EXCEPTION_POINTERS *exceptions)
{
    return EXCEPTION_CONTINUE_SEARCH;
}

/**
 * @brief		Fake SetUnhandledExceptionFilter().
 *
 * @param[in]	exception		Exception pointser.
 */
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI
    CrashHandler::fakeSetUnhandledExceptionFilter(
        LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    ::MessageBoxA(NULL, "fake", "fake", MB_OK);
    return _instance.m_topLevelExceptionFiler.exchange(
        lpTopLevelExceptionFilter);
}

/**
 * @@brief		Enable EAT hook.
 */
bool CrashHandler::enableEATHook()
{
    return true;
}

/**
 * @@brief		Disable EAT hook.
 */
void CrashHandler::disableEATHook() {}

/**
 * @@brief		Enable IAT hook.
 */
bool CrashHandler::enableIATHook()
{
    // Traverse loaded modules.
    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    AutoRelease<HANDLE> releaseSnapshot(snapshot, [](HANDLE &hnd) -> void {
        ::CloseHandle(hnd);
    });

    MODULEENTRY32 entery;
    for (BOOL hasNext = ::Module32First(snapshot, &entery); hasNext;
         hasNext      = ::Module32Next(snapshot, &entery)) {
        uint8_t *moduleBaseAddr
            = reinterpret_cast<uint8_t *>(entery.modBaseAddr);
        // Find DOS header.
        PIMAGE_DOS_HEADER dosHeader
            = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBaseAddr);

        if (dosHeader == NULL) {
            return false;
        }

        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            return false;
        }

        // Find NT header.
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
            moduleBaseAddr + dosHeader->e_lfanew);

        if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
            return false;
        }

        // Find optional header.
        PIMAGE_OPTIONAL_HEADER optionalHeader = &(ntHeader->OptionalHeader);

        // Find import descriptors.
        for (PIMAGE_IMPORT_DESCRIPTOR importDescriptor
             = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
                 moduleBaseAddr
                 + optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                       .VirtualAddress);
             importDescriptor->Name != 0; ++importDescriptor) {
            LPCTSTR dllName = reinterpret_cast<LPCTSTR>(
                moduleBaseAddr + importDescriptor->Name);

            if (::_stricmp(dllName, "KERNEL32.dll") == 0) {
                PIMAGE_THUNK_DATA originalThunkData
                    = reinterpret_cast<PIMAGE_THUNK_DATA>(
                        moduleBaseAddr + importDescriptor->OriginalFirstThunk);
                PIMAGE_THUNK_DATA thunkData
                    = reinterpret_cast<PIMAGE_THUNK_DATA>(
                        moduleBaseAddr + importDescriptor->FirstThunk);
                for (; originalThunkData->u1.Ordinal != 0;
                     ++originalThunkData, ++thunkData) {
                    if (! (originalThunkData->u1.Ordinal
                           & IMAGE_ORDINAL_FLAG)) {
                        PIMAGE_IMPORT_BY_NAME importByName
                            = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                                moduleBaseAddr + originalThunkData->u1.Ordinal);
                        if (::_stricmp(importByName->Name,
                                       "SetUnhandledExceptionFilter")
                            == 0) {
                            DWORD  oldMemAttr    = 0;
                            SIZE_T written       = 0;
                            void * targetAddress = &(thunkData->u1.Ordinal);
                            // Make memory writable.
                            ::VirtualProtect(targetAddress, sizeof(void *),
                                             PAGE_READWRITE, &oldMemAttr);

                            // Set hook.
                            ::WriteProcessMemory(
                                ::GetCurrentProcess(), targetAddress,
                                &CrashHandler::fakeSetUnhandledExceptionFilter,
                                sizeof(void *), &written);

                            // Resume memory attribute.
                            ::VirtualProtect(targetAddress, sizeof(void *),
                                             oldMemAttr, &oldMemAttr);

                            ::MessageBoxA(NULL, "hook", "hook", MB_OK);
                        }
                    }
                }
            }
        }
    }

    return true;
}

#endif
