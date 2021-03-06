//
// Created by mfran on 04/09/2020.
//

#include <memory>
#if defined(_WIN32)
#   include <Windows.h>
#   include "WindowsAPIHelper.hpp"
#else
#   include <dlfcn.h>
#   include <cstring>
#   include <signal.h>
#endif

#include "MF/DynamicLibrary.hpp"

#if defined(_WIN32)
const char* DynamicLibrary::LocalExtension = ".dll";
#define _GetLibraryPointer static_cast<HMODULE>(_library)
#else
const char* DynamicLibrary::LocalExtension = ".so";
#endif

void* DynamicLibrary::GetFunction(const std::string& functionName) {
    LOCK_t lockGuard(_mutex);

    void* functionAddress;

#if defined(_WIN32)
    functionAddress = reinterpret_cast<void*>(GetProcAddress(_GetLibraryPointer, functionName.c_str()));
    if (functionAddress == nullptr) {
        throw element_not_found_exception ("Unable to find given function: " + functionName);
    }
#else
    (void)dlerror(); // Clear any previous error.
    functionAddress = dlsym(library, functionName.c_str());
    if (dlerror()) {
        // We must run this check because the "functionAddress" value may be null but still valid.
        throw element_not_found_exception ("Unable to find given function: " + functionName);
    }
#endif
    return functionAddress;
}

void DynamicLibrary::AddToSearchPaths(const std::string& path) {
    if (path.size() < 2) {
        return;
    }

#if defined(_WIN32)
    std::unique_ptr<const wchar_t[]> wPath(Windows_ConvertString(path.c_str()));
    AddDllDirectory(wPath.get());
#else
    char* ldLibraryPath = getenv("LD_LIBRARY_PATH");
    char* newLdLibraryPath = (char*) calloc(
        (ldLibraryPath ? strlen(ldLibraryPath) + 1 : 0) + // Length of the environment variable, if any, + ":" separator
        path.length() + // Length of the value to add
        1, // End of string
        1);
    strcpy(newLdLibraryPath, ldLibraryPath);
    strcat(newLdLibraryPath, ":");
    strcat(newLdLibraryPath, path.c_str());
    setenv("LD_LIBRARY_PATH", newLdLibraryPath, true);
#endif
}

DynamicLibrary::~DynamicLibrary() {
    LOCK_t lockGuard(_mutex);

    bool success;
#if defined(_WIN32)
    success = FreeLibrary(_GetLibraryPointer);
#else
    success = !dlclose(library);
#endif

    if (success) {
        _library = nullptr;
    }
    else {
        // TODO handle error
    }
}

DynamicLibrary::DynamicLibrary(const std::string &libName) {
    LOCK_t lockGuard(_mutex);

#if defined(_WIN32)
    std::unique_ptr<const wchar_t[]> wLibName(Windows_ConvertString(libName.c_str()));
    static constexpr HANDLE hFile = nullptr;
    static constexpr DWORD dwFlags = LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
    _library = LoadLibraryExW(wLibName.get(), hFile, dwFlags);
#else
    library = dlopen(libName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif

    if (_library != nullptr) {
        return; // We can add stuff there
    }
    else {
        throw element_not_found_exception ("The requested library could not be found: " + libName);
    }
}
