//---------------------------------------------------------------------------

#ifndef RigidChipH
#define RigidChipH
//---------------------------------------------------------------------------
#include <GL/gl.h>
#include <GL/glu.h>
#include <map>
//---------------------------------------------------------------------------
#pragma warn -8027
//---------------------------------------------------------------------------
class TRigidChip;
class TRigidChipCore;
//---------------------------------------------------------------------------
class TRigidChipsVariable
{
private:
        TRigidChipCore *core;
        double FValue;
        void SetValue(double v);
public:
        __property double Value = {read=FValue, write=SetValue};
        double Default, Min, Max, Step;
        bool FlagColor, FlagMax, FlagStep, Disp;

        bool Modify;
        void Act();

        TRigidChipsVariable(TRigidChipCore *c, AnsiString opt);
};
//---------------------------------------------------------------------------
class TRigidChipsKey
{
private:
        TRigidChipCore *core;
public:
        AnsiString Variable;
        double Step;
        void Act();

        TRigidChipsKey(TRigidChipCore *c, AnsiString var, AnsiString opt);
};
//---------------------------------------------------------------------------
typedef enum { rdCore = 0, rdNorth = 1, rdEast = 2, rdSouth = 3, rdWest = 4 } TRigidChipsDirection;
typedef enum { ctNone = 0, ctCore, ctChip, ctFrame, ctWeight, ctWheel, ctRLW, ctJet, ctRudder, ctRudderF, ctTrim, ctTrimF, ctArm, ctCowl } TRigidChipsType;
//---------------------------------------------------------------------------
class TRigidChip
{
private:
        TStringList *FOptions;
        TList *ChipList;

        TRigidChipCore *FCore;
        void SetCore(TRigidChipCore *c);

        int GetSubChipsCount();
        TRigidChip *GetSubChip(int i);
protected:
        virtual void Translate();
        virtual void Rotate();
        virtual void Rotate2(){}
        virtual void TranslateR();
        virtual void RotateR();
        virtual void RotateR2(){}
        virtual void DrawMain();
        virtual void DrawTranslucentMain(){}

        TRigidChip();
public:
        TRigidChipsDirection Direction;

        __property TStringList *Options = {read=FOptions};
        void SetOptions(AnsiString opt);

        virtual TRigidChipsType GetType(){ return ctNone; }
        virtual AnsiString GetTypeString(){ return "NullPo"; }
        virtual GLuint GetTexture(){ return 0; }
        int Draw(TRigidChip *caller = NULL);
        void DrawTranslucent(TRigidChip *caller = NULL);
        virtual void Act();

        __property TRigidChipCore *Core = {read=FCore, write=SetCore};
        TRigidChip *Parent;
        AnsiString MemoChip;

        virtual float GetMass(){ return 25.20; }

        __property TRigidChip *SubChips[int i] = {read=GetSubChip};
        __property int SubChipsCount = {read=GetSubChipsCount};
        void AddSubChip(TRigidChip *chip);
        void DelSubChip(int i);
        void DelSubChip(TRigidChip *chip);

        virtual ~TRigidChip();
};
//---------------------------------------------------------------------------
typedef std::map<AnsiString, TRigidChipsVariable*> TRigidChipsVariableMap;
class TRCScript;
class TRigidChipCore : public TRigidChip
{
private:
        TStringList *FKeyList;
        TRigidChipsKey* GetKey(int i);
public:
        TRigidChipsVariableMap Variables;
        void CheckVariable();
        void StepVariable();

        __property TStringList *KeyList = {read=FKeyList};
        __property TRigidChipsKey *Keys[int i] = {read=GetKey};
        void AddKey(AnsiString keyn, TRigidChipsKey *key);
        void ActKey(AnsiString keyn);

        bool FlagRCD;
        AnsiString MemoModel;
        AnsiString Lua;
        TRCScript *Script;

        bool StrToDouble(AnsiString str, double *d);

        bool ShowGhost, ShowCowl, ShowBalloon;
        TRigidChip *Select, *Deleted;
        double CGravity[3], Weight;

        virtual TRigidChipsType GetType(){ return ctCore; }
        virtual AnsiString GetTypeString(){ return "Core"; }
        static GLuint Texture;
        virtual GLuint GetTexture(){ return Texture; }

        TRigidChipCore();
        virtual ~TRigidChipCore();
};
//---------------------------------------------------------------------------
class TRigidChipChip : public TRigidChip
{
public:
        virtual TRigidChipsType GetType(){ return ctChip; }
        virtual AnsiString GetTypeString(){ return "Chip"; }
        static GLuint Texture;
        virtual GLuint GetTexture(){ return Texture; }
};
class TRigidChipFrame : public TRigidChipChip
{
protected:
        virtual void DrawMain();
        virtual void DrawTranslucentMain();
public:
        virtual TRigidChipsType GetType(){ return ctFrame; }
        virtual AnsiString GetTypeString(){ return "Frame"; }
        virtual float GetMass(){ return 12.60; }
};
//---------------------------------------------------------------------------
class TRigidChipWeight : public TRigidChip
{
public:
        virtual TRigidChipsType GetType(){ return ctWeight; }
        virtual AnsiString GetTypeString(){ return "Weight"; }
        virtual float GetMass();
        static GLuint Texture;
        virtual GLuint GetTexture(){ return Texture; }
};
//---------------------------------------------------------------------------
class TRigidChipWheel : public TRigidChip
{
protected:
        int angle;
        virtual void Rotate2();
        virtual void RotateR2();
        virtual void DrawMain();
public:
        virtual TRigidChipsType GetType(){ return ctWheel; }
        virtual AnsiString GetTypeString(){ return "Wheel"; }
        virtual float GetMass(){ return 36.76+42.41; }
        static GLuint Texture;
        virtual GLuint GetTexture(){ return Texture; }
        virtual void Act();
        TRigidChipWheel();
};
class TRigidChipRLW : public TRigidChipWheel
{
public:
        virtual TRigidChipsType GetType(){ return ctRLW; }
        virtual AnsiString GetTypeString(){ return "RLW"; }
        static GLuint Texture;
        virtual GLuint GetTexture(){ return Texture; }
};
//---------------------------------------------------------------------------
class TRigidChipJet : public TRigidChip
{
protected:
        static GLUquadricObj *quadric;
        int anime;
        virtual void DrawMain();
        virtual void DrawTranslucentMain();
public:
        virtual TRigidChipsType GetType(){ return ctJet; }
        virtual AnsiString GetTypeString(){ return "Jet"; }
        virtual float GetMass(){ return 36.76; }
        static GLuint Texture, TextureBall;
        virtual GLuint GetTexture(){ return Texture; }
        virtual void Act();
};
//---------------------------------------------------------------------------
class TRigidChipRudder : public TRigidChip
{
protected:
        virtual void Rotate();
        virtual void RotateR();
public:
        virtual TRigidChipsType GetType(){ return ctRudder; }
        virtual AnsiString GetTypeString(){ return "Rudder"; }
        static GLuint Texture;
        virtual GLuint GetTexture(){ return Texture; }
};
class TRigidChipRudderF : public TRigidChipRudder
{
protected:
        virtual void DrawMain();
        virtual void DrawTranslucentMain();
public:
        virtual TRigidChipsType GetType(){ return ctRudderF; }
        virtual AnsiString GetTypeString(){ return "RudderF"; }
        virtual float GetMass(){ return 12.60; }
};
//---------------------------------------------------------------------------
class TRigidChipTrim : public TRigidChip
{
protected:
        virtual void Rotate();
        virtual void RotateR();
public:
        virtual TRigidChipsType GetType(){ return ctTrim; }
        virtual AnsiString GetTypeString(){ return "Trim"; }
        static GLuint Texture;
        virtual GLuint GetTexture(){ return Texture; }
};
class TRigidChipTrimF : public TRigidChipTrim
{
protected:
        virtual void DrawMain();
        virtual void DrawTranslucentMain();
public:
        virtual TRigidChipsType GetType(){ return ctTrimF; }
        virtual AnsiString GetTypeString(){ return "TrimF"; }
        virtual float GetMass(){ return 12.60; }
};
//---------------------------------------------------------------------------
class TRigidChipArm : public TRigidChip
{
protected:
        static GLUquadricObj* quadric;
        int energy;
        bool anime;
        virtual void DrawMain();
        virtual void DrawTranslucentMain();
public:
        virtual TRigidChipsType GetType(){ return ctArm; }
        virtual AnsiString GetTypeString(){ return "Arm"; }
        virtual float GetMass(){ return 50.40; }
        static GLuint Texture;
        virtual GLuint GetTexture(){ return Texture; }
        virtual void Act();

        TRigidChipArm();
};
//---------------------------------------------------------------------------
class TRigidChipCowl : public TRigidChip
{
protected:
        virtual void DrawCowl(int transmode);
        virtual void DrawMain();
        virtual void DrawTranslucentMain();
public:
        virtual TRigidChipsType GetType(){ return ctCowl; }
        virtual AnsiString GetTypeString(){ return "Cowl"; }
        virtual float GetMass(){ return 0; }
        static GLuint Texture0, Texture2, Texture34, Texture5;
};
//---------------------------------------------------------------------------
#pragma warn .8027
//---------------------------------------------------------------------------
#endif
