#if defined(OS_WINDOWS)

    #include <atomic>
    #include <cstring>
    #include <ctime>

    #include <Windows.h>

    #include <Dbghelp.h>
    #include <Memoryapi.h>
    #include <shlobj.h>
    #include <tlhelp32.h>

    #include <common/auto_release.h>

class CrashHandler {

    using SetUnhandledExceptionFilterFuncType
        = LPTOP_LEVEL_EXCEPTION_FILTER (*)(LPTOP_LEVEL_EXCEPTION_FILTER);

    static CrashHandler _instance; ///< Instance.
    WCHAR
    m_dumpFilePath[32000]{}; ///< Buffer of the path of the dump file.
    WCHAR m_dumpFileMessage[32000
                            + 256]{}; ///< Buffer of the message of the dump file.
    SetUnhandledExceptionFilterFuncType
        m_realSetUnhandledExceptionFilter; ///< Real
                                           ///< SetUnhandledExceptionFilter().
    std::atomic<LPTOP_LEVEL_EXCEPTION_FILTER>
        m_topLevelExceptionFiler; ///< Top level exception filter.

  public:
    /**
     * @brief	Constructor.
     */
    CrashHandler();
    CrashHandler(const CrashHandler&) = delete;
    CrashHandler(CrashHandler&&) = delete;
    CrashHandler operator=(const CrashHandler&) = delete;
    CrashHandler operator=(CrashHandler&&) = delete;

    /**
     * @brief	Destructor.
     */
    virtual ~CrashHandler();

  private:
    /**
     * @brief		Exception handler.
     *
     * @param[in]	exceptionInfo       Exception information.
     */
    static LONG WINAPI onCrash(struct _EXCEPTION_POINTERS *exceptionInfo);

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
     * @@brief		Enable IAT hook.
     *
     * @return      On success, the method returns \c true. Otherwise returns
     *              \c false.
     */
    bool enableIATHook();

    /**
     * @brief		Save the dump file.
     *
     * @param[in]	exceptionInfo       Exception information.
     */
    void saveDump(struct _EXCEPTION_POINTERS *exceptionInfo);
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
            GetProcAddress(GetModuleHandle(L"kernel32.dll"),
                           "SetUnhandledExceptionFilter"));
    if (m_realSetUnhandledExceptionFilter == nullptr) {
        MessageBoxW(NULL, L"Failed to find SetUnhandledExceptionFilter().",
                    L"Unknow Error", MB_OK | MB_ICONERROR);
    }

    // Enable EAT hook.
    if (! this->enableEATHook()) {
        MessageBoxW(
            NULL, L"Failed to make EAT hook, the crash handler is disabled.",
            L"EAT Hook Failed", MB_OK | MB_ICONERROR);
    }

    // Enable IAT hook.
    if (! this->enableIATHook()) {
        MessageBoxW(
            NULL, L"Failed to make IAT hook, the crash handler is disabled.",
            L"IAT Hook Failed", MB_OK | MB_ICONERROR);
    }

    // Register crash handler.
    m_topLevelExceptionFiler
        = m_realSetUnhandledExceptionFilter(&CrashHandler::onCrash);
}

/**
 * @brief	Destructor.
 */
CrashHandler::~CrashHandler() = default;

/**
 * @brief		Exception handler.
 */
LONG CrashHandler::onCrash(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    // Ask user to save a dump file.
    if (MessageBoxW(
            NULL,
            L"X4 Station Calculator has been crashed, would you like to "
            "save a core dump?",
            L"Crash", MB_YESNO | MB_ICONERROR)
        == IDYES) {
        _instance.saveDump(exceptionInfo);
    }

    // Call default exception handler.
    if (_instance.m_topLevelExceptionFiler != nullptr) {
        return _instance.m_topLevelExceptionFiler.load()(exceptionInfo);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

/**
 * @brief		Fake SetUnhandledExceptionFilter().
 */
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI
    CrashHandler::fakeSetUnhandledExceptionFilter(
        LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    return _instance.m_topLevelExceptionFiler.exchange(
        lpTopLevelExceptionFilter);
}

/**
 * @@brief		Enable EAT hook.
 */
bool CrashHandler::enableEATHook()
{
    auto *moduleBaseAddr
        = reinterpret_cast<uint8_t *>(GetModuleHandleA("KERNEL32.dll"));

    // Find DOS header.
    auto dosHeader
        = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBaseAddr);

    if (dosHeader == NULL) {
        return false;
    }

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }

    // Find NT header.
    auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
        moduleBaseAddr + dosHeader->e_lfanew);

    if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }

    // Find optional header.
    PIMAGE_OPTIONAL_HEADER optionalHeader = &(ntHeader->OptionalHeader);

    // Find exprot descriptor.
    auto exportDirectory
        = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
            moduleBaseAddr
            + optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
                  .VirtualAddress);
    auto addressOfNames = reinterpret_cast<PDWORD>(
        moduleBaseAddr + exportDirectory->AddressOfNames);

    // Search for symbol name.
    for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
        const auto symbolName
            = reinterpret_cast<char*>(moduleBaseAddr + addressOfNames[i]);
        
        if (_stricmp(symbolName, "SetUnhandledExceptionFilter") == 0) {
            // Find RVA.
            auto addressOfNameOrdinals = reinterpret_cast<PWORD>(
                moduleBaseAddr + exportDirectory->AddressOfNameOrdinals);
            auto addressOfFunctions = reinterpret_cast<PDWORD>(
                moduleBaseAddr + exportDirectory->AddressOfFunctions);
            PDWORD targetRVA = addressOfFunctions + addressOfNameOrdinals[i];

            // Get real address.
            m_realSetUnhandledExceptionFilter
                = reinterpret_cast<SetUnhandledExceptionFilterFuncType>(
                    moduleBaseAddr + *targetRVA);

            DWORD  oldMemAttr = 0;
            SIZE_T written    = 0;

            // Make memory writable.
            VirtualProtect(targetRVA, sizeof(*targetRVA), PAGE_READWRITE,
                           &oldMemAttr);

            // Set hook.
            auto newRVA = static_cast<DWORD>(
                reinterpret_cast<UINT_PTR>(
                    &CrashHandler::fakeSetUnhandledExceptionFilter)
                - reinterpret_cast<UINT_PTR>(moduleBaseAddr));
            WriteProcessMemory(GetCurrentProcess(), targetRVA, &newRVA,
                               sizeof(newRVA), &written);

            // Resume memory attribute.
            VirtualProtect(targetRVA, sizeof(*targetRVA), oldMemAttr,
                           &oldMemAttr);

            return true;
        }
    }

    return false;
}

/**
 * @@brief		Enable IAT hook.
 */
bool CrashHandler::enableIATHook()
{
    // Traverse loaded modules.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    AutoRelease<HANDLE> releaseSnapshot(snapshot, [](HANDLE &hnd) -> void {
        CloseHandle(hnd);
    });

    MODULEENTRY32 entery;
    for (BOOL hasNext = ::Module32First(snapshot, &entery); hasNext;
         hasNext      = ::Module32Next(snapshot, &entery)) {
        auto*moduleBaseAddr
            = reinterpret_cast<uint8_t *>(entery.modBaseAddr);
        // Find DOS header.
        auto dosHeader
            = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBaseAddr);

        if (dosHeader == NULL) {
            return false;
        }

        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            return false;
        }

        // Find NT header.
        auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
            moduleBaseAddr + dosHeader->e_lfanew);

        if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
            return false;
        }

        // Find optional header.
        PIMAGE_OPTIONAL_HEADER optionalHeader = &(ntHeader->OptionalHeader);

        // Find import descriptors.
        for (auto importDescriptor
                 = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
                     moduleBaseAddr
                     + optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                     .VirtualAddress);
             importDescriptor->Name != 0; ++importDescriptor) {
            auto dllName = reinterpret_cast<LPCTSTR>(
                moduleBaseAddr + importDescriptor->Name);

            if (_wcsicmp(dllName, L"kernel32.dll") == 0) {
                auto originalThunkData
                    = reinterpret_cast<PIMAGE_THUNK_DATA>(
                        moduleBaseAddr + importDescriptor->OriginalFirstThunk);
                auto thunkData
                    = reinterpret_cast<PIMAGE_THUNK_DATA>(
                        moduleBaseAddr + importDescriptor->FirstThunk);
                for (; originalThunkData->u1.Ordinal != 0;
                     ++originalThunkData, ++thunkData) {
                    if (! (originalThunkData->u1.Ordinal
                           & IMAGE_ORDINAL_FLAG)) {
                        auto importByName
                            = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                                moduleBaseAddr + originalThunkData->u1.Ordinal);
                        if (strcmp(importByName->Name,
                                   "SetUnhandledExceptionFilter")
                            == 0) {
                            DWORD  oldMemAttr    = 0;
                            SIZE_T written       = 0;
                            void * targetAddress = &(thunkData->u1.Ordinal);
                            // Make memory writable.
                            VirtualProtect(targetAddress, sizeof(void *),
                                           PAGE_READWRITE, &oldMemAttr);

                            // Set hook.
                            
                            auto *hook_func = fakeSetUnhandledExceptionFilter;
                            WriteProcessMemory(GetCurrentProcess(),
                                               targetAddress, &hook_func,
                                               sizeof(hook_func), &written);

                            // Resume memory attribute.
                            VirtualProtect(targetAddress, sizeof(void *),
                                           oldMemAttr, &oldMemAttr);
                        }
                    }
                }
            }
        }
    }

    return true;
}

/**
 * @@brief		Save the dump file.
 */
void CrashHandler::saveDump(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    // Make path.
    time_t     timestamp   = time(NULL);
    struct tm *currentTime = localtime(&timestamp);
    WCHAR *    p           = m_dumpFilePath;
    wcscpy(p, L"\\\\?\\");
    p += 4;
    PWSTR myDocumentsPath;
    if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &myDocumentsPath)
        != S_OK) {
        MessageBoxW(NULL, L"Failed to get the path of \"My Documents\".",
                    L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    wcscpy(p, myDocumentsPath);
    while (*p != L'\0') {
        ++p;
    }
    _snwprintf(p,
               sizeof(m_dumpFilePath) / sizeof(WCHAR) - (p - m_dumpFilePath),
               L"\\x4-station-calc-%d-%.2d-%.2d_%.2d_%.2d_%.2d.dmp",
               currentTime->tm_year + 1900, currentTime->tm_mon + 1,
               currentTime->tm_mday, currentTime->tm_hour,
               currentTime->tm_min, currentTime->tm_sec);

    // Save dump file.
    HANDLE dumpFile
        = CreateFileW(m_dumpFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (dumpFile != INVALID_HANDLE_VALUE) {
        // Write dump file.
        MINIDUMP_EXCEPTION_INFORMATION exInfo;
        exInfo.ThreadId          = GetCurrentThreadId();
        exInfo.ExceptionPointers = exceptionInfo;
        exInfo.ClientPointers    = NULL;
        if (MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                              dumpFile, MiniDumpNormal, &exInfo, NULL,
                              NULL)) {
            _snwprintf(m_dumpFileMessage,
                       sizeof(m_dumpFileMessage) / sizeof(WCHAR),
                       L"Dump file \"%s\" saved.", m_dumpFilePath + 4);
            MessageBoxW(NULL, m_dumpFileMessage, L"Dump File Saved",
                        MB_OK | MB_ICONINFORMATION);
        } else {
            _snwprintf(
                m_dumpFileMessage, sizeof(m_dumpFileMessage) / sizeof(WCHAR),
                L"Failed to write dump file \"%s\".", m_dumpFilePath + 4);
            MessageBoxW(NULL, m_dumpFileMessage, L"Error",
                        MB_OK | MB_ICONERROR);
        }
        CloseHandle(dumpFile);
    } else {
        _snwprintf(m_dumpFileMessage,
                   sizeof(m_dumpFileMessage) / sizeof(WCHAR),
                   L"Failed to create dump file \"%s\".", m_dumpFilePath);
        MessageBoxW(NULL, m_dumpFileMessage, L"Error", MB_OK | MB_ICONERROR);
    }
}

#endif
