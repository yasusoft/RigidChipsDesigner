//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RCDLoader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
  PaintPanel = new TPaintPanel(this);
  PaintPanel->Parent = this;
  PaintPanel->Left = PaintPanelDummy->Left;
  PaintPanel->Top = PaintPanelDummy->Top;
  PaintPanel->Width = PaintPanelDummy->Width;
  PaintPanel->Height = PaintPanelDummy->Height;
  PaintPanel->Anchors = TAnchors() << akLeft << akTop << akRight << akBottom; //PaintPanelDummy->Anchors;
  PaintPanel->OnMouseDown = PaintPanelMouseDown;
  PaintPanel->OnMouseMove = PaintPanelMouseMove;
  PaintPanel->OnMouseUp = PaintPanelMouseUp;
  PaintPanel->OnPaint = PaintPanelPaint;
  PaintPanel->OnResize = PaintPanelResize;

  ghDC = GetDC(PaintPanel->Handle);
  //ghDC = PaintPanel->Canvas->Handle;

  // bSetupPixelFormat(ghDC)
  PIXELFORMATDESCRIPTOR pfd;
  ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.dwLayerMask = PFD_MAIN_PLANE;
  pfd.iPixelType = PFD_TYPE_RGBA; // PFD_TYPE_COLORINDEX
  pfd.cColorBits = 24;
  pfd.cDepthBits = 32;

  int pixelformat = ChoosePixelFormat(ghDC, &pfd);
  if (pixelformat == 0)
  {
    Application->MessageBox("ChoosePixelFormat failed", "Error", MB_ICONERROR | MB_OK);
    Close();
    return;
  }
  if (SetPixelFormat(ghDC, pixelformat, &pfd) == FALSE)
  {
    Application->MessageBox("SetPixelFormat failed", "Error", MB_ICONERROR | MB_OK);
    Close();
    return;
  }

  ghRC = wglCreateContext(ghDC);
  wglMakeCurrent(ghDC, ghRC);

  // InitilizeGL
  glClearColor(0, 0, 0, 0);
  glViewport(0, 0, PaintPanel->Width, PaintPanel->Height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-2, 2, -2, 2, 10, 1000);
  //glOrtho(-2, 2, -2, 2, 10, 1000);
  gluLookAt(0, 0, 40, 0, 0, 0, 0, 1, 0);

  PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);

  glEnable(GL_DEPTH_TEST);

  // textures
  Graphics::TBitmap *bmp = new Graphics::TBitmap;
  try
  {
    BITMAP bm;
    bmp->LoadFromFile("textures\\core.bmp");
    if (GetObject(bmp->Handle, sizeof(BITMAP), &bm))
    {
      glGenTextures(1, &TRigidChipCore::Texture);
      glBindTexture(GL_TEXTURE_2D, TRigidChipCore::Texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    DeleteObject(bmp->Handle);

    bmp->LoadFromFile("textures\\chip.bmp");
    if (GetObject(bmp->Handle, sizeof(BITMAP), &bm))
    {
      glGenTextures(1, &TRigidChipChip::Texture);
      glBindTexture(GL_TEXTURE_2D, TRigidChipChip::Texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    DeleteObject(bmp->Handle);

    bmp->LoadFromFile("textures\\wheel.bmp");
    if (GetObject(bmp->Handle, sizeof(BITMAP), &bm))
    {
      glGenTextures(1, &TRigidChipWheel::Texture);
      glBindTexture(GL_TEXTURE_2D, TRigidChipWheel::Texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    DeleteObject(bmp->Handle);

    bmp->LoadFromFile("textures\\jet.bmp");
    if (GetObject(bmp->Handle, sizeof(BITMAP), &bm))
    {
      glGenTextures(1, &TRigidChipJet::Texture);
      glBindTexture(GL_TEXTURE_2D, TRigidChipJet::Texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    DeleteObject(bmp->Handle);

    bmp->LoadFromFile("textures\\rudder.bmp");
    if (GetObject(bmp->Handle, sizeof(BITMAP), &bm))
    {
      glGenTextures(1, &TRigidChipRudder::Texture);
      glBindTexture(GL_TEXTURE_2D, TRigidChipRudder::Texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    DeleteObject(bmp->Handle);

    bmp->LoadFromFile("textures\\trim.bmp");
    if (GetObject(bmp->Handle, sizeof(BITMAP), &bm))
    {
      glGenTextures(1, &TRigidChipTrim::Texture);
      glBindTexture(GL_TEXTURE_2D, TRigidChipTrim::Texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    DeleteObject(bmp->Handle);
  }
  __finally
  {
    delete bmp;
  }

  /*
  TRCDLoader loader;
  Core = loader.Load("Basic.txt");
  if (Core == NULL)
  {
    Application->MessageBox(loader.ErrorMessage.c_str(), "エラー", MB_OK);
    return;
  }
  */
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
  if (Core)
    Core->Delete();

  delete PaintPanel;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
  if (ghRC)
    wglDeleteContext(ghRC);
  if (ghDC)
    ReleaseDC(PaintPanel->Handle, ghDC);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelPaint(TObject *Sender)
{
  Display();
  //Yield();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Display()
{
  if (!Core)
    return;

  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslated(-camera_x, -camera_y, -camera_z);
  glRotated(camera_rx, 1, 0, 0);
  glRotated(camera_ry, 0, 1, 0);

  glColor3f(1, 1, 1);
  glBegin(GL_LINES);
        glVertex3f(-5, 0, 0);
        glVertex3f( 5, 0, 0);
        glVertex3f( 0,-5, 0);
        glVertex3f( 0, 5, 0);
        glVertex3f( 0, 0,-5);
        glVertex3f( 0, 0, 5);
  glEnd();

  Core->Draw();
  
  glPopMatrix();

  glFlush();
  SwapBuffers(ghDC);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
  mouse_x = X;
  mouse_y = Y;
  if (Button == mbRight)
  {
    mouse_right = true;
    mouse_crx = camera_rx;
    mouse_cry = camera_ry;
  }
  else if (mouse_right && Button == mbLeft)
  {
    mouse_left = true;
    mouse_cx = camera_x;
    mouse_cy = camera_y;
  }
  else if (Button == mbMiddle)
  {
    camera_x = 0;
    camera_y = 0;
    camera_z = 0;
    camera_rx = 0;
    camera_ry = 0;
    Display();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  if (mouse_left && mouse_right)
  {
    camera_x = mouse_cx - (X - mouse_x) / 50.0;
    camera_y = mouse_cy + (Y - mouse_y) / 50.0;
    Display();
  }
  else if (mouse_right)
  {
    camera_ry = mouse_cry + (X - mouse_x);
    camera_rx = mouse_crx + (Y - mouse_y);
    Display();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormMouseWheel(TObject *Sender, TShiftState Shift,
      int WheelDelta, TPoint &MousePos, bool &Handled)
{
  if (WheelDelta < 0 && camera_z < 250)
    camera_z += camera_z > 0 ? camera_z * 0.1 + 1 : 1;
  if (WheelDelta > 0 && camera_z > -25)
    camera_z -= camera_z > 0 ? camera_z * 0.1 + 1 : 1;
  Display();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft)
    mouse_left = false;
  if (Button == mbRight)
    mouse_right = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelResize(TObject *Sender)
{
  glViewport(0, 0, PaintPanel->Width, PaintPanel->Height);
  Display();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonOpenClick(TObject *Sender)
{
  OpenDialog->FileName = "";
  if (OpenDialog->Execute())
  {
    if (Core)
    {
      TRigidChip *c = Core;
      Core = NULL;
      c->Delete();
    }

    TRCDLoader loader;
    Core = loader.Load(OpenDialog->FileName);
    if (Core == NULL)
    {
      Application->MessageBox(loader.ErrorMessage.c_str(), "エラー", MB_OK);
      return;
    }

    PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  Timer1->Enabled = false;
  if (Core)
  {
    Core->Act();
    Display();
    Yield();
  }
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

