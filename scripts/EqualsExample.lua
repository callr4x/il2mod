print("Calling String.Equals function");

Domain = Il2CppDomain.Get();
Image = Domain:GetAssembly("mscorlib"):GetImage();
Class = Image:GetClass("System", "String");
Method = Class:GetMethod("Equals", 1);

-- code bellow is for calling 
-- bool Equals(string value)
-- which is compiled with il2cpp to
-- bool System_String__Equals(System_String_o *this, Il2CppObject *obj, const MethodInfo *method)
-- I'd advice you to recreate the jit caller after each usage for the best results
StringEquals = JITFunctionCaller(Method:GetMethodPointer(), "void*", {"void*", "void*", "void*"});

Str1 = Il2CppString.FromString("hi");
Str2 = Il2CppString.FromString("hi");
print(StringEquals(Str1, Str2, nil));