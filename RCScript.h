//---------------------------------------------------------------------------

#ifndef RCScriptH
#define RCScriptH
//---------------------------------------------------------------------------
#include <map>
//---------------------------------------------------------------------------
class TRigidChipCore;
class TRCScript
{
private:
        TRigidChipCore *Core;
        AnsiString ScriptString;
        char *Script;
        void SetScript(AnsiString script);

        double Tick;

        std::map<AnsiString, int> Labels;
        int PC, Length;
        AnsiString BackUp1, BackUp2;
        void SkipSP();
        void PushBack(AnsiString s);
        AnsiString PopBack();
        AnsiString Symbol();
        bool Expect(AnsiString s, bool err = true, bool keep = false);
        AnsiString Expression(bool run);
        AnsiString Term(bool run);
        AnsiString Section(bool run);
        AnsiString Argument(bool run);
        AnsiString Factor(bool run);
        bool Statement(bool run);
public:
        __property AnsiString ScriptText = {read=ScriptString, write=SetScript};
        __property int ProgramCounter = {read=PC};

        std::map<AnsiString, AnsiString> Variables;
        bool Run();

        TRCScript(TRigidChipCore *core);
        ~TRCScript();
};
//---------------------------------------------------------------------------
#endif
 