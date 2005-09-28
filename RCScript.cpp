//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RCScript.h"
#include "RigidChip.h"
#include <math.h>
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TRCScript::TRCScript(TRigidChipCore *core)
{
  Core = core;
  Script = NULL;
  ScriptText = "";
}
//---------------------------------------------------------------------------
TRCScript::~TRCScript()
{
  delete[] Script;
}
//---------------------------------------------------------------------------
void TRCScript::SetScript(AnsiString s)
{
  ScriptString = s;
  Length = s.Length();
  Tick = 1;

  if (Script) delete[] Script;
  Script = new char[Length+1];
  CopyMemory(Script, s.c_str(), Length);
  Script[Length] = 0;
}
//---------------------------------------------------------------------------
bool TRCScript::Run()
{
  Variables.clear();
  Labels.clear();
  PC = 0;
  BackUp1 = "";
  BackUp2 = "";
  while (Statement(false));
  Variables.clear();
  PC = 0;
  BackUp1 = "";
  BackUp2 = "";
  while (Statement(true));
  Tick ++;
  return true;
}
//---------------------------------------------------------------------------
void TRCScript::SkipSP()
{ // スペース等を飛ばす
  while (PC < Length && Script[PC] <= ' ')
    PC ++;
}
//---------------------------------------------------------------------------
void TRCScript::PushBack(AnsiString s)
{
  if (BackUp1 != "")
    BackUp2 = BackUp1;
  BackUp1 = s;
}
AnsiString TRCScript::PopBack()
{
  AnsiString s = BackUp1;
  BackUp1 = BackUp2;
  BackUp2 = "";
  return s;
}
//---------------------------------------------------------------------------
AnsiString TRCScript::Symbol()
{
  if (BackUp1 != "")
    return PopBack();

  SkipSP();

  // コメントを飛ばす
  while (PC < Length && Script[PC] == '/')
  {
    PC ++;
    if (PC < Length && Script[PC] == '/')
    {
      while (PC < Length && Script[PC] != '\n')
        PC ++;
      SkipSP();
    }
    else
      return "/";
  }

  if (PC >= Length) return "";

  if ('0' <= Script[PC] && Script[PC] <= '9' || Script[PC] == '.')
  { // 数値
    AnsiString d;
    while (PC < Length && '0' <= Script[PC] && Script[PC] <= '9')
      d += AnsiString(Script[PC++]);
    if (PC < Length && Script[PC] == '.')
    {
      d += AnsiString(Script[PC++]);
      while (PC < Length && '0' <= Script[PC] && Script[PC] <= '9')
        d += AnsiString(Script[PC++]);
    }
    return d;
  }

  if (Script[PC] == '#')
  { // 色
    AnsiString d = "0x";
    PC ++;
    while (PC < Length && (
        '0' <= Script[PC] && Script[PC] <= '9'
     || 'a' <= Script[PC] && Script[PC] <= 'z'
     || 'A' <= Script[PC] && Script[PC] <= 'Z'
    ))
      d += AnsiString(Script[PC++]);
    d = IntToStr(StrToIntDef(d, 0));
    return d;
  }

  if ('a' <= Script[PC] && Script[PC] <= 'z'
   || 'A' <= Script[PC] && Script[PC] <= 'Z'
   || Script[PC] == '_'
  )
  { // 識別子
    AnsiString n;
    while (PC < Length && (
        'a' <= Script[PC] && Script[PC] <= 'z'
     || 'A' <= Script[PC] && Script[PC] <= 'Z'
     || '0' <= Script[PC] && Script[PC] <= '9'
     || Script[PC] == '_'
    ))
      n += AnsiString(Script[PC++]);
    return n;
  }

  if (Script[PC] == '"')
  { // 文字列
    AnsiString s = Script[PC++];
    while (PC < Length && Script[PC] != '"')
    {
      if (Script[PC] == '\n')
        return s+"\"";
      if (Script[PC] == '\\')
      {
        if (++PC < Length)
        {
          switch (Script[PC])
          {
            case '\\': s += "\\"; break;
            case '"': s += "\""; break;
            case 'n': s += "\n"; break;
            case 'r': s += "\r"; break;
            case 't': s += "\t"; break;
          }
          PC ++;
        }
        else
          s += "\\";
        continue;
      }
      s += AnsiString(Script[PC++]);
    }
    if (PC < Length)
      s += AnsiString(Script[PC++]);
    return s;
  }

  if (Script[PC] == '=' || Script[PC] == '!' || Script[PC] == '<' || Script[PC] == '>')
  { // 比較演算子
    AnsiString op = Script[PC++];
    if (Script[PC] == '=' || Script[PC] == '>')
    {
      op += AnsiString(Script[PC++]);
      return op;
    }
    return op;
  }

  // その他は１文字
  return AnsiString(Script[PC++]);
}
//---------------------------------------------------------------------------
bool TRCScript::Expect(AnsiString s, bool err, bool keep)
{
  AnsiString sym = Symbol();
  if (sym != s)
  {
    PushBack(sym);
    if (err)
      throw Exception("Expected: " + s);
    return false;
  }
  else if (keep)
    PushBack(sym);
  return true;
}
//---------------------------------------------------------------------------
AnsiString TRCScript::Expression(bool run)
{ // 式を処理
  AnsiString val = Term(run);
  AnsiString sym;
  while (1)
  {
    sym = Symbol();
    if (sym == "&" || sym == "and")
      val = (int)StrToFloatDef(Term(run), 0) & (int)StrToFloatDef(val, 0) ? "1" : "0";
    else if (sym == "|" || sym == "or")
      val = (int)StrToFloatDef(Term(run), 0) | (int)StrToFloatDef(val, 0) ? "1" : "0";
    else
      break;
  }
  PushBack(sym);
  return val;
}
//---------------------------------------------------------------------------
AnsiString TRCScript::Term(bool run)
{ // 部分式を繋げて式にする
  AnsiString val = Section(run);
  AnsiString sym;
  while (1)
  {
    sym = Symbol();
    if (sym == "=" || sym == "==")
      val = val == Section(run) ? "1" : "0";
    else if (sym == "<")
      val = StrToFloatDef(val, 0) < StrToFloatDef(Section(run), 0) ? "1" : "0";
    else if (sym == "<=")
      val = StrToFloatDef(val, 0) <= StrToFloatDef(Section(run), 0) ? "1" : "0";
    else if (sym == ">")
      val = StrToFloatDef(val, 0) > StrToFloatDef(Section(run), 0) ? "1" : "0";
    else if (sym == ">=")
      val = StrToFloatDef(val, 0) >= StrToFloatDef(Section(run), 0) ? "1" : "0";
    else if (sym == "!=" || sym == "<>")
      val = val != Section(run) ? "1" : "0";
    else
      break;
  }
  PushBack(sym);
  return val;
}
//---------------------------------------------------------------------------
AnsiString TRCScript::Section(bool run)
{ // 項を繋げて部分式にする
  AnsiString val, sym;
  
  // 単項
  sym = Symbol();
  if (sym == "+")
    val = Argument(run);
  else if (sym == "-")
    val = FloatToStr(- StrToFloatDef(Argument(run), 0));
  else
  {
    PushBack(sym);
    val = Argument(run);
  }

  while (1)
  {
    sym = Symbol();
    if (sym == "+")
      val = FloatToStr(StrToFloatDef(val, 0) + StrToFloatDef(Argument(run), 0));
    else if (sym == "-")
      val = FloatToStr(StrToFloatDef(val, 0) - StrToFloatDef(Argument(run), 0));
    else
      break;
  }
  PushBack(sym);
  return val;
}
//---------------------------------------------------------------------------
AnsiString TRCScript::Argument(bool run)
{ // 因子を繋げて項にする
  AnsiString val = Factor(run);
  AnsiString sym;
  while (1)
  {
    sym = Symbol();
    if (sym == "*")
      val = FloatToStr(StrToFloatDef(val, 0) * StrToFloatDef(Factor(run), 0));
    else if (sym == "/")
    {
      float f = StrToFloatDef(Factor(run), 0);
      val = FloatToStr(StrToFloatDef(val, 0) / (run ? f : 1));
    }
    else
      break;
  }
  PushBack(sym);
  return val;
}
//---------------------------------------------------------------------------
AnsiString TRCScript::Factor(bool run)
{ // 因子を処理
  AnsiString sym = Symbol();
  if (sym == "")
    return "";

  AnsiString symup = sym.UpperCase();
  if (sym == "-")
  {
    float f = StrToFloatDef(Factor(run), 0);
    return -f;
  }
  else if (sym[1] == '(')
  {
    AnsiString val = Expression(run);
    Expect(")");
    return val;
  }
  else if (sym[1] == '"')
  {
    return sym.SubString(2, sym.Length()-2);
  }
  else if ('0' <= sym[1] && sym[1] <= '9')
  {
    return sym;
  }
  else if ('A' <= symup[1] && symup[1] <= 'Z' || sym[1] == '_')
  { // 識別子
    if (Expect('(', false))
    { // 関数
      AnsiString val;
      if (symup == "_ABS")
      {
        float f = StrToFloatDef(Expression(run), 0);
        if (f < 0) f = -f;
        val = FloatToStr(f);
      }
      else if (symup == "_MOD")
      {
        int i1 = (int)StrToFloatDef(Expression(run), 0);
        Expect(",");
        int i2 = (int)StrToFloatDef(Expression(run), 0);
        if (i2 != 0)
          val = IntToStr(i1 % i2);
        else
          val = "-1.#J";
      }
      else if (symup == "_INT" || symup == "_FIX")
      {
        val = IntToStr((int)StrToFloatDef(Expression(run), 0));
      }
      else if (symup == "_FLOOR")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = floor(f);
        val = FloatToStr(f);
      }
      else if (symup == "_CEIL")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = ceil(f);
        val = FloatToStr(f);
      }
      else if (symup == "_ROUND")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = floor(f + 0.5);
        val = FloatToStr(f);
      }
      else if (symup == "_SGN")
      {
        float f = StrToFloatDef(Expression(run), 0);
        if (f > 0)
          val = "1";
        else if (f < 0)
          val = "-1";
        else
          val = "0";
      }
      else if (symup == "_RND")
      {
        val = FloatToStr(rand() / (float)RAND_MAX);
      }
      else if (symup == "_SQRT")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = sqrt(f);
        val = FloatToStr(f);
      }
      else if (symup == "_POW")
      {
        float f1 = StrToFloatDef(Expression(run), 0);
        Expect(",");
        float f2 = StrToFloatDef(Expression(run), 0);
        val = FloatToStr(pow(f1, f2));
      }
      else if (symup == "_EXP")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = exp(f);
        val = FloatToStr(f);
      }
      else if (symup == "_LEN2")
      {
        float a = StrToFloatDef(Expression(run), 0);
        Expect(",");
        float b = StrToFloatDef(Expression(run), 0);
        val = FloatToStr(sqrt(a*a+b*b));
      }
      else if (symup == "_LEN3")
      {
        float a = StrToFloatDef(Expression(run), 0);
        Expect(",");
        float b = StrToFloatDef(Expression(run), 0);
        Expect(",");
        float c = StrToFloatDef(Expression(run), 0);
        val = FloatToStr(sqrt(a*a+b*b+c*c));
      }
      else if (symup == "_PI")
      {
        val = "3.141592653589793238462643383279502884197169399375105820974944592";
      }
      else if (symup == "_TODEG")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = f * 180 / M_PI;
        val = FloatToStr(f);
      }
      else if (symup == "_TORAD")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = f * M_PI / 180;
        val = FloatToStr(f);
      }
      else if (symup == "_SIN")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = sin(f);
        val = FloatToStr(f);
      }
      else if (symup == "_COS")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = cos(f);
        val = FloatToStr(f);
      }
      else if (symup == "_TAN")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = tan(f);
        val = FloatToStr(f);
      }
      else if (symup == "_ASIN")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = asin(f);
        val = FloatToStr(f);
      }
      else if (symup == "_ACOS")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = acos(f);
        val = FloatToStr(f);
      }
      else if (symup == "_ATAN")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = atan(f);
        val = FloatToStr(f);
      }
      else if (symup == "_ATAN2")
      {
        float f1 = StrToFloatDef(Expression(run), 0);
        Expect(",");
        float f2 = StrToFloatDef(Expression(run), 0);
        if (f1 == 0 && f2 == 0)
          val = "0";
        else
          val = FloatToStr(atan2(f1, f2));
      }
      else if (symup == "_LOG")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = log(f);
        val = FloatToStr(f);
      }
      else if (symup == "_LOG10")
      {
        float f = StrToFloatDef(Expression(run), 0);
        f = log10(f);
        val = FloatToStr(f);
      }
      else if (symup == "_DT")
      {
        val = "0.03";
      }
      else if (symup == "_FPS")
      {
        val = "30";
      }
      else if (symup == "_TICK" || symup == "_TICKS")
      {
        val = FloatToStr(Tick);
      }
      else if (symup == "_SETTICK" || symup == "_SETTICKS")
      {
        if (!Expect(")", false, true))
        {
          float f = StrToFloatDef(Expression(run), 0);
          if (run)
            Tick = f;
        }
        val = "";
      }
      else if (symup == "_CHIPS")
      {
        val = "";
      }
      else if (symup == "_WEIGHT")
      {
        val = "";
      }
      else if (symup == "_WIDTH")
      {
        val = IntToStr(Form1->PaintPanel->Width);
      }
      else if (symup == "_HEIGHT")
      {
        val = IntToStr(Form1->PaintPanel->Height);
      }
      else if (symup == "_FACE")
      {
        val = "0";
      }
      else if (symup == "_KEY")
      {
        int key = (int)StrToFloatDef(Expression(run), 0);
        val = Form1->Keys[key] ? "1" : "0";
      }
      else if (symup == "_KEYUP")
      {
        int key = (int)StrToFloatDef(Expression(run), 0);
        val = Form1->KeyUp[key] ? "0" : "1";
      }
      else if (symup == "_KEYDOWN")
      {
        int key = (int)StrToFloatDef(Expression(run), 0);
        val = Form1->KeyDown[key] ? "1" : "0";
      }
      else if (symup == "_ANALOG" || symup == "_HAT")
      {
        Expression(run);
        val = "0";
      }
      else if (symup == "_MX")
      {
        val = IntToStr(Form1->MouseX);
      }
      else if (symup == "_MY")
      {
        val = IntToStr(Form1->MouseY);
      }
      else if (symup == "_ML")
      {
        val = Form1->MouseLeft ? "1" : "0";
      }
      else if (symup == "_MM")
      {
        val = Form1->MouseMiddle ? "1" : "0";
      }
      else if (symup == "_MR")
      {
        val = Form1->MouseRight ? "1" : "0";
      }
      else if (symup == "_SETCOLOR")
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_MOVE2D" || symup == "_LINE2D")
      {
        Expression(run);
        Expect(",");
        Expression(run);
        val = "";
      }
      else if (symup == "_MOVE3D" || symup == "_LINE3D")
      {
        Expression(run);
        Expect(",");
        Expression(run);
        Expect(",");
        Expression(run);
        val = "";
      }
      else if (symup == "_X" || symup == "_Y" || symup == "_Z"
       || symup == "_EX" || symup == "_EY" || symup == "_EZ"
       || symup == "_AX" || symup == "_AY" || symup == "_AZ"
       || symup == "_LX" || symup == "_LY" || symup == "_LZ"
       || symup == "_WX" || symup == "_WY" || symup == "_WZ"
       || symup == "_VX" || symup == "_VY" || symup == "_VZ" || symup == "_VEL"
       || symup == "_FX" || symup == "_FY" || symup == "_FZ"
       || symup == "_GX" || symup == "_GY" || symup == "_GZ"
       || symup == "_XX" || symup == "_XY" || symup == "_XZ"
       || symup == "_YX" || symup == "_YY" || symup == "_YZ"
       || symup == "_ZX" || symup == "_ZY" || symup == "_ZZ"
       || symup == "_QX" || symup == "_QY" || symup == "_QZ"
      )
      {
        if (!Expect(")", false, true))
          Expression(run);
        val = "";
      }
      else if (symup == "_RX" || symup == "_RY" || symup == "_RZ")
      {
        Expression(run);
        Expect(",");
        Expression(run);
        val = "";
      }
      else if (symup == "_H")
      {
        if (!Expect(")", false, true))
          Expression(run);
        val = "-100000";
      }
      else if (symup == "_I")
      {
        Expression(run);
        Expect(",");
        Expression(run);
        Expect(",");
        Expression(run);
        val = "";
      }
      else if (symup == "_TYPE")
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_PARENT")
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_TOP")
      {
        Expression(run);
        val = "0";
      }
      else if (symup == "_DIR")
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_ANGLE"
       || symup == "_POWER"
       || symup == "_SPRING"
       || symup == "_DAMPER"
       || symup == "_BRAKE"
       || symup == "_COLOR"
       || symup == "_OPTION"
       || symup == "_EFFECT"
       || symup == "_USER1"
       || symup == "_USER2"
      )
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_M")
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_E")
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_T")
      {
        Expression(run);
        val = "-1";
      }
      else if (symup == "_BYE" || symup == "_SPLIT")
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_OX" || symup == "_OY" || symup == "_OZ" || symup == "_MOBJ")
      {
        Expression(run);
        val = "";
      }
      else if (symup == "_IOBJ")
      {
        Expression(run);
        Expect(",");
        Expression(run);
        Expect(",");
        Expression(run);
        val = "";
      }
      else if (symup == "_CCD" || symup == "_RED" || symup == "_GREEN" || symup == "_BLUE")
      {
        Expression(run);
        Expect(",");
        Expression(run);
        val = "0";
      }
      else if (symup == "_ZOOM")
      {
        Expression(run);
        val = "";
      }
      else
        throw Exception("Unkwon function: " + sym);

      Expect(')');
      return val;
    }
    else
    { // 変数
      if (!run)
        return "";
      TRigidChipsVariableMap::iterator i = Core->Variables.find(symup);
      if (i != Core->Variables.end())
        return FloatToStr(i->second->Value);
      else
        return Variables[symup];
    }
  }

  throw Exception("Unknown symbol: " + sym);
}
//---------------------------------------------------------------------------
bool TRCScript::Statement(bool run)
{ // 文を処理
  AnsiString sym = Symbol();
  if (sym == "")
    return false;

  AnsiString symup = sym.UpperCase();
  if (symup == "IF")
  {
    sym = Expression(run);
    bool flag = sym.ToIntDef(0) != 0;
    Expect("{");
    do
    {
      sym = Symbol();
      if (sym == "}")
        break;
      else
        PushBack(sym);
    } while (Statement(flag && run));
    return true;
  }
  else if (symup == "GOTO")
  {
    sym = Symbol();
    if (run)
    {
      std::map<AnsiString, int>::iterator i = Labels.find(sym.UpperCase());
      if (i != Labels.end())
      {
        PC = i->second;
        BackUp1 = "";
      }
      else
        throw Exception("Unknown label: " + sym);
    }
    return true;
  }
  else if (symup == "LABEL")
  {
    sym = Symbol();
    if (!run)
      Labels[sym.UpperCase()] = PC;
    return true;
  }
  else if (symup == "PRINT" || symup == "OUT")
  {
    sym = Expression(run);
    AnsiString s;
    while (Expect(',', false))
      s += Expression(run);
    if (run)
    {
      int i = sym.ToIntDef(0);
      if (0 <= i && i < 100)
      {
        while (Form1->ScriptOut->Lines->Count <= i)
          Form1->ScriptOut->Lines->Add("");
        Form1->ScriptOut->Lines->Strings[i] = s;
      }
    }
    return true;
  }
  else if ('A' <= symup[1] && symup[1] <= 'Z' || symup[1] == '_')
  { // 識別子
    if (Expect("=", false))
    { // 代入文
      AnsiString val = Expression(run);
      if (run)
      {
        TRigidChipsVariableMap::iterator i = Core->Variables.find(symup);
        if (i != Core->Variables.end())
          i->second->Value = StrToFloatDef(val, 0);
        else
          Variables[symup] = val;
      }
    }
    else
    {
      PushBack(sym);
      Expression(run);
    }
    return true;
  }
  else if (sym == ":" || sym == "," || sym == '}')
  {
    BackUp1 = "";
    return true;
  }
  else
  {
    PushBack(sym);
    Expression(run);
    return true;
  }
}
//---------------------------------------------------------------------------

