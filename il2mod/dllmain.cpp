#include "il2mod.h"

#include <print>
#include <vector>
#include <filesystem>
#include <functional>
#include <polyhook2/ErrorLog.hpp>
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Detour/ILCallback.hpp>

#include "JITCall.hpp"

#define SOL_PRINT_ERRORS 1
#include <sol/sol.hpp>

inline sol::state L;

inline PLH::ILCallback callback;

inline JITCall JITCaller;

using RuntimeHookFunction = std::function<void(const PLH::ILCallback::Parameters*, const PLH::ILCallback::ReturnValue*)>;

inline std::vector<std::pair<RuntimeHookFunction, JITCall::tJitCall /* JIT Trampoline */>> RuntimeCallbacks;

#pragma optimize("", off)
DECLSPEC_NOINLINE void runtimeHookCallback(const PLH::ILCallback::Parameters* p, const uint8_t count, const PLH::ILCallback::ReturnValue* retVal)
{
    for (auto& callback : RuntimeCallbacks)
    {
        callback.first(p, retVal);

        JITCall::Parameters CurrentTrampolineParams(count);
        for (uint8_t i = 0; i < count; i++)
        {
            CurrentTrampolineParams.setArg(i, p->getArg<void*>(i));
        }

        auto ret = callback.second(CurrentTrampolineParams.getDataPtr());
        memcpy(retVal->getRetPtr(), &ret, sizeof(void*));
    }
}
#pragma optimize("", on)

static void Main(HMODULE hModule)
{
    SetUnhandledExceptionFilter(Util::DllCrashHandler);

    Il2CppThread::Attach(Il2CppDomain::Get());

    Util::SetupConsole();

    L.open_libraries();
    {

        // TODO: replace GetName with lua's tostring from stream operator
        auto domainType = L.new_usertype<Il2CppDomain>("Il2CppDomain", sol::constructors<Il2CppDomain()>());
        domainType["Get"] = &Il2CppDomain::Get;
        domainType["GetAssembly"] = &Il2CppDomain::GetAssembly;
        domainType["GetAssemblies"] = &Il2CppDomain::GetAssemblies;

        auto assemblyType = L.new_usertype<Il2CppAssembly>("Il2CppAssembly", sol::constructors<Il2CppAssembly()>());
        assemblyType["GetImage"] = &Il2CppAssembly::GetImage;

        auto imageType = L.new_usertype<Il2CppImage>("Il2CppImage", sol::constructors<Il2CppImage()>());
        imageType["GetClass"] = &Il2CppImage::GetClass;

        auto classType = L.new_usertype<Il2CppClass>("Il2CppClass", sol::constructors<Il2CppClass()>());
        classType["GetMethod"] = &Il2CppClass::GetMethod;
        classType["GetMethods"] = &Il2CppClass::GetMethods;
        classType["GetName"] = &Il2CppClass::GetName;
        classType["GetProperty"] = &Il2CppClass::GetProperty;
        classType["GetNestedTypes"] = &Il2CppClass::GetNestedTypes;
        classType["GetFields"] = &Il2CppClass::GetFields;
        classType["GetParent"] = &Il2CppClass::GetParent;

        auto methodType = L.new_usertype<MethodInfo>("MethodInfo", sol::constructors<MethodInfo()>());
        methodType["GetName"] = &MethodInfo::GetName;
        methodType["GetParamType"] = &MethodInfo::GetParamType;
        methodType["GetParamName"] = &MethodInfo::GetParamName;
        methodType["GetMethodPointer"] = [](MethodInfo* method)
        {
            return method->methodPointer;
        };

        auto stringType = L.new_usertype<Il2CppString>("Il2CppString", sol::constructors<Il2CppString()>());
        stringType["ToString"] = &Il2CppString::ToString;
        stringType["FromString"] = &Il2CppString::FromCharPtr;

        auto objectType = L.new_usertype<Il2CppObject>("Il2CppObject", sol::constructors<Il2CppObject()>());
        objectType["Get"] = &Il2CppObject::Get<Il2CppObject*>;
    }

    // Extremely temporary, look into dumping the entire class list and searching for the class name
    L.set_function("Cast",
        [](void* ptr, std::string type)
        {
            if (type == "Il2CppString")
            {
                return sol::make_object(L, (Il2CppString*)ptr);
            }
            else if (type == "Il2CppDomain")
            {
                return sol::make_object(L, (Il2CppDomain*)ptr);
            }
        });

    L.set_function("JITFunctionCaller",
        [](void* function, std::string retType, std::vector<std::string> paramTypes)
        {
            auto func = JITCaller.getJitFunc(function, retType, paramTypes);

            return [func](sol::variadic_args va)
            {
                JITCall::Parameters params(va.size());

                for (auto i = 0; i < va.size(); i++)
                {
                    auto pointer = va[i].get<void*>();
                    // THIS NEEDS TO BE RECHECKED FOR A 3RD CONDITON POSSIBLITY
                    // std::println("type: {} {}", (int)va[i].get_type(), va[i].is<void*>());
                    if (pointer and !va[i].is<void*>())
                    {
                        // https://sol2.readthedocs.io/en/latest/api/usertype_memory.html#to-retrieve-a-t
                        pointer = *static_cast<void**>(pointer);
                    }

                    params.setArg<void*>(i, pointer);
                }

                return func(params.getDataPtr());
            };
        });

    auto paramsType = L.new_usertype<PLH::ILCallback::Parameters>("ILCallbackParameters");
    paramsType["GetArg"] = &PLH::ILCallback::Parameters::getArg<void*>;
    paramsType["SetArg"] = [](PLH::ILCallback::Parameters* params, uint8_t index, void* value)
    {
            params->setArg<void*>(index, *static_cast<void**>(value));
    };
    
    auto detourType = L.new_usertype<PLH::x64Detour>("Detour");
    detourType["Hook"] = &PLH::x64Detour::hook;
    detourType["Unhook"] = &PLH::x64Detour::unHook;

    L.set_function(
        "CreateDetour",
        [&](void* function, std::string retType, std::vector<std::string> paramTypes, sol::function hookFunction)
        {
            auto JIT = callback.getJitFunc(retType, paramTypes, asmjit::Arch::kHost, &runtimeHookCallback);

            auto detour = new PLH::x64Detour(
                (uint64_t)function,
                (uint64_t)JIT, callback.getTrampolineHolder());

            if (!detour->hook())
            {
                MessageBoxA(nullptr, "Failed to hook function", "Error", MB_OK);
                return detour;
            }

            auto currentTrampoline = *callback.getTrampolineHolder();
            auto jitCurrentTrampoline = JITCaller.getJitFunc((void*)currentTrampoline, retType, paramTypes);

            RuntimeCallbacks.emplace_back(hookFunction, jitCurrentTrampoline);
            return detour;
        });

    L.set_function("Eject", [&]()
        { FreeLibraryAndExitThread(hModule, 0); });

    auto path = std::filesystem::current_path().string() + "/scripts/test.lua";
    L.script_file(path);

    // auto logger = std::make_shared<PLH::ErrorLog>();
    // logger->setLogLevel(PLH::ErrorLevel::INFO);
    // PLH::Log::registerLogger(logger);

    // DumpAllTypes();
}

// Use stub to only make the dll crash and not the entire process
/*
void Stub()
{
    __try
    {
        Main();
    }
    __except (Util::DllCrashHandler(nullptr))
    {
        printf("Dll crashed!");
    }
}
*/

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, nullptr);
    }

    return true;
}
