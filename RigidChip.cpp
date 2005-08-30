//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RigidChip.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void glMaterialColor3f(float r, float g, float b)
{
  //glMaterialColor4f(r, g, b, 1)
  glColor3f(r, g, b);
  const float diffuse[4] = {0.8*r, 0.8*g, 0.8*b, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  const float ambient[4] = {0.2*r+0.1, 0.2*g+0.1, 0.2*b+0.1, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
void glMaterialColor4f(float r, float g, float b, float a)
{
  glColor3f(r, g, b);
  const float diffuse[4] = {0.8*r, 0.8*g, 0.8*b, a};
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  const float ambient[4] = {0.2*r+0.1, 0.2*g+0.1, 0.2*b+0.1, a};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRigidChipsVariable::TRigidChipsVariable(TRigidChipCore *c, AnsiString opt)
{
  core = c;
  FOptions = new TStringList;

  Options->CommaText = opt;
  for (int i = 0; i < Options->Count; i ++)
    Options->Strings[i] = Options->Names[i].LowerCase() + "=" + Options->Values[Options->Names[i]];

  def = min = max = step = 0;
          core->StrToDouble(Options->Values["default"], &def);
          core->StrToDouble(Options->Values["min"],     &min);
  fmax  = core->StrToDouble(Options->Values["max"],     &max);
  fstep = core->StrToDouble(Options->Values["step"],    &step);

  Value = def;
}
//---------------------------------------------------------------------------
TRigidChipsVariable::~TRigidChipsVariable()
{
  delete FOptions;
}
//---------------------------------------------------------------------------
void TRigidChipsVariable::SetValue(double v)
{
  Modify = true;
  if (v < min)
    FValue = min;
  else if (fmax && v > max)
    FValue = max;
  else
    FValue = v;
}
//---------------------------------------------------------------------------
void TRigidChipsVariable::Act()
{
  if (fstep)
  {
    if (FValue < def)
    {
      if (def - FValue < step)
        FValue = def;
      else
        FValue += step;
    }
    else if (FValue > def)
    {
      if (FValue - def < step)
        FValue = def;
      else
        FValue -= step;
    }
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRigidChipsKey::TRigidChipsKey(TRigidChipCore *c, AnsiString var, AnsiString opt)
{
  core = c;
  variable = var;
  FOptions = new TStringList;

  Options->CommaText = opt;
  for (int i = 0; i < Options->Count; i ++)
    Options->Strings[i] = Options->Names[i].LowerCase() + "=" + Options->Values[Options->Names[i]];

  step = 0;
  core->StrToDouble(Options->Values["step"], &step);
}
//---------------------------------------------------------------------------
TRigidChipsKey::~TRigidChipsKey()
{
  delete FOptions;
}
//---------------------------------------------------------------------------
void TRigidChipsKey::Act()
{
  TRigidChipsVariable *var = core->Variables[variable];
  if (var)
    var->Value = var->Value + step;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRigidChip::TRigidChip()
{
  FOptions = new TStringList;
  ChipList = new TList;
}
//---------------------------------------------------------------------------
TRigidChip::~TRigidChip()
{
  for (int i = ChipList->Count-1; i >= 0; i --)
    ((TRigidChip*)ChipList->Items[i])->Delete();
  delete ChipList;
  delete FOptions;
}
//---------------------------------------------------------------------------
void TRigidChip::SetOptions(AnsiString opt)
{
  Options->CommaText = opt;
  for (int i = 0; i < Options->Count; i ++)
    Options->Strings[i] = Options->Names[i].LowerCase() + "=" + Options->Values[Options->Names[i]];
}
//---------------------------------------------------------------------------
int TRigidChip::GetSubChipsCount()
{
  return ChipList->Count;
}
//---------------------------------------------------------------------------
TRigidChip* TRigidChip::GetSubChip(int i)
{
  return (TRigidChip*)ChipList->Items[i];
}
//---------------------------------------------------------------------------
void TRigidChip::AddSubChip(TRigidChip *chip)
{
  ChipList->Add(chip);
  chip->Core = Core;
  chip->Parent = this;
}
//---------------------------------------------------------------------------
void TRigidChip::translate()
{
  switch (Direction)
  {
    case rdNorth: glTranslatef( 0,  1, 0); break;
    case rdSouth: glTranslatef( 0, -1, 0); break;
    case rdEast:  glTranslatef( 1,  0, 0); break;
    case rdWest:  glTranslatef(-1,  0, 0); break;
  }
}
void TRigidChip::rotate()
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
//---------------------------------------------------------------------------
void TRigidChip::setcolor()
{
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
}
void TRigidChip::direct()
{
  switch (Direction)
  {
    case rdNorth: break;
    case rdEast:  glRotatef(-90, 0, 0, 1); break;
    case rdWest:  glRotatef( 90, 0, 0, 1); break;
    case rdSouth: glRotatef(180, 0, 0, 1); break;
  }
}
//---------------------------------------------------------------------------
void TRigidChip::Draw()
{
  for (int i = 0; i < ChipList->Count; i ++)
    ((TRigidChip*)ChipList->Items[i])->Draw();
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
  VariableList = new TStringList;
  KeyList = new TStringList;

  Direction = rdCore;
  Core = this;
  Parent = NULL;
}
//---------------------------------------------------------------------------
TRigidChipCore::~TRigidChipCore()
{
  for (int i = KeyList->Count-1; i >= 0; i --)
    delete (TRigidChipsKey*)KeyList->Objects[i];
  delete KeyList;

  for (int i = VariableList->Count-1; i >= 0; i --)
    delete (TRigidChipsVariable*)VariableList->Objects[i];
  delete VariableList;
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
void TRigidChipCore::CheckVariable()
{
  for (int i = 0; i < VariableList->Count; i ++)
    ((TRigidChipsVariable*)VariableList->Objects[i])->Modify = false;
}
void TRigidChipCore::StepVariable()
{
  for (int i = 0; i < VariableList->Count; i ++)
    if (!((TRigidChipsVariable*)VariableList->Objects[i])->Modify)
      ((TRigidChipsVariable*)VariableList->Objects[i])->Act();
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
void TRigidChipCore::AddKey(AnsiString keyn, TRigidChipsKey *key)
{
  KeyList->AddObject(IntToStr(keyn.ToIntDef(0)), (TObject*)key);
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
      TRigidChipsVariable *var = Variables[str.SubString(2, str.Length())];
      if (var)
      {
        *d = - var->Value;
        return true;
      }
    }

    TRigidChipsVariable *var = Variables[str];
    if (var)
    {
      *d = var->Value;
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
      *d = str.ToDouble();
      return true;
    }
    catch (...)
    {
    }
  }
  return false;
}
//---------------------------------------------------------------------------
void TRigidChipCore::Draw()
{
  glMatrixMode(GL_MODELVIEW);

  glLoadName((GLuint)this);
  setcolor();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  TRigidChip::Draw();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipChip::Texture;
//---------------------------------------------------------------------------
void TRigidChipChip::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  rotate();
  translate();

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
void TRigidChipFrame::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  rotate();
  translate();

  int opt = Options->Values["option"].ToIntDef(0);
  if (opt != 1 || Core->ShowGhost)
  {
    glLoadName((GLuint)this);
    setcolor();
    glPushMatrix();
    direct();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture);
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
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
  }
  
  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipWeight::Texture;
//---------------------------------------------------------------------------
void TRigidChipWeight::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  rotate();
  translate();

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
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
void TRigidChipWheel::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  rotate();
  translate();

  glRotatef(angle, 0, 0, 1);

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
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
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
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
void TRigidChipRLW::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  rotate();
  translate();

  glRotatef(angle, 0, 0, 1);

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
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
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipJet::Texture;
GLuint TRigidChipJet::TextureBall;
GLUquadricObj* TRigidChipJet::quadric = NULL;
//---------------------------------------------------------------------------
void TRigidChipJet::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  rotate();
  translate();

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  int opt = Options->Values["option"].ToIntDef(0);
  if (opt == 1 || opt == 2)
  {
    glBindTexture(GL_TEXTURE_2D, TextureBall);
    double power = 0;
    Core->StrToDouble(Options->Values["power"], &power);
    if (power < 0) power *= -1;
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
  }
  else
  {
    glBindTexture(GL_TEXTURE_2D, Texture);
    glBegin(GL_POLYGON);
    glNormal3f(0, 0, 1);
    for (double a = -M_PI; a < M_PI; a += 0.2)
    {
      glTexCoord2d(cos(a)/2+0.5, sin(a)/2+0.5);
      glVertex3d(cos(a), sin(a), 0);
    }
    glEnd();

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
      glBlendFunc(GL_SRC_ALPHA , GL_ONE);
      glMaterialColor4f(0, 1, 1, 0.7); // 小さい炎
      gluCylinder(quadric, 0.25, 0, power / 300 + 0.25*sin(anime * M_PI / 180), 8, 1);
      glMaterialColor4f(0, 0, 1, 0.7); // 大きい炎
      gluCylinder(quadric, 0.5, 0, power / 200 + 0.25*sin(anime * M_PI / 180), 8, 1);
      glDisable(GL_BLEND);
      glPopMatrix();
    }
  }
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
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
void TRigidChipRudder::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
    glRotatef(-angle, 0, 0, 1);
  translate();

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
void TRigidChipRudderF::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
    glRotatef(-angle, 0, 0, 1);
  translate();

  int opt = Options->Values["option"].ToIntDef(0);
  if (opt != 1 || Core->ShowGhost)
  {
    glLoadName((GLuint)this);
    setcolor();
    glPushMatrix();
    direct();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture);
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
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
  }
  
  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipTrim::Texture;
//---------------------------------------------------------------------------
void TRigidChipTrim::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
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
  translate();

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
void TRigidChipTrimF::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
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
  translate();

  int opt = Options->Values["option"].ToIntDef(0);
  if (opt != 1 || Core->ShowGhost)
  {
    glLoadName((GLuint)this);
    setcolor();
    glPushMatrix();
    direct();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture);
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
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
  }
  
  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipArm::Texture;
GLUquadricObj* TRigidChipArm::quadric = NULL;
//---------------------------------------------------------------------------
TRigidChipArm::TRigidChipArm()
        : TRigidChip()
{
  energy = 0;
}
//---------------------------------------------------------------------------
void TRigidChipArm::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  rotate();
  translate();

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
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
    glBlendFunc(GL_SRC_ALPHA , GL_ONE);
    glMaterialColor4f(1, 1, 0, 0.5);
    gluCylinder(quadric, opt/200000.0+0.1, opt/250000.0, 2.5, 8, 4);
    glDisable(GL_BLEND);
    glPopMatrix();
  }
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
void TRigidChipArm::Act()
{
  bool a = anime;
  if (anime)
    anime = false;

  unsigned int opt = Options->Values["option"].ToIntDef(0);
  if (opt < 1) return;

  energy += 5000;
  if (energy < opt)
    return;
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
  TRigidChip::Act();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipCowl::Texture0;
GLuint TRigidChipCowl::Texture2;
GLuint TRigidChipCowl::Texture34;
GLuint TRigidChipCowl::Texture5;
//---------------------------------------------------------------------------
TRigidChipCowl::TRigidChipCowl()
        : TRigidChip()
{
  fopt = false;
}
//---------------------------------------------------------------------------
void TRigidChipCowl::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  // スタックオーバーするので自前でバックアップ
  GLfloat matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  translate();
  rotate();
  translate();

  if (!fopt)
  {
    opt = Options->Values["option"].ToIntDef(0);
    fopt = true;
  }

  glLoadName((GLuint)this);
  setcolor();
  glPushMatrix();
  direct();
  glEnable(GL_TEXTURE_2D);
  glNormal3f(0, 0, 1);
  switch (opt)
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
  glPopMatrix();

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
