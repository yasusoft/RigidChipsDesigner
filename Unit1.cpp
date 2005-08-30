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
void __fastcall TForm1::WMDropFiles(TWMDropFiles &Msg)
{
  HDROP hDrop = (HDROP)Msg.Drop;
  //int n = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
  char buf[MAX_PATH];
  DragQueryFile(hDrop, 0, buf, MAX_PATH);

  AnsiString filename = buf;
  if (ExtractFileExt(filename).LowerCase() == ".txt"
   || ExtractFileExt(filename).LowerCase() == ".rcd")
  {
    if (Core)
    {
      TRigidChip *c = Core;
      Core = NULL;
      c->Delete();
    }

    TRCDLoader loader;
    Core = loader.Load(filename);
    if (Core == NULL)
    {
      Application->MessageBox(loader.ErrorMessage.c_str(), "エラー", MB_OK);
      return;
    }
    FileName = filename;

    PaintPanelMouseDown(NULL, mbMiddle, TShiftState(), 0, 0);
  }

  DragFinish(hDrop);
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
  DragAcceptFiles(Handle, true);
  WM_RIGHTCHIP_LOAD = RegisterWindowMessage("WM_RIGHTCHIP_LOAD");

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

  // Lighting
  GLfloat col[] = {1, 1, 1, 1};
  glEnable(GL_LIGHTING);
  // ambient
  glLightfv(GL_LIGHT0 , GL_AMBIENT , col);
  glEnable(GL_LIGHT0);
  // diffuse1
  GLfloat pos1[] = {0, 0, 1000, 1};
  glLightfv(GL_LIGHT1 , GL_DIFFUSE , col);
  glLightfv(GL_LIGHT1 , GL_POSITION , pos1);
  glEnable(GL_LIGHT1);
  // diffuse2
  GLfloat pos2[] = {0, 0, -1000, 1};
  glLightfv(GL_LIGHT2 , GL_DIFFUSE , col);
  glLightfv(GL_LIGHT2 , GL_POSITION , pos2);
  glEnable(GL_LIGHT2);

  // Textures
  Graphics::TBitmap *bmp = new Graphics::TBitmap;
  try
  {
    AnsiString dir = ExtractFileDir(Application->ExeName);
    BITMAP bm;

    #define LOAD_TEXTURE(file, var) { \
        bmp->LoadFromFile(dir + "\\textures\\" file ".bmp"); \
        if (GetObject(bmp->Handle, sizeof(BITMAP), &bm)) \
        { \
          glGenTextures(1, &var); \
          glBindTexture(GL_TEXTURE_2D, var); \
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits); \
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
          glBindTexture(GL_TEXTURE_2D, 0); \
        } \
        DeleteObject(bmp->Handle); \
    }

    LOAD_TEXTURE("core", TRigidChipCore::Texture);
    LOAD_TEXTURE("chip", TRigidChipChip::Texture);
    LOAD_TEXTURE("weight", TRigidChipWeight::Texture);
    LOAD_TEXTURE("wheel", TRigidChipWheel::Texture);
    LOAD_TEXTURE("rlw", TRigidChipRLW::Texture);
    LOAD_TEXTURE("jet", TRigidChipJet::Texture);
    LOAD_TEXTURE("ball", TRigidChipJet::TextureBall);
    LOAD_TEXTURE("rudder", TRigidChipRudder::Texture);
    LOAD_TEXTURE("trim", TRigidChipTrim::Texture);
    LOAD_TEXTURE("arm", TRigidChipArm::Texture);
    LOAD_TEXTURE("cowl0", TRigidChipCowl::Texture0);
    LOAD_TEXTURE("cowl2", TRigidChipCowl::Texture2);
    LOAD_TEXTURE("cowl34", TRigidChipCowl::Texture34);
    LOAD_TEXTURE("cowl5", TRigidChipCowl::Texture5);
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

  if (PickUp)
  {
    PickUp = false;

    const int SELBUFSIZE = 100;
    GLuint selbuf[SELBUFSIZE];
    GLint viewport[4];
    GLfloat matrix[16];

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetFloatv(GL_PROJECTION_MATRIX, matrix);
    glSelectBuffer(SELBUFSIZE, selbuf);
    glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(PickUpX, viewport[3]-PickUpY, 2.0, 2.0, viewport);
    glMultMatrixf(matrix);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(-camera_x, -camera_y, -camera_z);
    glMultMatrixd(camera_matrix);

    Core->ShowGhost = CheckGhost->Checked;
    Core->Draw();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    GLuint hitnum = glRenderMode(GL_RENDER);
    TRigidChip *chip = NULL;
    float maximum = 0;
    for (GLuint i = 0, j = 0; i < hitnum && j < SELBUFSIZE; i ++)
    {
      GLuint namenum = selbuf[j++];
      float depthmin = *((float*)&selbuf[j++]);
      float depthmax = *((float*)&selbuf[j++]);
      for (GLuint k = 0; k < namenum; k ++)
      {
        GLuint name = selbuf[j++];
        if (chip == NULL || depthmax > maximum)
        {
          chip = (TRigidChip*)name;
          maximum = depthmax;
        }
      }
    }
    //if (chip)
    //  chip->Options->Values["color"] = "#FF0000";
  }

  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslated(-camera_x, -camera_y, -camera_z);
  glMultMatrixd(camera_matrix);

  Core->ShowGhost = CheckGhost->Checked;
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
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(camera_matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, mouse_cm);
    glPopMatrix();
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
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, camera_matrix);
    glPopMatrix();
    Display();
  }
  else if (Button == mbLeft)
  {
    PickUpX = X;
    PickUpY = Y;
    PickUp = true;
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
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRotated(X - mouse_x, 0, 1, 0);
    glRotated(Y - mouse_y, 1, 0, 0);
    glMultMatrixd(mouse_cm);
    glGetDoublev(GL_MODELVIEW_MATRIX, camera_matrix);
    glPopMatrix();
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
  //if (Button == mbLeft)
    mouse_left = false;
  //if (Button == mbRight)
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
    FileName = OpenDialog->FileName;

    PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
  }
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  Timer1->Enabled = false;
  if (Core)
  {
    Core->CheckVariable();
    for (int i = 0; i < 10; i ++)
      if (keys_down[i])
        Core->ActKey(IntToStr(i));
    Core->StepVariable();
    Core->Act();
    Display();
    Yield();
  }
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonSendRCClick(TObject *Sender)
{
  if (FileName != "")
  {
    PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_START, 0);
    for (int i = 1; i <= FileName.Length(); i ++)
      PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_CHAR, FileName[i]);
    PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_END, 0);
  }
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditKeyTestKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  const bool f = true;
  switch (Key)
  {
    case VK_UP: keys_down[0] = f; break;
    case VK_DOWN: keys_down[1] = f; break;
    case VK_LEFT: keys_down[2] = f; break;
    case VK_RIGHT: keys_down[3] = f; break;
    case 'Z': case 'z': keys_down[4] = f; break;
    case 'X': case 'x': keys_down[5] = f; break;
    case 'C': case 'c': keys_down[6] = f; break;
    case 'A': case 'a': keys_down[7] = f; break;
    case 'S': case 's': keys_down[8] = f; break;
    case 'D': case 'd': keys_down[9] = f; break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditKeyTestKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  const bool f = false;
  switch (Key)
  {
    case VK_UP: keys_down[0] = f; break;
    case VK_DOWN: keys_down[1] = f; break;
    case VK_LEFT: keys_down[2] = f; break;
    case VK_RIGHT: keys_down[3] = f; break;
    case 'Z': case 'z': keys_down[4] = f; break;
    case 'X': case 'x': keys_down[5] = f; break;
    case 'C': case 'c': keys_down[6] = f; break;
    case 'A': case 'a': keys_down[7] = f; break;
    case 'S': case 's': keys_down[8] = f; break;
    case 'D': case 'd': keys_down[9] = f; break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerReloadTimer(TObject *Sender)
{
  if (FileName == "")
    return;

  int age = FileAge(FileName);
  if (age == TimerReload->Tag)
    return;
  TimerReload->Tag = age;
  
  TRCDLoader loader;
  TRigidChipCore *corenew = loader.Load(FileName);
  if (corenew)
  {
    TRigidChipCore *coreback = Core;
    Core = corenew;
    Display();

    if (coreback)
      coreback->Delete();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckReloadClick(TObject *Sender)
{
  TimerReload->Enabled = CheckReload->Checked;
  TimerReload->Interval = EditReload->Text.ToIntDef(1000);
  EditReload->Text = IntToStr(TimerReload->Interval);
  TimerReload->Tag = 0;
  EditReload->Enabled = !TimerReload->Enabled;
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckGhostClick(TObject *Sender)
{
  Display();
}
//---------------------------------------------------------------------------

