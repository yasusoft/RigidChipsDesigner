//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RigidChip.h"
#include <math.h>
#include "RCScript.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void glMaterialColor3f(float r, float g, float b)
{
  //glMaterialColor4f(r, g, b, 1)
  glColor3f(r, g, b);
  const float diffuse[4] = {0.8*r, 0.8*g, 0.8*b, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  const float ambient[4] = {0.1*r+0.1, 0.1*g+0.1, 0.1*b+0.1, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
void glMaterialColor4f(float r, float g, float b, float a)
{
  glColor3f(r, g, b);
  const float diffuse[4] = {0.8*r, 0.8*g, 0.8*b, a};
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  const float ambient[4] = {0.1*r+0.1, 0.1*g+0.1, 0.1*b+0.1, a};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRigidChipsVariable::TRigidChipsVariable(TRigidChipCore *c, AnsiString opt)
{
  core = c;

  TStringList *opts = new TStringList;
  opt = StringReplace(opt, " ", "", TReplaceFlags() << rfReplaceAll);
  opts->CommaText = opt;
  for (int i = 0; i < opts->Count; i ++)
    opts->Strings[i] = opts->Names[i].LowerCase() + "=" + opts->Values[opts->Names[i]];

  Default = Min = Max = Step = 0;
  FlagColor = opts->Values["default"].SubString(1, 1) == "#";
             core->StrToDouble(opts->Values["default"], &Default);
             core->StrToDouble(opts->Values["min"],     &Min);
  FlagMax  = core->StrToDouble(opts->Values["max"],     &Max);
  FlagStep = core->StrToDouble(opts->Values["step"],    &Step);
  Disp = opts->Values["disp"] != "0";

  delete opts;
  Value = Default;
}
//---------------------------------------------------------------------------
void TRigidChipsVariable::SetValue(double v)
{
  Modify = true;
  if (v < Min)
    FValue = Min;
  else if (FlagMax && v > Max)
    FValue = Max;
  else
    FValue = v;
}
//---------------------------------------------------------------------------
void TRigidChipsVariable::Act()
{
  if (FlagStep)
  {
    if (FValue < Default)
    {
      if (Default - FValue < Step)
        FValue = Default;
      else
        FValue += Step;
    }
    else if (FValue > Default)
    {
      if (FValue - Default < Step)
        FValue = Default;
      else
        FValue -= Step;
    }
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRigidChipsKey::TRigidChipsKey(TRigidChipCore *c, AnsiString var, AnsiString opt)
{
  core = c;
  Variable = var;

  TStringList *opts = new TStringList;
  opt = StringReplace(opt, " ", "", TReplaceFlags() << rfReplaceAll);
  opts->CommaText = opt;
  for (int i = 0; i < opts->Count; i ++)
    opts->Strings[i] = opts->Names[i].LowerCase() + "=" + opts->Values[opts->Names[i]];

  Step = 0;
  core->StrToDouble(opts->Values["step"], &Step);
  delete opts;
}
//---------------------------------------------------------------------------
void TRigidChipsKey::Act()
{
  TRigidChipsVariableMap::iterator i = core->Variables.find(Variable.UpperCase());
  if (i != core->Variables.end())
    i->second->Value = i->second->Value + Step;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRigidChip::TRigidChip()
{
  FCore = NULL;
  Parent = NULL;
  FOptions = new TStringList;
  ChipList = new TList;
}
//---------------------------------------------------------------------------
TRigidChip::~TRigidChip()
{
  if (Core && Core->Deleted && (Core->Deleted == this || Core->Deleted->Parent == this))
    Core->Deleted = NULL;
  if (Parent)
    Parent->DelSubChip(this);
  for (int i = ChipList->Count-1; i >= 0; i --)
    delete (TRigidChip*)ChipList->Items[i];
  delete ChipList;
  delete FOptions;
}
//---------------------------------------------------------------------------
void TRigidChip::SetOptions(AnsiString opt)
{
  opt = StringReplace(opt, " ", "", TReplaceFlags() << rfReplaceAll);
  Options->CommaText = opt;
  for (int i = 0; i < Options->Count; i ++)
    Options->Strings[i] = Options->Names[i].LowerCase() + "=" + Options->Values[Options->Names[i]];
}
//---------------------------------------------------------------------------
void TRigidChip::SetCore(TRigidChipCore *c)
{
  FCore = c;
  for (int i = 0; i < ChipList->Count; i ++)
    ((TRigidChip*)ChipList->Items[i])->Core = Core;
}
//---------------------------------------------------------------------------
TRigidChip* TRigidChip::GetSubChip(int i)
{
  return (TRigidChip*)ChipList->Items[i];
}
//---------------------------------------------------------------------------
int TRigidChip::GetSubChipsCount()
{
  return ChipList->Count;
}
//---------------------------------------------------------------------------
void TRigidChip::AddSubChip(TRigidChip *chip)
{
  ChipList->Add(chip);
  chip->Core = Core;
  chip->Parent = this;
}
//---------------------------------------------------------------------------
void TRigidChip::DelSubChip(int i)
{
  ChipList->Delete(i);
}
void TRigidChip::DelSubChip(TRigidChip *chip)
{
  int i = ChipList->IndexOf(chip);
  if (i != -1)
    ChipList->Delete(i);
}
//---------------------------------------------------------------------------
void TRigidChip::Translate()
{
  switch (Direction)
  {
    case rdNorth: glTranslatef( 0,  1, 0); break;
    case rdSouth: glTranslatef( 0, -1, 0); break;
    case rdEast:  glTranslatef( 1,  0, 0); break;
    case rdWest:  glTranslatef(-1,  0, 0); break;
  }
}
void TRigidChip::Rotate()
{
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
  {
    switch (Direction)
    {
      case rdNorth: glRotatef(-angle, 1, 0, 0); break;
      case rdSouth: glRotatef( angle, 1, 0, 0); break;
      case rdEast: glRotatef( angle, 0, 1, 0); break;
      case rdWest: glRotatef(-angle, 0, 1, 0); break;
    }
  }
}
void TRigidChip::TranslateR()
{
  switch (Direction)
  {
    case rdNorth: glTranslatef( 0, -1, 0); break;
    case rdSouth: glTranslatef( 0,  1, 0); break;
    case rdEast:  glTranslatef(-1,  0, 0); break;
    case rdWest:  glTranslatef( 1,  0, 0); break;
  }
}
void TRigidChip::RotateR()
{
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
  {
    switch (Direction)
    {
      case rdNorth: glRotatef( angle, 1, 0, 0); break;
      case rdSouth: glRotatef(-angle, 1, 0, 0); break;
      case rdEast: glRotatef(-angle, 0, 1, 0); break;
      case rdWest: glRotatef( angle, 0, 1, 0); break;
    }
  }
}
void TRigidChip::DrawMain()
{
  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
}
void TRigidChip::Draw(TRigidChip *caller)
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat mat_backup[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mat_backup);

  if (caller == Parent)
  {
    Translate();
    Rotate();
    Translate();
    Rotate2();
  }

  // 色の設定
  double d;
  if (Core->StrToDouble(Options->Values["color"], &d))
  {
    int c = d;
    float rgb[4];
    rgb[0] = ((c >> 16) & 0xFF) / 255.0;
    rgb[1] = ((c >>  8) & 0xFF) / 255.0;
    rgb[2] = ((c >>  0) & 0xFF) / 255.0;
    rgb[3] = 1;
    glMaterialColor3f(rgb[0], rgb[1], rgb[2]);
  }
  else
    glMaterialColor3f(1, 1, 1);

  glLoadName((GLuint)this);
  glPushMatrix();
  switch (Direction)
  {
    case rdNorth: break;
    case rdEast:  glRotatef(-90, 0, 0, 1); break;
    case rdWest:  glRotatef( 90, 0, 0, 1); break;
    case rdSouth: glRotatef(180, 0, 0, 1); break;
  }

  GLuint tex = GetTexture();
  if (tex)
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
  }

  DrawMain();

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  for (int i = 0; i < ChipList->Count; i ++)
  {
    TRigidChip *c = (TRigidChip*)ChipList->Items[i];
    if (c != caller)
      c->Draw(this);
  }

  if (caller != Parent && Parent != NULL)
  {
    RotateR2();
    TranslateR();
    RotateR();
    TranslateR();
    
    Parent->Draw(this);
  }

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(mat_backup);
  //glPopMatrix();
}
void TRigidChip::DrawTranslucent(TRigidChip *caller)
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat mat_backup[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mat_backup);

  if (caller == Parent)
  {
    Translate();
    Rotate();
    Translate();
    Rotate2();
  }

  glPushMatrix();
  switch (Direction)
  {
    case rdNorth: break;
    case rdEast:  glRotatef(-90, 0, 0, 1); break;
    case rdWest:  glRotatef( 90, 0, 0, 1); break;
    case rdSouth: glRotatef(180, 0, 0, 1); break;
  }

  DrawTranslucentMain();

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  if (Core->Select == this)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    #define DRAW_SELECTION(r, g, b) { \
        glMaterialColor4f(r, g, b, 0.5); \
        glBegin(GL_QUADS); \
        glNormal3f(0, 0, 1); \
        glVertex3f(-1, -1, 0.1); \
        glVertex3f(-1,  1, 0.1); \
        glVertex3f( 1,  1, 0.1); \
        glVertex3f( 1, -1, 0.1); \
        glNormal3f(0, 0, -1); \
        glVertex3f(-1, -1, -0.1); \
        glVertex3f(-1,  1, -0.1); \
        glVertex3f( 1,  1, -0.1); \
        glVertex3f( 1, -1, -0.1); \
        glEnd(); \
    }
    glLoadName(0);
    DRAW_SELECTION(1, 1, 1);
    glLoadName(rdNorth);
    glPushMatrix();
        glTranslatef(0, 2, 0);
        DRAW_SELECTION(1, 0, 0);
    glPopMatrix();
    glLoadName(rdSouth);
    glPushMatrix();
        glTranslatef(0, -2, 0);
        DRAW_SELECTION(0, 0, 1);
    glPopMatrix();
    glLoadName(rdEast);
    glPushMatrix();
        glTranslatef(2, 0, 0);
        DRAW_SELECTION(0, 1, 0);
    glPopMatrix();
    glLoadName(rdWest);
    glPushMatrix();
        glTranslatef(-2, 0, 0);
        DRAW_SELECTION(1, 1, 0);
    glPopMatrix();
    glDisable(GL_BLEND);
    glLoadName(0);
  }

  for (int i = 0; i < ChipList->Count; i ++)
  {
    TRigidChip *c = (TRigidChip*)ChipList->Items[i];
    if (c != caller)
      c->DrawTranslucent(this);
  }

  if (caller != Parent && Parent != NULL)
  {
    RotateR2();
    TranslateR();
    RotateR();
    TranslateR();
    
    Parent->DrawTranslucent(this);
  }

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(mat_backup);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
void TRigidChip::Act()
{
  for (int i = 0; i < ChipList->Count; i ++)
    ((TRigidChip*)ChipList->Items[i])->Act();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipCore::Texture;
//---------------------------------------------------------------------------
TRigidChipCore::TRigidChipCore()
        : TRigidChip()
{
  //FVariableList = new TStringList;
  FKeyList = new TStringList;

  Direction = rdCore;
  Core = this;
  Parent = NULL;
  
  Select = NULL;
  Deleted = NULL;

  Script = new TRCScript(this);
}
//---------------------------------------------------------------------------
TRigidChipCore::~TRigidChipCore()
{
  if (Deleted)
    delete Deleted;

  delete Script;
  
  for (int i = KeyList->Count-1; i >= 0; i --)
    delete (TRigidChipsKey*)KeyList->Objects[i];
  delete KeyList;

  //for (int i = VariableList->Count-1; i >= 0; i --)
  //  delete (TRigidChipsVariable*)VariableList->Objects[i];
  //delete VariableList;
  for (TRigidChipsVariableMap::iterator i = Variables.begin(); i != Variables.end(); i ++)
    delete i->second;
}
//---------------------------------------------------------------------------
/*
TRigidChipsVariable* TRigidChipCore::GetVariable(AnsiString name)
{
  int i = VariableList->IndexOf(name.UpperCase());
  if (i != -1)
    return (TRigidChipsVariable*)VariableList->Objects[i];
  return NULL;
}
//---------------------------------------------------------------------------
void TRigidChipCore::AddVariable(AnsiString name, TRigidChipsVariable *var)
{
  VariableList->AddObject(name.UpperCase(), (TObject*)var);
}
*/
//---------------------------------------------------------------------------
void TRigidChipCore::CheckVariable()
{
  for (TRigidChipsVariableMap::iterator i = Variables.begin(); i != Variables.end(); i ++)
    i->second->Modify = false;
}
void TRigidChipCore::StepVariable()
{
  for (TRigidChipsVariableMap::iterator i = Variables.begin(); i != Variables.end(); i ++)
    if (!i->second->Modify)
      i->second->Act();
}
//---------------------------------------------------------------------------
TRigidChipsKey* TRigidChipCore::GetKey(int i)
{
  return (TRigidChipsKey*)KeyList->Objects[i];
}
//---------------------------------------------------------------------------
void TRigidChipCore::AddKey(AnsiString keyn, TRigidChipsKey *key)
{
  KeyList->AddObject(IntToStr(keyn.ToIntDef(0)), (TObject*)key);
}
//---------------------------------------------------------------------------
void TRigidChipCore::ActKey(AnsiString keyn)
{
  keyn = IntToStr(keyn.ToIntDef(0));
  for (int i = 0; i < KeyList->Count; i ++)
    if (KeyList->Strings[i] == keyn)
      ((TRigidChipsKey*)KeyList->Objects[i])->Act();
}
//---------------------------------------------------------------------------
bool TRigidChipCore::StrToDouble(AnsiString str, double *d)
{
  if (str != "")
  {
    if (str[1] == '#')
    {
      try {
        *d = ("0x"+str.SubString(2, 6)).ToInt();
        return true;
      } catch (...) { }
    }

    if (str[1] == '-')
    {
      TRigidChipsVariableMap::iterator i = Variables.find(str.SubString(2, str.Length()).UpperCase());
      if (i != Variables.end())
      {
        *d = - i->second->Value;
        return true;
      }
    }

    TRigidChipsVariableMap::iterator i = Variables.find(str.UpperCase());
    if (i != Variables.end())
    {
      *d = i->second->Value;
      return true;
    }

    /*
    str = str.LowerCase();
    if (str == "red")   { *d = 0xFF0000; return true; }
    if (str == "green") { *d = 0x00FF00; return true; }
    if (str == "blue")  { *d = 0x0000FF; return true; }
    */

    try
    {
      //*d = str.ToDouble();
      if ((*d = StrToFloatDef(str, -123454321)) == -123454321)
        return false;
      return true;
    }
    catch (...)
    {
    }
  }
  return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipChip::Texture;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TRigidChipFrame::DrawMain()
{
  int opt = Options->Values["option"].ToIntDef(0);
  if (opt == 1 && !Core->ShowGhost)
    return;

  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        const float w = 0.15, w2 = w / 2;
        // bottom
        glTexCoord2f(0, 0 ); glVertex3f(-1, -1  , 0);
        glTexCoord2f(0, w2); glVertex3f(-1, -1+w, 0);
        glTexCoord2f(1, w2); glVertex3f( 1, -1+w, 0);
        glTexCoord2f(1, 0 ); glVertex3f( 1, -1  , 0);
        // top
        glTexCoord2f(0, 1-w2); glVertex3f(-1, 1-w, 0);
        glTexCoord2f(0, 1   ); glVertex3f(-1, 1  , 0);
        glTexCoord2f(1, 1   ); glVertex3f( 1, 1  , 0);
        glTexCoord2f(1, 1-w2); glVertex3f( 1, 1-w, 0);
        // left
        glTexCoord2f(0 ,   w2); glVertex3f(-1  , -1+w, 0);
        glTexCoord2f(0 , 1-w2); glVertex3f(-1  ,  1-w, 0);
        glTexCoord2f(w2, 1-w2); glVertex3f(-1+w,  1-w, 0);
        glTexCoord2f(w2,   w2); glVertex3f(-1+w, -1+w, 0);
        // right
        glTexCoord2f(1-w2,   w2); glVertex3f(1-w, -1+w, 0);
        glTexCoord2f(1-w2, 1-w2); glVertex3f(1-w,  1-w, 0);
        glTexCoord2f(1   , 1-w2); glVertex3f(1  ,  1-w, 0);
        glTexCoord2f(1   ,   w2); glVertex3f(1  , -1+w, 0);
  glEnd();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipWeight::Texture;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipWheel::Texture;
//---------------------------------------------------------------------------
TRigidChipWheel::TRigidChipWheel()
        : TRigidChip()
{
  angle = 0;
}
//---------------------------------------------------------------------------
void TRigidChipWheel::Rotate2()
{
  glRotatef(angle, 0, 0, 1);
}
void TRigidChipWheel::RotateR2()
{
  glRotatef(-angle, 0, 0, 1);
}
//---------------------------------------------------------------------------
void TRigidChipWheel::DrawMain()
{
  glBegin(GL_POLYGON);
  glNormal3f(0, 0, 1);
  for (double a = -M_PI; a < M_PI; a += 0.2)
  {
    glTexCoord2d(cos(a)/2+0.5, sin(a)/2+0.5);
    glVertex3d(cos(a), sin(a), 0);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);

  int opt = Options->Values["option"].ToIntDef(0);
  int effect = Options->Values["effect"].ToIntDef(0);
  if (opt == 1 || opt == 2 || effect > 1)
  { // 周りの輪を描画
    double r = 1;
    switch (opt)
    {
      case 1: r = 1.5; break;
      case 2: r = 2; break;
    }

    double w = 0.2;
    switch (effect)
    {
      case 2: w = 0.3; break;
      case 3: w = 0.4; break;
      case 4: w = 0.6; break;
      case 5: w = 0.8; break;
      case 6: w = 1.0; break;
      case 7: w = 1.2; break;
      case 8: w = 1.4; break;
      case 9: w = 1.5; break;
      case 10: w = 1.7; break;
    }

    glBegin(GL_QUADS);
    glMaterialColor3f(0.25, 0.25, 0.25);
    const double d = 0.2;
    for (double a = -M_PI; a < M_PI; a += d)
    {
      glNormal3f(cos(a), sin(a), 0);
      glVertex3d(r*cos(a  ), r*sin(a  ),  w);
      glVertex3d(r*cos(a  ), r*sin(a  ), -w);
      glNormal3f(cos(a+d), sin(a+d), 0);
      glVertex3d(r*cos(a+d), r*sin(a+d), -w);
      glVertex3d(r*cos(a+d), r*sin(a+d),  w);
    }
    glMaterialColor3f(0.5, 0.5, 0.5);
    glNormal3f(0, 0, 1);
    // 十字棒縦
    glVertex3f(-0.05, -r, 0);
    glVertex3f(-0.05,  r, 0);
    glVertex3f( 0.05,  r, 0);
    glVertex3f( 0.05, -r, 0);
    // 十字棒横
    glVertex3f(-r, -0.05, 0);
    glVertex3f(-r,  0.05, 0);
    glVertex3f( r,  0.05, 0);
    glVertex3f( r, -0.05, 0);
    glEnd();
  }
}
//---------------------------------------------------------------------------
void TRigidChipWheel::Act()
{
  AnsiString sp = Options->Values["power"];
  if (sp != "")
  {
    double power;
    if (Core->StrToDouble(sp, &power))
    {
      if (power >  1000) power =  1000;
      if (power < -1000) power = -1000;
      angle = (int)(angle - power / 50) % 360;
    }
  }
  TRigidChip::Act();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipRLW::Texture;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipJet::Texture;
GLuint TRigidChipJet::TextureBall;
GLUquadricObj* TRigidChipJet::quadric = NULL;
//---------------------------------------------------------------------------
void TRigidChipJet::DrawMain()
{
  int opt = Options->Values["option"].ToIntDef(0);
  if (opt == 1 || opt == 2)
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureBall);
    double power = 0;
    Core->StrToDouble(Options->Values["power"], &power);
    if (power < 0) power *= -1;
    if (!Core->ShowBalloon) power = 0;
    // 0: 半径0.5
    // 150: 半径1
    // 3500: 半径3
    // 15000: 半径5
    // 45000: 半径7
    // 90000: 半径9
    // power = 4 / 3 * pi * r^3 * (30|32) - (15|16)
    // r = ((power+(15|16)) * 3 / ((30|32) * 4 * pi)) ^ (1/3)
    if (quadric == NULL)
      quadric = gluNewQuadric();
    //gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);
    gluSphere(quadric, pow((power+15)*3/(30*4*M_PI), 1 / 3.0), 32, 32);
    glDisable(GL_TEXTURE_2D);
  }
  else
  {
    glBegin(GL_POLYGON);
    glNormal3f(0, 0, 1);
    for (double a = -M_PI; a < M_PI; a += 0.2)
    {
      glTexCoord2d(cos(a)/2+0.5, sin(a)/2+0.5);
      glVertex3d(cos(a), sin(a), 0);
    }
    glEnd();
  }
}
void TRigidChipJet::DrawTranslucentMain()
{
  int opt = Options->Values["option"].ToIntDef(0);
  if (opt == 1 || opt == 2)
  {
  }
  else
  {
    // エフェクト描画
    double power;
    if (Core->StrToDouble(Options->Values["power"], &power) && (power < -1 || 1 < power))
    {
      if (quadric == NULL)
        quadric = gluNewQuadric();
      //gluQuadricNormals(quadratic, GLU_SMOOTH);
      gluQuadricTexture(quadric, GL_FALSE);

      glPushMatrix();
      if (power > 0)
        glRotatef(180, 1, 0, 0);
      else
        power *= -1;
      if (power > 1000)
        power = 1000;
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glMaterialColor4f(0, 1, 1, 0.7); // 小さい炎
      gluCylinder(quadric, 0.25, 0, power / 300 + 0.25*sin(anime * M_PI / 180), 8, 1);
      glMaterialColor4f(0, 0, 1, 0.7); // 大きい炎
      gluCylinder(quadric, 0.5, 0, power / 200 + 0.25*sin(anime * M_PI / 180), 8, 1);
      glDisable(GL_BLEND);
      glPopMatrix();
    }
  }
}
//---------------------------------------------------------------------------
void TRigidChipJet::Act()
{
  anime = (anime + 30) % 360;
  TRigidChip::Act();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipRudder::Texture;
//---------------------------------------------------------------------------
void TRigidChipRudder::Rotate()
{
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
    glRotatef(-angle, 0, 0, 1);
}
void TRigidChipRudder::RotateR()
{
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
    glRotatef(angle, 0, 0, 1);
}
//---------------------------------------------------------------------------
void TRigidChipRudderF::DrawMain()
{
  int opt = Options->Values["option"].ToIntDef(0);
  if (opt == 1 && !Core->ShowGhost)
    return;
  
  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        const float w = 0.15, w2 = w / 2;
        // bottom-left
        glTexCoord2f(0,    0 ); glVertex3f(-1,   -1  , 0);
        glTexCoord2f(0,    w2); glVertex3f(-1,   -1+w, 0);
        glTexCoord2f(0.25, w2); glVertex3f(-0.5, -1+w, 0);
        glTexCoord2f(0.25, 0 ); glVertex3f(-0.5, -1  , 0);
        // bottom-right
        glTexCoord2f(0.75, 0 ); glVertex3f(0.5, -1  , 0);
        glTexCoord2f(0.75, w2); glVertex3f(0.5, -1+w, 0);
        glTexCoord2f(1,    w2); glVertex3f(  1, -1+w, 0);
        glTexCoord2f(1,    0 ); glVertex3f(  1, -1  , 0);
        // top
        glTexCoord2f(0, 1-w2); glVertex3f(-1, 1-w, 0);
        glTexCoord2f(0, 1   ); glVertex3f(-1, 1  , 0);
        glTexCoord2f(1, 1   ); glVertex3f( 1, 1  , 0);
        glTexCoord2f(1, 1-w2); glVertex3f( 1, 1-w, 0);
        // left
        glTexCoord2f(0 ,   w2); glVertex3f(-1  , -1+w, 0);
        glTexCoord2f(0 , 1-w2); glVertex3f(-1  ,  1-w, 0);
        glTexCoord2f(w2, 1-w2); glVertex3f(-1+w,  1-w, 0);
        glTexCoord2f(w2,   w2); glVertex3f(-1+w, -1+w, 0);
        // right
        glTexCoord2f(1-w2,   w2); glVertex3f(1-w, -1+w, 0);
        glTexCoord2f(1-w2, 1-w2); glVertex3f(1-w,  1-w, 0);
        glTexCoord2f(1   , 1-w2); glVertex3f(1  ,  1-w, 0);
        glTexCoord2f(1   ,   w2); glVertex3f(1  , -1+w, 0);
  glEnd();
  glBegin(GL_POLYGON);
        // bottom-center
        const double d = 0.2;
        for (double a = 0; a < M_PI; a += d)
        {
          glTexCoord2f(0.5+0.25*cos(a), 0.25*sin(a));
          glVertex3f(0.5*cos(a), -1+0.5*sin(a), 0);
        }
        glTexCoord2f(0.25, 0);
        glVertex3f(-0.5, -1, 0);
  glEnd();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipTrim::Texture;
//---------------------------------------------------------------------------
void TRigidChipTrim::Rotate()
{
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
  {
    switch (Direction)
    {
      case rdNorth: glRotatef( angle, 0, 1, 0); break;
      case rdSouth: glRotatef(-angle, 0, 1, 0); break;
      case rdEast: glRotatef( angle, 1, 0, 0); break;
      case rdWest: glRotatef(-angle, 1, 0, 0); break;
    }
  }
}
void TRigidChipTrim::RotateR()
{
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
  {
    switch (Direction)
    {
      case rdNorth: glRotatef(-angle, 0, 1, 0); break;
      case rdSouth: glRotatef( angle, 0, 1, 0); break;
      case rdEast: glRotatef(-angle, 1, 0, 0); break;
      case rdWest: glRotatef( angle, 1, 0, 0); break;
    }
  }
}
//---------------------------------------------------------------------------
void TRigidChipTrimF::DrawMain()
{
  int opt = Options->Values["option"].ToIntDef(0);
  if (opt == 1 && !Core->ShowGhost)
    return;

  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        const float w = 0.15, w2 = w / 2;
        // bottom
        glTexCoord2f(0, 0 ); glVertex3f(-1, -1  , 0);
        glTexCoord2f(0, w2); glVertex3f(-1, -1+w, 0);
        glTexCoord2f(1, w2); glVertex3f( 1, -1+w, 0);
        glTexCoord2f(1, 0 ); glVertex3f( 1, -1  , 0);
        // top
        glTexCoord2f(0, 1-w2); glVertex3f(-1, 1-w, 0);
        glTexCoord2f(0, 1   ); glVertex3f(-1, 1  , 0);
        glTexCoord2f(1, 1   ); glVertex3f( 1, 1  , 0);
        glTexCoord2f(1, 1-w2); glVertex3f( 1, 1-w, 0);
        // left
        glTexCoord2f(0 ,   w2); glVertex3f(-1  , -1+w, 0);
        glTexCoord2f(0 , 1-w2); glVertex3f(-1  ,  1-w, 0);
        glTexCoord2f(w2, 1-w2); glVertex3f(-1+w,  1-w, 0);
        glTexCoord2f(w2,   w2); glVertex3f(-1+w, -1+w, 0);
        // right
        glTexCoord2f(1-w2,   w2); glVertex3f(1-w, -1+w, 0);
        glTexCoord2f(1-w2, 1-w2); glVertex3f(1-w,  1-w, 0);
        glTexCoord2f(1   , 1-w2); glVertex3f(1  ,  1-w, 0);
        glTexCoord2f(1   ,   w2); glVertex3f(1  , -1+w, 0);
  glEnd();
  glBegin(GL_TRIANGLES);
        glTexCoord2f(0.4, 0+w2); glVertex3f(-0.3, -1+w, 0);
        glTexCoord2f(0.6, 0+w2); glVertex3f( 0.3, -1+w, 0);
        glTexCoord2f(0.5, 0.35); glVertex3f( 0,   -0.3, 0);
  glEnd();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipArm::Texture;
GLUquadricObj* TRigidChipArm::quadric = NULL;
//---------------------------------------------------------------------------
TRigidChipArm::TRigidChipArm()
        : TRigidChip()
{
  anime = false;
  energy = 0;
}
//---------------------------------------------------------------------------
void TRigidChipArm::DrawMain()
{
  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        // bottom
        glTexCoord2f(0, 0   ); glVertex3f(-1, -1,   0);
        glTexCoord2f(0, 0.45); glVertex3f(-1, -0.1, 0);
        glTexCoord2f(1, 0.45); glVertex3f( 1, -0.1, 0);
        glTexCoord2f(1, 0   ); glVertex3f( 1, -1,   0);
        // left
        glTexCoord2f(0,    0.45); glVertex3f(-1,   -0.1, 0);
        glTexCoord2f(0,    1   ); glVertex3f(-1,    1,   0);
        glTexCoord2f(0.2,  1   ); glVertex3f(-0.6,  1,   0);
        glTexCoord2f(0.35, 0.45); glVertex3f(-0.3, -0.1, 0);
        // right
        glTexCoord2f(0.65, 0.45); glVertex3f(0.3, -0.1, 0);
        glTexCoord2f(0.8,  1   ); glVertex3f(0.6,  1,   0);
        glTexCoord2f(1,    1   ); glVertex3f(1,    1,   0);
        glTexCoord2f(1,    0.45); glVertex3f(1,   -0.1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}
void TRigidChipArm::DrawTranslucentMain()
{
  if (anime)
  {
    if (quadric == NULL)
      quadric = gluNewQuadric();
    //gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_FALSE);

    unsigned int opt = Options->Values["option"].ToIntDef(0);
    if (opt > 50000) opt = 50000;
    glPushMatrix();
    glTranslatef(0, -0.1, 0);
    glRotatef(-90, 1, 0, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMaterialColor4f(1, 1, 0, 0.5);
    gluCylinder(quadric, opt/200000.0+0.1, opt/250000.0, 2.5, 8, 4);
    glDisable(GL_BLEND);
    glPopMatrix();
  }
}
//---------------------------------------------------------------------------
void TRigidChipArm::Act()
{
  bool a = anime;
  if (anime)
    anime = false;

  int opt = Options->Values["option"].ToIntDef(0);
  if (opt < 0) opt *= -1;
  if (opt > 1)
  {
    energy += 5000;
    if (energy > opt)
    {
      energy = opt;

      AnsiString sp = Options->Values["power"];
      if (sp != "")
      {
        double power;
        if (Core->StrToDouble(sp, &power))
        {
          if (power >= opt)
          {
            energy -= opt;
            if (!a) anime = true;
          }
        }
      }
    }
  }
  TRigidChip::Act();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipCowl::Texture0;
GLuint TRigidChipCowl::Texture2;
GLuint TRigidChipCowl::Texture34;
GLuint TRigidChipCowl::Texture5;
//---------------------------------------------------------------------------
void TRigidChipCowl::DrawMain()
{
  if (!Core->ShowCowl)
    return;

  glEnable(GL_TEXTURE_2D);
  glNormal3f(0, 0, 1);
  switch (Options->Values["option"].ToIntDef(0))
  {
    default:
    { // 四角
      glBindTexture(GL_TEXTURE_2D, Texture0);
      glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
      glEnd();
      break;
    }
    case 1:
    { // フレーム
      glBindTexture(GL_TEXTURE_2D, Texture0);
      glBegin(GL_QUADS);
        const float w = 0.15, w2 = w / 2;
        // bottom
        glTexCoord2f(0, 0 ); glVertex3f(-1, -1  , 0);
        glTexCoord2f(0, w2); glVertex3f(-1, -1+w, 0);
        glTexCoord2f(1, w2); glVertex3f( 1, -1+w, 0);
        glTexCoord2f(1, 0 ); glVertex3f( 1, -1  , 0);
        // top
        glTexCoord2f(0, 1-w2); glVertex3f(-1, 1-w, 0);
        glTexCoord2f(0, 1   ); glVertex3f(-1, 1  , 0);
        glTexCoord2f(1, 1   ); glVertex3f( 1, 1  , 0);
        glTexCoord2f(1, 1-w2); glVertex3f( 1, 1-w, 0);
        // left
        glTexCoord2f(0 ,   w2); glVertex3f(-1  , -1+w, 0);
        glTexCoord2f(0 , 1-w2); glVertex3f(-1  ,  1-w, 0);
        glTexCoord2f(w2, 1-w2); glVertex3f(-1+w,  1-w, 0);
        glTexCoord2f(w2,   w2); glVertex3f(-1+w, -1+w, 0);
        // right
        glTexCoord2f(1-w2,   w2); glVertex3f(1-w, -1+w, 0);
        glTexCoord2f(1-w2, 1-w2); glVertex3f(1-w,  1-w, 0);
        glTexCoord2f(1   , 1-w2); glVertex3f(1  ,  1-w, 0);
        glTexCoord2f(1   ,   w2); glVertex3f(1  , -1+w, 0);
      glEnd();
      break;
    }
    case 2:
    { // 丸
      glBindTexture(GL_TEXTURE_2D, Texture2);
      glBegin(GL_POLYGON);
      for (double a = -M_PI; a < M_PI; a += 0.2)
      {
        glTexCoord2d(cos(a)/2+0.5, sin(a)/2+0.5);
        glVertex3d(cos(a), sin(a), 0);
      }
      glEnd();
      break;
    }
    case 3:
    { // 右上がり三角形
      glBindTexture(GL_TEXTURE_2D, Texture34);
      glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
      glEnd();
      break;
    }
    case 4:
    { // 左上がり三角形
      glBindTexture(GL_TEXTURE_2D, Texture34);
      glBegin(GL_TRIANGLES);
        glTexCoord2f(1, 1); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f( 1, -1, 0);
        glTexCoord2f(0, 0); glVertex3f(-1,  1, 0);
      glEnd();
      break;
    }
    case 5:
    { // 上丸下角
      // bottom
      glBindTexture(GL_TEXTURE_2D, Texture5);
      glBegin(GL_POLYGON);
        for (double a = 0; a < M_PI; a += 0.2)
        {
          glTexCoord2d(cos(a)/2+0.5, sin(a)/2+0.5);
          glVertex3d(cos(a), sin(a), 0);
        }
        glTexCoord2f(0.25, 0);
        glVertex3f(-0.5, -1, 0);
      glEnd();
      glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 0.5); glVertex3f(-1,  0, 0);
        glTexCoord2f(1, 0.5); glVertex3f( 1,  0, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
      glEnd();
      break;
    }
  }
  glDisable(GL_TEXTURE_2D);
}
//---------------------------------------------------------------------------
