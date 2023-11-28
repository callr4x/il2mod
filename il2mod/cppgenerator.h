#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <functional>
#include <queue>

namespace CPPGenerator
{
    // https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/
    template <size_t N>
    struct StringLiteral
    {
        constexpr StringLiteral(const char (&str)[N])
        {
            std::copy_n(str, N, value);
        }

        char value[N];
    };

    class Header
    {
        bool isInNamespace;
        bool isInStruct;
        bool isInClass;

        std::vector<std::string> generatedStructs;
        std::vector<std::string> generatedClasses;

        std::queue<std::function<void()>> queuedStructs;

        std::string fileName = "";
        std::stringstream file;

    public:
        Header(std::string _fileName)
            : fileName(_fileName)
        {
        }

        ~Header()
        {
            std::fstream out;
            out.open(fileName + ".h", std::fstream::out | std::fstream::trunc);

            out << file.rdbuf();
            out.close();
        }

        auto AddText(std::string text)
        {
            file << text;
        }

        auto isAlreadyGeneratedStruct(std::string name)
        {
            for (auto&& str : generatedStructs)
            {
                if (str == name)
                {
                    return true;
                }
            }

            return false;
        }

        auto isAlreadyGeneratedClass(std::string name)
        {
            for (auto&& str : generatedClasses)
            {
                if (str == name)
                {
                    return true;
                }
            }

            return false;
        }

        auto queueStruct(std::function<void()> lambda)
        {
            queuedStructs.push(lambda);
        }

        void flushStructs()
        {
            while (!queuedStructs.empty())
            {
                queuedStructs.front()();
                queuedStructs.pop();
            }
        }

        void tab(size_t count)
        {
            for (size_t i = 0; i < count; i++)
            {
                file << "\t";
            }
        }

        void forwardDeclare(std::string name, std::string type = "struct")
        {
            file << type << " " << name << ";\n";
        }

        void pragma(std::string expr)
        {
            file << "#pragma " << expr << "\n"
                 << std::endl;
        }

        void include(std::string fileName, bool useQuotes = true)
        {
            if (useQuotes)
                file << "#include \"" << fileName << "\"" << std::endl;
            else
                file << "#include <" << fileName << ">" << std::endl;
        }

        void namespaceStart(std::string name)
        {
            file << "namespace " << name << "\n{" << std::endl;
            isInNamespace = true;
        }

        void namespaceEnd()
        {
            if (isInNamespace)
            {
                file << "}\n"
                     << std::endl;
                isInNamespace = false;
            }
        }

        void classStart(std::string name, std::string parent)
        {
            generatedClasses.push_back(name);

            if (isInNamespace)
                tab(1);

            file << "class " << name;

            if (!parent.empty())
                file << " : public " << parent;

            file << "\n";

            if (isInNamespace)
                tab(1);

            file << "{" << std::endl;
            isInClass = true;
        }

        void definePublic()
        {
            if (isInClass)
            {
                file << "public:" << std::endl;
            }
        }

        void definePrivate()
        {
            if (isInClass)
            {
                file << "private:" << std::endl;
            }
        }

        void classEnd()
        {
            if (isInClass)
            {
                if (isInNamespace)
                    tab(1);

                file << "};\n"
                     << std::endl;
                isInClass = false;
            }
        }

        void structStart(std::string name, std::string parent = "")
        {
            generatedStructs.push_back(name);

            if (isInClass)
                tab(1);

            if (isInNamespace)
                tab(1);

            file << "struct " << name;

            if (!parent.empty())
                file << " : " << parent;

            file << "\n";

            if (isInClass)
                tab(1);

            if (isInNamespace)
                tab(1);

            file << "{" << std::endl;
            isInStruct = true;
        }

        void structEnd(bool flushQueue = true)
        {
            if (isInStruct)
            {
                if (isInClass)
                    tab(1);

                if (isInNamespace)
                    tab(1);

                file << "};\n"
                     << std::endl;
                isInStruct = false;
            }

            if (flushQueue)
                flushStructs();
        }

        template <StringLiteral lit>
        void variable(std::string name, bool isPtr = false, bool isRef = false, bool isStatic = false, bool isConst = false, bool isConstexpr = false, bool isStruct = false)
        {
            constexpr auto typeName = lit.value;

            variable(typeName, name, isPtr, isRef, isStatic, isConst, isConstexpr, isStruct);
        }

        template <typename T>
        void variable(std::string name, bool isPtr = false, bool isRef = false, bool isStatic = false, bool isConst = false, bool isConstexpr = false, bool isStruct = false)
        {
            variable(typeid(T).name(), name, isPtr, isRef, isStatic, isConst, isConstexpr, isStruct);
        }

        void variable(std::string type, std::string name, bool isPtr = false, bool isRef = false, bool isStatic = false, bool isConst = false, bool isConstexpr = false, bool isStruct = false)
        {
            variable(type.c_str(), name, isPtr, isRef, isStatic, isConst, isConstexpr, isStruct);
        }

        void variable(const char* type, std::string name, bool isPtr = false, bool isRef = false, bool isStatic = false, bool isConst = false, bool isConstexpr = false, bool isStruct = false)
        {
            if (isInStruct)
                tab(1);

            if (isInClass)
                tab(1);

            if (isInNamespace)
                tab(1);

            if (isConstexpr)
                file << "constexpr ";

            if (isConst)
                file << "const ";

            if (isStatic)
                file << "static ";

            if (isStruct)
                file << "struct ";

            file << type;

            if (isPtr)
                file << "*";

            if (isRef)
                file << "&";

            file << " " << name << ";" << std::endl;
        }
    };
}
