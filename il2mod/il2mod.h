#pragma once
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "cppgenerator.h"
#include "util.h"
#include <vector>

inline uintptr_t GameAssembly = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll"));

DefineNativeExport(GameAssembly, void*, il2cpp_thread_attach, (class Il2CppDomain * domain));

class Il2CppObject
{
public:
    union
    {
        class Il2CppClass* klass;
        void* vtable;
    };

    void* monitor;

    template <typename T>
    auto GetByOffset(size_t offset) -> T
    {
        return reinterpret_cast<T>((void*)(*reinterpret_cast<__int64*>(__int64(this) + offset)));
    }

    template <typename T>
    auto GetFromType(class Il2CppType* type, std::string filedName) -> T;

    template <typename T>
    auto Get(std::string filedName) -> T;
};

struct Il2CppThread
{
public:
    Il2CppObject obj;
    class Il2CppInternalThread* internal_thread;
    Il2CppObject* start_obj;
    class Il2CppException* pending_exception;
    Il2CppObject* principal;
    int32_t principal_version;
    class Il2CppDelegate* delegate;
    Il2CppObject* executionContext;
    uint8_t executionContextBelongsToOuterScope;

    static auto Attach(Il2CppDomain* domain) -> void*
    {
        return il2cpp_thread_attach(domain);
    }
};

DefineNativeExport(GameAssembly, class Il2CppString*, il2cpp_string_new, (const char* str));

class Il2CppString
{
public:
    Il2CppObject object;
    int32_t length;
    uint16_t chars[32];

    // in lua
    static auto FromCharPtr(const char* str) -> Il2CppString*
    {
        return il2cpp_string_new(str);
    }

    // in lua
    auto ToString() -> std::string
    {
        std::u16string u16(reinterpret_cast<const char16_t*>(chars));
        return Util::UTF16ToUTF8(std::move(u16));
    }

    auto ToWString() -> std::wstring
    {
        return std::wstring(reinterpret_cast<const wchar_t*>(chars));
    }
};

DefineNativeExport(GameAssembly, const char*, il2cpp_method_get_name, (class MethodInfo * method));
DefineNativeExport(GameAssembly, class Il2CppType*, il2cpp_method_get_param, (class MethodInfo * method, uint32_t index));
DefineNativeExport(GameAssembly, const char*, il2cpp_method_get_param_name, (class MethodInfo * method, uint32_t index));

typedef void (*Il2CppMethodPointer)();
typedef void* (*InvokerMethod)(Il2CppMethodPointer, class MethodInfo*, void*, void**);

DefineNativeExport(GameAssembly, class MethodInfo*, il2cpp_property_get_set_method, (class PropertyInfo * prop));
DefineNativeExport(GameAssembly, class MethodInfo*, il2cpp_property_get_get_method, (class PropertyInfo * prop));

class PropertyInfo
{
public:
    class Il2CppClass* parent;
    const char* name;
    const MethodInfo* get;
    const MethodInfo* set;
    uint32_t attrs;
    uint32_t token;

    auto GetSetMethod() -> MethodInfo*
    {
        return il2cpp_property_get_set_method(this);
    }

    auto GetGetMethod() -> MethodInfo*
    {
        return il2cpp_property_get_get_method(this);
    }
};

DefineNativeExport(GameAssembly, class Il2CppImage*, il2cpp_assembly_get_image, (class Il2CppAssembly * assembly));

class Il2CppAssembly
{
public:
    struct Il2CppAssemblyName
    {
        const char* name;
        const char* culture;
        const uint8_t* public_key;
        uint32_t hash_alg;
        int32_t hash_len;
        uint32_t flags;
        int32_t major;
        int32_t minor;
        int32_t build;
        int32_t revision;
        uint8_t public_key_token[8];
    };

    class Il2CppImage* image;
    uint32_t token;
    int32_t referencedAssemblyStart;
    int32_t referencedAssemblyCount;
    Il2CppAssemblyName aname;

    // in lua
    auto GetImage() -> Il2CppImage*
    {
        return il2cpp_assembly_get_image(this);
    }
};

DefineNativeExport(GameAssembly, class Il2CppDomain*, il2cpp_domain_get, ());
DefineNativeExport(GameAssembly, class Il2CppAssembly*, il2cpp_domain_assembly_open, (class Il2CppDomain * domain, const char* name));
DefineNativeExport(GameAssembly, class Il2CppAssembly**, il2cpp_domain_get_assemblies, (class Il2CppDomain * domain, size_t* size));

class Il2CppDomain
{
public:
    class Il2CppAppDomain* domain;
    class Il2CppAppDomainSetup* setup;
    class Il2CppAppContext* default_context;
    const char* friendly_name;
    uint32_t domain_id;
    volatile int threadpool_jobs;
    void* agent_info;

    // in lua
    static auto Get()
    {
        return il2cpp_domain_get();
    }

    Il2CppDomain()
    {
        //*this = *Get();
    }

    // in lua
    auto GetAssemblies() -> std::vector<Il2CppAssembly*>
    {
        size_t size;
        auto asms = il2cpp_domain_get_assemblies(this, &size);

        std::vector<Il2CppAssembly*> assemblies;
        for (auto i = 0; i < size; i++)
        {
            assemblies.push_back(asms[i]);
        }

        return assemblies;
    }

    // in lua
    auto GetAssembly(const char* name) -> Il2CppAssembly*
    {
        return il2cpp_domain_assembly_open(this, name);
    }
};

class MethodInfo
{
public:
    // in lua
    void* methodPointer;
    InvokerMethod invoker_method;
    const char* name;
    Il2CppClass* klass;
    const Il2CppType* return_type;
    const void* parameters;
    union
    {
        const union Il2CppRGCTXData* rgctx_data;
        const void* methodMetadataHandle;
    };

    union
    {
        const void* genericMethod;
        const void* genericContainerHandle;
    };

    uint32_t token;
    uint16_t flags;
    uint16_t iflags;
    uint16_t slot;
    uint8_t parameters_count;
    uint8_t bitflags;

    // in lua
    auto GetName() -> const char*
    {
        return il2cpp_method_get_name(this);
    }

    // in lua
    auto GetParamType(uint32_t index) -> Il2CppType*
    {
        return il2cpp_method_get_param(this, index);
    }

    // in lua
    auto GetParamName(uint32_t index) -> const char*
    {
        return il2cpp_method_get_param_name(this, index);
    }
};

DefineNativeExport(GameAssembly, const char*, il2cpp_field_get_name, (class FieldInfo * field));
DefineNativeExport(GameAssembly, class Il2CppType*, il2cpp_field_get_type, (class FieldInfo * field));
DefineNativeExport(GameAssembly, size_t, il2cpp_field_get_offset, (class FieldInfo * field));

class FieldInfo
{
public:
    const char* name;
    const Il2CppType* type;
    Il2CppClass* parent;
    int32_t offset;
    uint32_t token;

    auto GetOffset() -> size_t
    {
        return il2cpp_field_get_offset(this);
    }

    auto GetType() -> Il2CppType*
    {
        return il2cpp_field_get_type(this);
    }

    auto GetName(bool csharpName = false) -> std::string
    {
        std::string name(il2cpp_field_get_name(this));

        if (!csharpName)
        {
            name = Util::FixName(name);
        }

        return name;
    }

    auto GetNameW(bool csharpName = false) -> std::wstring
    {
        auto name = GetName(csharpName);
        return std::wstring(name.begin(), name.end());
    }
};

DefineNativeExport(GameAssembly, int, il2cpp_array_element_size, (class Il2CppClass * klass));

template <typename T>
class Il2CppArray : public Il2CppObject
{
public:
    struct Il2CppArrayBounds
    {
        uintptr_t length;
        int32_t lower_bound;
    };

    Il2CppArrayBounds* bounds;
    uintptr_t max_length;

    void* vector[65535];

    auto GetElementSize() -> size_t
    {
        return il2cpp_array_element_size(this->klass);
    }

    auto GetLength() -> int32_t
    {
        return (int32_t)max_length;
    }

    auto GetElementAddr(int32_t i) -> void*
    {
        return (void*)((uintptr_t)this + (i * GetElementSize()) + offsetof(Il2CppArray, vector));
    }

    auto GetElement(int32_t i) -> T&
    {
        assert(GetLength() > i);

        return *(T*)GetElementAddr(i);
    }

    T& operator[](int i)
    {
        return GetElement(i);
    }
};

DefineNativeExport(GameAssembly, class Il2CppClass*, il2cpp_image_get_class, (class Il2CppImage * image, size_t index));
DefineNativeExport(GameAssembly, size_t, il2cpp_image_get_class_count, (class Il2CppImage * image));
DefineNativeExport(GameAssembly, class Il2CppClass*, il2cpp_class_from_name, (class Il2CppImage * image, const char* namespaze, const char* name));

typedef int32_t TypeDefinitionIndex;
typedef int32_t CustomAttributeIndex;
typedef int32_t MethodIndex;

class Il2CppImage
{
public:
    const char* name;
    const char* nameNoExt;
    class Il2CppAssembly* assembly;
    TypeDefinitionIndex typeStart;
    uint32_t typeCount;
    TypeDefinitionIndex exportedTypeStart;
    uint32_t exportedTypeCount;
    CustomAttributeIndex customAttributeStart;
    uint32_t customAttributeCount;
    MethodIndex entryPointIndex;
    void* /*Il2CppNameToTypeDefinitionIndexHashTable**/ nameToClassHashTable;
    const /*Il2CppCodeGenModule*/ void* codeGenModule;
    uint32_t token;
    uint8_t dynamic;

    auto GetClassCount() -> size_t
    {
        return il2cpp_image_get_class_count(this);
    }

    auto GetClassByIndex(size_t index) -> Il2CppClass*
    {
        return il2cpp_image_get_class(this, index);
    }

    // in lua
    auto GetClass(const char* namespaze, const char* name) -> Il2CppClass*
    {
        return il2cpp_class_from_name(this, namespaze, name);
    }
};

DefineNativeExport(GameAssembly, char*, il2cpp_type_get_name, (class Il2CppType * type));
DefineNativeExport(GameAssembly, class Il2CppClass*, il2cpp_type_get_class_or_element_class, (class Il2CppType * type));

typedef int32_t GenericParameterIndex;

class Il2CppType
{
public:
    struct Il2CppArrayType
    {
        Il2CppType* etype;
        uint8_t rank;
        uint8_t numsizes;
        uint8_t numlobounds;
        int* sizes;
        int* lobounds;
    };

    enum Il2CppTypeEnum
    {
        IL2CPP_TYPE_END = 0x00,
        IL2CPP_TYPE_VOID = 0x01,
        IL2CPP_TYPE_BOOLEAN = 0x02,
        IL2CPP_TYPE_CHAR = 0x03,
        IL2CPP_TYPE_I1 = 0x04,
        IL2CPP_TYPE_U1 = 0x05,
        IL2CPP_TYPE_I2 = 0x06,
        IL2CPP_TYPE_U2 = 0x07,
        IL2CPP_TYPE_I4 = 0x08,
        IL2CPP_TYPE_U4 = 0x09,
        IL2CPP_TYPE_I8 = 0x0a,
        IL2CPP_TYPE_U8 = 0x0b,
        IL2CPP_TYPE_R4 = 0x0c,
        IL2CPP_TYPE_R8 = 0x0d,
        IL2CPP_TYPE_STRING = 0x0e,
        IL2CPP_TYPE_PTR = 0x0f,
        IL2CPP_TYPE_BYREF = 0x10,
        IL2CPP_TYPE_VALUETYPE = 0x11,
        IL2CPP_TYPE_CLASS = 0x12,
        IL2CPP_TYPE_VAR = 0x13,
        IL2CPP_TYPE_ARRAY = 0x14,
        IL2CPP_TYPE_GENERICINST = 0x15,
        IL2CPP_TYPE_TYPEDBYREF = 0x16,
        IL2CPP_TYPE_I = 0x18,
        IL2CPP_TYPE_U = 0x19,
        IL2CPP_TYPE_FNPTR = 0x1b,
        IL2CPP_TYPE_OBJECT = 0x1c,
        IL2CPP_TYPE_SZARRAY = 0x1d,
        IL2CPP_TYPE_MVAR = 0x1e,
        IL2CPP_TYPE_CMOD_REQD = 0x1f,
        IL2CPP_TYPE_CMOD_OPT = 0x20,
        IL2CPP_TYPE_INTERNAL = 0x21,
        IL2CPP_TYPE_MODIFIER = 0x40,
        IL2CPP_TYPE_SENTINEL = 0x41,
        IL2CPP_TYPE_PINNED = 0x45,
        IL2CPP_TYPE_ENUM = 0x55
    };

    union
    {
        void* dummy;
        TypeDefinitionIndex klassIndex;
        Il2CppType* type;
        Il2CppArrayType* array;
        GenericParameterIndex genericParameterIndex;
        struct Il2CppGenericClass* generic_class;
    } data;

    unsigned int attrs : 16;
    Il2CppTypeEnum type : 8;
    unsigned int num_mods : 6;
    unsigned int byref : 1;
    unsigned int pinned : 1;

    auto GetName(bool csharpName = false) -> std::string
    {
        std::string name(il2cpp_type_get_name(this));

        if (!csharpName)
        {
            name = Util::FixName(name);
        }

        return name;
    }

    auto GetNameW(bool csharpName = false) -> std::wstring
    {
        std::string name(il2cpp_type_get_name(this));

        if (!csharpName)
        {
            name = Util::FixName(name);
        }

        return std::wstring(name.begin(), name.end());
    }

    auto GetClass() -> class Il2CppClass*
    {
        return il2cpp_type_get_class_or_element_class(this);
    }

    bool isPrimitive()
    {
        switch (type)
        {
        case IL2CPP_TYPE_BOOLEAN:
        case IL2CPP_TYPE_CHAR:
        case IL2CPP_TYPE_U:
        case IL2CPP_TYPE_I:
        case IL2CPP_TYPE_U1:
        case IL2CPP_TYPE_I2:
        case IL2CPP_TYPE_U2:
        case IL2CPP_TYPE_I4:
        case IL2CPP_TYPE_U4:
        case IL2CPP_TYPE_I8:
        case IL2CPP_TYPE_U8:
        case IL2CPP_TYPE_R4:
        case IL2CPP_TYPE_R8:
            return true;

        default:
            break;
        }

        return false;
    }

    size_t GetFieldOffset(std::string name);

    std::string Parse();
};
DefineNativeExport(GameAssembly, class PropertyInfo*, il2cpp_class_get_property_from_name, (class Il2CppClass * klass, const char* name));
DefineNativeExport(GameAssembly, const char*, il2cpp_class_get_name, (class Il2CppClass * klass));
DefineNativeExport(GameAssembly, class MethodInfo*, il2cpp_class_get_method_from_name, (class Il2CppClass * klass, const char* name, int argsCount));
DefineNativeExport(GameAssembly, class MethodInfo*, il2cpp_class_get_methods, (Il2CppClass * klass, void** iter));
DefineNativeExport(GameAssembly, class FieldInfo*, il2cpp_class_get_fields, (class Il2CppClass * klass, void** iter));
DefineNativeExport(GameAssembly, class Il2CppClass*, il2cpp_class_get_nested_types, (class Il2CppClass * klass, void** iter));
DefineNativeExport(GameAssembly, class Il2CppType*, il2cpp_class_get_type, (class Il2CppClass * klass));
DefineNativeExport(GameAssembly, size_t, il2cpp_class_num_fields, (class Il2CppClass * klass));
DefineNativeExport(GameAssembly, bool, il2cpp_class_is_valuetype, (class Il2CppClass * klass));
DefineNativeExport(GameAssembly, bool, il2cpp_class_is_enum, (class Il2CppClass * klass));
DefineNativeExport(GameAssembly, Il2CppClass*, il2cpp_class_get_parent, (Il2CppClass * klass));

struct Il2CppRuntimeInterfaceOffsetPair
{
    class Il2CppClass* interfaceType;
    int32_t offset;
};

struct Il2CppGenericInst
{
    uint32_t type_argc;
    class Il2CppType** type_argv;
};

struct Il2CppGenericContext
{
    struct Il2CppGenericInst* class_inst;
    struct Il2CppGenericInst* method_inst;
};

struct Il2CppGenericClass
{
    TypeDefinitionIndex typeDefinitionIndex;
    struct Il2CppGenericContext context;
    class Il2CppClass* cached_class;
};

struct VirtualInvokeData
{
    void* methodPtr;
    const MethodInfo* method;
};

union Il2CppRGCTXData
{
    void* rgctxDataDummy;
    const MethodInfo* method;
    const Il2CppType* type;
    Il2CppClass* klass;
};

class Il2CppClass
{
public:
    struct Il2CppClass_1
    {
        void* image;
        void* gc_desc;
        const char* name;
        const char* namespaze;
        class Il2CppType byval_arg;
        class Il2CppType this_arg;
        class Il2CppClass* element_class;
        class Il2CppClass* castClass;
        class Il2CppClass* declaringType;
        class Il2CppClass* parent;
        void* generic_class;
        void* typeMetadataHandle;
        void* interopData;
        Il2CppClass* klass;
        void* fields;
        void* events;
        void* properties;
        void* methods;
        Il2CppClass** nestedTypes;
        Il2CppClass** implementedInterfaces;
        Il2CppRuntimeInterfaceOffsetPair* interfaceOffsets;
    };

    struct Il2CppClass_2
    {
        Il2CppClass** typeHierarchy;
        void* unity_user_data;
        uint32_t initializationExceptionGCHandle;
        uint32_t cctor_started;
        uint32_t cctor_finished;
        size_t cctor_thread;
        void* genericContainerHandle;
        uint32_t instance_size;
        uint32_t actualSize;
        uint32_t element_size;
        int32_t native_size;
        uint32_t static_fields_size;
        uint32_t thread_static_fields_size;
        int32_t thread_static_fields_offset;
        uint32_t flags;
        uint32_t token;
        uint16_t method_count;
        uint16_t property_count;
        uint16_t field_count;
        uint16_t event_count;
        uint16_t nested_type_count;
        uint16_t vtable_count;
        uint16_t interfaces_count;
        uint16_t interface_offsets_count;
        uint8_t typeHierarchyDepth;
        uint8_t genericRecursionDepth;
        uint8_t rank;
        uint8_t minimumAlignment;
        uint8_t naturalAligment;
        uint8_t packingSize;
        uint8_t bitflags1;
        uint8_t bitflags2;
    };

    Il2CppClass_1 _1;
    void* static_fields;
    Il2CppRGCTXData* rgctx_data;
    Il2CppClass_2 _2;
    VirtualInvokeData vtable[255];

    // in lua
    auto GetName(bool csharpName = false) -> std::string
    {
        std::string name(il2cpp_class_get_name(this));

        if (!csharpName)
        {
            name = Util::FixName(name);
        }

        return name;
    }

    // in lua
    auto GetProperty(const char* name) -> class PropertyInfo*
    {
        return il2cpp_class_get_property_from_name(this, name);
    }

    // in lua
    auto GetNestedTypes()
    {
        std::vector<class Il2CppClass*> classes;

        class Il2CppClass* klass = nullptr;
        void* iter = nullptr;

        if (GetFieldCount() != 0)
        {
            while ((klass = il2cpp_class_get_nested_types(this, &iter)))
            {
                classes.push_back(klass);
            }
        }

        return classes;
    }

    // in lua
    auto GetMethods() -> std::vector<class MethodInfo*>
    {
        std::vector<class MethodInfo*> methods;

        class MethodInfo* method = nullptr;
        void* iter = nullptr;

        if (GetFieldCount() != 0)
        {
            while ((method = il2cpp_class_get_methods(this, &iter)))
            {
                methods.push_back(method);
            }
        }

        return methods;
    }

    // in lua
    auto GetMethod(const char* name, int argsCount) -> class MethodInfo*
    {
        return il2cpp_class_get_method_from_name(this, name, argsCount);
    }

    // not stable enough.
    /* auto GetMethod(const char* name) -> class MethodInfo*
        {
        void* ret = nullptr;

        for (auto method : GetMethods())
                {
            printf("Test: %s\n", method->GetName());

                        if (method->GetName() == name)
                        {
                                ret = method;
                                break;
                        }
                }

        return (class MethodInfo*)ret;
        }*/

    auto GetFieldCount() -> size_t
    {
        return il2cpp_class_num_fields(this);
    }

    // in lua
    auto GetFields() -> std::vector<FieldInfo*>
    {
        std::vector<FieldInfo*> fields;

        FieldInfo* field = nullptr;
        void* iter = nullptr;

        if (GetFieldCount() != 0)
        {
            while ((field = il2cpp_class_get_fields(this, &iter)))
            {
                fields.push_back(field);
            }
        }

        return fields;
    }

    auto GetFieldOffset(std::string field) -> size_t
    {
        auto Fields = GetFields();

        for (auto& Field : Fields)
        {
            if (Field->GetName() == field)
            {
                return Field->GetOffset();
            }
        }

        return 0;
    }

    auto GetType() -> class Il2CppType*
    {
        return il2cpp_class_get_type(this);
    }

    // in lua
    auto GetParent() -> class Il2CppClass*
    {
        return il2cpp_class_get_parent(this);
    }

    auto IsValueType() -> bool
    {
        return il2cpp_class_is_valuetype(this);
    }

    auto IsEnum() -> bool
    {
        return il2cpp_class_is_enum(this);
    }
};

template <typename T>
inline auto Il2CppObject::GetFromType(class Il2CppType* type, std::string filedName) -> T
{
    return GetByOffset<T>(type->GetFieldOffset(filedName));
}

template <typename T>
inline auto Il2CppObject::Get(std::string fieldName) -> T
{
    return GetByOffset<T>(klass->GetFieldOffset(fieldName));
}

inline CPPGenerator::Header* dumpHeader;

std::string Il2CppType::Parse()
{
    std::string ret = " UNKNOWN TYPE (" + std::to_string(type) + ") ";

    auto generateArrayStruct = [=](Il2CppType* arrayType)
    {
        dumpHeader->queueStruct(
            // arrayType->GetName() + "_array",
            [=]()
            {
                if (!dumpHeader->isAlreadyGeneratedStruct(arrayType->GetName() + "_array"))
                {
                    dumpHeader->structStart(arrayType->GetName() + "_array");
                    dumpHeader->variable<"Il2CppObject">("obj");
                    dumpHeader->variable<"Il2CppArrayBounds">("bounds", true);
                    dumpHeader->variable<"il2cpp_array_size_t">("max_length");

                    auto varName = arrayType->Parse() + "**";

                    if (!varName.starts_with("struct ") && !arrayType->isPrimitive())
                    {
                        varName = "struct " + varName;
                    }

                    dumpHeader->variable(varName, "m_Items");
                    dumpHeader->structEnd(false);
                }
            });
    };

    switch (type)
    {
    case IL2CPP_TYPE_VOID:
        ret = "void";
        break;

    case IL2CPP_TYPE_BOOLEAN:
        ret = "bool";
        break;

    case IL2CPP_TYPE_CHAR:
        ret = "uint16_t"; // Il2CppChar
        break;

    case IL2CPP_TYPE_I1:
        ret = "int8_t";
        break;

    case IL2CPP_TYPE_U1:
        ret = "uint8_t";
        break;

    case IL2CPP_TYPE_I2:
        ret = "int16_t";
        break;

    case IL2CPP_TYPE_U2:
        ret = "uint16_t";
        break;

    case IL2CPP_TYPE_I4:
        ret = "int32_t";
        break;

    case IL2CPP_TYPE_U4:
        ret = "uint32_t";
        break;

    case IL2CPP_TYPE_I8:
        ret = "int64_t";
        break;

    case IL2CPP_TYPE_U8:
        ret = "uint64_t";
        break;

    case IL2CPP_TYPE_R4:
        ret = "float";
        break;

    case IL2CPP_TYPE_R8:
        ret = "double";
        break;

    case IL2CPP_TYPE_STRING:
        ret = "System_String_o*";
        break;

    case IL2CPP_TYPE_TYPEDBYREF:
        ret = "Il2CppObject*";
        break;

    case IL2CPP_TYPE_I:
        ret = "intptr_t";
        break;

    case IL2CPP_TYPE_U:
        ret = "uintptr_t";
        break;

    case IL2CPP_TYPE_OBJECT:
        ret = "Il2CppObject*";
        break;

    case IL2CPP_TYPE_VAR:
    {
        // if (context != null)
        //{
        //     var genericParameter = executor.GetGenericParameteFromIl2CppType(il2CppType);
        //     var genericInst = il2Cpp.MapVATR<Il2CppGenericInst>(context.class_inst);
        //     var pointers = il2Cpp.MapVATR<ulong>(genericInst.type_argv, genericInst.type_argc);
        //     var pointer = pointers[genericParameter.num];
        //     var type = il2Cpp.GetIl2CppType(pointer);
        //     ret = type->Parse();
        // }

        ret = "Il2CppObject* ";
        break;
    }

    case IL2CPP_TYPE_PTR:
    {
        ret = data.type->Parse() + "* ";
        break;
    }

    case IL2CPP_TYPE_CLASS:
    {
        ret = this->GetName() + "_o* ";
        break;
    }

    case IL2CPP_TYPE_VALUETYPE:
    {
        if (this->data.generic_class && this->data.generic_class->cached_class; auto klass = this->data.generic_class->cached_class)
        {
            if (auto parent = klass->GetParent())
            {
                if (parent->GetName() != "Enum")
                {
                    ret = "";
                    break;
                }
            }

            if (klass->IsEnum())
            {
                ret = klass->GetName() + " ";
                break;
            }
        }

        // ret = this->GetName() + "_o* "; // THIS SHOULDN'T BE A POINTER I JUST DID THIS TO SEE IF IT COMPILES!!!!!
        break;
    }

    case IL2CPP_TYPE_GENERICINST:
    {
        if (this->data.generic_class && this->data.generic_class->cached_class; auto klass = this->data.generic_class->cached_class)
        {
            if (klass->IsValueType())
            {
                if (klass->IsEnum())
                {
                    ret = klass->GetType()->Parse();
                    break;
                }
                ret = klass->GetName() + "_o ";
                break;
            }
            ret = klass->GetName() + "_o* ";
            break;
        }
        ret = this->GetName() + "_o* ";
        break;
    }

    case IL2CPP_TYPE_ARRAY:
    {
        // @todo: Temporary workaround cuz some games has issues with this idk why
        if (__int64(data.array->etype) > 0x7FF000000000)
        {
            generateArrayStruct(data.array->etype);
            ret = data.array->etype->GetName() + "_array* ";
        }
        else
        {
            generateArrayStruct(this);
            ret = this->GetName() + "_array* ";
        }
        break;
    }

    case IL2CPP_TYPE_SZARRAY:
    {
        generateArrayStruct(data.type);
        ret = data.type->GetName() + "_array* ";
        break;
    }

    case IL2CPP_TYPE_MVAR:
    {
        // if (context != null)
        //{
        //     var genericParameter = executor.GetGenericParameteFromIl2CppType(il2CppType);
        //     var genericInst = il2Cpp.MapVATR<Il2CppGenericInst>(context.method_inst);
        //     var pointers = il2Cpp.MapVATR<ulong>(genericInst.type_argv, genericInst.type_argc);
        //     var pointer = pointers[genericParameter.num];
        //     var type = il2Cpp.GetIl2CppType(pointer);
        //    return ParseType(type);
        //}

        ret = "Il2CppObject* ";
        break;
    }
    }

    return ret;
}

bool shouldAddStructKeyword(Il2CppType* type, std::string typeName)
{
    if (!type->isPrimitive() && !typeName.starts_with("struct ") && typeName.find("*") != std::string::npos && typeName.find("int") == std::string::npos)
    {
        return true;
    }

    return false;
}

void DumpAllTypes()
{
    dumpHeader = new CPPGenerator::Header("il2cppDump");

    dumpHeader->pragma("once");
    dumpHeader->include("iostream", false);

    dumpHeader->AddText(R"(
typedef uintptr_t il2cpp_array_size_t;
typedef int32_t il2cpp_array_lower_bound_t;
struct Il2CppArrayBounds
{
    il2cpp_array_size_t length;
    il2cpp_array_lower_bound_t lower_bound;
};
struct Il2CppObject
{
};
        )");

    auto assemblies = Il2CppDomain::Get()->GetAssemblies();

    for (auto assembly : assemblies)
    {
        for (auto i = 0; i < assembly->GetImage()->GetClassCount(); i++)
        {
            auto klass = assembly->GetImage()->GetClassByIndex(i);

            dumpHeader->queueStruct(
                // klass->GetType()->GetName() + "_Fields",
                [=]()
                {
                    if (!dumpHeader->isAlreadyGeneratedStruct(klass->GetType()->GetName() + "_Fields"))
                    {
                        std::string parentName;

                        if (auto parent = klass->GetParent())
                        {
                            if (parent->GetName() != "Object" && parent->GetName() != "ValueType")
                            {
                                parentName = parent->GetType()->GetName() + "_Fields";
                            }

                            if (!parentName.empty() && !dumpHeader->isAlreadyGeneratedStruct(parentName))
                            {
                                dumpHeader->structStart(parentName);

                                auto fields = parent->GetFields();

                                for (auto field : fields)
                                {
                                    dumpHeader->variable(field->GetType()->Parse(), field->GetName(), false, false, false, false, false, shouldAddStructKeyword(field->GetType(), field->GetType()->Parse()));
                                }

                                dumpHeader->structEnd(false);
                            }
                        }

                        dumpHeader->structStart(klass->GetType()->GetName() + "_Fields", parentName);

                        auto fields = klass->GetFields();

                        for (auto field : fields)
                        {
                            dumpHeader->variable(field->GetType()->Parse(), field->GetName(), false, false, false, false, false, shouldAddStructKeyword(field->GetType(), field->GetType()->Parse()));
                        }

                        dumpHeader->structEnd(false);
                    }
                });

            dumpHeader->queueStruct(
                // klass->GetType()->GetName() + "_Fields",
                [=]()
                {
                    if (!dumpHeader->isAlreadyGeneratedStruct(klass->GetType()->GetName() + "_o"))
                    {
                        dumpHeader->structStart(klass->GetType()->GetName() + "_o");

                        if (!klass->IsValueType())
                        {
                            dumpHeader->variable<"void*">("klass");
                            dumpHeader->variable<"void*">("monitor");
                        }

                        dumpHeader->variable(klass->GetType()->GetName() + "_Fields", "fields");

                        dumpHeader->structEnd(false);
                    }
                });

            dumpHeader->flushStructs();
        }
    }

    delete dumpHeader;

    printf("Dumped types in %i assemblies\n", (int)assemblies.size());
}
