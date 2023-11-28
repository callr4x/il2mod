---@class Il2CppDomain @This test
Il2CppDomain = {}

---@class Il2CppAssembly @This test
Il2CppAssembly = {}

---@class Il2CppImage @This test
Il2CppImage = {}

---@class Il2CppObject @This test
Il2CppObject = {}

---@class Il2CppClass @This test
Il2CppClass = {}

---@class Il2CppType @This test
Il2CppType = {}

---@class Il2CppString @This test
Il2CppString = {}

---@class MethodInfo @This test
MethodInfo = {}

---@class FieldInfo @This test
FieldInfo = {}

---@class PropertyInfo @This test
PropertyInfo = {}

-- DOMAIN FUNCTIONS

---@return Il2CppDomain* 
function Il2CppDomain.Get() end

---@param assemblyName string
---@return Il2CppAssembly* 
function Il2CppDomain:GetAssembly(assemblyName) end

---@return Il2CppAssembly*[]
function Il2CppDomain:GetAssemblies() end

-- ASSEMBLY FUNCTIONS

---@return Il2CppImage*
function Il2CppAssembly:GetImage() end

-- IMAGE FUNCTIONS

---@param namespace string
---@param name string
---@return Il2CppClass*
function Il2CppImage:GetClass(namespace, name) end

-- CLASS FUNCTIONS

---@param name string
---@param argsCount number
---@return MethodInfo*
function Il2CppClass:GetMethod(name, argsCount) end

---@return MethodInfo*[]
function Il2CppClass:GetMethods() end

---@param csharpName boolean optional
---@return string
function Il2CppClass:GetName(csharpName) end

---@param name string
---@return PropertyInfo*
function Il2CppClass:GetProperty(name) end

---@return Il2CppClass*[]
function Il2CppClass:GetNestedTypes() end

---@return FieldInfo*[]
function Il2CppClass:GetFields() end

---@return Il2CppClass*
function Il2CppClass:GetParent() end

-- METHOD FUNCTIONS

---@return string
function MethodInfo:GetName() end

---@param index number
---@return string
function MethodInfo:GetParamName(index) end

---@param index number
---@return Il2CppType*
function MethodInfo:GetParamType() end

---@return void*
function MethodInfo:GetMethodPointer() end

-- STRING FUNCTIONS

---@return string
function Il2CppString:ToString() end

---@param string string
---@return Il2CppString*
function Il2CppString.FromString(string) end

-- OBJECT FUNCTIONS

---@param fieldName string
---@return Il2CppObject*
function Il2CppObject:Get(fieldName) end

-- JIT FUNCTIONS

---@param methodPointer void*
---@param returnType string
---@param paramTypes string[]
---@return function
function JITFunctionCaller(methodPointer, returnType, paramTypes) end