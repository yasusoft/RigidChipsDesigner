//---------------------------------------------------------------------------

#ifndef RigidChipH
#define RigidChipH
//---------------------------------------------------------------------------
#include <GL/gl.h>
#include <GL/glu.h>
//---------------------------------------------------------------------------
class TRigidChip;
class TRigidChipCore;
//---------------------------------------------------------------------------
class TRigidChipsVariable
{
private:
        TRigidChipCore *core;
        TStringList *FOptions;
        double FValue, def, min, max, step;
        bool fmax, fstep;
        void SetValue(double v);
public:
        __property TStringList *Options = {read=FOptions};

        __property double Value = {read=FValue, write=SetValue};
        bool Modify;
        void Act();

        TRigidChipsVariable(TRigidChipCore *c, AnsiString opt);
        ~TRigidChipsVariable();
};
//---------------------------------------------------------------------------
class TRigidChipsKey
{
private:
        TRigidChipCore *core;
        AnsiString variable;
        TStringList *FOptions;
        double step;
public:
        __property TStringList *Options = {read=FOptions};
        void Act();

        TRigidChipsKey(TRigidChipCore *c, AnsiString var, AnsiString opt);
        ~TRigidChipsKey();
};
//---------------------------------------------------------------------------
typedef enum { rdCore, rdNorth, rdEast, rdWest, rdSouth } TRigidChipsDirection;
//---------------------------------------------------------------------------
class TRigidChip
{
private:
        TStringList *FOptions;
        TList *ChipList;

        int GetSubChipsCount();
        TRigidChip *GetSubChip(int i);
protected:
        void translate();
        void rotate();
        void setcolor();
        void direct();

        TRigidChip();
        ~TRigidChip();
public:
        TRigidChipsDirection Direction;

        __property TStringList *Options = {read=FOptions};
        void SetOptions(AnsiString opt);

        virtual void Draw();
        virtual void Act();
        virtual void Delete(){ delete this; }

        TRigidChipCore *Core;
        TRigidChip *Parent;
        __property int SubChipsCount = {read=GetSubChipsCount};
        __property TRigidChip *SubChips[int i] = {read=GetSubChip};
        void AddSubChip(TRigidChip *chip);
};
//---------------------------------------------------------------------------
class TRigidChipCore : public TRigidChip
{
private:
        TStringList *VariableList;
        TRigidChipsVariable* GetVariable(AnsiString name);
        TStringList *KeyList;
public:
        __property TRigidChipsVariable *Variables[AnsiString name] = {read=GetVariable};
        void AddVariable(AnsiString name, TRigidChipsVariable *var);
        void CheckVariable();
        void StepVariable();

        void ActKey(AnsiString keyn);
        void AddKey(AnsiString keyn, TRigidChipsKey *key);

        bool StrToDouble(AnsiString str, double *d);

        bool ShowGhost;

        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
        TRigidChipCore();
        ~TRigidChipCore();
};
//---------------------------------------------------------------------------
class TRigidChipChip : public TRigidChip
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
class TRigidChipFrame : public TRigidChipChip
{
public:
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
class TRigidChipWeight : public TRigidChip
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
class TRigidChipWheel : public TRigidChip
{
protected:
        int angle;
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Act();
        virtual void Delete(){ delete this; }
        TRigidChipWheel();
};
class TRigidChipRLW : public TRigidChipWheel
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
class TRigidChipJet : public TRigidChip
{
private:
        static GLUquadricObj *quadric;
        int anime;
public:
        static GLuint Texture, TextureBall;
        virtual void Draw();
        virtual void Act();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
class TRigidChipRudder : public TRigidChip
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
class TRigidChipRudderF : public TRigidChipRudder
{
public:
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
class TRigidChipTrim : public TRigidChip
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
class TRigidChipTrimF : public TRigidChipTrim
{
public:
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
class TRigidChipArm : public TRigidChip
{
private:
        static GLUquadricObj* quadric;
        unsigned int energy;
        bool anime;
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Act();
        virtual void Delete(){ delete this; }
        TRigidChipArm();
};
//---------------------------------------------------------------------------
class TRigidChipCowl : public TRigidChip
{
private:
        bool fopt;
        int opt;
public:
        static GLuint Texture0, Texture2, Texture34, Texture5;
        virtual void Draw();
        virtual void Delete(){ delete this; }
        TRigidChipCowl();
};
//---------------------------------------------------------------------------
#endif
 