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
{ // �X�y�[�X�����΂�
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
{ // �w�蕶���܂œǂ�
  AnsiString s;
  char c;
  while (FileRead(fp, &c, 1) == 1 && c != to)
    s += AnsiString(c);
  return s;
}
bool TRCDLoader::skipcomment(int fp)
{ // �R�����g����������s�܂Ŕ�΂�
  char c;
  if (FileRead(fp, &c, 1) == 1)
  {
    if (c != '/')
    {
      FileSeek(fp, -1, 1);
      return false;
    }
  }
  else
    return false;

  if (FileRead(fp, &c, 1) == 1)
  {
    if (c != '/')
    {
      FileSeek(fp, -2, 1);
      return false;
    }
  }
  else
    return false;

  readto(fp, '\n');
  return true;
}
AnsiString TRCDLoader::readkey(int fp)
{ // �p������ǂݍ���
  while (skipsp(fp), skipcomment(fp));

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

  while (skipsp(fp), skipcomment(fp));

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
        core->Lua = core->Lua.SubString(1, p-1);
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
  while (skipsp(fp), skipcomment(fp));

  AnsiString key;
  while ((key = readkey(fp)) != "")
  {
    FileRead(fp, &c, 1);
    if (c != '(')
    {
      ErrorMessage = "open square bracket must be after variable name";
      return NULL;
    }
    while (skipsp(fp), skipcomment(fp));

    TRigidChipsVariable *var = new TRigidChipsVariable(core, readto(fp, ')'));
    core->Variables[key.UpperCase()] = var;
  }

  FileRead(fp, &c, 1);
  if (c != '}')
  {
    ErrorMessage = "close bracket of val was not found";
    return NULL;
  }
  while (skipsp(fp), skipcomment(fp));

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
  while (skipsp(fp), skipcomment(fp));

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

      while (skipsp(fp), skipcomment(fp));
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
  while (skipsp(fp), skipcomment(fp));

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
  while (skipsp(fp), skipcomment(fp));

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
  while (skipsp(fp), skipcomment(fp));

  FileRead(fp, &c, 1);
  if (c != '{')
  {
    ErrorMessage = "open bracket must be after core()";
    return NULL;
  }
  while (skipsp(fp), skipcomment(fp));

  TStack *stack = new TStack;
  TRigidChip *parent = core;
  try
  {
    while (FileRead(fp, &c, 1) == 1)
    {
      if (c == '}')
      {
        while (skipsp(fp), skipcomment(fp));
        if (stack->Count() == 0)
          break;
        parent = (TRigidChip*)stack->Pop();
        continue;
      }

      TRigidChipsDirection direct;
      if ('a' <= c && c <= 'z') c += 'A' - 'a';
      if (c == 'N') direct = rdNorth;
      else if (c == 'E') direct = rdEast;
      else if (c == 'W') direct = rdWest;
      else if (c == 'S') direct = rdSouth;
      else
      {
        ErrorMessage = "Unknown direction (" + AnsiString(c) + ")";
        return NULL;
      }

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

      FileRead(fp, &c, 1);
      if (c != '(')
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

      while (skipsp(fp), skipcomment(fp));

      FileRead(fp, &c, 1);
      if (c != '{')
      {
        ErrorMessage = "open bracket must be after chip definition";
        return NULL;
      }
      while (skipsp(fp), skipcomment(fp));
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
  while (skipsp(fp), skipcomment(fp));
  
  return core;
}
//---------------------------------------------------------------------------
void TRCDLoader::skipsp(TStringStream *stream)
{ // �X�y�[�X�����΂�
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
{ // �w�蕶���܂œǂ�
  AnsiString s;
  char c;
  while (stream->Read(&c, 1) == 1 && c != to)
    s += AnsiString(c);
  return s;
}
bool TRCDLoader::skipcomment(TStringStream *stream)
{ // �R�����g����������s�܂Ŕ�΂�
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
{ // �p������ǂݍ���
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
TRigidChip* TRCDLoader::StringToChip(AnsiString str)
{
  ErrorMessage = "";
  TRigidChip *parent = NULL;
  
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
          break;
        }
        parent = (TRigidChip*)stack->Pop();
        continue;
      }

      TRigidChipsDirection direct;
      if ('a' <= c && c <= 'z') c += 'A' - 'a';
      if (c == 'N') direct = rdNorth;
      else if (c == 'E') direct = rdEast;
      else if (c == 'W') direct = rdWest;
      else if (c == 'S') direct = rdSouth;
      else
      {
        ErrorMessage = "Unknown direction (" + AnsiString(c) + ")";
        return NULL;
      }

      if (stream->Read(&c, 1) != 1 || c != ':')
      {
        ErrorMessage = ": must be after direction";
        return NULL;
      }

      AnsiString key = readkey(stream).LowerCase();
      if (key == "")
      {
        ErrorMessage = "chip type must be after direction:";
        return NULL;
      }

      stream->Read(&c, 1);
      if (c != '(')
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
    while (stack->Count() > 0)
      parent = (TRigidChip*)stack->Pop();
    if (ErrorMessage != "")
      delete parent;
    delete stream;
    delete stack;
  }

  return parent;
}
//---------------------------------------------------------------------------

