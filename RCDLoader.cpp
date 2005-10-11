//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RCDLoader.h"
#include "RigidChip.h"
#include "RCScript.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void TRCDLoader::skipsp(int fp)
{ // スペース等を飛ばす
  unsigned char c;
  while (FileRead(fp, &c, 1) == 1)
  {
    if (c > ' ')
    {
      FileSeek(fp, -1, 1);
      break;
    }
  }
}
AnsiString TRCDLoader::readto(int fp, char to)
{ // 指定文字まで読む
  AnsiString s;
  char c;
  while (FileRead(fp, &c, 1) == 1 && c != to)
    s += AnsiString(c);
  return s;
}
AnsiString TRCDLoader::readcomment(int fp)
{ // コメントだったら改行まで読み込む
  char c;
  if (FileRead(fp, &c, 1) == 1)
  {
    if (c != '/')
    {
      FileSeek(fp, -1, 1);
      return "";
    }
  }
  else
    return "";

  if (FileRead(fp, &c, 1) == 1)
  {
    if (c != '/')
    {
      FileSeek(fp, -2, 1);
      return "";
    }
  }
  else
    return "";

  if (FileRead(fp, &c, 1) == 1 && c != ' ')
    FileSeek(fp, -1, 1);

  AnsiString s;
  while (FileRead(fp, &c, 1) == 1)
  {
    s += AnsiString(c);
    if (c == '\n')
      break;
  }
  return s;
}
AnsiString TRCDLoader::readkey(int fp)
{ // 英数字を読み込む
  while (skipsp(fp), readcomment(fp) != "");

  AnsiString key;
  char c;
  while (FileRead(fp, &c, 1) == 1)
  {
    if ('A' <= c && c <= 'Z' || 'a' <= c && c <= 'z' || '0' <= c && c <= '9' || c == '_' || c == '-')
      key += AnsiString(c);
    else
    {
      FileSeek(fp, -1, 1);
      break;
    }
  }

  while (skipsp(fp), readcomment(fp) != "");

  return key;
}
//---------------------------------------------------------------------------
TRigidChipCore* TRCDLoader::Load(AnsiString filename)
{
  int fp = FileOpen(filename, fmOpenRead);
  if (fp < 0)
  {
    ErrorMessage = "Can't open the file '" + filename + "'";
    return NULL;
  }

  TRigidChipCore *core = new TRigidChipCore;

  AnsiString cmt;
  core->Comment = "";
  while (skipsp(fp), (cmt = readcomment(fp)) != "")
    core->Comment += cmt;

  AnsiString key;
  while ((key = readkey(fp).LowerCase()) != "")
  {
    if (key == "val")
    {
      if (procval(fp, core) == NULL)
      {
        delete core;
        FileClose(fp);
        return NULL;
      }
    }
    else if (key == "key")
    {
      if (prockey(fp, core) == NULL)
      {
        delete core;
        FileClose(fp);
        return NULL;
      }
    }
    else if (key == "body")
    {
      if (procbody(fp, core) == NULL)
      {
        delete core;
        FileClose(fp);
        return NULL;
      }
    }
    else if (key == "script")
    {
      char c;
      FileRead(fp, &c, 1);
      if (c != '{')
      {
        ErrorMessage = "open bracket must be after script";
        delete core;
        FileClose(fp);
        return NULL;
      }

      AnsiString s;
      int bracket = 0;
      while (FileRead(fp, &c, 1) == 1)
      {
        if (c == '"')
        {
          s += AnsiString(c);
          while (FileRead(fp, &c, 1) == 1)
          {
            s += AnsiString(c);
            if (c == '\n')
              break;
            if (c == '\\')
            {
              if (FileRead(fp, &c, 1) == 1)
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
          if (FileRead(fp, &c, 1) == 1)
          {
            s += AnsiString(c);
            if (c == '/')
            {
              while (FileRead(fp, &c, 1) == 1)
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
      FileRead(fp, &c, 1);
      if (c != '{')
      {
        ErrorMessage = "open bracket must be after lua";
        delete core;
        FileClose(fp);
        return NULL;
      }

      while (FileRead(fp, &c, 1) == 1)
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
        FileClose(fp);
        return NULL;
      }
    }
    else
    {
      //readto(fp, '}');
      ErrorMessage = "Unknown block type (" + key + ")";
      delete core;
      FileClose(fp);
      return NULL;
    }
  }

  FileClose(fp);
  return core;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::procval(int fp, TRigidChipCore* core)
{
  char c;
  FileRead(fp, &c, 1);
  if (c != '{')
  {
    ErrorMessage = "open bracket must be after val";
    return NULL;
  }
  while (skipsp(fp), readcomment(fp) != "");

  AnsiString key;
  while ((key = readkey(fp)) != "")
  {
    FileRead(fp, &c, 1);
    if (c != '(')
    {
      ErrorMessage = "open square bracket must be after variable name";
      return NULL;
    }
    while (skipsp(fp), readcomment(fp) != "");

    TRigidChipsVariable *var = new TRigidChipsVariable(core, readto(fp, ')'));
    core->Variables[key.UpperCase()] = var;
  }

  FileRead(fp, &c, 1);
  if (c != '}')
  {
    ErrorMessage = "close bracket of val was not found";
    return NULL;
  }
  while (skipsp(fp), readcomment(fp) != "");

  return core;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::prockey(int fp, TRigidChipCore* core)
{
  char c;
  FileRead(fp, &c, 1);
  if (c != '{')
  {
    ErrorMessage = "open bracket must be after key";
    return NULL;
  }
  while (skipsp(fp), readcomment(fp) != "");

  AnsiString keyn, var;
  while ((keyn = readkey(fp)) != "")
  {
    FileRead(fp, &c, 1);
    if (c != ':')
    {
      ErrorMessage = ": must be after key number";
      return NULL;
    }

    while ((var = readkey(fp)) != "")
    {
      FileRead(fp, &c, 1);
      if (c != '(')
      {
        ErrorMessage = "open square bracket must be after variable name";
        return NULL;
      }
      TRigidChipsKey *key = new TRigidChipsKey(core, var, readto(fp, ')'));
      core->AddKey(keyn, key);

      while (skipsp(fp), readcomment(fp) != "");
      FileRead(fp, &c, 1);
      if (c != ',')
      {
        FileSeek(fp, -1, 1);
        break;
      }
    }
  }

  FileRead(fp, &c, 1);
  if (c != '}')
  {
    ErrorMessage = "close bracket of val was not found";
    return NULL;
  }
  while (skipsp(fp), readcomment(fp) != "");

  return core;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::procbody(int fp, TRigidChip* core)
{
  char c;
  FileRead(fp, &c, 1);
  if (c != '{')
  {
    ErrorMessage = "open bracket must be after body";
    return NULL;
  }
  while (skipsp(fp), readcomment(fp) != "");

  AnsiString key = readkey(fp).LowerCase();
  if (key != "core")
  {
    ErrorMessage = "first element of body is not core (" + key + ")";
    return NULL;
  }

  FileRead(fp, &c, 1);
  if (c != '(')
  {
    ErrorMessage = "open square bracket must be after core";
    return NULL;
  }

  core->SetOptions(readto(fp, ')'));
  while (skipsp(fp), readcomment(fp) != "");

  FileRead(fp, &c, 1);
  if (c != '{')
  {
    ErrorMessage = "open bracket must be after core()";
    return NULL;
  }
  while (skipsp(fp), readcomment(fp) != "");

  TStack *stack = new TStack;
  TRigidChip *parent = core;
  try
  {
    while (FileRead(fp, &c, 1) == 1)
    {
      if (c == '}')
      {
        while (skipsp(fp), readcomment(fp) != "");
        if (stack->Count() == 0)
          break;
        parent = (TRigidChip*)stack->Pop();
        continue;
      }
      FileSeek(fp, -1, 1);

      key = readkey(fp).LowerCase();
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
        if (FileRead(fp, &c, 1) != 1 || c != ':')
        {
          ErrorMessage = ": must be after direction";
          return NULL;
        }

        key = readkey(fp).LowerCase();
        if (key == "")
        {
          ErrorMessage = "chip type must be after direction:";
          return NULL;
        }
      }
      else
        direct = rdNorth;

      if (FileRead(fp, &c, 1) != 1 || c != '(')
      {
        ErrorMessage = "open square bracket must be after chip type";
        return NULL;
      }

      TRigidChip *chip;
      if (parent->GetType() == ctCowl && key != "cowl")
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
      chip->SetOptions(readto(fp, ')'));
      chip->Direction = direct;

      parent->AddSubChip(chip);
      stack->Push(parent);
      parent = chip;

      while (skipsp(fp), readcomment(fp) != "");

      FileRead(fp, &c, 1);
      if (c != '{')
      {
        ErrorMessage = "open bracket must be after chip definition";
        return NULL;
      }
      while (skipsp(fp), readcomment(fp) != "");
    }
  }
  __finally
  {
    delete stack;
  }

  FileRead(fp, &c, 1);
  if (c != '}')
  {
    ErrorMessage = "close bracket of body was not found";
    return NULL;
  }
  while (skipsp(fp), readcomment(fp) != "");

  return core;
}
//---------------------------------------------------------------------------
void TRCDLoader::skipsp(TStringStream *stream)
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
AnsiString TRCDLoader::readto(TStringStream *stream, char to)
{ // 指定文字まで読む
  AnsiString s;
  char c;
  while (stream->Read(&c, 1) == 1 && c != to)
    s += AnsiString(c);
  return s;
}
bool TRCDLoader::skipcomment(TStringStream *stream)
{ // コメントだったら改行まで飛ばす
  char c;
  if (stream->Read(&c, 1) == 1)
  {
    if (c != '/')
    {
      stream->Seek(-1, soFromCurrent);
      return false;
    }
  }
  else
    return false;

  if (stream->Read(&c, 1) == 1)
  {
    if (c != '/')
    {
      stream->Seek(-2, soFromCurrent);
      return false;
    }
  }
  else
    return false;

  readto(stream, '\n');
  return true;
}
AnsiString TRCDLoader::readkey(TStringStream *stream)
{ // 英数字を読み込む
  while (skipsp(stream), skipcomment(stream));

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

  while (skipsp(stream), skipcomment(stream));

  return key;
}
//---------------------------------------------------------------------------
TRigidChip* TRCDLoader::StringToChip(AnsiString str, TRigidChip *addto)
{
  ErrorMessage = "";
  TRigidChip *parent = addto;

  TRigidChip *add = NULL;
  TList *adds = new TList;
  TStringStream *stream = new TStringStream(str);
  TStack *stack = new TStack;
  try
  {
    while (skipsp(stream), skipcomment(stream));

    char c;
    while (stream->Read(&c, 1) == 1)
    {
      if (c == '}')
      {
        while (skipsp(stream), skipcomment(stream));
        if (stack->Count() == 0)
        {
          if (parent == NULL)
          {
            ErrorMessage = "too many close bracket\n";
            return NULL;
          }
          parent = NULL;
          continue;
        }
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

      if (parent == addto && stack->Count() == 0)
        adds->Add((TObject*)chip);
      if (parent)
      {
        parent->AddSubChip(chip);
        stack->Push(parent);
      }
      parent = chip;

      while (skipsp(stream), skipcomment(stream));

      stream->Read(&c, 1);
      if (c != '{')
      {
        ErrorMessage = "open bracket must be after chip definition";
        return NULL;
      }
      while (skipsp(stream), skipcomment(stream));
    }
  }
  __finally
  {
    if (ErrorMessage != "")
    {
      for (int i = adds->Count-1; i >= 0; i --)
        delete (TRigidChip*)adds->Items[i];
      adds->Clear();
    }
    delete stream;
    delete stack;
    if (adds->Count > 0)
    {
      add = (TRigidChip*)adds->Items[0];
      if (addto == NULL)
      {
        for (int i = adds->Count-1; i >= 1; i --)
          delete (TRigidChip*)adds->Items[i];
      }
    }
    delete adds;
  }

  return add;
}
//---------------------------------------------------------------------------

