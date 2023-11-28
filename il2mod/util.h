#pragma once

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <algorithm>
#include <codecvt>
#include <format>
#include <iostream>
#include <string>
#include <windows.h>
#include <intrin.h>
#include <detours.h>
#include <format>
#include <stacktrace>

#define DefineNativeExport(module, ret, name, params) static ret(*name) params = (ret(*) params)(GetProcAddress((HMODULE)module, #name));

#define DefineNative(address, ret, name, params) static ret(*name) params = (ret(*) params)(address);

#define AddressTo(a, f) f = reinterpret_cast<decltype(f)>(a)

#define DEFINE_STACK_ARRAY(name) auto name = (void**)_AddressOfReturnAddress()

namespace Util
{
    static void SetupConsole()
    {
        AllocConsole();

        FILE* pFile;
        freopen_s(&pFile, "CONOUT$", "w", stdout);
    }

    std::string UTF16ToUTF8(std::u16string&& utf16_string)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
        auto p = reinterpret_cast<const int16_t*>(utf16_string.data());
        return convert.to_bytes(p, p + utf16_string.size());
    }

    static auto FixName(std::string& name)
    {
        constexpr static std::string_view chars = " ?${}|`+_-=:/^()[]<>.,#\'\"%";

        std::replace_if(
            name.begin(), name.end(), [](const unsigned char c)
            { return chars.find(c) != std::string_view::npos; },
            '_');

        return name;
    }

    LONG DllCrashHandler(EXCEPTION_POINTERS* ex)
    {
        MessageBoxA(nullptr, std::format("Basic stacktrace\n{}", std::to_string(std::stacktrace::current())).c_str(), "A crash has been detected!", MB_OK | MB_ICONERROR);

        return EXCEPTION_EXECUTE_HANDLER;
    }
}
