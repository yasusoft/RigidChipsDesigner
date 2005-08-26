//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RigidChip.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
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
          core->StrToDouble(Options->Values["max"],     &max);
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
  if (v < min)
    FValue = min;
  else if (v > max)
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
TRigidChip::TRigidChip()
{
  FOptions = new TStringList;
  ChipsList = new TList;
}
//---------------------------------------------------------------------------
TRigidChip::~TRigidChip()
{
  for (int i = ChipsList->Count-1; i >= 0; i --)
    ((TRigidChip*)ChipsList->Items[i])->Delete();
  delete ChipsList;
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
  return ChipsList->Count;
}
//---------------------------------------------------------------------------
TRigidChip* TRigidChip::GetSubChip(int i)
{
  return (TRigidChip*)ChipsList->Items[i];
}
//---------------------------------------------------------------------------
void TRigidChip::AddSubChip(TRigidChip *chip)
{
  ChipsList->Add(chip);
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
void TRigidChip::sphere(float r)
{
  glBegin(GL_QUADS);
  const double pi = 3.14159265358979323846264338327950;
  const double d = 0.05;
  for (double a = -pi; a < pi; a += d)
  {
    for (double b = 0; b < pi; b += d)
    {
      /*
      if (cos(b+d) < -0.99 || 0.99 < cos(b+d)
      )
        glColor3f(0.3, 0.3, 0.3);
      else
        glColor3f(0.7, 0.7, 0.7);
      */
      glVertex3d(r*cos(a  )*sin(b  ), r*sin(a  )*sin(b  ), r*cos(b  ));
      glVertex3d(r*cos(a+d)*sin(b  ), r*sin(a+d)*sin(b  ), r*cos(b  ));
      glVertex3d(r*cos(a+d)*sin(b+d), r*sin(a+d)*sin(b+d), r*cos(b+d));
      glVertex3d(r*cos(a  )*sin(b+d), r*sin(a  )*sin(b+d), r*cos(b+d));
    }
  }
  glEnd();
}
//---------------------------------------------------------------------------
void TRigidChip::setcolor()
{
  double d;
  if (Core->StrToDouble(Options->Values["color"], &d))
  {
    int c = d;
    float r = ((c >> 16) & 0xFF) / 255.0;
    float g = ((c >>  8) & 0xFF) / 255.0;
    float b = ((c >>  0) & 0xFF) / 255.0;
    glColor3f(0.1+r, 0.1+g, 0.1+b);
  }
  else
    glColor3f(1, 1, 1);
}
void TRigidChip::rotatetex()
{
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5, 0.5, 0);
  switch (Direction)
  {
    case rdNorth: break;
    case rdEast:  glRotatef( 90, 0, 0, 1); break;
    case rdWest:  glRotatef(-90, 0, 0, 1); break;
    case rdSouth: glRotatef(180, 0, 0, 1); break;
  }
  glTranslatef(-0.5, -0.5, 0);
  glMatrixMode(GL_MODELVIEW);
}
//---------------------------------------------------------------------------
void TRigidChip::Draw()
{
  for (int i = 0; i < ChipsList->Count; i ++)
    ((TRigidChip*)ChipsList->Items[i])->Draw();
}
//---------------------------------------------------------------------------
void TRigidChip::Act()
{
  for (int i = 0; i < ChipsList->Count; i ++)
    ((TRigidChip*)ChipsList->Items[i])->Act();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipCore::Texture;
//---------------------------------------------------------------------------
TRigidChipCore::TRigidChipCore()
        : TRigidChip()
{
  VariableList = new TStringList;

  Direction = rdCore;
  Core = this;
  Parent = NULL;
}
//---------------------------------------------------------------------------
TRigidChipCore::~TRigidChipCore()
{
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

    str = str.LowerCase();
    if (str == "red")   { *d = 0xFF0000; return true; }
    if (str == "green") { *d = 0x00FF00; return true; }
    if (str == "blue")  { *d = 0x0000FF; return true; }

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
  glPushMatrix();

  setcolor();
  rotatetex();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  TRigidChip::Draw();

  glPopMatrix();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipChip::Texture;
//---------------------------------------------------------------------------
void TRigidChipChip::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  translate();
  rotate();
  translate();

  setcolor();
  rotatetex();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  TRigidChip::Draw();

  glPopMatrix();
}
//---------------------------------------------------------------------------
void TRigidChipFrame::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  translate();
  rotate();
  translate();

  setcolor();
  rotatetex();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
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
  glDisable(GL_TEXTURE_2D);

  TRigidChip::Draw();

  glPopMatrix();
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
  glPushMatrix();

  translate();
  rotate();
  translate();

  glRotatef(angle, 0, 0, 1);

  setcolor();
  rotatetex();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_POLYGON);
  for (double a = -3.14159265358979323846264338327950; a < 3.14159265358979323846264338327950; a += 0.01)
  {
    glTexCoord2d(cos(a)/2+0.5, sin(a)/2+0.5);
    glVertex3d(cos(a), sin(a), 0);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);

  TRigidChip::Draw();

  glPopMatrix();
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
      if (power > 0)
        angle = (angle - 20) % 360;
      else if (power < 0)
        angle = (angle + 20) % 360;
    }
  }
  TRigidChip::Act();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipJet::Texture;
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

  setcolor();
  int opt = Options->Values["option"].ToIntDef(0);
  if (opt == 1 || opt == 2)
  {
    // 0: 半径0.5
    // 150: 半径1
    // 3500: 半径3
    // 15000: 半径5
    // 45000: 半径7
    // 90000: 半径9
    //glutSolidSphere(0.5 + Options->Values["power"].ToIntDef(0), 0, 0);
    sphere(0.5 + sqrt(Options->Values["power"].ToIntDef(0))/32);
  }
  else
  {
    rotatetex();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glBegin(GL_POLYGON);
    for (double a = -3.14159265358979323846264338327950; a < 3.14159265358979323846264338327950; a += 0.01)
    {
      glTexCoord2d(cos(a)/2+0.5, sin(a)/2+0.5);
      glVertex3d(cos(a), sin(a), 0);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }

  TRigidChip::Draw();

  // スタックオーバーするので自前で戻す
  glLoadMatrixf(matrix);
  //glPopMatrix();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipRudder::Texture;
//---------------------------------------------------------------------------
void TRigidChipRudder::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  translate();
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
    glRotatef(-angle, 0, 0, 1);
  translate();

  setcolor();
  rotatetex();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  TRigidChip::Draw();

  glPopMatrix();
}
//---------------------------------------------------------------------------
void TRigidChipRudderF::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  translate();
  double angle;
  if (Core->StrToDouble(Options->Values["angle"], &angle))
    glRotatef(-angle, 0, 0, 1);
  translate();

  setcolor();
  rotatetex();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        const float w = 0.25, w2 = w / 2;
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
  glDisable(GL_TEXTURE_2D);

  TRigidChip::Draw();

  glPopMatrix();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLuint TRigidChipTrim::Texture;
//---------------------------------------------------------------------------
void TRigidChipTrim::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  translate();
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
  translate();

  setcolor();
  rotatetex();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, 0);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, 0);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  TRigidChip::Draw();

  glPopMatrix();
}
//---------------------------------------------------------------------------
void TRigidChipTrimF::Draw()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  translate();
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
  translate();

  setcolor();
  rotatetex();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glBegin(GL_QUADS);
        const float w = 0.25, w2 = w / 2;
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

  TRigidChip::Draw();

  glPopMatrix();
}
//---------------------------------------------------------------------------
