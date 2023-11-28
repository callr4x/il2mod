PrepareMethod = Il2CppDomain.Get():GetAssembly("Assembly-CSharp"):GetImage():GetClass("", "MainMenu2"):GetMethod(
    "OpenSocialLink", 1);

function PrepareHook(params, returnValue)
    Text = Cast(params:GetArg(1), "Il2CppString");
    print("Text:", Text:ToString());

    params:SetArg(1, Il2CppString.FromString("https://example.com/"));
end

PrepareDetour = CreateDetour(PrepareMethod:GetMethodPointer(), "void", {"void*", "void*", "void*"}, PrepareHook);
