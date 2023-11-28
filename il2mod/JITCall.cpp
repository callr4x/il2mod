#include "JITCall.hpp"

#define TYPEID_MATCH_STR_IF(var, T)                                      \
    if (var == #T)                                                       \
    {                                                                    \
        return asmjit::TypeId(asmjit::TypeUtils::TypeIdOfT<T>::kTypeId); \
    }
#define TYPEID_MATCH_STR_ELSEIF(var, T)                                  \
    else if (var == #T)                                                  \
    {                                                                    \
        return asmjit::TypeId(asmjit::TypeUtils::TypeIdOfT<T>::kTypeId); \
    }

asmjit::TypeId JITCall::getTypeId(const std::string& type)
{
    // we only care about ptr value, size doesn't matter
    // so just use a uintptr size for any type
    if (type.find("*") != std::string::npos)
    {
        return asmjit::TypeId::kUIntPtr;
    }

    TYPEID_MATCH_STR_IF(type, signed char)
    TYPEID_MATCH_STR_ELSEIF(type, unsigned char)
    TYPEID_MATCH_STR_ELSEIF(type, short)
    TYPEID_MATCH_STR_ELSEIF(type, unsigned short)
    TYPEID_MATCH_STR_ELSEIF(type, int)
    TYPEID_MATCH_STR_ELSEIF(type, unsigned int)
    TYPEID_MATCH_STR_ELSEIF(type, long)
    TYPEID_MATCH_STR_ELSEIF(type, unsigned long)
    TYPEID_MATCH_STR_ELSEIF(type, __int64)
    TYPEID_MATCH_STR_ELSEIF(type, unsigned __int64)
    TYPEID_MATCH_STR_ELSEIF(type, long long)
    TYPEID_MATCH_STR_ELSEIF(type, unsigned long long)
    TYPEID_MATCH_STR_ELSEIF(type, char)
    TYPEID_MATCH_STR_ELSEIF(type, char16_t)
    TYPEID_MATCH_STR_ELSEIF(type, char32_t)
    TYPEID_MATCH_STR_ELSEIF(type, wchar_t)
    TYPEID_MATCH_STR_ELSEIF(type, uint8_t)
    TYPEID_MATCH_STR_ELSEIF(type, int8_t)
    TYPEID_MATCH_STR_ELSEIF(type, uint16_t)
    TYPEID_MATCH_STR_ELSEIF(type, int16_t)
    TYPEID_MATCH_STR_ELSEIF(type, int32_t)
    TYPEID_MATCH_STR_ELSEIF(type, uint32_t)
    TYPEID_MATCH_STR_ELSEIF(type, uint64_t)
    TYPEID_MATCH_STR_ELSEIF(type, int64_t)
    TYPEID_MATCH_STR_ELSEIF(type, float)
    TYPEID_MATCH_STR_ELSEIF(type, double)
    TYPEID_MATCH_STR_ELSEIF(type, bool)
    TYPEID_MATCH_STR_ELSEIF(type, void)
    else if (type == "intptr_t")
    {
        return asmjit::TypeId::kIntPtr;
    }
    else if (type == "uintptr_t")
    {
        return asmjit::TypeId::kUIntPtr;
    }

    return asmjit::TypeId::kVoid;
}

JITCall::tJitCall JITCall::getJitFunc(void* target, const asmjit::FuncSignature& sig)
{
    asmjit::CodeHolder code; // Holds code and relocation information.
    code.init(m_jitRuntime.environment()); // Initialize to the same arch as JIT runtime.

    asmjit::StringLogger log;
    auto kFormatFlags = asmjit::FormatFlags::kMachineCode | asmjit::FormatFlags::kExplainImms | asmjit::FormatFlags::kRegCasts
        | asmjit::FormatFlags::kHexImms | asmjit::FormatFlags::kHexOffsets | asmjit::FormatFlags::kPositions;

    log.addFlags(kFormatFlags);

    code.setLogger(&log);

    asmjit::x86::Compiler cc(&code);
    asmjit::FuncNode* func = cc.addFunc( // Create the wrapper function around call we JIT
        asmjit::FuncSignatureT<void*, JITCall::Parameters*>());

    asmjit::x86::Gp paramImm = cc.newUIntPtr();
    asmjit::x86::Gp i = cc.newUIntPtr();
    // cc.setArg(0, paramImm);
    func->setArg(0, paramImm);

    // paramMem = ((char*)paramImm) + i (char* size walk, uint64_t size r/w)
    asmjit::x86::Mem paramMem = asmjit::x86::ptr(paramImm, i);
    paramMem.setSize(sizeof(uint64_t));

    // i = 0
    cc.mov(i, 0);

    struct ArgRegSlot
    {
        ArgRegSlot(uint32_t idx)
        {
            argIdx = idx;
            useHighReg = false;
        }

        asmjit::x86::Reg low;
        asmjit::x86::Reg high;

        uint32_t argIdx;
        bool useHighReg;
    };

    // map argument slots to registers, following abi. (We can have multiple register per arg slot such as high and low 32bits of a 64bit slot)
    std::vector<ArgRegSlot> argRegSlots;
    uint32_t offsetNextSlot = sizeof(uint64_t);
    for (uint8_t arg_idx = 0; arg_idx < sig.argCount(); arg_idx++)
    {
        const auto argType = sig.args()[arg_idx];

        // increment arg slot if not first one
        if (arg_idx != 0)
        {
            cc.add(i, offsetNextSlot);
            offsetNextSlot = sizeof(uint64_t);
        }

        ArgRegSlot argSlot(arg_idx);
        if (isGeneralReg(argType))
        {
            argSlot.low = cc.newUIntPtr();
            cc.mov(argSlot.low.as<asmjit::x86::Gp>(), paramMem);

            if (hasHiArgSlot(cc, argType))
            {
                cc.add(i, sizeof(uint32_t));
                offsetNextSlot -= sizeof(uint32_t);

                argSlot.high = cc.newUIntPtr();
                argSlot.useHighReg = true;
                cc.mov(argSlot.high.as<asmjit::x86::Gp>(), paramMem);
            }
        }
        else if (isXmmReg(argType))
        {
            argSlot.low = cc.newXmm();
            cc.movq(argSlot.low.as<asmjit::x86::Xmm>(), paramMem);
        }
        else
        {
            // ex: void example(__m128i xmmreg) is invalid: https://github.com/asmjit/asmjit/issues/83
            printf("Parameters wider than 64bits not supported\n");
            return 0;
        }

        argRegSlots.push_back(argSlot);
    }

    // Gen the call
    asmjit::InvokeNode* invokeNode;
    cc.invoke(&invokeNode,
        (uint64_t)target,
        sig);

    // Map call params to the args
    for (auto& argSlot : argRegSlots)
    {
        invokeNode->setArg(argSlot.argIdx, 0, argSlot.low);
        if (argSlot.useHighReg)
        {
            invokeNode->setArg(argSlot.argIdx, 1, argSlot.high);
        }
    }

    cc.ret();
    cc.endFunc(); // End of the function body.

    cc.finalize();
    // ----> x86::Compiler is no longer needed from here and can be destroyed <----

    tJitCall wrapperFunc;
    asmjit::Error err = m_jitRuntime.add(&wrapperFunc, &code);
    if (err)
    {
        // printf("Error: %s\nGenerated so far%s\n", asmjit::DebugUtils::errorAsString(err), log.data());
        return 0;
    }

    // ----> CodeHolder is no longer needed from here and can be destroyed <----
    //printf("JIT Wrapper:\n%s\n", log.data());
    return wrapperFunc;
}

JITCall::tJitCall JITCall::getJitFunc(void* target, const std::string& retType, const std::vector<std::string>& paramTypes)
{
    asmjit::FuncSignature sig;
    std::vector<asmjit::TypeId> args;
    for (const std::string& s : paramTypes)
    {
        args.push_back(getTypeId(s));
    }
    sig.init(asmjit::CallConvId::kHost, asmjit::FuncSignature::kNoVarArgs, getTypeId(retType), args.data(), (uint32_t)args.size());
    return getJitFunc(target, sig);
}

bool JITCall::isGeneralReg(const asmjit::TypeId typeId) const
{
    switch (typeId)
    {
    case asmjit::TypeId::kInt8:
    case asmjit::TypeId::kUInt8:
    case asmjit::TypeId::kInt16:
    case asmjit::TypeId::kUInt16:
    case asmjit::TypeId::kInt32:
    case asmjit::TypeId::kUInt32:
    case asmjit::TypeId::kInt64:
    case asmjit::TypeId::kUInt64:
    case asmjit::TypeId::kIntPtr:
    case asmjit::TypeId::kUIntPtr:
        return true;
    default:
        return false;
    }
}

bool JITCall::isXmmReg(const asmjit::TypeId typeId) const
{
    switch (typeId)
    {
    case asmjit::TypeId::kFloat32:
    case asmjit::TypeId::kFloat64:
        return true;
    default:
        return false;
    }
}

bool JITCall::hasHiArgSlot(const asmjit::x86::Compiler& compiler, const asmjit::TypeId typeId) const
{
    // 64bit width regs can fit wider args
    if (compiler.is64Bit())
    {
        return false;
    }

    switch (typeId)
    {
    case asmjit::TypeId::kInt64:
    case asmjit::TypeId::kUInt64:
        return true;
    }
    return false;
}