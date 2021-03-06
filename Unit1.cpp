//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RigidChip.h"
#include "RCDLoader.h"
#include "RCDSaver.h"
#include "RCScript.h"
#include <IniFiles.hpp>
#include <Clipbrd.hpp>
#include <stack>
#include "RCLua.h"
#include "UnitOptionSave.h"
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

    TRigidChipCore *core = RCDLoader->LoadFromFile(filename);
    if (core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
      return;
    }
    if (Core)
    {
      TRigidChip *c = Core;
      Origin = Core = NULL;
      delete c;
    }
    Origin = Core = core;

    FileName = filename;
    Modify = false;
    PaintPanelMouseDown(NULL, mbMiddle, TShiftState(), 0, 0);
    ScriptOut->Clear();
    LuaOut->Clear();
    RunScript = false;
    RunLua = false;
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
  PageControlRight->ActivePage = TabVal;
  PageControlRight->ActivePage = TabBody;

  RCDLoader = new TRCDLoader;
  RCDSaver = new TRCDSaver;
  RCLua = new TRCLua;

  PaintPanel = new TPaintPanel(this);
  PaintPanel->Parent = this;
  PaintPanel->Align = alClient;
  //PaintPanel->Left = PaintPanelDummy->Left;
  //PaintPanel->Top = PaintPanelDummy->Top;
  //PaintPanel->Width = PaintPanelDummy->Width;
  //PaintPanel->Height = PaintPanelDummy->Height;
  //PaintPanel->Anchors = TAnchors() << akLeft << akTop << akRight << akBottom; //PaintPanelDummy->Anchors;
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
    Application->Terminate();
    return;
  }
  if (SetPixelFormat(ghDC, pixelformat, &pfd) == FALSE)
  {
    Application->MessageBox("SetPixelFormat failed", "Error", MB_ICONERROR | MB_OK);
    Application->Terminate();
    return;
  }

  ghRC = wglCreateContext(ghDC);
  wglMakeCurrent(ghDC, ghRC);

  // InitilizeGL
  glClearColor(0, 0, 0, 0);
  glViewport(0, 0, PaintPanel->Width, PaintPanel->Height);

  KeyDefViewInitilizeClick(Sender);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-2, 2, -2, 2, 10, 1000);
  //glOrtho(-2, 2, -2, 2, 10, 1000);
  gluLookAt(0, 0, 40, 0, 0, 0, 0, 1, 0);

  glEnable(GL_DEPTH_TEST);

  // Lighting
  GLfloat col0[] = {0, 0, 0, 1};
  GLfloat col1[] = {1, 1, 1, 1};
  glEnable(GL_LIGHTING);
  glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, col1);
  // light0
  GLfloat pos0[] = {0, 0, 1000, 1};
  glLightfv(GL_LIGHT0, GL_AMBIENT, col1);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, col1);
  glLightfv(GL_LIGHT0, GL_SPECULAR, col0);
  glLightfv(GL_LIGHT0, GL_POSITION, pos0);
  glEnable(GL_LIGHT0);
  // diffuse2
  GLfloat pos1[] = {0, 0, -1000, 1};
  glLightfv(GL_LIGHT1, GL_AMBIENT, col0);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, col1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, col0);
  glLightfv(GL_LIGHT1, GL_POSITION, pos1);
  glEnable(GL_LIGHT1);

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
  WM_RIGIDCHIP_LOAD = RegisterWindowMessage("WM_RIGIDCHIP_LOAD");

  TIniFile *ini = NULL;
  try
  {
    ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
    int bgc = ini->ReadInteger("RCD", "BGColor", 0);
    if (bgc)
      glClearColor(((bgc >> 16) & 0xFF) / 255.0, ((bgc >> 8) & 0xFF) / 255.0, (bgc & 0xFF) / 255.0, 0);
    Width = ini->ReadInteger("RCD", "Width", Width);
    Height = ini->ReadInteger("RCD", "Height", Height);
    Left = ini->ReadInteger("RCD", "Left", (Screen->Width - Width) / 2);
    Top = ini->ReadInteger("RCD", "Top", (Screen->Height - Height) / 2);
    PanelRight->Width = ini->ReadInteger("RCD", "RightWidth", PanelRight->Width);
    KeySelectAdd->Checked = ini->ReadBool("Option", "SelectAdd", KeySelectAdd->Checked);
    KeyShowVoidOptions->Checked = ini->ReadBool("Option", "ShowVoid", KeyShowVoidOptions->Checked);
    KeyShowCGravity->Checked = ini->ReadBool("Option", "ShowCGravity", KeyShowCGravity->Checked);
    KeyHideCowl->Checked = ini->ReadBool("Option", "HideCowl", KeyHideCowl->Checked);
    KeyHideGhost->Checked = ini->ReadBool("Option", "HideGhost", KeyHideGhost->Checked);
    KeyHideBalloon->Checked = ini->ReadBool("Option", "HideBalloon", KeyHideBalloon->Checked);
    def_cx = ini->ReadFloat("Option", "CameraX", def_cx);
    def_cy = ini->ReadFloat("Option", "CameraY", def_cy);
    def_cz = ini->ReadFloat("Option", "CameraZ", def_cz);
    def_caH = ini->ReadFloat("Option", "CameraH", def_caH);
    def_caV = ini->ReadFloat("Option", "CameraV", def_caV);
    RCDSaver->optSpaceAfterBlockType = ini->ReadBool("OptionSave", "SpaceBlock", RCDSaver->optSpaceAfterBlockType);
    RCDSaver->optNewLineAfterBlockType = ini->ReadBool("OptionSave", "NewLineBlock", RCDSaver->optNewLineAfterBlockType);
    RCDSaver->optSpaceAfterOptions = ini->ReadBool("OptionSave", "SpaceOption", RCDSaver->optSpaceAfterOptions);
    RCDSaver->optNewLineAfterOptions = ini->ReadBool("OptionSave", "NewLineOption", RCDSaver->optNewLineAfterOptions);
    RCDSaver->optSpaceAfterComma = ini->ReadBool("OptionSave", "SpaceComma", RCDSaver->optSpaceAfterComma);
    RCDSaver->optNoSubNoNewLine = ini->ReadBool("OptionSave", "NoSubNoNewLine", RCDSaver->optNoSubNoNewLine);
    RCDSaver->optTabSpaces = ini->ReadInteger("OptionSave", "TabSpaces", RCDSaver->optTabSpaces);
    delete ini;
  }
  catch (...)
  {
    if (ini)
      delete ini;
  }

  if (ParamCount() >= 1 && FileExists(ParamStr(1)))
  {
    Origin = Core = RCDLoader->LoadFromFile(ParamStr(1));
    if (Core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
    }
    else
    {
      FileName = ParamStr(1);
      Modify = false;
      PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
      ScriptOut->Clear();
      LuaOut->Clear();
      RunScript = false;
      RunLua = false;
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

  delete RCLua;
  delete RCDSaver;
  delete RCDLoader;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
  TIniFile *ini = NULL;
  try
  {
    ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
    ini->WriteInteger("RCD", "Left", Left);
    ini->WriteInteger("RCD", "Top", Top);
    ini->WriteInteger("RCD", "Width", Width);
    ini->WriteInteger("RCD", "Height", Height);
    ini->WriteInteger("RCD", "RightWidth", PanelRight->Width);
    ini->WriteBool("Option", "SelectAdd", KeySelectAdd->Checked);
    ini->WriteBool("Option", "ShowVoid", KeyShowVoidOptions->Checked);
    ini->WriteBool("Option", "ShowCGravity", KeyShowCGravity->Checked);
    ini->WriteBool("Option", "HideCowl", KeyHideCowl->Checked);
    ini->WriteBool("Option", "HideGhost", KeyHideGhost->Checked);
    ini->WriteBool("Option", "HideBalloon", KeyHideBalloon->Checked);
    ini->WriteFloat("Option", "CameraX", def_cx);
    ini->WriteFloat("Option", "CameraY", def_cy);
    ini->WriteFloat("Option", "CameraZ", def_cz);
    ini->WriteFloat("Option", "CameraH", def_caH);
    ini->WriteFloat("Option", "CameraV", def_caV);
    ini->WriteBool("OptionSave", "SpaceBlock", RCDSaver->optSpaceAfterBlockType);
    ini->WriteBool("OptionSave", "NewLineBlock", RCDSaver->optNewLineAfterBlockType);
    ini->WriteBool("OptionSave", "SpaceOption", RCDSaver->optSpaceAfterOptions);
    ini->WriteBool("OptionSave", "NewLineOption", RCDSaver->optNewLineAfterOptions);
    ini->WriteBool("OptionSave", "SpaceComma", RCDSaver->optSpaceAfterComma);
    ini->WriteBool("OptionSave", "NoSubNoNewLine", RCDSaver->optNoSubNoNewLine);
    ini->WriteInteger("OptionSave", "TabSpaces", RCDSaver->optTabSpaces);
    delete ini;
  }
  catch (...)
  {
    if (ini)
      delete ini;
  }
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
TTreeNode* TreeViewBodyAdd(TTreeNode *node, TRigidChip *chip, TRigidChip *select)
{
  TTreeNode *sel = NULL;
  AnsiString dir[] = {"", "N:", "E:", "S:", "W:"};
  node = Form1->TreeViewBody->Items->AddChildObject(node, dir[chip->Direction] + chip->GetTypeString() + "(" + chip->Options->CommaText + ")", chip);
  for (int i = 0; i < chip->SubChipsCount; i ++)
  {
    TTreeNode *s = TreeViewBodyAdd(node, chip->SubChips[i], select);
    if (s) sel = s;
  }
  if (chip == select)
    return node;
  return sel;
}
void __fastcall TForm1::SelectionChange(TRigidChip *chip)
{
  ListNorth->Items->Clear();
  ListEast->Items->Clear();
  ListWest->Items->Clear();
  ListSouth->Items->Clear();
  Direction = rdCore;

  if (PageControlBody->ActivePage == TabCowlEffect)
    PageControlBody->ActivePage = TabOptions;

  if (!Core) return;
  Core->Select = NULL;

  TreeViewBody->Items->BeginUpdate();
  TreeViewBody->Items->Clear();
  TTreeNode *node = TreeViewBodyAdd(NULL, Core, chip);
  //TreeViewBody->FullExpand();
  TreeViewBody->OnChange = NULL;
  TreeViewBody->Selected = node;
  //TreeViewBody->TopItem = node;
  if (node)
  {
    node->Expand(false);
    node->MakeVisible();
  }
  else
  {
    TreeViewBody->Items->Item[0]->Expand(true);
    TreeViewBody->Items->Item[0]->MakeVisible();
  }
  TreeViewBody->OnChange = TreeViewBodyChange;
  TreeViewBody->Items->EndUpdate();

  if (chip)
  {
    // e`bvðt¤ÌXgÉÇÁ
    if (chip->Parent)
    {
      TListBox *list = NULL;
      switch (chip->Direction)
      {
        case rdNorth: list = ListSouth; break;
        case rdEast: list = ListWest; break;
        case rdWest: list = ListEast; break;
        case rdSouth: list = ListNorth; break;
      }
      if (list)
        list->Items->AddObject("p:" + chip->Parent->GetTypeString() + "(" + chip->Parent->Options->CommaText + ")", (TObject*)chip->Parent);
    }

    for (int i = 0; i < chip->SubChipsCount; i ++)
    {
      TListBox *list = NULL;
      switch (chip->SubChips[i]->Direction)
      {
        case rdNorth: list = ListNorth; break;
        case rdEast: list = ListEast; break;
        case rdWest: list = ListWest; break;
        case rdSouth: list = ListSouth; break;
      }
      if (list)
        list->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString() + "(" + chip->SubChips[i]->Options->CommaText + ")", (TObject*)chip->SubChips[i]);
    }
    //OptionsEditor->Strings->Assign(chip->Options);
    OptionsEditor->Strings->Clear();
    for (int i = 0; i < chip->Options->Count; i ++)
    {
      try {
        OptionsEditor->Strings->Add(chip->Options->Strings[i]);
      } catch (...) { }
    }
    if (KeyShowVoidOptions->Checked)
    {
      bool core = chip->GetType() == ctCore;
      bool frame = chip->GetType() == ctFrame
             || chip->GetType() == ctRudderF
             || chip->GetType() == ctTrimF;
      bool weight = chip->GetType() == ctWeight;
      bool wheel = chip->GetType() == ctWheel
             || chip->GetType() == ctRLW;
      bool jet = chip->GetType() == ctJet;
      bool arm = chip->GetType() == ctArm;
      bool cowl = chip->GetType() == ctCowl;

      int i, j = OptionsEditor->Strings->Count;
      #define VOIDOPT0(name) {\
        i = j; \
        if ((j = OptionsEditor->Strings->IndexOfName(name)) == -1) \
          j = i; \
      }
      #define VOIDOPT(name) {\
        i = j; \
        if ((j = OptionsEditor->Strings->IndexOfName(name)) == -1) \
          OptionsEditor->Strings->Insert(j = i, name "="); \
      }
      VOIDOPT0("user2");
      VOIDOPT0("user1");
      VOIDOPT("name");
      if (!core) VOIDOPT("damper");
      if (!core) VOIDOPT("spring");
      if (wheel || jet || cowl) VOIDOPT("effect");
      VOIDOPT("color");
      if (frame || weight || wheel || jet || arm || cowl) VOIDOPT("option");
      if (wheel) VOIDOPT("brake");
      if (wheel || jet || arm) VOIDOPT("power");
      if (!core) VOIDOPT("angle");
    }
    ComboDirection->ItemIndex = chip->Direction - 1;
    ComboType->ItemIndex = ComboType->Items->IndexOf(chip->GetTypeString());
    MemoChip->Text = chip->MemoChip;

    Core->Select = chip;
    bool tmp = true;
    OptionsEditorSelectCell(NULL, OptionsEditor->Col, OptionsEditor->Row, tmp);
  }
  else
  {
    ComboDirection->ItemIndex = -1;
    ComboType->ItemIndex = -1;
    OptionsEditor->Strings->Clear();
    MemoChip->Clear();
  }

  ButtonParent->Enabled = Core->Select && Core->Select->Parent;
  ButtonPlus->Enabled = Core->Select && Core->Select->Parent;
  ButtonMinus->Enabled = Core->Select && Core->Select->Parent;
  ComboDirection->Enabled = Core->Select && Core->Select != Core;
  ComboType->Enabled = Core->Select && Core->Select != Core;
  OptionsEditor->Enabled = Core->Select;
  MemoChip->Enabled = Core->Select;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Display()
{
  if (!Core)
    return;

  Core->ShowCowl = !KeyHideCowl->Checked;
  Core->ShowGhost = !KeyHideGhost->Checked;
  Core->ShowBalloon = !KeyHideBalloon->Checked;

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
    glTranslatef(-camera_x, -camera_y, -camera_z);
    glRotatef(camera_angleV, 1, 0, 0);
    glRotatef(camera_angleH, 0, 0, 1);

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
  glTranslatef(-camera_x, -camera_y, -camera_z);
  glRotatef(camera_angleV, 1, 0, 0);
  glRotatef(camera_angleH, 0, 0, 1);

  Core->CGravity[0] = Core->CGravity[1] = Core->CGravity[2] = Core->Weight = 0;
  Origin->Draw();
  Core->CGravity[0] /= Core->Weight;
  Core->CGravity[1] /= Core->Weight;
  Core->CGravity[2] /= Core->Weight;

  if (KeyShowCGravity->Checked)
  { // dSvZ`æ
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(Core->CGravity[0], Core->CGravity[1], Core->CGravity[2]);
    glRotatef(camera_angleV, 1, 0, 0);
    glRotatef(camera_angleH, 0, 0, 1);
    const float zero[4] = {0, 0, 0, 1};
    const float ambient[4] = {0.5, 0.5, 0.5, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    glBegin(GL_LINES);
    glVertex3f(-8,  0,  0);
    glVertex3f( 8,  0,  0);
    glVertex3f( 0, -8,  0);
    glVertex3f( 0,  8,  0);
    glVertex3f( 0,  0, -8);
    glVertex3f( 0,  0,  8);
    glEnd();
    glPopMatrix();
  }
  
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
    mouse_caH = camera_angleH;
    mouse_caV = camera_angleV;
  }
  else if (mouse_right && Button == mbLeft)
  {
    mouse_left = true;
    mouse_cx = camera_x;
    mouse_cy = camera_y;
  }
  else if (Button == mbMiddle)
  {
    if (PageControlRight->ActivePage != TabPanekit)
    {
      PageControlRight->ActivePage = TabVal;
      PageControlRight->ActivePage = TabBody;
      PageControlRightChange(Sender);
    }
    if (Core)
      SelectionChange(NULL);
    camera_x = def_cx;
    camera_y = def_cy;
    camera_z = def_cz;
    camera_angleH = def_caH;
    camera_angleV = def_caV;
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
    camera_x = mouse_cx - (X - mouse_x) / 25.0;
    camera_y = mouse_cy + (Y - mouse_y) / 25.0;
    Display();
  }
  else if (mouse_right)
  {
    camera_angleH = mouse_caH + (X - mouse_x);
    camera_angleV = mouse_caV + (Y - mouse_y);
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
  while (camera_angleV < 0) camera_angleV += 360;
  while (camera_angleV > 360) camera_angleV -= 360;
  while (camera_angleH < 0) camera_angleH += 360;
  while (camera_angleH > 360) camera_angleH -= 360;
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
    {
      if (Keys[i])
        Core->ActKey(IntToStr(i));
      KeyDown[i] = Keys[i] != KeysOld[i] && Keys[i];
      KeyUp[i] = Keys[i] != KeysOld[i] && !Keys[i];
      KeysOld[i] = Keys[i];
    }

    if (RunLua && RCLua->ScriptText != "")
    {
      LuaOut->Lines->BeginUpdate();
      try {
        for (int i = 0; i < LuaOut->Lines->Count; i ++)
          LuaOut->Lines->Strings[i] = "";
        RCLua->ErrorMessage = "";
        if (!RCLua->Run(Core))
          throw Exception(RCLua->ErrorMessage);
      } catch (Exception &e) {
        RunLua = false;
        LuaOut->Lines->Clear();
        LuaOut->Lines->Add("LuaError");
        LuaOut->Lines->Add(e.Message);
      }
      LuaOut->Lines->EndUpdate();
    }
    else if (RunScript && Core->Script->ScriptText != "")
    {
      ScriptOut->Lines->BeginUpdate();
      try {
        for (int i = 0; i < ScriptOut->Lines->Count; i ++)
          ScriptOut->Lines->Strings[i] = "";
        Core->Script->Run();
      } catch (Exception &e) {
        RunScript = false;
        ScriptOut->Lines->Clear();
        ScriptOut->Lines->Add("ScriptError: Near " + IntToStr(Core->Script->ProgramCounter));
        ScriptOut->Lines->Add(e.Message);
        MemoScript->SelStart = Core->Script->ProgramCounter;
        MemoScript->SelLength = 1;
      }
      ScriptOut->Lines->EndUpdate();
    }

    Core->StepVariable();
    Core->Act();

    Display();
    Yield();
  }
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckAnimeClick(TObject *Sender)
{
  Timer1->Enabled = CheckAnime->Checked;
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditKeyTestKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  EditKeyTest->SelLength = 0;
  
  if (Shift.Contains(ssCtrl))
  {
    switch (Key)
    {
      case 'Z': KeyUndoClick(Sender); return;
      case 'X': KeyCutClick(Sender); break;
      case 'C': KeyCopyClick(Sender); break;
      case 'V': KeyPasteClick(Sender); break;
      case VK_DELETE: ButtonMinusClick(Sender); break;
    }
    return;
  }
  switch (Key)
  {
    case VK_ESCAPE: SelectionChange(NULL); return;
    case VK_DELETE: KeyDeleteClick(Sender); return;
  }

  CheckAnime->Checked = true;
  Timer1->Enabled = true;
  const bool f = true;
  switch (Key)
  {
    case VK_UP: Keys[0] = f; break;
    case VK_DOWN: Keys[1] = f; break;
    case VK_LEFT: Keys[2] = f; break;
    case VK_RIGHT: Keys[3] = f; break;
    case 'Z': case 'z': Keys[4] = f; break;
    case 'X': case 'x': Keys[5] = f; break;
    case 'C': case 'c': Keys[6] = f; break;
    case 'A': case 'a': Keys[7] = f; break;
    case 'S': case 's': Keys[8] = f; break;
    case 'D': case 'd': Keys[9] = f; break;
    case 'V': case 'v': Keys[10] = f; break;
    case 'B': case 'b': Keys[11] = f; break;
    case 'F': case 'f': Keys[12] = f; break;
    case 'G': case 'g': Keys[13] = f; break;
    case 'Q': case 'q': Keys[14] = f; break;
    case 'W': case 'w': Keys[15] = f; break;
    case 'E': case 'e': Keys[16] = f; break;

    case 'K': // JªãûüÖñ]
      camera_angleV += 15;
      while (camera_angleV > 360) camera_angleV -= 360;
      Display();
      break;
    case 'L': // JªºûüÖñ]
      camera_angleV -= 15;
      while (camera_angleV < 0) camera_angleV += 360;
      Display();
      break;
    case 0xBC: // KEY_OEM_COMMA Jª¶ûüÖñ]
      camera_angleH += 15;
      while (camera_angleH > 360) camera_angleH -= 360;
      Display();
      break;
    case 0xBE: // KEY_OEM_PERIOD JªEûüÖñ]
      camera_angleH -= 15;
      while (camera_angleH < 0) camera_angleH += 360;
      Display();
      break;
    case 'I': case 'i': // gå
      if (camera_z > -25)
        camera_z -= camera_z > 0 ? camera_z * 0.5 + 1 : 5;
      Display();
      break;
    case 'O': case 'o': // k¬
      if (camera_z < 250)
        camera_z += camera_z > 0 ? camera_z * 0.5 + 1 : 5;
      Display();
      break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditKeyTestKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if (Shift.Contains(ssCtrl))
    return;

  const bool f = false;
  switch (Key)
  {
    case VK_UP: Keys[0] = f; break;
    case VK_DOWN: Keys[1] = f; break;
    case VK_LEFT: Keys[2] = f; break;
    case VK_RIGHT: Keys[3] = f; break;
    case 'Z': case 'z': Keys[4] = f; break;
    case 'X': case 'x': Keys[5] = f; break;
    case 'C': case 'c': Keys[6] = f; break;
    case 'A': case 'a': Keys[7] = f; break;
    case 'S': case 's': Keys[8] = f; break;
    case 'D': case 'd': Keys[9] = f; break;
    case 'V': case 'v': Keys[10] = f; break;
    case 'B': case 'b': Keys[11] = f; break;
    case 'F': case 'f': Keys[12] = f; break;
    case 'G': case 'g': Keys[13] = f; break;
    case 'Q': case 'q': Keys[14] = f; break;
    case 'W': case 'w': Keys[15] = f; break;
    case 'E': case 'e': Keys[16] = f; break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerReloadTimer(TObject *Sender)
{
  if (FileName == "" || Modify)
  {
    TimerReload->Enabled = false;
    KeyAutoReload->Checked = false;
    return;
  }

  int age = FileAge(FileName);
  if (age == TimerReload->Tag)
    return;
  TimerReload->Tag = age;
  
  TRigidChipCore *corenew = RCDLoader->LoadFromFile(FileName);
  if (corenew)
  {
    TRigidChipCore *coreback = Core;
    Origin = Core = corenew;
    Display();
    PageControlRightChange(Sender);

    if (coreback)
      delete coreback;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::HideClick(TObject *Sender)
{
  ((TMenuItem*)Sender)->Checked = !((TMenuItem*)Sender)->Checked;
  Display();
  EditKeyTest->SetFocus();
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

  // Tu`bvÉCowlÈOðÂ©`FbN
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
  TRigidChip *chip = RCDLoader->StringToChip(cap, Core->Select->Parent);
  if (chip)
  {
    chip->Direction = Core->Select->Direction;
    chip->SetOptions(Core->Select->Options->Text);
    for (int i = 0; i < Core->Select->SubChipsCount; i ++)
      chip->AddSubChip(Core->Select->SubChips[i]);
    for (int i = Core->Select->SubChipsCount-1; i >= 0; i --)
      Core->Select->DelSubChip(i);

    delete Core->Select;
    SelectionChange(chip);

    Modify = true;
  }
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
      else if (Core->Select->GetType() == ctWeight)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "1\n2\n3\n4\n5\n6\n7\n8";
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
      else if (Core->Select->GetType() == ctCowl)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esEllipsis;
      }
    }
    else if (key == "spring" || key == "damper")
    {
      OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
      OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n0.5\n1";
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OptionsEditorEditButtonClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;

  if (OptionsEditor->Keys[OptionsEditor->Row].LowerCase() == "effect")
  {
    ComboCowlEffect->Text = OptionsEditor->Values["effect"];
    ComboCowlEffectChange(ComboCowlEffect);
    PageControlBody->ActivePage = TabCowlEffect;
    return;
  }

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
void __fastcall TForm1::ComboCowlEffectChange(TObject *Sender)
{
  int effect;
  if (Sender == ComboCowlEffect)
  {
    if (ComboCowlEffect->Text != "" && ComboCowlEffect->Text[1] == '#')
      effect = StrToIntDef("0x"+ComboCowlEffect->Text.SubString(2, ComboCowlEffect->Text.Length()),0x00FB);
    else
      effect = ComboCowlEffect->Text.ToIntDef(0x00FB);

    ComboCowlTrans->ItemIndex     = (effect >> 12) & 0xF;
    ComboCowlEmissive->ItemIndex  = (effect >>  8) & 0xF;
    ComboCowlShininess->ItemIndex = (effect >>  4) & 0xF;
    ComboCowlSpecular->ItemIndex  =  effect        & 0xF;
  }
  else
  {
    effect = 0;
    effect <<= 4; effect |= ComboCowlTrans->ItemIndex;
    effect <<= 4; effect |= ComboCowlEmissive->ItemIndex;
    effect <<= 4; effect |= ComboCowlShininess->ItemIndex;
    effect <<= 4; effect |= ComboCowlSpecular->ItemIndex;
    ComboCowlEffect->Text = "#" + IntToHex(effect, 4);
  }
  OptionsEditor->Values["effect"] = ComboCowlEffect->Text;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MemoChipChange(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (Core->Select->MemoChip != MemoChip->Text)
  {
    Core->Select->MemoChip = MemoChip->Text;
    Modify = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MemoModelChange(TObject *Sender)
{
  if (!Core) return;
  if (Core->MemoModel != MemoModel->Text)
  {
    Core->MemoModel = MemoModel->Text;
    Modify = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PopupMenuOptionsPopup(TObject *Sender)
{
  if (!Core || !Core->Select) return;

  bool core = Core->Select->GetType() == ctCore;
  bool frame = Core->Select->GetType() == ctFrame
         || Core->Select->GetType() == ctRudderF
         || Core->Select->GetType() == ctTrimF;
  bool weight = Core->Select->GetType() == ctWeight;
  bool wheel = Core->Select->GetType() == ctWheel
         || Core->Select->GetType() == ctRLW;
  bool jet = Core->Select->GetType() == ctJet;
  bool arm = Core->Select->GetType() == ctArm;
  bool cowl = Core->Select->GetType() == ctCowl;
  KeyOptionsAngle->Visible = OptionsEditor->Strings->IndexOfName("angle") == -1 && !core;
  KeyOptionsPower->Visible = OptionsEditor->Strings->IndexOfName("power") == -1 && (wheel || jet || arm);
  KeyOptionsBrake->Visible = OptionsEditor->Strings->IndexOfName("brake") == -1 && wheel;
  KeyOptionsOption->Visible = OptionsEditor->Strings->IndexOfName("option") == -1 && (frame || weight || wheel || jet || arm || cowl);
  KeyOptionsColor->Visible = OptionsEditor->Strings->IndexOfName("color") == -1;
  KeyOptionsEffect->Visible = OptionsEditor->Strings->IndexOfName("effect") == -1 && (wheel || jet || cowl);
  KeyOptionsSpring->Visible = OptionsEditor->Strings->IndexOfName("spring") == -1 && !core;
  KeyOptionsDamper->Visible = OptionsEditor->Strings->IndexOfName("damper") == -1 && !core;
  KeyOptionsName->Visible = OptionsEditor->Strings->IndexOfName("name") == -1;
  KeyOptionsUser1->Visible = OptionsEditor->Strings->IndexOfName("user1") == -1;
  KeyOptionsUser2->Visible = OptionsEditor->Strings->IndexOfName("user2") == -1;

  KeyOptionUp->Enabled = OptionsEditor->Strings->Count && OptionsEditor->Row > 1;
  KeyOptionDown->Enabled = OptionsEditor->Row < OptionsEditor->Strings->Count;
  KeyOptionDelete->Enabled = OptionsEditor->Strings->Count;
  KeyOptionsSort->Enabled = OptionsEditor->Strings->Count;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptionsClick(TObject *Sender)
{
  AnsiString cap = ((TMenuItem*)Sender)->Caption;
  cap = StringReplace(cap, "&", "", TReplaceFlags());
  OptionsEditor->InsertRow(cap, "", true);
  bool tmp = true;
  OptionsEditorSelectCell(NULL, OptionsEditor->Col, OptionsEditor->Row, tmp);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptionUpClick(TObject *Sender)
{
  if (OptionsEditor->Strings->Count && OptionsEditor->Row > 1)
  {
    OptionsEditor->Strings->Move(OptionsEditor->Row-1, OptionsEditor->Row-2);
    OptionsEditor->Row = OptionsEditor->Row - 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptionDownClick(TObject *Sender)
{
  if (OptionsEditor->Row < OptionsEditor->Strings->Count)
  {
    OptionsEditor->Strings->Move(OptionsEditor->Row-1, OptionsEditor->Row);
    OptionsEditor->Row = OptionsEditor->Row + 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptionDeleteClick(TObject *Sender)
{
  if (OptionsEditor->Strings->Count)
    OptionsEditor->DeleteRow(OptionsEditor->Row);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptionsSortClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  TStringList *list = new TStringList;
  list->Assign(OptionsEditor->Strings);
  OptionsEditor->Strings->Clear();
  OptionsEditor->Strings->Values["angle"]  = list->Values["angle"];
  OptionsEditor->Strings->Values["power"]  = list->Values["power"];
  OptionsEditor->Strings->Values["brake"]  = list->Values["brake"];
  OptionsEditor->Strings->Values["option"] = list->Values["option"];
  OptionsEditor->Strings->Values["color"]  = list->Values["color"];
  OptionsEditor->Strings->Values["effect"] = list->Values["effect"];
  OptionsEditor->Strings->Values["spring"] = list->Values["spring"];
  OptionsEditor->Strings->Values["damper"] = list->Values["damper"];
  OptionsEditor->Strings->Values["name"]   = list->Values["name"];
  OptionsEditor->Strings->Values["user1"]  = list->Values["user1"];
  OptionsEditor->Strings->Values["user2"]  = list->Values["user2"];
  SelectionChange(Core->Select);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyAboutClick(TObject *Sender)
{
  ShowMessage(
        "RigidChips Designer\n"
        "Copyright(C) Yasu software\n"
        "http://www.yasu.nu/"
  );
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyFileClick(TObject *Sender)
{
  KeyLoadClipboard->Enabled = Clipboard()->HasFormat(CF_TEXT);
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
  ScriptOut->Clear();
  LuaOut->Clear();
  RunScript = false;
  RunLua = false;
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
    TRigidChipCore *core = RCDLoader->LoadFromFile(OpenDialog->FileName);
    if (core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
      return;
    }
    if (Core)
    {
      TRigidChip *c = Core;
      Origin = Core = NULL;
      delete c;
    }
    Origin = Core = core;

    FileName = OpenDialog->FileName;
    Modify = false;
    PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
    ScriptOut->Clear();
    LuaOut->Clear();
    RunScript = false;
    RunLua = false;
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

  if (!Core->FlagRCD)
  {
    if (Application->MessageBox(
        "±Ìft@CÉÍRigidChipsDesignerVOl`ª èÜ¹ñ\n"
        "Û¶ð·éÆRgÈÇÌîñª¸íêéÂ\«ª èÜ·\n"
        "ã«Û¶ð±sµÜ·©H", "x", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDNO)
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
    if (ExtractFileExt(FileName) == "")
      FileName = FileName + ".rcd";
    RCDSaver->Save(FileName, Core);
    Modify = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyLoadClipboardClick(TObject *Sender)
{
  if (!Clipboard()->HasFormat(CF_TEXT))
    return;

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
  
  TRigidChipCore *core = RCDLoader->LoadFromString(Clipboard()->AsText);
  if (core == NULL)
  {
    Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
    return;
  }
  if (Core)
  {
    TRigidChip *c = Core;
    Origin = Core = NULL;
    delete c;
  }
  Origin = Core = core;

  FileName = "";
  Modify = false;
  PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
  ScriptOut->Clear();
  LuaOut->Clear();
  RunScript = false;
  RunLua = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyImportClick(TObject *Sender)
{
  KeyNewClick(Sender);
  if (Modify) return;
  PageControlRight->ActivePage = TabPanekit;
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
    PostMessage(HWND_BROADCAST, WM_RIGIDCHIP_LOAD, UMSG_RCLOAD_START, 0);
    for (int i = 1; i <= FileName.Length(); i ++)
      PostMessage(HWND_BROADCAST, WM_RIGIDCHIP_LOAD, UMSG_RCLOAD_CHAR, FileName[i] & 0xFF);
    PostMessage(HWND_BROADCAST, WM_RIGIDCHIP_LOAD, UMSG_RCLOAD_END, 0);
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
void __fastcall TForm1::KeySelectAddClick(TObject *Sender)
{
  KeySelectAdd->Checked = !KeySelectAdd->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyShowVoidOptionsClick(TObject *Sender)
{
  KeyShowVoidOptions->Checked = !KeyShowVoidOptions->Checked;
  SelectionChange(Core->Select);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyShowCGravityClick(TObject *Sender)
{
  KeyShowCGravity->Checked = !KeyShowCGravity->Checked;
  Display();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyDefViewThisClick(TObject *Sender)
{
  def_cx = camera_x;
  def_cy = camera_y;
  def_cz = camera_z;
  def_caH = camera_angleH;
  def_caV = camera_angleV;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyDefViewInitilizeClick(TObject *Sender)
{
  def_cx = 0;
  def_cy = 0;
  def_cz = 0;
  def_caH = 0;
  def_caV = 0;
  PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptSaveClick(TObject *Sender)
{
  TFormOptionSave *form = new TFormOptionSave(this);
  form->ShowModal();
  delete form;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyEditClick(TObject *Sender)
{
  KeyUndo->Enabled = Core && Core->Deleted;
  
  KeyCut->Enabled = Core && Core->Select && Core->Select != Core;
  KeyCopy->Enabled = Core && Core->Select && Core->Select != Core;
  KeyPaste->Enabled = Core && Core->Select && Clipboard()->HasFormat(CF_TEXT);
  KeyDelete->Enabled = Core && Core->Select && Core->Select != Core;
  KeyDeleteOne->Enabled = Core && Core->Select && Core->Select->Parent;

  KeyOrigin->Visible = Core && Core->Select;

  KeyCut2->Enabled = KeyCut->Enabled;
  KeyCopy2->Enabled = KeyCopy->Enabled;
  KeyPaste2->Enabled = KeyPaste->Enabled;
  KeyDelete2->Enabled = KeyDelete->Enabled;
  KeyDeleteOne2->Enabled = KeyDeleteOne->Enabled;

  KeyConvertCowlS->Visible = Core && Core->Select;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyUndoClick(TObject *Sender)
{
  if (!Core || !Core->Deleted) return;
  Core->Deleted->Parent->AddSubChip(Core->Deleted);
  if (KeySelectAdd->Checked)
    SelectionChange(Core->Deleted);
  else
    SelectionChange(Core->Select);
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
  TRigidChip *chip = RCDLoader->StringToChip(Clipboard()->AsText, Core->Select);
  if (chip == NULL)
  {
    if (RCDLoader->ErrorMessage != "")
      Application->MessageBox(("Invalid clipboard text\n\n" + RCDLoader->ErrorMessage).c_str(), "Paste", MB_ICONERROR | MB_OK);
    return;
  }
  if (Sender == KeyPasteDirection && Direction != rdCore)
    ChangeDirection(chip, Direction);
  if (KeySelectAdd->Checked)
    SelectionChange(chip);
  else
    SelectionChange(Core->Select);
  Modify = true;
  Display();
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
  Display();
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

  //if (chip->GetType() == ctChip || chip->GetType() == ctWeight)
  //  cowl->Options->Values["option"] = "0";
  if (chip->GetType() == ctFrame)
    cowl->Options->Values["option"] = "1";

  delete chip;
  return true;
}
void __fastcall TForm1::KeyConvertCowlClick(TObject *Sender)
{
  if (!Core) return;

  SelectionChange(NULL);
  ConvertCowl(Core);
  EditKeyTest->SetFocus();
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
  Display();
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
  KeyPasteDirection->Visible = sel && Direction != rdCore;
  KeyPasteDirection->Enabled = Clipboard()->HasFormat(CF_TEXT);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyAddChipClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;

  AnsiString cap = ((TMenuItem*)Sender)->Caption;
  cap = StringReplace(cap, "&", "", TReplaceFlags());
  if (Core->Select->GetType() == ctCowl && cap != "Cowl")
  {
    Application->MessageBox(("Can't join " + cap + " to cowl").c_str(), "cowl", MB_ICONERROR | MB_OK);
    return;
  }

  cap = "N:" + cap + "(){}";
  TRigidChip *chip = RCDLoader->StringToChip(cap, Core->Select);
  if (chip)
  {
    if (Direction != rdCore)
      chip->Direction = Direction;
    Modify = true;
  }
  if (KeySelectAdd->Checked)
    SelectionChange(chip);
  else
    SelectionChange(Core->Select);
  EditKeyTest->SetFocus();
  Display();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonPlusClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (!Core->Select->Parent) return;

  bool copy = true;
  AnsiString t = Core->Select->GetTypeString();
  if (Core->Select->GetType() != ctChip
   && Core->Select->GetType() != ctWeight
   && Core->Select->GetType() != ctFrame
   && Core->Select->GetType() != ctCowl
  )
  {
    t = "Chip";
    copy = false;
  }

  t = "N:" + t + "(){}";
  TRigidChip *chip = RCDLoader->StringToChip(t, Core->Select->Parent);
  if (chip)
  {
    chip->Direction = Core->Select->Direction;
    if (copy)
    {
      chip->Options->Values["option"] = Core->Select->Options->Values["option"];
      chip->Options->Values["color"] = Core->Select->Options->Values["color"];
      chip->Options->Values["spring"] = Core->Select->Options->Values["spring"];
      chip->Options->Values["damper"] = Core->Select->Options->Values["damper"];
    }
    
    Core->Select->Parent->DelSubChip(Core->Select);
    chip->AddSubChip(Core->Select);

    SelectionChange(chip);
    Modify = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonMinusClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (!Core->Select->Parent) return;

  TRigidChip *sel = Core->Select->Parent;
  for (int i = 0; i < Core->Select->SubChipsCount; i ++)
    if (Core->Select->SubChips[i]->Direction == Core->Select->Direction)
    {
      sel = Core->Select->SubChips[i];
      break;
    }

  for (int i = 0; i < Core->Select->SubChipsCount; i ++)
    Core->Select->Parent->AddSubChip(Core->Select->SubChips[i]);
  for (int i = Core->Select->SubChipsCount-1; i >= 0; i --)
    Core->Select->DelSubChip(i);

  delete Core->Select;
  SelectionChange(sel);

  EditKeyTest->SetFocus();
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonParentClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  SelectionChange(Core->Select->Parent);
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListNorthClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (((TListBox*)Sender)->ItemIndex == -1) return;
  SelectionChange((TRigidChip*)((TListBox*)Sender)->Items->Objects[((TListBox*)Sender)->ItemIndex]);
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TreeViewBodyChange(TObject *Sender,
      TTreeNode *Node)
{
  if (!Core) return;
  SelectionChange((TRigidChip*)Node->Data);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonSwitchClick(TObject *Sender)
{
  if (TreeViewBody->Visible)
  {
    TreeViewBody->Visible = false;
    ButtonPlus->Left = 5;
    ButtonPlus->Width = 25;
    ButtonMinus->Left = 5;
    ButtonMinus->Width = 25;
    ListNorth->Visible = true;
    ListEast->Visible = true;
    ListWest->Visible = true;
    ListSouth->Visible = true;
    ButtonParent->Visible = true;
  }
  else
  {
    TreeViewBody->Visible = true;
    ButtonPlus->Left = 0;
    ButtonPlus->Width = 20;
    ButtonMinus->Left = 0;
    ButtonMinus->Width = 20;
    ListNorth->Visible = false;
    ListEast->Visible = false;
    ListWest->Visible = false;
    ListSouth->Visible = false;
    ButtonParent->Visible = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PageControlRightChange(TObject *Sender)
{
  if (!Core) return;
  if (PageControlRight->ActivePage == TabVal)
  {
    ListVariables->Items->Clear();
    for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
      ListVariables->Items->AddObject(i->first, (TObject*)i->second);
  }
  else if (PageControlRight->ActivePage == TabKey)
  {
    ComboKeyVarName->Items->Clear();
    for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
      ComboKeyVarName->Items->AddObject(i->first, (TObject*)i->second);
    ComboKeyNoChange(Sender);
  }
  else if (PageControlRight->ActivePage == TabBody)
  {
    PageControlBody->ActivePage = TabOptions;
    if (Core->MemoModel != MemoModel->Text)
      MemoModel->Text = Core->MemoModel;
  }
  else if (PageControlRight->ActivePage == TabScript)
  {
    if (Core->Script->ScriptText != MemoScript->Text)
      MemoScript->Text = Core->Script->ScriptText;
  }
  else if (PageControlRight->ActivePage == TabLua)
  {
    if (Core->Lua != MemoLua->Text)
      MemoLua->Text = Core->Lua;
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
  PageControlRightChange(Sender);
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
  var->Value = var->Default;

  Modify = true;
  PageControlRightChange(Sender);
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
  PageControlRightChange(Sender);
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
  PageControlRightChange(Sender);
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
  RunLua = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonScriptStopClick(TObject *Sender)
{
  RunScript = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonSetLuaClick(TObject *Sender)
{
  if (!Core) return;
  if (Core->Lua != MemoLua->Text)
  {
    Core->Lua = MemoLua->Text;
    Modify = true;
  }
  for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
    i->second->Value = i->second->Default;
  LuaOut->Lines->Text = RCLua->ErrorMessage;
  RunScript = false;
  RCLua->ErrorMessage = "";
  RCLua->ScriptText = Core->Lua;
  if (RCLua->ErrorMessage != "")
  {
    RunLua = false;
    LuaOut->Lines->Clear();
    LuaOut->Lines->Add("LoadError");
    LuaOut->Lines->Add(RCLua->ErrorMessage);
  }
  else
    RunLua = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonStopLuaClick(TObject *Sender)
{
  RunLua = false;
}
//---------------------------------------------------------------------------
int CheckPanekitFile(int fp)
{ // plLbgÌt@C©`FbN
  char c;

  char *match = "SC\x13\x04plLbg";
  int len = AnsiString(match).Length();
  int pos = 0;
  // match¶ñðTµÈªçXLbv
  while (FileRead(fp, &c, 1) == 1)
  {
    if (c == match[pos])
    {
      pos ++;
      if (pos == len)
        break;
    }
    else
    {
      FileSeek(fp, -pos, 1);
      pos = 0;
    }
  }

  if (pos != len)
  {
    FileSeek(fp, -pos, 1);
    return -1;
  }
  return FileSeek(fp, -pos, 1);
}
const char* const PanekitStringMap1 =
"@??????????????????????????????OPQRSTUVWX??????????????`abcdefghijklmn"
"opqrstuvwxyabcde????????????"
" ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎ"
"ÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñ@ABCDEFGHIJKL"
"MNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvÖ×Øz{|"
"}~??ABCDEFGHIJLMNOPQRSTUV"
"WXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~??"
" ¡¢£¤¥¦§¨©ª«¬????????TUVWXY"
"Z[\]??êñOlÜZµªã\SçëëóQÞEäÝb³¸Oã¶Eãºå¬áàO½­dyø"
"Uhè«¤CóúÎØàynÑRÄ§ÉÆI^üVæàÛÅÔ^®Ô@òsí¼p®Í"
"ìdl??³ùñ@~tSü¡`³Àå½­ã»SâºóDøplõê³G`à|_¬³´É"
"°Ôa¯KEeFEÒöb°´éhÖøSn~ÕPçT@Rmd¸ï`¯»ôj¹¶½Åe"
"¤cégk«²ÑR·ºgÂ©ÎtÄH~¼ìk\ s{§Ssæ¬ºjÔìãc"
"¡lL{¨cÌß±ÍÅçìt]Z|îXeå¼ÊkírqØ÷¾¡ºaû¬]ËÃã´"
"n¢NªbéI~}Ù­i«ÀÍpx@¾YÕ¬²å³êVvÇ¤óôePü¡åjVK"
"±@¹ìZ??EY³cÆ¥gêA·å«âÎsvc`HìÓC°yï¯nQPur¢¹Më"
"awé§ v¶fëiÁó¾ðgY¬ ¹ªõöC£|¿÷ØÔÙlRÜMäÈwÅI"
"¼}zd]¢EÕN¥Á????????"
;
const char* const PanekitStringMap2 =
"L2R2L1R1¢~ ª¨«© ß¶G"
"owxylsirb??????????????"
;
AnsiString ReadPanekitString(int fp, int len)
{
  AnsiString n;
  bool pre = true;
  unsigned char c1, c2;
  int i;
  while (len-- > 0 && FileRead(fp, &c1, 1) == 1)
  {
    if (pre)
    {
      if (len-- <= 0 || FileRead(fp, &c2, 1) != 1) return n;
      i = (c1 << 4) | (c2 >> 4);
    }
    else
    {
      i = ((c2 & 0xF) << 8) | c1;
    }
    if (i < 0x340)
    {
      i *= 2;
      n += AnsiString(PanekitStringMap1[i]) + AnsiString(PanekitStringMap1[i+1]);
    }
    else if (0x400 <= i && i < 0x420)
    {
      i = (i - 0x400) * 2;
      n += AnsiString(PanekitStringMap2[i]) + AnsiString(PanekitStringMap2[i+1]);
    }
    pre = !pre;
  }
  return n;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonPanekitBrowseClick(TObject *Sender)
{
  OpenDialogPanekit->FileName = EditPanekitFile->Text;
  if (!OpenDialogPanekit->Execute())
    return;
  
  EditPanekitFile->Text = OpenDialogPanekit->FileName;

  if (EditPanekitFile->Text == "")
  {
    Application->MessageBox("Select panekit file", "LoadList", MB_OK | MB_ICONINFORMATION);
    return;
  }

  ListPanekit->Items->Clear();
  MemoPanekit->Lines->Clear();

  int fp = FileOpen(EditPanekitFile->Text, fmOpenRead | fmShareDenyWrite);
  if (fp == -1)
  {
    Application->MessageBox("Can't open panekit file", "LoadList", MB_OK | MB_ICONERROR);
    return;
  }

  int pos = CheckPanekitFile(fp);
  if (pos == -1)
  {
    FileClose(fp);
    Application->MessageBox("This file is not panekit file", "LoadList", MB_OK | MB_ICONERROR);
    return;
  }

  FileSeek(fp, 0x600, 1);

  // ¼OÌÇÝÝ
  for (int i = 0; i < 32; i ++)
  {
    AnsiString n = ReadPanekitString(fp, 15);
    if (n == "") n = "-";
    ListPanekit->Items->Add(n);
  }

  // «ë­¤f
  FileSeek(fp, pos, 0);
  FileSeek(fp, 0x800, 1);
  FileSeek(fp, 0x380, 1);
  AnsiString n;
  for (int i = 0; i < 10; i ++)
  {
    unsigned short c;
    FileRead(fp, &c, 2);
    if (c < 0x340)
    {
      c *= 2;
      n += AnsiString(PanekitStringMap1[c]) + AnsiString(PanekitStringMap1[c+1]);
    }
    else if (0x400 <= c && c < 0x420)
    {
      c = (i - 0x400) * 2;
      n += AnsiString(PanekitStringMap2[c]) + AnsiString(PanekitStringMap2[c+1]);
    }
  }
  ListPanekit->Items->Add(n);

  FileClose(fp);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListPanekitClick(TObject *Sender)
{
  KeyNewClick(Sender);
  if (Modify)
  {
    ListPanekit->ItemIndex = -1;
    return;
  }

  MemoPanekit->Lines->Clear();

  if (ListPanekit->ItemIndex == -1 || ListPanekit->Items->Strings[ListPanekit->ItemIndex] == "-")
    return;

  if (EditPanekitFile->Text == "")
  {
    ListPanekit->Items->Clear();
    Application->MessageBox("Select panekit file", "LoadModel", MB_OK | MB_ICONINFORMATION);
    return;
  }

  int fp = FileOpen(EditPanekitFile->Text, fmOpenRead | fmShareDenyWrite);
  if (fp == -1)
  {
    ListPanekit->Items->Clear();
    Application->MessageBox("Can't open panekit file", "LoadModel", MB_OK | MB_ICONERROR);
    return;
  }

  if (CheckPanekitFile(fp) == -1)
  {
    FileClose(fp);
    ListPanekit->Items->Clear();
    Application->MessageBox("This file is not panekit file", "LoadModel", MB_OK | MB_ICONERROR);
    return;
  }

  if (ListPanekit->ItemIndex < 32)
  { // f
    FileSeek(fp, 0x1000, 1);
    FileSeek(fp, 0x380 * ListPanekit->ItemIndex, 1);
  }
  else
  { // «ë­¤f
    FileSeek(fp, 0x800, 1);
  }
  
  // o[W
  FileSeek(fp, 4, 1);
  // GfBbgñ
  FileSeek(fp, 4, 1);
  // GfBbgÔ[1/60s]
  FileSeek(fp, 4, 1);
  // H
  FileSeek(fp, 4, 1);

  // RAîñ{{^gptO
  unsigned char coreflag;
  FileRead(fp, &coreflag, 1);
  int corecon = coreflag & 0xF;
  // {^gptO
  FileSeek(fp, 1, 1);
  // ep[cgp
  FileSeek(fp, 9, 1);

  // ep[cîñ 7bytes x 99
  int angles[] = {
        0, -165, -150, -135, -120, -105, -90, -75, -60, -45, -30, -15,
        0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165,
  };
  int colors[] = {
        0xC0C0C0, 0x808080, 0x404040, 0x202020,
        0xFF2020, 0xFF0000, 0x800000, 0x402020,
        0x400000, 0x802000, 0xC0A080, 0xC08000,
        0xFF8000, 0xA08000, 0xA0A000, 0x80A000,
        0x002000, 0x004000, 0x408000, 0x008000,
        0x004040, 0x008040, 0x00A080, 0x8080A0,
        0x202040, 0x404080, 0x4040C0, 0x000000,
        0x402040, 0x802080, 0x804080, 0xFF80A0,
  };
  int buttons[] = {
        7,  // L2¨A
        12, // R2¨F
        8,  // L1¨S
        9,  // R1¨D
        10, // ¢¨V
        6,  // ¨C
        5,  // ~¨X
        4,  //  ¨Z
        -1, -1, -1, -1,
        0,  // ª
        3,  // ¨
        1,  // «
        2,  // ©
  };
  AnsiString err;
  std::stack<TRigidChip*> stackChip;
  std::stack<int> stackCon;
  std::stack<bool> stackInvertNS, stackInvertWE;
  for (int i = 0; err == ""; )
  {
    TRigidChipsDirection dir;
    TRigidChip *parent = Core;
    if (corecon & 1)
    {
      dir = rdNorth;
      corecon &= ~1;
    }
    else if (corecon & 2)
    {
      dir = rdEast;
      corecon &= ~2;
    }
    else if (corecon & 4)
    {
      dir = rdSouth;
      corecon &= ~4;
    }
    else if (corecon & 8)
    {
      dir = rdWest;
      corecon &= ~8;
    }
    else // if (dir == rdCore)
    {
      FileSeek(fp, (99-i)*7, 1);
      break;
    }
    
    const TRigidChipsDirection dirmap[2][2][5] = {
      { // invertNS = false
        {rdCore, rdNorth, rdEast, rdSouth, rdWest}, // invertWE = false
        {rdCore, rdNorth, rdWest, rdSouth, rdEast}, // invertWE = true
      },
      {
        {rdCore, rdSouth, rdEast, rdNorth, rdWest}, // invertWE = false
        {rdCore, rdSouth, rdWest, rdNorth, rdEast}, // invertWE = true
      },
    };
    bool invertNS = false, invertWE = false;
    
    while (i++ < 99)
    {
      TRigidChip *chip;
      unsigned char data[7];
      FileRead(fp, data, 7);

      // Ú±óÔ(0:Èµ 1:¼i, 2:E 4:¶ ÉÚ± è)Ì_a
      int con = data[0] >> 4;

      // px
      int angle = data[3] & 0x1F;
      angle = angle < 24 ? angles[angle] : 0;
      if (!(invertNS ^ invertWE)) // PANEKITÆRCÍt
        angle *= -1;

      // {^
      int button[] = {8,
        data[1]&0xF,
        data[1]>>4,
        data[2]&0xF,
        data[2]>>4,
      };
      for (int j = 0; j < 5; j ++)
        button[j] = button[j] < 0x10 ? buttons[button[j]] : -1;
      // p[^(plÈO)
      int param[] = {
        ((data[3]&0xC0)>>3) | (data[6]>>5),
        data[4] & 0x1F,
        ((data[5]&3)<<3) | (data[4]>>5),
        (data[5]>>2) & 0x1F,
        data[6] & 0x1F,
      };
      int hold = data[5]>>7;

      // p[cíÞ
      int type = data[0] & 0xF;
      if (type == 2)
      { // pl
        // F
        int color = data[4];
        color = color < 0x20 ? colors[color] : 0xFFFFFF;

        // ¿
        // chip[5];
        // 0x80
        // chip[6];

        chip = new TRigidChipChip;
        if (angle != 0) chip->Options->Values["angle"] = IntToStr(angle);
        if (color != 0xFFFFFF) chip->Options->Values["color"] = "#"+IntToHex(color,6);
      }
      else if (type == 3)
      { // XWCg
        if (CheckPanekitTruly->Checked)
        {
          // «üè
          chip = new TRigidChipChip;
          if (angle != 0) chip->Options->Values["angle"] = IntToStr(angle);
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);

          chip = new TRigidChipFrame;
          chip->Options->Values["angle"] = IntToStr(-75+angle);
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          chip = new TRigidChipFrame;
          chip->Options->Values["angle"] = "150";
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;

          chip = new TRigidChipFrame;
        }
        else
          chip = new TRigidChipChip;

        for (int j = 0; j < 5; j ++)
        {
          param[j] = param[j] < 24 ? angles[param[j]] : 0;
          // PANEKITÆRCÍt
          if (!(invertNS ^ invertWE)) param[j] *= -1;
          if (!CheckPanekitTruly->Checked)
            param[j] += angle;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*5));
          Core->Variables["JOINT"+IntToStr(i)] = var;
          chip->Options->Values["angle"] = "JOINT"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "JOINT"+IntToStr(i), "step="+IntToStr((param[j]-param[0])/5)));
        }
        else
          if (param[0] != 0) chip->Options->Values["angle"] = IntToStr(param[0]);
        if (hold == 0) chip->Options->Values["spring"] = "0.5";

        if (CheckPanekitTruly->Checked)
        {
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          chip = new TRigidChipFrame;
          chip->Options->Values["angle"] = "-150";
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;

          chip = new TRigidChipChip;
          chip->Options->Values["angle"] = "-105";
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;

          chip = NULL;
          if (dir == rdNorth || dir == rdSouth) invertNS = !invertNS;
          if (dir == rdEast || dir == rdWest) invertWE = !invertWE;
        }
      }
      else if (type == 4 || type == 5)
      { // Y/ZWCg
        if (CheckPanekitTruly->Checked)
        {
          chip = new TRigidChipFrame;
          chip->Options->Values["angle"] = IntToStr(-105+angle);
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          chip = new TRigidChipFrame;
          chip->Options->Values["angle"] = "-150";
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          chip = new TRigidChipChip;
          chip->Options->Values["angle"] = "-105";
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
        }
        else if (angle != 0)
        {
          chip = new TRigidChipFrame;
          chip->Options->Values["angle"] = IntToStr(180+angle);
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          if (dir == rdNorth || dir == rdSouth) invertNS = !invertNS;
          if (dir == rdEast || dir == rdWest) invertWE = !invertWE;
        }

        if (type == 4)
          chip = new TRigidChipRudder;
        else // if (type == 5)
          chip = new TRigidChipTrim;

        for (int j = 0; j < 5; j ++)
        {
          param[j] = param[j] < 24 ? angles[param[j]] : 0;
          // PANEKITÆRC¯¶ATrimÍ½]µÈ¢
          if (type == 4 && invertNS ^ invertWE) param[j] *= -1;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*5));
          Core->Variables["JOINT"+IntToStr(i)] = var;
          chip->Options->Values["angle"] = "JOINT"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "JOINT"+IntToStr(i), "step="+IntToStr((param[j]-param[0])/5)));
        }
        else
          if (param[0] != 0) chip->Options->Values["angle"] = IntToStr(param[0]);
        if (hold == 0) chip->Options->Values["spring"] = "0.5";

        if (CheckPanekitTruly->Checked)
        {
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;

          chip = new TRigidChipFrame;
          chip->Options->Values["angle"] = "-105";
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          chip = new TRigidChipFrame;
          chip->Options->Values["angle"] = "-150";
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Options->Values["option"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          chip = new TRigidChipChip;
          chip->Options->Values["angle"] = "75";
          if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;

          chip = NULL;
          if (dir == rdNorth || dir == rdSouth) invertNS = !invertNS;
          if (dir == rdEast || dir == rdWest) invertWE = !invertWE;
        }
      }
      else if (type == 6)
      { // [^[
        chip = new TRigidChipFrame;
        chip->Options->Values["angle"] = IntToStr(15+angle);
        if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
        chip->Options->Values["option"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        chip = new TRigidChipFrame;
        chip->Options->Values["angle"] = "150";
        if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
        chip->Options->Values["option"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;

        chip = new TRigidChipWheel;
        chip->Options->Values["angle"] = "105";

        for (int j = 0; j < 5; j ++)
        {
          param[j] -= 0x0C;
          param[j] *= 5000;
          // [^[Í½]µÈ¢
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*5000));
          Core->Variables["MOTOR"+IntToStr(i)] = var;
          chip->Options->Values["power"] = "MOTOR"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "MOTOR"+IntToStr(i), "step="+IntToStr(param[j]-param[0])));
        }
        else
          if (param[0] != 0) chip->Options->Values["power"] = IntToStr(param[0]);
        if (hold == 0) chip->Options->Values["spring"] = "0.5";

        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;

        chip = new TRigidChipFrame;
        chip->Options->Values["angle"] = "105";
        if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
        chip->Options->Values["option"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        chip = new TRigidChipFrame;
        chip->Options->Values["angle"] = "150";
        if (CheckPanekitDamper->State != cbUnchecked) chip->Options->Values["damper"] = "1";
        chip->Options->Values["option"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        chip = new TRigidChipFrame;
        chip->Options->Values["angle"] = "-165";
      }
      else if (type == 7)
      { // ^C
        chip = new TRigidChipWheel;
        if (angle != 0) chip->Options->Values["angle"] = IntToStr(angle);

        for (int j = 0; j < 5; j ++)
        {
          param[j] -= 0x0C;
          param[j] *= 750;
          if (invertNS ^ invertWE) param[j] *= -1;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*1000));
          Core->Variables["TIRE"+IntToStr(i)] = var;
          chip->Options->Values["power"] = "TIRE"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "TIRE"+IntToStr(i), "step="+IntToStr((param[j]-param[0])/5)));
        }
        else
          if (param[0] != 0) chip->Options->Values["power"] = IntToStr(param[0]);
      }
      else if (type == 8)
      { // WFbg
        chip = new TRigidChipJet;
        if (angle != 0) chip->Options->Values["angle"] = IntToStr(angle);

        for (int j = 0; j < 5; j ++)
        {
          param[j] -= 0x0C;
          param[j] *= 10000;
          if (invertNS ^ invertWE) param[j] *= -1;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*5000));
          Core->Variables["JET"+IntToStr(i)] = var;
          chip->Options->Values["power"] = "JET"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "JET"+IntToStr(i), "step="+IntToStr(param[j]-param[0])));
        }
        else
          if (param[0] != 0) chip->Options->Values["power"] = IntToStr(param[0]);
      }
      else if (type == 9)
      { // V[^[
        chip = new TRigidChipArm;
        if (angle != 0) chip->Options->Values["angle"] = IntToStr(angle);

        for (int j = 0; j < 5; j ++)
        {
          param[j] -= 0x0C;
          param[j] *= 25000;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(max));
          Core->Variables["SHOT"+IntToStr(i)] = var;
          chip->Options->Values["option"] = IntToStr(max);
          chip->Options->Values["power"] = "SHOT"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "SHOT"+IntToStr(i), "step="+IntToStr(param[j]-param[0])));
        }
        else
          if (param[0] != 0)
          {
            chip->Options->Values["option"] = IntToStr(param[0]);
            chip->Options->Values["power"] = IntToStr(param[0]);
          }
      }
      else
      {
        err = "¢mÌp[c^CvÔ(" + IntToStr(type) + ")";
        break;
      }
      if (chip)
      {
        if (CheckPanekitDamper->State == cbChecked) chip->Options->Values["damper"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
      }
      
      if (con == 0)
      { // Ú±Èµ(s«~Üè)
        if (stackChip.size() == 0)
          break;

        parent = stackChip.top(); stackChip.pop();
        con = stackCon.top(); stackCon.pop();
        invertNS = stackInvertNS.top(); stackInvertNS.pop();
        invertWE = stackInvertWE.top(); stackInvertWE.pop();
        dir = dirmap[invertNS][invertWE][parent->Direction];
      }
      if (con & 1)
      { // ¼iûüÉÚ± è
        con &= ~1;
      }
      else if (con & 2)
      { // EÉÚ± è
        con &= ~2;
        dir = (TRigidChipsDirection)((dir % 4) + 1);
      }
      else if (con & 8)
      { // ¶ÉÚ± è
        con &= ~8;
        dir = (TRigidChipsDirection)(((dir - 2 + 4) % 4) + 1);
      }
      if (con)
      { // ¼ÉàÚ± è
        stackChip.push(parent);
        stackCon.push(con);
        stackInvertNS.push(invertNS);
        stackInvertWE.push(invertWE);
      }
    }
  }

  // dlRg
  if (err == "")
  {
    for (int i = 0; i < 8; i ++)
      MemoPanekit->Lines->Add(ReadPanekitString(fp, 15));
    Core->MemoModel = ListPanekit->Items->Strings[ListPanekit->ItemIndex] + "\r\n" + MemoPanekit->Text;
  }
  else
  {
    MemoPanekit->Lines->Add(err);
    MemoPanekit->Lines->Add("t@CÊu: 0x" + IntToHex(FileSeek(fp, 0, 1), 4));
  }

  FileClose(fp);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckPanekitTrulyClick(TObject *Sender)
{
  ListPanekitClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TabBodyResize(TObject *Sender)
{
  ListNorth->Width = TabBody->Width - 80;
  ListSouth->Width = TabBody->Width - 80;
  ListEast->Width = (TabBody->Width - 5) / 2;
  ListWest->Width = (TabBody->Width - 5) / 2;
  ListEast->Left = ListWest->Width + 5;

  ListNorth->Height = (TabBody->Height - 245 - 10) / 3;
  ListEast->Height = ListNorth->Height;
  ListWest->Height = ListNorth->Height;
  ListSouth->Height = ListNorth->Height;
  ListEast->Top = ListNorth->Height + 5;
  ListWest->Top = ListNorth->Height + 5;
  ListSouth->Top = ListNorth->Height + ListEast->Height + 10;

  ComboDirection->Width = (TabBody->Width - 5) / 2;
  CheckRotate->Left = ComboDirection->Width - CheckRotate->Width;
  ComboType->Width = (TabBody->Width - 5) / 2;
  ComboType->Left = ComboDirection->Width + 5;
  LabelType->Left = ComboType->Left;
}
//---------------------------------------------------------------------------

