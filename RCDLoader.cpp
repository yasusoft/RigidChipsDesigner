//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RCDLoader.h"
#include "RigidChip.h"
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
bool TRCDLoader::skipcomment(int fp)
{ // コメントだったら改行まで飛ばす
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
{ // 英数字を読み込む
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
TRigidChip* TRCDLoader::Load(AnsiString filename)
{
  int fp = FileOpen(filename, fmOpenRead);
  if (fp < 0)
  {
    FileClose(fp);
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
        FileClose(fp);
        core->Delete();
        return NULL;
      }
    }
    else if (key == "key")
    {
      if (prockey(fp, core) == NULL)
      {
        FileClose(fp);
        core->Delete();
        return NULL;
      }
    }
    else if (key == "body")
    {
      if (procbody(fp, core) == NULL)
      {
        FileClose(fp);
        core->Delete();
        return NULL;
      }
      break;
    }
    else
      readto(fp, '}');
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
    ErrorMessage = "open bracket must be after body";
    return NULL;
  }
  while (skipsp(fp), skipcomment(fp));

  AnsiString key;
  while ((key = readkey(fp).LowerCase()) != "")
  {
    FileRead(fp, &c, 1);
    if (c != '(')
    {
      ErrorMessage = "open square bracket must be after variable name";
      return NULL;
    }
    while (skipsp(fp), skipcomment(fp));

    TRigidChipsVariable *var = new TRigidChipsVariable(core, readto(fp, ')'));
    core->AddVariable(key, var);
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
TRigidChip* TRCDLoader::prockey(int fp, TRigidChip* core)
{
  readto(fp, '}');
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
        if (stack->Count() == 0)
          break;
        parent = (TRigidChip*)stack->Pop();
        while (skipsp(fp), skipcomment(fp));
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
      if (key == "chip")
        chip = new TRigidChipChip;
      else if (key == "frame")
        chip = new TRigidChipFrame;
      else if (key == "wheel" || key == "rlw")
        chip = new TRigidChipWheel;
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
      else if (key == "arm" || key == "weight" || key == "cowl")
        chip = new TRigidChipChip;
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
  
  return core;
}
//---------------------------------------------------------------------------

