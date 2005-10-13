//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RCDSaver.h"
#include "RigidChip.h"
#include "RCScript.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TRCDSaver::TRCDSaver()
{
  optSpaceAfterBlockType = false;
  optNewLineAfterBlockType = true;
  optSpaceAfterOptions = true;
  optNewLineAfterOptions = false;
  optSpaceAfterComma = true;
  optNoSubNoNewLine = true;
  optTabSpaces = 0;
}
//---------------------------------------------------------------------------
AnsiString TRCDSaver::ChipToString(AnsiString tabs, TRigidChip *chip)
{
  // settings begin
  AnsiString afteropt;
  if (optNewLineAfterOptions)
    afteropt = "\r\n";
  else if (optSpaceAfterOptions)
    afteropt = " ";

  AnsiString aftercomma;
  if (optSpaceAfterComma)
    aftercomma = " ";

  AnsiString tab;
  if (optTabSpaces > 0)
  {
    for (int i = 0; i < optTabSpaces; i ++)
      tab += " ";
  }
  else
    tab = "\t";

  AnsiString sp = " ";
  AnsiString br = "\r\n";

  AnsiString dir[] = {"", "N:", "E:", "S:", "W:"};
  // settings end

  tabs += tab;
  AnsiString str;

  TStringList *lines = new TStringList;
  try {
    lines->Text = chip->MemoChip;
    for (int i = 0; i < lines->Count; i ++)
      if (lines->Strings[i] != "")
        lines->Strings[i] = tabs + "// " + lines->Strings[i];
      else
        lines->Strings[i] = tabs + "//";
    str += lines->Text;
  } __finally {
    delete lines;
  }

  str += tabs + dir[chip->Direction] + chip->GetTypeString();

  AnsiString opt;
  for (int i = 0; i < chip->Options->Count; i ++)
  {
    if (chip->Options->Names[i] != "" && chip->Options->Values[chip->Options->Names[i]] != "")
    {
      if (opt != "") opt += "," + aftercomma;
      opt += chip->Options->Strings[i];
    }
  }
  str += "(" + opt + ")";

  if (chip->SubChipsCount != 0 || !optNoSubNoNewLine)
  {
    str += afteropt;
    if (optNewLineAfterOptions) str += tabs;
    str += "{" + br;
    for (int i = 0; i < chip->SubChipsCount; i ++)
      str += ChipToString(tabs, chip->SubChips[i]);
    str += tabs + "}" + br;
  }
  else if (optSpaceAfterOptions)
    str += sp + "{" + sp + "}" + br;
  else
    str += "{}" + br;

  return str;
}
//---------------------------------------------------------------------------
AnsiString TRCDSaver::Save(AnsiString filename, TRigidChipCore *core)
{
  TStringList *lines = new TStringList;
  try
  {
    AnsiString aftertype;
    if (optNewLineAfterBlockType)
      aftertype = "\r\n";
    else if (optSpaceAfterBlockType)
      aftertype = " ";

    AnsiString afteropt;
    if (optNewLineAfterOptions)
      afteropt = "\r\n";
    else if (optSpaceAfterOptions)
      afteropt = " ";

    AnsiString aftercomma;
    if (optSpaceAfterComma)
      aftercomma = " ";

    AnsiString tab;
    if (optTabSpaces > 0)
    {
      for (int i = 0; i < optTabSpaces; i ++)
        tab += " ";
    }
    else
      tab = "\t";

    // ModelMemo
    core->FlagRCD = true;
    lines->Text = "[RCD] " + core->MemoModel;
    for (int i = 0; i < lines->Count; i ++)
      if (lines->Strings[i] != "")
        lines->Strings[i] = "// " + lines->Strings[i];
      else
        lines->Strings[i] = "//";

    // Val
    lines->Add("Val" + aftertype + "{");
    for (TRigidChipsVariableMap::iterator i = core->Variables.begin(); i != core->Variables.end(); i ++)
    {
      AnsiString varname = i->first;
      TRigidChipsVariable *var = i->second;
      AnsiString opts;
      if (var->FlagColor)
        opts += "default=#" + IntToHex((int)var->Default, 6);
      else
        opts += "default=" + FloatToStr(var->Default);
      if (var->Min != 0)
      {
        opts += "," + aftercomma;
        opts += "min=" + FloatToStr(var->Min);
      }
      if (var->FlagMax)
      {
        opts += "," + aftercomma;
        opts += "max=" + FloatToStr(var->Max);
      }
      if (var->FlagStep && var->Step != 0)
      {
        opts += "," + aftercomma;
        opts += "step=" + FloatToStr(var->Step);
      }
      if (!var->Disp)
      {
        opts += "," + aftercomma;
        opts += "disp=0";
      }
      lines->Add(tab + varname + "(" + opts + ")");
    }
    lines->Add("}");

    // Key
    int keymin = -1, keymax = -1;
    for (int i = 0; i < core->KeyList->Count; i ++)
    {
      int k = core->KeyList->Strings[i].ToIntDef(0);
      if (keymin == -1 || k < keymin)
        keymin = k;
      if (keymax == -1 || k > keymax)
        keymax = k;
    }
    lines->Add("Key" + aftertype + "{");
    for (int k = keymin; k <= keymax; k ++)
    {
      AnsiString keyn = IntToStr(k);
      AnsiString line;
      for (int i = 0; i < core->KeyList->Count; i ++)
      {
        if (core->KeyList->Strings[i] == keyn)
        {
          if (line != "") line += "," + aftercomma;
          line += core->Keys[i]->Variable + "(step=" + FloatToStr(core->Keys[i]->Step) + ")";
        }
      }
      if (line != "")
        lines->Add(tab + keyn + ":" + line);
    }
    lines->Add("}");

    // Body
    lines->Add("Body" + aftertype + "{");
    lines->Add(ChipToString("", core) + "}");

    // Script or Lua
    if (core->Script->ScriptText != "")
    {
      lines->Add("Script" + aftertype + "{" + core->Script->ScriptText + "}");
    }
    if (core->Lua != "")
    {
      lines->Add("Lua" + aftertype + "{" + core->Lua + "}");
    }

    if (filename == "")
      return lines->Text;
    lines->SaveToFile(filename);
  }
  __finally
  {
    delete lines;
  }
  return "";
}
//---------------------------------------------------------------------------

