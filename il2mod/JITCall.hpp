#pragma once

#define ASMJIT_EMBED
#include "asmjit/asmjit.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <cassert>

#if defined(__clang__)

#elif defined(__GNUC__) || defined(__GNUG__)
#define NOINLINE __attribute__((noinline))
#define PH_ATTR_NAKED __attribute__((naked))
#define OPTS_OFF _Pragma("GCC push_options") \
    _Pragma("GCC optimize (\"O0\")")
#define OPTS_ON #pragma GCC pop_options
#elif defined(_MSC_VER)
#define NOINLINE __declspec(noinline)
#define JC_ATTR_NAKED __declspec(naked)
#define OPTS_OFF __pragma(optimize("", off))
#define OPTS_ON __pragma(optimize("", on))
#endif

class JITCall
{
public:
    class Parameters
    {
    public:
        // Makes our types look nice to typdef this
        typedef char* Data;

        Parameters(int count)
        {
            data.resize(count, 0);
        }

        template <typename T>
        auto setArg(const int idx, const T val)
        {
            assert(idx < data.size());
            if (idx >= data.size())
            {
                printf("Argument set on invalid index. Bad user. Bad. Go fix or you get heap corruption\n");
                return;
            }

            *(T*)getArgPtr(idx) = val;
        }

        template <typename T>
        auto getArg(const int idx) const
        {
            assert(idx < data.size());
            if (idx >= data.size())
            {
                printf("Argument get on invalid index. Bad user. Bad. Go fix or you get heap corruption\n");
                return T {};
            }

            return *(T*)getArgPtr(idx);
        }

        Data getDataPtr()
        {
            return (Data)data.data();
        }

    private:
        std::vector<uint64_t> data;

        // must be char* for aliasing rules to work when reading back out
        char* getArgPtr(const uint8_t idx) const
        {
            assert(idx < data.size());
            if (idx >= data.size())
            {
                return nullptr;
            }

            return (char*)&data[idx];
        }
    };

    typedef void* (*tJitCall)(void* params);

    /* Construct a callback given the raw signature at runtime. Optionally insert a breakpoint before the call*/
    tJitCall getJitFunc(void* target, const std::string& retType, const std::vector<std::string>& paramTypes);

private:
    // given a signature, JIT a call stub, optionally inserting a breakpoint before the call
    tJitCall getJitFunc(void* target, const asmjit::FuncSignature& sig);

    // does a given type fit in a general purpose register (i.e. is it integer type)
    bool isGeneralReg(const asmjit::TypeId typeId) const;

    // float, double, simd128
    bool isXmmReg(const asmjit::TypeId typeId) const;

    bool hasHiArgSlot(const asmjit::x86::Compiler& compiler, const asmjit::TypeId typeId) const;

    // std C type to asmjit type
    asmjit::TypeId getTypeId(const std::string& type);

    asmjit::JitRuntime m_jitRuntime;
};