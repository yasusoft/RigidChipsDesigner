//---------------------------------------------------------------------------

#ifndef RCLuaH
#define RCLuaH
//---------------------------------------------------------------------------
class TRigidChipCore;
class lua_State;
class TRCLua
{
private:
        AnsiString ScriptString;
        char *Script;
        void SetScript(AnsiString script);

        lua_State *L;
public:
        static TRCLua *RCLua;
        double Tick;

        AnsiString ErrorMessage;
        __property AnsiString ScriptText = {read=ScriptString, write=SetScript};
        bool Run(TRigidChipCore *core);

        TRCLua();
        ~TRCLua();
};
//---------------------------------------------------------------------------
#endif
