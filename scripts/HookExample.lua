--local inspect = require('scripts/inspect')

print("Hello from LUA!");

-- string CalculateChecksum(ObscuredString input)
-- System_String_o* com_youdagames_gop_tt_client_communications_api_WWWRequest__CalculateChecksum (com_youdagames_gop_tt_client_communications_api_WWWRequest_o* __this, CodeStage_AntiCheat_ObscuredTypes_ObscuredString_o* input, const MethodInfo* method);
CalculateChecksumMethod = Il2CppDomain.Get():GetAssembly("GopRuntime"):GetImage():GetClass(
    "com.youdagames.gop_tt.client.communications.api", "WWWRequest"):GetMethod("CalculateChecksum", 1);

-- string InternalDecryptToString()
-- System_String_o* CodeStage_AntiCheat_ObscuredTypes_ObscuredString__InternalDecryptToString (CodeStage_AntiCheat_ObscuredTypes_ObscuredString_o* __this, const MethodInfo* method);
InternalDecryptToStringMethod = Il2CppDomain.Get():GetAssembly("ACTk.Runtime"):GetImage():GetClass(
    "CodeStage.AntiCheat.ObscuredTypes", "ObscuredString"):GetMethod("InternalDecryptToString", 0);

InternalDecryptToString = JITFunctionCaller(InternalDecryptToStringMethod:GetMethodPointer(), "void*",
    {"void*", "void*"});

function CalculateChecksumHook(params, returnValue)
    DecryprtedString = Cast(InternalDecryptToString(params:GetArg(1), nil), "Il2CppString");
    print("Input:", DecryprtedString:ToString());
end

CalculateChecksumDetour = CreateDetour(CalculateChecksumMethod:GetMethodPointer(), "void*", {"void*", "void*", "void*"},
    CalculateChecksumHook);
