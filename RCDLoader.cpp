//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RCDLoader.h"
#include "RigidChip.h"
#include "RCScript.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TRigidChipCore* TRCDLoader::LoadFromString(AnsiString str)
{
  ErrorMessage = "";

  TStringStream *stream = new TStringStream(str);
  try
  {
    return load(stream);
  }
  __finally
  {
    delete stream;
  }
  return NULL;
}
//---------------------------------------------------------------------------
TRigidChipCore* TRCDLoader::LoadFromFile(AnsiString filename)
{
  ErrorMessage = "";

  TFileStream *stream = NULL;
  try
  {
    stream = new TFileStream(filename, fmOpenRead);
    return load(stream);
  }
  __finally
  {
    if (stream)
      delete stream;
  }
  return NULL;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::StringToChip(AnsiString str, TRigidChip *addto)
{
  ErrorMessage = "";

  TStringStream *stream = new TStringStream(str);
  try
  {
    return readchips(stream, addto);
  }
  __finally
  {
    delete stream;
  }
  return NULL;
}
//---------------------------------------------------------------------------
void TRCDLoader::skipsp(TStream *stream)
{ // スペース等を飛ばす
  unsigned char c;
  while (stream->Read(&c, 1) == 1)
  {
    if (c > ' ')
    {
      stream->Seek(-1, soFromCurrent);
      break;
    }
  }
}
AnsiString TRCDLoader::readto(TStream *stream, char to)
{ // 指定文字まで読む
  AnsiString s;
  char c;
  while (stream->Read(&c, 1) == 1 && c != to)
    s += AnsiString(c);
  return s;
}
AnsiString TRCDLoader::readcomment(TStream *stream)
{ // コメントだったら改行まで飛ばす
  char c;
  if (stream->Read(&c, 1) == 1)
  {
    if (c != '/')
    {
      stream->Seek(-1, soFromCurrent);
      return "";
    }
  }
  else
    return "";

  if (stream->Read(&c, 1) == 1)
  {
    if (c != '/')
    {
      stream->Seek(-2, soFromCurrent);
      return "";
    }
  }
  else
    return "";

  if (stream->Read(&c, 1) == 1 && c != ' ')
    stream->Seek(-1, soFromCurrent);

  AnsiString s;
  while (stream->Read(&c, 1) == 1)
  {
    s += AnsiString(c);
    if (c == '\n')
      break;
  }
  return s;
}
AnsiString TRCDLoader::readkey(TStream *stream)
{ // 英数字を読み込む
  while (skipsp(stream), readcomment(stream) != "");

  AnsiString key;
  char c;
  while (stream->Read(&c, 1) == 1)
  {
    if ('A' <= c && c <= 'Z' || 'a' <= c && c <= 'z' || '0' <= c && c <= '9' || c == '_' || c == '-')
      key += AnsiString(c);
    else
    {
      stream->Seek(-1, soFromCurrent);
      break;
    }
  }

  while (skipsp(stream), readcomment(stream) != "");

  return key;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::readchips(TStream *stream, TRigidChip *addto)
{
  TRigidChip *add = NULL;
  TList *adds = new TList;

  TStack *stack = new TStack;
  TRigidChip *parent = addto;
  try
  {
    AnsiString memo, cmt;
    while (skipsp(stream), (cmt = readcomment(stream)) != "")
      memo += cmt;

    char c;
    while (stream->Read(&c, 1) == 1)
    {
      if (c == '}')
      {
        memo = "";
        while (skipsp(stream), (cmt = readcomment(stream)) != "")
          memo += cmt;
        if (stack->Count() == 0)
          break;
        parent = (TRigidChip*)stack->Pop();
        continue;
      }
      stream->Seek(-1, soFromCurrent);

      AnsiString key = readkey(stream).LowerCase();
      if (key == "")
      {
        ErrorMessage = "can't read direction or chip type";
        return NULL;
      }

      TRigidChipsDirection direct = rdCore;
      if (key == "n")
        direct = rdNorth;
      else if (key == "e")
        direct = rdEast;
      else if (key == "w")
        direct = rdWest;
      else if (key == "s")
        direct = rdSouth;

      if (direct != rdCore)
      {
        if (stream->Read(&c, 1) != 1 || c != ':')
        {
          ErrorMessage = ": must be after direction";
          return NULL;
        }

        key = readkey(stream).LowerCase();
        if (key == "")
        {
          ErrorMessage = "chip type must be after direction:";
          return NULL;
        }
      }
      else
        direct = rdNorth;

      if (stream->Read(&c, 1) != 1 || c != '(')
      {
        ErrorMessage = "open square bracket must be after chip type";
        return NULL;
      }

      TRigidChip *chip;
      if (parent && parent->GetType() == ctCowl && key != "cowl")
      {
        ErrorMessage = "Can't join " + key + " to cowl";
        return NULL;
      }
      else if (key == "chip")
        chip = new TRigidChipChip;
      else if (key == "frame")
        chip = new TRigidChipFrame;
      else if (key == "weight")
        chip = new TRigidChipWeight;
      else if (key == "wheel")
        chip = new TRigidChipWheel;
      else if (key == "rlw")
        chip = new TRigidChipRLW;
      else if (key == "jet")
        chip = new TRigidChipJet;
      else if (key == "rudder")
        chip = new TRigidChipRudder;
      else if (key == "rudderf")
        chip = new TRigidChipRudderF;
      else if (key == "trim")
        chip = new TRigidChipTrim;
      else if (key == "trimf")
        chip = new TRigidChipTrimF;
      else if (key == "arm")
        chip = new TRigidChipArm;
      else if (key == "cowl")
        chip = new TRigidChipCowl;
      else
      {
        ErrorMessage = "Unknown chip type (" + key + ")";
        return NULL;
      }
      chip->SetOptions(readto(stream, ')'));
      chip->Direction = direct;

      if (parent == addto)
        adds->Add((TObject*)chip);
      parent->AddSubChip(chip);
      stack->Push(parent);
      parent = chip;

      while (skipsp(stream), (cmt = readcomment(stream)) != "")
        memo += cmt;

      chip->MemoChip = memo;
      memo = "";

      stream->Read(&c, 1);
      if (c != '{')
      {
        ErrorMessage = "open bracket must be after chip definition";
        return NULL;
      }
      while (skipsp(stream), (cmt = readcomment(stream)) != "")
        memo += cmt;
    }
  }
  __finally
  {
    delete stack;

    if (ErrorMessage != "")
    {
      for (int i = adds->Count-1; i >= 0; i --)
        delete (TRigidChip*)adds->Items[i];
      adds->Clear();
    }
    if (adds->Count > 0)
      add = (TRigidChip*)adds->Items[0];
    delete adds;
  }

  return add;
}
//---------------------------------------------------------------------------
TRigidChipCore* TRCDLoader::load(TStream *stream)
{
  TRigidChipCore *core = new TRigidChipCore;

  AnsiString cmt;
  core->MemoModel = "";
  while (skipsp(stream), (cmt = readcomment(stream)) != "")
    core->MemoModel += cmt;
  if (core->MemoModel.SubString(1, 6) == "[RCD] ")
    core->MemoModel = core->MemoModel.SubString(7, core->MemoModel.Length());
  else if (core->MemoModel.SubString(1, 5) == "[RCD]")
    core->MemoModel = core->MemoModel.SubString(6, core->MemoModel.Length());
  else
    core->FlagRCD = false;

  AnsiString key;
  while ((key = readkey(stream).LowerCase()) != "")
  {
    if (key == "val")
    {
      if (procval(stream, core) == NULL)
      {
        delete core;
        return NULL;
      }
    }
    else if (key == "key")
    {
      if (prockey(stream, core) == NULL)
      {
        delete core;
        return NULL;
      }
    }
    else if (key == "body")
    {
      if (procbody(stream, core) == NULL)
      {
        delete core;
        return NULL;
      }
    }
    else if (key == "script")
    {
      char c;
      if (stream->Read(&c, 1) != 1 || c != '{')
      {
        ErrorMessage = "open bracket must be after script";
        delete core;
        return NULL;
      }

      AnsiString s;
      int bracket = 0;
      while (stream->Read(&c, 1) == 1)
      {
        if (c == '"')
        {
          s += AnsiString(c);
          while (stream->Read(&c, 1) == 1)
          {
            s += AnsiString(c);
            if (c == '\n')
              break;
            if (c == '\\')
            {
              if (stream->Read(&c, 1) == 1)
                s += AnsiString(c);
              continue;
            }
            if (c == '"')
              break;
          }
        }
        else if (c == '/')
        {
          s += AnsiString(c);
          if (stream->Read(&c, 1) == 1)
          {
            s += AnsiString(c);
            if (c == '/')
            {
              while (stream->Read(&c, 1) == 1)
              {
                s += AnsiString(c);
                if (c == '\n')
                  break;
              }
            }
          }
        }
        else if (c == '{')
        {
          bracket ++;
          s += AnsiString(c);
        }
        else if (c == '}')
        {
          if (bracket == 0)
            break;
          bracket --;
          s += AnsiString(c);
        }
        else
          s += AnsiString(c);
      }

      s = StringReplace(s, "\r", "", TReplaceFlags() << rfReplaceAll);
      s = StringReplace(s, "\n", "\r\n", TReplaceFlags() << rfReplaceAll);
      core->Script->ScriptText = s;
    }
    else if (key == "lua")
    {
      char c;
      if (stream->Read(&c, 1) != 1 || c != '{')
      {
        ErrorMessage = "open bracket must be after lua";
        delete core;
        return NULL;
      }

      while (stream->Read(&c, 1) == 1)
        core->Lua += AnsiString(c);

      core->Lua = core->Lua.TrimRight();
      int p = core->Lua.LastDelimiter("}");
      if (p != 0)
      {
        core->Lua = core->Lua.SubString(1, p-1);
        core->Lua = StringReplace(core->Lua, "\r", "", TReplaceFlags() << rfReplaceAll);
        core->Lua = StringReplace(core->Lua, "\n", "\r\n", TReplaceFlags() << rfReplaceAll);
      }
      else
      {
        ErrorMessage = "close bracket of lua was not found";
        delete core;
        return NULL;
      }
    }
    else
    {
      //readto(stream, '}');
      ErrorMessage = "Unknown block type (" + key + ")";
      delete core;
      return NULL;
    }
  }

  return core;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::procval(TStream *stream, TRigidChipCore* core)
{
  char c;
  if (stream->Read(&c, 1) != 1 || c != '{')
  {
    ErrorMessage = "open bracket must be after val";
    return NULL;
  }
  while (skipsp(stream), readcomment(stream) != "");

  AnsiString key;
  while ((key = readkey(stream)) != "")
  {
    stream->Read(&c, 1);
    if (c != '(')
    {
      ErrorMessage = "open square bracket must be after variable name";
      return NULL;
    }
    while (skipsp(stream), readcomment(stream) != "");

    TRigidChipsVariable *var = new TRigidChipsVariable(core, readto(stream, ')'));
    core->Variables[key.UpperCase()] = var;
  }

  stream->Read(&c, 1);
  if (c != '}')
  {
    ErrorMessage = "close bracket of val was not found";
    return NULL;
  }
  while (skipsp(stream), readcomment(stream) != "");

  return core;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::prockey(TStream *stream, TRigidChipCore* core)
{
  char c;
  if (stream->Read(&c, 1) != 1 || c != '{')
  {
    ErrorMessage = "open bracket must be after key";
    return NULL;
  }
  while (skipsp(stream), readcomment(stream) != "");

  AnsiString keyn, var;
  while ((keyn = readkey(stream)) != "")
  {
    if (stream->Read(&c, 1) != 1 || c != ':')
    {
      ErrorMessage = ": must be after key number";
      return NULL;
    }

    while ((var = readkey(stream)) != "")
    {
      if (stream->Read(&c, 1) != 1 || c != '(')
      {
        ErrorMessage = "open square bracket must be after variable name";
        return NULL;
      }
      TRigidChipsKey *key = new TRigidChipsKey(core, var, readto(stream, ')'));
      core->AddKey(keyn, key);

      while (skipsp(stream), readcomment(stream) != "");
      if (stream->Read(&c, 1) == 1 && c != ',')
      {
        stream->Seek(-1, soFromCurrent);
        break;
      }
    }
  }

  if (stream->Read(&c, 1) != 1 || c != '}')
  {
    ErrorMessage = "close bracket of val was not found";
    return NULL;
  }
  while (skipsp(stream), readcomment(stream) != "");

  return core;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::procbody(TStream *stream, TRigidChip* core)
{
  char c;
  if (stream->Read(&c, 1) != 1 || c != '{')
  {
    ErrorMessage = "open bracket must be after body";
    return NULL;
  }

  AnsiString memo, cmt;
  while (skipsp(stream), (cmt = readcomment(stream)) != "")
    memo += cmt;

  AnsiString key = readkey(stream).LowerCase();
  if (key == "n" || key == "e" || key == "w" || key == "s")
  {
    if (stream->Read(&c, 1) != 1 || c != ':')
    {
      ErrorMessage = ": must be before core";
      return NULL;
    }
    key = readkey(stream).LowerCase();
  }
  if (key != "core")
  {
    ErrorMessage = "first element of body is not core (" + key + ")";
    return NULL;
  }

  if (stream->Read(&c, 1) != 1 || c != '(')
  {
    ErrorMessage = "open square bracket must be after core";
    return NULL;
  }

  core->SetOptions(readto(stream, ')'));
  while (skipsp(stream), (cmt = readcomment(stream)) != "")
    memo += cmt;

  core->MemoChip = memo;
  memo = "";

  if (stream->Read(&c, 1) != 1 || c != '{')
  {
    ErrorMessage = "open bracket must be after core()";
    return NULL;
  }

  readchips(stream, core);
  if (ErrorMessage != "")
    return NULL;

  if (stream->Read(&c, 1) != 1 || c != '}')
  {
    ErrorMessage = "close bracket of body was not found";
    return NULL;
  }
  while (skipsp(stream), readcomment(stream) != "");

  return core;
}
//---------------------------------------------------------------------------

