//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RigidChip.h"
#include "RCDLoader.h"
#include "RCDSaver.h"
#include <Clipbrd.hpp>
#include "RCScript.h"
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
    if (Modify)
      switch (Application->MessageBox("Data is modified.\nDo you want to save?", "Modified", MB_YESNOCANCEL | MB_ICONQUESTION))
      {
        case IDYES:
          KeySaveClick(NULL);
          break;
        case IDNO:
          Modify = false;
          break;
      }
    if (Modify)
      return;

    if (Core)
    {
      TRigidChip *c = Core;
      Origin = Core = NULL;
      delete c;
    }

    Origin = Core = RCDLoader->Load(filename);
    if (Core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "�G���[", MB_OK);
      return;
    }
    FileName = filename;
    Modify = false;
    PaintPanelMouseDown(NULL, mbMiddle, TShiftState(), 0, 0);
    PageControl1->ActivePage = TabBody;
    ScriptOut->Clear();
    RunScript = true;
  }

  DragFinish(hDrop);
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
  randomize();
  Origin = Core = NULL;
  FileName = "";
  Modify = false;
  PageControl1->ActivePage = TabBody;

  RCDLoader = new TRCDLoader;
  RCDSaver = new TRCDSaver;

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
    BITMAP bm;

    #define LOAD_TEXTURE(rc, var) { \
        bmp->LoadFromResourceName((int)HInstance, rc); \
        if (GetObject(bmp->Handle, sizeof(BITMAP), &bm)) \
        { \
          glGenTextures(1, &var); \
          glBindTexture(GL_TEXTURE_2D, var); \
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits); \
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
          glBindTexture(GL_TEXTURE_2D, 0); \
        } \
    }
    LOAD_TEXTURE("TEX_CORE", TRigidChipCore::Texture);
    LOAD_TEXTURE("TEX_CHIP", TRigidChipChip::Texture);
    LOAD_TEXTURE("TEX_WEIGHT", TRigidChipWeight::Texture);
    LOAD_TEXTURE("TEX_WHEEL", TRigidChipWheel::Texture);
    LOAD_TEXTURE("TEX_RLW", TRigidChipRLW::Texture);
    LOAD_TEXTURE("TEX_JET", TRigidChipJet::Texture);
    LOAD_TEXTURE("TEX_BALL", TRigidChipJet::TextureBall);
    LOAD_TEXTURE("TEX_RUDDER", TRigidChipRudder::Texture);
    LOAD_TEXTURE("TEX_TRIM", TRigidChipTrim::Texture);
    LOAD_TEXTURE("TEX_ARM", TRigidChipArm::Texture);
    LOAD_TEXTURE("TEX_COWL0", TRigidChipCowl::Texture0);
    LOAD_TEXTURE("TEX_COWL2", TRigidChipCowl::Texture2);
    LOAD_TEXTURE("TEX_COWL34", TRigidChipCowl::Texture34);
    LOAD_TEXTURE("TEX_COWL5", TRigidChipCowl::Texture5);
  }
  __finally
  {
    delete bmp;
  }

  DragAcceptFiles(Handle, true);
  WM_RIGHTCHIP_LOAD = RegisterWindowMessage("WM_RIGHTCHIP_LOAD");

  if (ParamCount() >= 1 && FileExists(ParamStr(1)))
  {
    Origin = Core = RCDLoader->Load(ParamStr(1));
    if (Core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "�G���[", MB_OK);
    }
    else
    {
      FileName = ParamStr(1);
      Modify = false;
      PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
      PageControl1->ActivePage = TabBody;
      ScriptOut->Clear();
      RunScript = true;
    }
  }
  if (Core == NULL)
    KeyNewClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
  if (Core)
    delete Core;

  if (ghRC)
    wglDeleteContext(ghRC);
  if (ghDC)
    ReleaseDC(PaintPanel->Handle, ghDC);

  delete PaintPanel;

  delete RCDSaver;
  delete RCDLoader;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  KeyNewClick(Sender);
  CanClose = !Modify;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelPaint(TObject *Sender)
{
  Display();
  //Yield();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SelectionChange(TRigidChip *chip)
{
  ListNorth->Items->Clear();
  ListEast->Items->Clear();
  ListWest->Items->Clear();
  ListSouth->Items->Clear();
  Direction = rdCore;

  if (!Core) return;
  Core->Select = NULL;

  if (chip)
  {
    // �e�`�b�v���t���̃��X�g�ɒǉ�
    if (chip->Parent)
      switch (chip->Direction)
      {
        case rdNorth:
          ListSouth->Items->AddObject("p:" + chip->Parent->GetTypeString(), (TObject*)chip->Parent);
          break;
        case rdEast:
          ListWest->Items->AddObject("p:" + chip->Parent->GetTypeString(), (TObject*)chip->Parent);
          break;
        case rdWest:
          ListEast->Items->AddObject("p:" + chip->Parent->GetTypeString(), (TObject*)chip->Parent);
          break;
        case rdSouth:
          ListNorth->Items->AddObject("p:" + chip->Parent->GetTypeString(), (TObject*)chip->Parent);
          break;
      }

    for (int i = 0; i < chip->SubChipsCount; i ++)
    {
      switch (chip->SubChips[i]->Direction)
      {
        case rdNorth:
          ListNorth->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString(), (TObject*)chip->SubChips[i]);
          break;
        case rdEast:
          ListEast->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString(), (TObject*)chip->SubChips[i]);
          break;
        case rdWest:
          ListWest->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString(), (TObject*)chip->SubChips[i]);
          break;
        case rdSouth:
          ListSouth->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString(), (TObject*)chip->SubChips[i]);
          break;
      }
    }
    OptionsEditor->Strings->Assign(chip->Options);
    ComboDirection->ItemIndex = chip->Direction - 1;
    ComboType->ItemIndex = ComboType->Items->IndexOf(chip->GetTypeString());

    Core->Select = chip;
    bool tmp = true;
    OptionsEditorSelectCell(NULL, OptionsEditor->Col, OptionsEditor->Row, tmp);
  }
  else
  {
    ComboDirection->ItemIndex = -1;
    ComboType->ItemIndex = -1;
    OptionsEditor->Strings->Clear();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Display()
{
  if (!Core)
    return;

  Core->ShowCowl = CheckCowl->Checked;
  Core->ShowGhost = CheckGhost->Checked;
  Core->ShowBalloon = CheckBalloon->Checked;

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
    gluPickMatrix(PickUpX, PickUpY, 2.0, 2.0, viewport);
    glMultMatrixf(matrix);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(-camera_x, -camera_y, -camera_z);
    glMultMatrixd(camera_matrix);

    Origin->Draw();
    Origin->DrawTranslucent();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    GLuint hitnum = glRenderMode(GL_RENDER);
    float minimum;
    GLuint name = 0;
    for (GLuint i = 0, j = 0; i < hitnum && j < SELBUFSIZE; i ++)
    {
      GLuint namenum = selbuf[j++];
      float depthmin = (float)selbuf[j] / 0x7FFFFFFF;
      j ++;
      //float depthmax = (float)selbuf[j] / 0x7FFFFFFF;
      j ++;
      for (GLuint k = 0; k < namenum; k ++)
      {
        GLuint n = selbuf[j++];
        if (n == 0)
          continue;
        if (name == 0 || depthmin < minimum)
        {
          name = n;
          minimum = depthmin;
        }
      }
    }

    Direction = rdCore;
    if (name == rdNorth || name == rdEast || name == rdWest || name == rdSouth)
    {
      Direction = (TRigidChipsDirection)name;
      PopupMenuAdd->Popup(Mouse->CursorPos.x, Mouse->CursorPos.y);
    }
    else if (name)
    {
      TRigidChip *chip = (TRigidChip*)name;
      if (Core->Select == chip)
        PopupMenu1->Popup(Mouse->CursorPos.x, Mouse->CursorPos.y);
      SelectionChange(chip);
    }
    else
    {
      if (Core->Select == NULL)
        Origin = Core;
      SelectionChange(NULL);
    }
  }

  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslated(-camera_x, -camera_y, -camera_z);
  glMultMatrixd(camera_matrix);

  Origin->Draw();
  Origin->DrawTranslucent();

  glPopMatrix();

  glFlush();
  SwapBuffers(ghDC);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
  switch (Button)
  {
    case mbLeft: MouseLeft = true; break;
    case mbMiddle: MouseMiddle = true; break;
    case mbRight: MouseRight = true; break;
  }

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
    if (Core)
      SelectionChange(NULL);
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
    PickUpY = PaintPanel->Height - Y - 1;
    PickUp = true;
    Display();
  }

  if (Button != mbMiddle)
    EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  MouseX = X;
  MouseY = Y;
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
  if (!EditKeyTest->Focused())
    return;
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
  switch (Button)
  {
    case mbLeft: MouseLeft = false; break;
    case mbMiddle: MouseMiddle = false; break;
    case mbRight: MouseRight = false; break;
  }

  //if (Button == mbLeft)
    mouse_left = false;
  //if (Button == mbRight)
    mouse_right = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelResize(TObject *Sender)
{
  int max = PaintPanel->Width > PaintPanel->Height ? PaintPanel->Width : PaintPanel->Height;
  glViewport((PaintPanel->Width - max) / 2, (PaintPanel->Height - max) / 2, max, max);
  Display();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  Timer1->Enabled = false;
  if (Core)
  {
    Core->CheckVariable();
    for (int i = 0; i < 17; i ++)
      if (KeysDown[i])
        Core->ActKey(IntToStr(i));
    Core->StepVariable();
    Core->Act();

    if (RunScript && Core->Script->ScriptText != "")
    {
      ScriptOut->Items->BeginUpdate();
      try {
        for (int i = 0; i < ScriptOut->Items->Count; i ++)
          ScriptOut->Items->Strings[i] = "";
        Core->Script->Run();
      } catch (Exception &e) {
        RunScript = false;
        ScriptOut->Clear();
        ScriptOut->Items->Add("ScriptError: Near " + IntToStr(Core->Script->ProgramCounter));
        ScriptOut->Items->Add(e.Message);
        MemoScript->SelStart = Core->Script->ProgramCounter;
        MemoScript->SelLength = 1;
      }
      ScriptOut->Items->EndUpdate();
    }

    Display();
    Yield();
  }
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckAnimeClick(TObject *Sender)
{
  Timer1->Enabled = CheckAnime->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditKeyTestKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  CheckAnime->Checked = true;
  Timer1->Enabled = true;
  const bool f = true;
  switch (Key)
  {
    case VK_UP: KeysDown[0] = f; break;
    case VK_DOWN: KeysDown[1] = f; break;
    case VK_LEFT: KeysDown[2] = f; break;
    case VK_RIGHT: KeysDown[3] = f; break;
    case 'Z': case 'z': KeysDown[4] = f; break;
    case 'X': case 'x': KeysDown[5] = f; break;
    case 'C': case 'c': KeysDown[6] = f; break;
    case 'A': case 'a': KeysDown[7] = f; break;
    case 'S': case 's': KeysDown[8] = f; break;
    case 'D': case 'd': KeysDown[9] = f; break;
    case 'V': case 'v': KeysDown[10] = f; break;
    case 'B': case 'b': KeysDown[11] = f; break;
    case 'F': case 'f': KeysDown[12] = f; break;
    case 'G': case 'g': KeysDown[13] = f; break;
    case 'Q': case 'q': KeysDown[14] = f; break;
    case 'W': case 'w': KeysDown[15] = f; break;
    case 'E': case 'e': KeysDown[16] = f; break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditKeyTestKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  const bool f = false;
  switch (Key)
  {
    case VK_UP: KeysDown[0] = f; break;
    case VK_DOWN: KeysDown[1] = f; break;
    case VK_LEFT: KeysDown[2] = f; break;
    case VK_RIGHT: KeysDown[3] = f; break;
    case 'Z': case 'z': KeysDown[4] = f; break;
    case 'X': case 'x': KeysDown[5] = f; break;
    case 'C': case 'c': KeysDown[6] = f; break;
    case 'A': case 'a': KeysDown[7] = f; break;
    case 'S': case 's': KeysDown[8] = f; break;
    case 'D': case 'd': KeysDown[9] = f; break;
    case 'V': case 'v': KeysDown[10] = f; break;
    case 'B': case 'b': KeysDown[11] = f; break;
    case 'F': case 'f': KeysDown[12] = f; break;
    case 'G': case 'g': KeysDown[13] = f; break;
    case 'Q': case 'q': KeysDown[14] = f; break;
    case 'W': case 'w': KeysDown[15] = f; break;
    case 'E': case 'e': KeysDown[16] = f; break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerReloadTimer(TObject *Sender)
{
  if (FileName == "")
    return;
  if (Modify)
  {
    TimerReload->Enabled = false;
    KeyAutoReload->Checked = false;
    return;
  }

  int age = FileAge(FileName);
  if (age == TimerReload->Tag)
    return;
  TimerReload->Tag = age;
  
  TRigidChipCore *corenew = RCDLoader->Load(FileName);
  if (corenew)
  {
    TRigidChipCore *coreback = Core;
    Origin = Core = corenew;
    Display();
    PageControl1Change(Sender);

    if (coreback)
      delete coreback;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckClick(TObject *Sender)
{
  Display();
}
//---------------------------------------------------------------------------
void ChangeDirection(TRigidChip *chip, int dir)
{
  int diff = dir - chip->Direction + 4;
  chip->Direction = (TRigidChipsDirection)dir;
  for (int i = 0; i < chip->SubChipsCount; i ++)
    ChangeDirection(chip->SubChips[i], (chip->SubChips[i]->Direction - 1 + diff) % 4 + 1);
}
void __fastcall TForm1::ComboDirectionChange(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (Core->Select == Core)
  {
    ComboDirection->ItemIndex = -1;
    return;
  }
  int d = ComboDirection->ItemIndex + 1;
  if (CheckRotate->Checked)
    ChangeDirection(Core->Select, d);
  else
    Core->Select->Direction = (TRigidChipsDirection)d;
  SelectionChange(Core->Select);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ComboTypeChange(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (Core->Select == Core)
  {
    ComboType->ItemIndex = -1;
    return;
  }

  AnsiString cap = ComboType->Text;

  // �T�u�`�b�v��Cowl�ȊO�������`�F�b�N
  bool haschip = false;
  for (int i = 0; i < Core->Select->SubChipsCount; i ++)
    if (Core->Select->SubChips[i]->GetType() != ctCowl)
    {
      haschip = true;
      break;
    }
  if (haschip && cap == "Cowl")
  {
    ComboType->ItemIndex = ComboType->Items->IndexOf(Core->Select->GetTypeString());
    Application->MessageBox("Chip which has sub chips can't be cowl", "ChangeType", MB_ICONERROR | MB_OK);
    return;
  }

  cap = "N:" + cap + "(){}";
  TRigidChip *chip = RCDLoader->StringToChip(cap);
  if (chip)
  {
    Core->Select->Parent->AddSubChip(chip);
    chip->Direction = Core->Select->Direction;
    chip->SetOptions(Core->Select->Options->Text);
    for (int i = 0; i < Core->Select->SubChipsCount; i ++)
      chip->AddSubChip(Core->Select->SubChips[i]);
    for (int i = Core->Select->SubChipsCount-1; i >= 0; i --)
      Core->Select->DelSubChip(i);

    TRigidChip *c = Core->Select;
    SelectionChange(chip);
    delete c;
  }
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OptionsEditorSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
  if (!Core || !Core->Select) return;

  if (ACol == 1 && ARow > 0)
  {
    AnsiString key = OptionsEditor->Keys[ARow].LowerCase();
    if (key == "color")
      OptionsEditor->ItemProps[ARow-1]->EditStyle = esEllipsis;
    else if (key == "angle")
    {
      OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
      OptionsEditor->ItemProps[ARow-1]->PickList->Clear();
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("30");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("45");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("60");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("90");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("120");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("135");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("150");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("180");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-30");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-45");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-60");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-90");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-120");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-135");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-150");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-180");
      for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
        OptionsEditor->ItemProps[ARow-1]->PickList->Add(i->first);
      for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
        OptionsEditor->ItemProps[ARow-1]->PickList->Add("-"+i->first);
    }
    else if (key == "power" || key == "brake")
    {
      OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
      OptionsEditor->ItemProps[ARow-1]->PickList->Clear();
      for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
        OptionsEditor->ItemProps[ARow-1]->PickList->Add(i->first);
      for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
        OptionsEditor->ItemProps[ARow-1]->PickList->Add("-"+i->first);
    }
    else if (key == "option")
    {
      if (Core->Select->GetType() == ctFrame
       || Core->Select->GetType() == ctRudderF
       || Core->Select->GetType() == ctTrimF)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1";
      }
      else if (Core->Select->GetType() == ctWheel)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1\n2";
      }
      else if (Core->Select->GetType() == ctJet)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1\n2";
      }
      else if (Core->Select->GetType() == ctCowl)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1\n2\n3\n4\n5";
      }
    }
    else if (key == "effect")
    {
      if (Core->Select->GetType() == ctWheel)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10";
      }
      else if (Core->Select->GetType() == ctJet)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1";
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OptionsEditorEditButtonClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;

  if (OptionsEditor->Keys[OptionsEditor->Row].LowerCase() != "color")
    return;
  double d = 0xFFFFFF;
  Core->StrToDouble(OptionsEditor->Values["color"], &d);
  int c = d;
  c = ((c&0xFF)<<16) | (c&0xFF00) | ((c&0xFF0000)>>16);
  ColorDialog->CustomColors->Text = "ColorA=" + IntToHex((int)c, 6);
  ColorDialog->Color = (TColor)c;
  if (ColorDialog->Execute())
  {
    c = ColorToRGB(ColorDialog->Color);
    c = ((c&0xFF)<<16) | (c&0xFF00) | ((c&0xFF0000)>>16);
    AnsiString s = "#"+IntToHex(c, 6);
    OptionsEditor->Values["color"] = s;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OptionsEditorStringsChange(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  Core->Select->SetOptions(OptionsEditor->Strings->CommaText);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PopupMenuOptionsPopup(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  bool core = Core->Select->GetType() == ctCore;
  bool frame = Core->Select->GetType() == ctFrame
         || Core->Select->GetType() == ctRudderF
         || Core->Select->GetType() == ctTrimF;
  bool wheel = Core->Select->GetType() == ctWheel
         || Core->Select->GetType() == ctRLW;
  bool jet = Core->Select->GetType() == ctJet;
  bool arm = Core->Select->GetType() == ctArm;
  bool cowl = Core->Select->GetType() == ctCowl;
  KeyOptionsAngle->Visible = !core;
  KeyOptionsSpring->Visible = !core;
  KeyOptionsDamper->Visible = !core;
  KeyOptionsPower->Visible = wheel || jet || arm;
  KeyOptionsBrake->Visible = wheel;
  KeyOptionsOption->Visible = frame || wheel || jet || arm || cowl;
  KeyOptionsEffect->Visible = wheel || jet;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyDelOptionClick(TObject *Sender)
{
  if (OptionsEditor->Strings->Count)
    OptionsEditor->DeleteRow(OptionsEditor->Row);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptionsClick(TObject *Sender)
{
  AnsiString cap = ((TMenuItem*)Sender)->Caption;
  OptionsEditor->InsertRow(cap, "", true);
  bool tmp = true;
  OptionsEditorSelectCell(NULL, OptionsEditor->Col, OptionsEditor->Row, tmp);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyAboutClick(TObject *Sender)
{
  ShowMessage(
        "RigidChips Designer\n"
        "Copyright(C) Yasu software\n"
        "http://www.yasu.nu/"
  );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyNewClick(TObject *Sender)
{
  if (Modify)
    switch (Application->MessageBox("Data is modified.\nDo you want to save?", "Modified", MB_YESNOCANCEL | MB_ICONQUESTION))
    {
      case IDYES:
        KeySaveClick(Sender);
        if (Modify)
          return;
        break;
      case IDCANCEL:
        return;
    }

  if (Core)
  {
    TRigidChip *c = Core;
    Origin = Core = NULL;
    delete c;
  }
  
  Origin = Core = new TRigidChipCore;
  FileName = "";
  Modify = false;
  PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
  PageControl1->ActivePage = TabBody;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOpenClick(TObject *Sender)
{
  if (Modify)
    switch (Application->MessageBox("Data is modified.\nDo you want to save?", "Modified", MB_YESNOCANCEL | MB_ICONQUESTION))
    {
      case IDYES:
        KeySaveClick(Sender);
        if (Modify)
          return;
        break;
      case IDCANCEL:
        return;
    }
  
  OpenDialog->FileName = "*.rcd;*.txt";
  if (OpenDialog->Execute())
  {
    if (Core)
    {
      TRigidChip *c = Core;
      Origin = Core = NULL;
      delete c;
    }

    Origin = Core = RCDLoader->Load(OpenDialog->FileName);
    if (Core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "�G���[", MB_OK);
      return;
    }
    FileName = OpenDialog->FileName;
    Modify = false;
    PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
    PageControl1->ActivePage = TabBody;
    ScriptOut->Clear();
    RunScript = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeySaveClick(TObject *Sender)
{
  if (!Core)
    return;
  if (FileName == "")
  {
    KeySaveAsClick(Sender);
    return;
  }

  RCDSaver->Save(FileName, Core);
  Modify = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeySaveAsClick(TObject *Sender)
{
  if (!Core)
    return;

  if (FileName != "")
  {
    SaveDialog->InitialDir = ExtractFileDir(FileName);
    SaveDialog->FileName = ExtractFileName(FileName);
  }
  else
    SaveDialog->FileName = "*.rcd;*.txt";
  if (SaveDialog->Execute())
  {
    FileName = SaveDialog->FileName;
    KeySaveClick(Sender);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyExitClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyRCLoadClick(TObject *Sender)
{
  if (FileName != "")
  {
    PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_START, 0);
    for (int i = 1; i <= FileName.Length(); i ++)
      PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_CHAR, FileName[i]);
    PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_END, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyAutoReloadClick(TObject *Sender)
{
  KeyAutoReload->Checked = !KeyAutoReload->Checked;
  TimerReload->Enabled = KeyAutoReload->Checked;
  TimerReload->Tag = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptSaveClick(TObject *Sender)
{
  KeySpaceBlock->Checked = RCDSaver->optSpaceAfterBlockType;
  KeyNLBlock->Checked = RCDSaver->optNewLineAfterBlockType;
  KeySpaceOpt->Checked = RCDSaver->optSpaceAfterOptions;
  KeyNLOpt->Checked = RCDSaver->optNewLineAfterOptions;
  KeySpaceComma->Checked = RCDSaver->optSpaceAfterComma;
  KeyNoSubNoNL->Checked = RCDSaver->optNoSubNoNewLine;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeySaveOptionClick(TObject *Sender)
{
  ((TMenuItem*)Sender)->Checked = !((TMenuItem*)Sender)->Checked;
  RCDSaver->optSpaceAfterBlockType = KeySpaceBlock->Checked;
  RCDSaver->optNewLineAfterBlockType = KeyNLBlock->Checked;
  RCDSaver->optSpaceAfterOptions = KeySpaceOpt->Checked;
  RCDSaver->optNewLineAfterOptions = KeyNLOpt->Checked;
  RCDSaver->optSpaceAfterComma = KeySpaceComma->Checked;
  RCDSaver->optNoSubNoNewLine = KeyNoSubNoNL->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyEditClick(TObject *Sender)
{
  KeyUndo->Enabled = Core && Core->Deleted;
  
  KeyCut->Enabled = Core && Core->Select;
  KeyCopy->Enabled = Core && Core->Select;
  KeyPaste->Enabled = Core && Core->Select && Clipboard()->HasFormat(CF_TEXT);
  KeyDelete->Enabled = Core && Core->Select;

  KeyOrigin->Visible = Core && Core->Select;

  KeyCut2->Visible = Core && Core->Select;
  KeyCopy2->Visible = Core && Core->Select;
  KeyPaste2->Visible = Core && Core->Select;
  KeyPaste2->Enabled = Clipboard()->HasFormat(CF_TEXT);
  KeyDelete2->Visible = Core && Core->Select;

  KeyConvertCowlS->Visible = Core && Core->Select;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyUndoClick(TObject *Sender)
{
  if (!Core || !Core->Deleted) return;
  Core->Deleted->Parent->AddSubChip(Core->Deleted);
  Core->Deleted = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyCutClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;
  if (Core->Select == Core)
  {
    Application->MessageBox("Can't cut core", "Cut", MB_ICONWARNING | MB_OK);
    return;
  }
  KeyCopyClick(Sender);
  KeyDeleteClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyCopyClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;
  if (Core->Select == Core)
  {
    Application->MessageBox("Can't copy core", "Copy", MB_ICONWARNING | MB_OK);
    return;
  }
  Clipboard()->AsText = RCDSaver->ChipToString("", Core->Select);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyPasteClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;
  if (!Clipboard()->HasFormat(CF_TEXT))
    return;
  TRigidChip *chip = RCDLoader->StringToChip(Clipboard()->AsText);
  if (chip == NULL)
  {
    Application->MessageBox(("Invalid clipboard text\n\n" + RCDLoader->ErrorMessage).c_str(), "Paste", MB_ICONERROR | MB_OK);
    return;
  }
  if (Direction != rdCore)
    chip->Direction = Direction;
  Core->Select->AddSubChip(chip);
  SelectionChange(Core->Select);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyDeleteClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;
  if (Core->Select == Core)
  {
    Application->MessageBox("Can't delete core", "Delete", MB_ICONWARNING | MB_OK);
    return;
  }
  if (Core->Deleted)
    delete Core->Deleted;
  Core->Deleted = Core->Select;
  Core->Select->Parent->DelSubChip(Core->Select);
  SelectionChange(Core->Select->Parent);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOriginClick(TObject *Sender)
{
  if (!Core) return;
  if (Core->Select)
    Origin = Core->Select;
  else
    Origin = Core;
  Display();
}
//---------------------------------------------------------------------------
bool ConvertCowl(TRigidChip *chip)
{
  if (chip->GetType() == ctCowl)
    return true;

  bool haschip = false;
  for (int i = 0; i < chip->SubChipsCount; i ++)
  {
    if (chip->SubChips[i]->GetType() != ctCowl)
    {
      if (ConvertCowl(chip->SubChips[i]))
        i --;
      else
        haschip = true;
    }
  }
  if (haschip)
    return false;

  if (chip->GetType() != ctChip
   && chip->GetType() != ctFrame
   && chip->GetType() != ctWeight
  )
    return false;

  TRigidChip *cowl = new TRigidChipCowl;
  chip->Parent->AddSubChip(cowl);
  cowl->Direction = chip->Direction;
  cowl->SetOptions(chip->Options->Text);
  for (int i = 0; i < chip->SubChipsCount; i ++)
    cowl->AddSubChip(chip->SubChips[i]);
  for (int i = chip->SubChipsCount-1; i >= 0; i --)
    chip->DelSubChip(i);

  if (chip->GetType() == ctChip || chip->GetType() == ctWeight)
    cowl->Options->Values["option"] = "0";
  else if (chip->GetType() == ctFrame)
    cowl->Options->Values["option"] = "1";

  delete chip;
  return true;
}
void __fastcall TForm1::KeyConvertCowlClick(TObject *Sender)
{
  if (!Core) return;

  SelectionChange(NULL);
  ConvertCowl(Core);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyConvertCowlSClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;

  ConvertCowl(Core->Select);
  SelectionChange(NULL);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListNorthContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
  if (Sender == ListNorth)
    Direction = rdNorth;
  else if (Sender == ListEast)
    Direction = rdEast;
  else if (Sender == ListWest)
    Direction = rdWest;
  else if (Sender == ListSouth)
    Direction = rdSouth;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PopupMenuAddPopup(TObject *Sender)
{
  bool sel = Core && Core->Select;
  bool cowl = sel && Core->Select->GetType() == ctCowl;
  KeyAddChip->Visible = sel && !cowl;
  KeyAddFrame->Visible = sel && !cowl;
  KeyAddWeight->Visible = sel && !cowl;
  KeyAddWheel->Visible = sel && !cowl;
  KeyAddRLW->Visible = sel && !cowl;
  KeyAddJet->Visible = sel && !cowl;
  KeyAddRudder->Visible = sel && !cowl;
  KeyAddRudderF->Visible = sel && !cowl;
  KeyAddTrim->Visible = sel && !cowl;
  KeyAddTrimF->Visible = sel && !cowl;
  KeyAddArm->Visible = sel && !cowl;
  KeyAddCowl->Visible = sel;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyAddChipClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;

  AnsiString cap = ((TMenuItem*)Sender)->Caption;
  if (Core->Select->GetType() == ctCowl && cap != "Cowl")
  {
    Application->MessageBox(("Can't join " + cap + " to cowl").c_str(), "cowl", MB_ICONERROR | MB_OK);
    return;
  }

  cap = "N:" + cap + "(){}";
  TRigidChip *chip = RCDLoader->StringToChip(cap);
  if (chip)
  {
    if (Direction != rdCore)
      chip->Direction = Direction;
    Core->Select->AddSubChip(chip);
  }
  SelectionChange(Core->Select);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonParentClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  SelectionChange(Core->Select->Parent);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListNorthClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (((TListBox*)Sender)->ItemIndex == -1) return;
  SelectionChange((TRigidChip*)((TListBox*)Sender)->Items->Objects[((TListBox*)Sender)->ItemIndex]);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PageControl1Change(TObject *Sender)
{
  if (!Core) return;
  if (PageControl1->ActivePage == TabVal)
  {
    ListVariables->Items->Clear();
    for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
      ListVariables->Items->AddObject(i->first, (TObject*)i->second);
  }
  else if (PageControl1->ActivePage == TabKey)
  {
    ComboKeyVarName->Items->Clear();
    for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
      ComboKeyVarName->Items->AddObject(i->first, (TObject*)i->second);
    ComboKeyNoChange(Sender);
  }
  else if (PageControl1->ActivePage == TabScript)
  {
    if (Core->Script->ScriptText != MemoScript->Text)
      MemoScript->Text = Core->Script->ScriptText;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListVariablesClick(TObject *Sender)
{
  if (ListVariables->ItemIndex == -1) return;
  TRigidChipsVariable *var = (TRigidChipsVariable*)ListVariables->Items->Objects[ListVariables->ItemIndex];
  EditVarName->Text = ListVariables->Items->Strings[ListVariables->ItemIndex];
  EditVarDefault->Text = var->FlagColor ? "#"+IntToHex((int)var->Default, 6) : FloatToStr(var->Default);
  EditVarMin->Text = FloatToStr(var->Min);
  EditVarMax->Text = var->FlagMax ? FloatToStr(var->Max) : AnsiString();
  EditVarStep->Text = var->FlagStep ? FloatToStr(var->Step) : AnsiString();
  CheckVarDisp->Checked = var->Disp;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonVarDelClick(TObject *Sender)
{
  if (!Core) return;
  if (ListVariables->ItemIndex == -1) return;
  Core->Variables.erase(ListVariables->Items->Strings[ListVariables->ItemIndex]);
  Modify = true;
  PageControl1Change(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonVarOkClick(TObject *Sender)
{
  if (!Core) return;
  if (EditVarName->Text == "")
  {
    Application->MessageBox("Input variable name", "variable", MB_ICONINFORMATION | MB_OK);
    return;
  }

  EditVarName->Text = EditVarName->Text.UpperCase();
  TRigidChipsVariableMap::iterator i = Core->Variables.find(EditVarName->Text);
  TRigidChipsVariable *var;
  if (i != Core->Variables.end())
    var = i->second;
  else
  {
    var = new TRigidChipsVariable(Core, "");
    Core->Variables[EditVarName->Text] = var;
  }
  var->FlagColor = EditVarDefault->Text.SubString(1, 1) == "#";
  var->Default = 0;
  Core->StrToDouble(EditVarDefault->Text, &var->Default);
  var->Min = StrToFloatDef(EditVarMin->Text, 0);
  var->FlagMax = EditVarMax->Text != "";
  var->Max = StrToFloatDef(EditVarMax->Text, 0);
  var->FlagStep = EditVarStep->Text != "";
  var->Step = StrToFloatDef(EditVarStep->Text, 0);
  var->Disp = CheckVarDisp->Checked;

  Modify = true;
  PageControl1Change(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonVarColorClick(TObject *Sender)
{
  if (!Core) return;

  double d = 0xFFFFFF;
  Core->StrToDouble(EditVarDefault->Text, &d);
  int c = d;
  c = ((c&0xFF)<<16) | (c&0xFF00) | ((c&0xFF0000)>>16);
  ColorDialog->CustomColors->Text = "ColorA=" + IntToHex((int)c, 6);
  ColorDialog->Color = (TColor)c;
  if (ColorDialog->Execute())
  {
    c = ColorToRGB(ColorDialog->Color);
    c = ((c&0xFF)<<16) | (c&0xFF00) | ((c&0xFF0000)>>16);
    AnsiString s = "#"+IntToHex(c, 6);
    EditVarDefault->Text = s;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ComboKeyNoChange(TObject *Sender)
{
  if (!Core) return;
  ComboKeyNo->Text = IntToStr(ComboKeyNo->Text.ToIntDef(0));
  ListKeyVar->Items->Clear();
  for (int i = 0; i < Core->KeyList->Count; i ++)
  {
    if (Core->KeyList->Strings[i] == ComboKeyNo->Text)
      ListKeyVar->Items->AddObject(Core->Keys[i]->Variable, (TObject*)Core->Keys[i]);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListKeyVarClick(TObject *Sender)
{
  if (ListKeyVar->ItemIndex == -1) return;
  TRigidChipsKey *key = (TRigidChipsKey*)ListKeyVar->Items->Objects[ListKeyVar->ItemIndex];
  ComboKeyVarName->Text = key->Variable;
  EditKeyStep->Text = FloatToStr(key->Step);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonKeyDelClick(TObject *Sender)
{
  if (!Core) return;
  if (ListKeyVar->ItemIndex == -1) return;
  TRigidChipsKey *key = (TRigidChipsKey*)ListKeyVar->Items->Objects[ListKeyVar->ItemIndex];
  int i = Core->KeyList->IndexOfObject((TObject*)key);
  if (i != -1)
    Core->KeyList->Delete(i);
  Modify = true;
  PageControl1Change(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonKeyOkClick(TObject *Sender)
{
  if (!Core) return;
  if (ComboKeyVarName->Text == "")
  {
    Application->MessageBox("Input variable name", "key", MB_ICONINFORMATION | MB_OK);
    return;
  }

  ComboKeyNo->Text = IntToStr(ComboKeyNo->Text.ToIntDef(0));
  bool found = false;
  for (int i = 0; i < Core->KeyList->Count; i ++)
  {
    if (Core->KeyList->Strings[i] == ComboKeyNo->Text
     && Core->Keys[i]->Variable.UpperCase() == ComboKeyVarName->Text.UpperCase())
    {
      Core->Keys[i]->Step = StrToFloatDef(EditKeyStep->Text, 0);
      found = true;
    }
  }
  
  if (!found)
  {
    TRigidChipsKey *key = new TRigidChipsKey(Core, ComboKeyVarName->Text, "step="+EditKeyStep->Text);
    Core->AddKey(ComboKeyNo->Text, key);
  }

  Modify = true;
  PageControl1Change(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonScriptResetClick(TObject *Sender)
{
  if (!Core) return;
  if (Core->Script->ScriptText != MemoScript->Text)
  {
    Core->Script->ScriptText = MemoScript->Text;
    Modify = true;
  }
  for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
    i->second->Value = i->second->Default;
  ScriptOut->Clear();
  RunScript = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonScriptStopClick(TObject *Sender)
{
  RunScript = false;
}
//---------------------------------------------------------------------------

