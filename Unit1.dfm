object Form1: TForm1
  Left = 194
  Top = 110
  AutoScroll = False
  Caption = 'RigidChips Designer'
  ClientHeight = 480
  ClientWidth = 660
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnMouseWheel = FormMouseWheel
  PixelsPerInch = 96
  TextHeight = 12
  object Splitter1: TSplitter
    Left = 480
    Top = 0
    Width = 3
    Height = 480
    Cursor = crHSplit
    Align = alRight
    ResizeStyle = rsUpdate
  end
  object PaintPanelDummy: TPanel
    Left = 0
    Top = 0
    Width = 480
    Height = 480
    Align = alClient
    BevelOuter = bvNone
    Color = clBlack
    TabOrder = 0
    Visible = False
    OnMouseDown = PaintPanelMouseDown
    OnMouseMove = PaintPanelMouseMove
    OnMouseUp = PaintPanelMouseUp
    OnResize = PaintPanelResize
  end
  object PanelRight: TPanel
    Left = 483
    Top = 0
    Width = 177
    Height = 480
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
    DesignSize = (
      177
      480)
    object PageControl1: TPageControl
      Left = 2
      Top = 0
      Width = 173
      Height = 450
      ActivePage = TabBody
      Anchors = [akLeft, akTop, akRight, akBottom]
      MultiLine = True
      TabIndex = 2
      TabOrder = 0
      OnChange = PageControl1Change
      object TabVal: TTabSheet
        Caption = 'Val'
        object ListVariables: TListBox
          Left = 0
          Top = 20
          Width = 165
          Height = 161
          Align = alClient
          ImeMode = imDisable
          ItemHeight = 12
          TabOrder = 0
          OnClick = ListVariablesClick
        end
        object PanelVariable: TPanel
          Left = 0
          Top = 181
          Width = 165
          Height = 225
          Align = alBottom
          BevelOuter = bvNone
          TabOrder = 2
          DesignSize = (
            165
            225)
          object LabelVarName: TLabel
            Left = 0
            Top = 5
            Width = 29
            Height = 12
            Caption = '&Name'
            FocusControl = EditVarName
          end
          object LabelVarDefault: TLabel
            Left = 0
            Top = 45
            Width = 37
            Height = 12
            Caption = '&Default'
            FocusControl = EditVarDefault
          end
          object LabelVarMin: TLabel
            Left = 0
            Top = 84
            Width = 18
            Height = 12
            Caption = '&Min'
            FocusControl = EditVarMin
          end
          object LabelVarMax: TLabel
            Left = 0
            Top = 125
            Width = 21
            Height = 12
            Caption = 'M&ax'
            FocusControl = EditVarMax
          end
          object LabelVarStep: TLabel
            Left = 0
            Top = 165
            Width = 23
            Height = 12
            Caption = '&Step'
            FocusControl = EditVarStep
          end
          object EditVarName: TEdit
            Left = 0
            Top = 20
            Width = 165
            Height = 20
            Anchors = [akLeft, akTop, akRight]
            ImeMode = imDisable
            TabOrder = 0
            Text = 'VAR'
          end
          object EditVarDefault: TEdit
            Left = 0
            Top = 60
            Width = 145
            Height = 20
            Anchors = [akLeft, akTop, akRight]
            ImeMode = imDisable
            TabOrder = 1
            Text = '0'
          end
          object EditVarMin: TEdit
            Left = 0
            Top = 100
            Width = 165
            Height = 20
            Anchors = [akLeft, akTop, akRight]
            ImeMode = imDisable
            TabOrder = 3
            Text = '0'
          end
          object EditVarMax: TEdit
            Left = 0
            Top = 140
            Width = 165
            Height = 20
            Anchors = [akLeft, akTop, akRight]
            ImeMode = imDisable
            TabOrder = 4
          end
          object EditVarStep: TEdit
            Left = 0
            Top = 180
            Width = 165
            Height = 20
            Anchors = [akLeft, akTop, akRight]
            ImeMode = imDisable
            TabOrder = 5
          end
          object ButtonVarOk: TButton
            Left = 95
            Top = 205
            Width = 70
            Height = 20
            Anchors = [akTop, akRight]
            Caption = 'OK'
            Default = True
            TabOrder = 7
            OnClick = ButtonVarOkClick
          end
          object CheckVarDisp: TCheckBox
            Left = 5
            Top = 205
            Width = 45
            Height = 15
            Caption = 'D&isp'
            Checked = True
            State = cbChecked
            TabOrder = 6
          end
          object ButtonVarColor: TButton
            Left = 145
            Top = 60
            Width = 20
            Height = 20
            Anchors = [akTop, akRight]
            Caption = '...'
            TabOrder = 2
            OnClick = ButtonVarColorClick
          end
        end
        object PanelVarLabel: TPanel
          Left = 0
          Top = 0
          Width = 165
          Height = 20
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Variables'
          TabOrder = 1
          DesignSize = (
            165
            20)
          object ButtonVarDel: TButton
            Left = 120
            Top = 0
            Width = 45
            Height = 20
            Anchors = [akTop, akRight]
            Caption = 'DEL'
            TabOrder = 0
            OnClick = ButtonVarDelClick
          end
        end
      end
      object TabKey: TTabSheet
        Caption = 'Key'
        ImageIndex = 1
        DesignSize = (
          165
          406)
        object LabelKeyNo: TLabel
          Left = 0
          Top = 5
          Width = 62
          Height = 12
          Caption = '&Key Number'
          FocusControl = ComboKeyNo
        end
        object LabelKeyVar: TLabel
          Left = 0
          Top = 55
          Width = 75
          Height = 12
          Caption = 'Step &Variables'
          FocusControl = ListKeyVar
        end
        object LabelKeyVarName: TLabel
          Left = 0
          Top = 290
          Width = 75
          Height = 12
          Anchors = [akLeft, akBottom]
          Caption = 'Variable &Name'
          FocusControl = ComboKeyVarName
        end
        object LabelKeyStep: TLabel
          Left = 0
          Top = 335
          Width = 23
          Height = 12
          Anchors = [akLeft, akBottom]
          Caption = '&Step'
          FocusControl = EditKeyStep
        end
        object ComboKeyNo: TComboBox
          Left = 0
          Top = 20
          Width = 165
          Height = 20
          Anchors = [akLeft, akTop, akRight]
          ImeMode = imDisable
          ItemHeight = 12
          TabOrder = 0
          Text = '0'
          OnChange = ComboKeyNoChange
          Items.Strings = (
            '0'
            '1'
            '2'
            '3'
            '4'
            '5'
            '6'
            '7'
            '8'
            '9'
            '10'
            '11'
            '12'
            '13'
            '14'
            '15'
            '16')
        end
        object ComboKeyVarName: TComboBox
          Left = 0
          Top = 305
          Width = 165
          Height = 20
          Anchors = [akLeft, akRight, akBottom]
          ImeMode = imDisable
          ItemHeight = 12
          TabOrder = 1
          Text = 'VAR'
        end
        object EditKeyStep: TEdit
          Left = 0
          Top = 350
          Width = 165
          Height = 20
          Anchors = [akLeft, akRight, akBottom]
          ImeMode = imDisable
          TabOrder = 2
          Text = '0'
        end
        object ListKeyVar: TListBox
          Left = 0
          Top = 70
          Width = 165
          Height = 210
          Anchors = [akLeft, akTop, akRight, akBottom]
          ImeMode = imDisable
          ItemHeight = 12
          TabOrder = 3
          OnClick = ListKeyVarClick
        end
        object ButtonKeyDel: TButton
          Left = 120
          Top = 50
          Width = 45
          Height = 20
          Anchors = [akTop, akRight]
          Caption = 'DEL'
          TabOrder = 4
          OnClick = ButtonKeyDelClick
        end
        object ButtonKeyOk: TButton
          Left = 95
          Top = 380
          Width = 70
          Height = 20
          Anchors = [akRight, akBottom]
          Caption = 'OK'
          Default = True
          TabOrder = 5
          OnClick = ButtonKeyOkClick
        end
      end
      object TabBody: TTabSheet
        Caption = 'Body'
        ImageIndex = 2
        OnResize = TabBodyResize
        DesignSize = (
          165
          406)
        object LabelDirection: TLabel
          Left = 0
          Top = 166
          Width = 46
          Height = 12
          Anchors = [akLeft, akBottom]
          Caption = '&Direction'
          FocusControl = ComboDirection
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
          Font.Style = []
          ParentFont = False
        end
        object LabelType: TLabel
          Left = 85
          Top = 166
          Width = 48
          Height = 12
          Anchors = [akLeft, akBottom]
          Caption = 'Chip&Type'
          FocusControl = ComboType
        end
        object ListNorth: TListBox
          Left = 40
          Top = 0
          Width = 85
          Height = 50
          Anchors = [akLeft, akTop, akRight]
          Color = 13421823
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ItemHeight = 12
          ParentFont = False
          PopupMenu = PopupMenuAdd
          TabOrder = 3
          OnClick = ListNorthClick
          OnContextPopup = ListNorthContextPopup
        end
        object ButtonParent: TButton
          Left = 138
          Top = 10
          Width = 20
          Height = 20
          Anchors = [akTop, akRight]
          Caption = #8593
          TabOrder = 2
          OnClick = ButtonParentClick
        end
        object ListEast: TListBox
          Left = 85
          Top = 55
          Width = 80
          Height = 50
          Color = 13434828
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ItemHeight = 12
          ParentFont = False
          PopupMenu = PopupMenuAdd
          TabOrder = 4
          OnClick = ListNorthClick
          OnContextPopup = ListNorthContextPopup
        end
        object ListWest: TListBox
          Left = 0
          Top = 55
          Width = 80
          Height = 50
          Color = 13434879
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ItemHeight = 12
          ParentFont = False
          PopupMenu = PopupMenuAdd
          TabOrder = 5
          OnClick = ListNorthClick
          OnContextPopup = ListNorthContextPopup
        end
        object ListSouth: TListBox
          Left = 40
          Top = 110
          Width = 85
          Height = 50
          Anchors = [akLeft, akTop, akRight]
          Color = 16764108
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ItemHeight = 12
          ParentFont = False
          PopupMenu = PopupMenuAdd
          TabOrder = 6
          OnClick = ListNorthClick
          OnContextPopup = ListNorthContextPopup
        end
        object ComboType: TComboBox
          Left = 85
          Top = 181
          Width = 80
          Height = 20
          Style = csDropDownList
          Anchors = [akLeft, akBottom]
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ItemHeight = 12
          ParentFont = False
          TabOrder = 9
          OnChange = ComboTypeChange
          Items.Strings = (
            'Chip'
            'Frame'
            'Weight'
            'Wheel'
            'RLW'
            'Jet'
            'Rudder'
            'RudderF'
            'Trim'
            'TrimF'
            'Arm'
            'Cowl')
        end
        object ComboDirection: TComboBox
          Left = 0
          Top = 181
          Width = 80
          Height = 20
          Style = csDropDownList
          Anchors = [akLeft, akBottom]
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ItemHeight = 12
          ParentFont = False
          TabOrder = 7
          OnChange = ComboDirectionChange
          Items.Strings = (
            'North'
            'East'
            'South'
            'West')
        end
        object OptionsEditor: TValueListEditor
          Left = 0
          Top = 206
          Width = 165
          Height = 200
          Align = alBottom
          DefaultColWidth = 55
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          KeyOptions = [keyEdit, keyAdd, keyDelete, keyUnique]
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goEditing, goAlwaysShowEditor, goThumbTracking]
          ParentFont = False
          PopupMenu = PopupMenuOptions
          ScrollBars = ssVertical
          Strings.Strings = (
            '')
          TabOrder = 10
          TitleCaptions.Strings = (
            'Key'
            'Value')
          OnEditButtonClick = OptionsEditorEditButtonClick
          OnSelectCell = OptionsEditorSelectCell
          OnStringsChange = OptionsEditorStringsChange
          ColWidths = (
            55
            104)
        end
        object CheckRotate: TCheckBox
          Left = 50
          Top = 166
          Width = 30
          Height = 12
          Anchors = [akLeft, akBottom]
          Caption = 'R'
          Checked = True
          State = cbChecked
          TabOrder = 8
        end
        object ButtonPlus: TButton
          Left = 5
          Top = 5
          Width = 25
          Height = 20
          Caption = #65291
          TabOrder = 0
          OnClick = ButtonPlusClick
        end
        object ButtonMinus: TButton
          Left = 5
          Top = 30
          Width = 25
          Height = 20
          Caption = #65293
          TabOrder = 1
          OnClick = ButtonMinusClick
        end
      end
      object TabScript: TTabSheet
        Caption = 'Script'
        ImageIndex = 3
        object SplitterScript: TSplitter
          Left = 0
          Top = 303
          Width = 165
          Height = 3
          Cursor = crVSplit
          Align = alBottom
          ResizeStyle = rsUpdate
        end
        object PanelScript: TPanel
          Left = 0
          Top = 0
          Width = 165
          Height = 25
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          object ButtonScriptReset: TButton
            Left = 0
            Top = 0
            Width = 75
            Height = 20
            Caption = 'SetScript'
            TabOrder = 0
            OnClick = ButtonScriptResetClick
          end
          object ButtonScriptStop: TButton
            Left = 80
            Top = 0
            Width = 75
            Height = 20
            Caption = 'StopScript'
            TabOrder = 1
            OnClick = ButtonScriptStopClick
          end
        end
        object MemoScript: TMemo
          Left = 0
          Top = 25
          Width = 165
          Height = 278
          Align = alClient
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          HideSelection = False
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 1
          WantTabs = True
        end
        object ScriptOut: TMemo
          Left = 0
          Top = 306
          Width = 165
          Height = 100
          Align = alBottom
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 2
          WantReturns = False
          WordWrap = False
        end
      end
      object TabLua: TTabSheet
        Caption = 'Lua'
        ImageIndex = 5
        object SplitterLua: TSplitter
          Left = 0
          Top = 303
          Width = 165
          Height = 3
          Cursor = crVSplit
          Align = alBottom
          ResizeStyle = rsUpdate
        end
        object Panel1: TPanel
          Left = 0
          Top = 0
          Width = 165
          Height = 25
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          object ButtonSetLua: TButton
            Left = 0
            Top = 0
            Width = 75
            Height = 20
            Caption = 'SetLua'
            TabOrder = 0
            OnClick = ButtonSetLuaClick
          end
          object ButtonStopLua: TButton
            Left = 80
            Top = 0
            Width = 75
            Height = 20
            Caption = 'StopLua'
            TabOrder = 1
            OnClick = ButtonStopLuaClick
          end
        end
        object MemoLua: TMemo
          Left = 0
          Top = 25
          Width = 165
          Height = 278
          Align = alClient
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          HideSelection = False
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 1
          WantTabs = True
        end
        object LuaOut: TMemo
          Left = 0
          Top = 306
          Width = 165
          Height = 100
          Align = alBottom
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 2
          WantReturns = False
          WordWrap = False
        end
      end
      object TabPanekit: TTabSheet
        Caption = 'Panekit'
        ImageIndex = 4
        object ListPanekit: TListBox
          Left = 0
          Top = 50
          Width = 165
          Height = 236
          Align = alClient
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ItemHeight = 12
          ParentFont = False
          TabOrder = 0
          OnClick = ListPanekitClick
        end
        object PanelPanekit: TPanel
          Left = 0
          Top = 0
          Width = 165
          Height = 50
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 1
          DesignSize = (
            165
            50)
          object LabelPanekitFile: TLabel
            Left = 5
            Top = 10
            Width = 19
            Height = 12
            Caption = 'File'
            FocusControl = EditPanekitFile
          end
          object EditPanekitFile: TEdit
            Left = 30
            Top = 5
            Width = 113
            Height = 20
            Anchors = [akLeft, akTop, akRight]
            Font.Charset = SHIFTJIS_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = #65325#65331' '#12468#12471#12483#12463
            Font.Style = []
            ImeMode = imDisable
            ParentFont = False
            ReadOnly = True
            TabOrder = 0
          end
          object ButtonPanekitBrowse: TButton
            Left = 143
            Top = 5
            Width = 20
            Height = 20
            Anchors = [akTop, akRight]
            Caption = '...'
            TabOrder = 1
            OnClick = ButtonPanekitBrowseClick
          end
          object CheckPanekitTruly: TCheckBox
            Left = 73
            Top = 30
            Width = 90
            Height = 15
            Anchors = [akTop, akRight]
            Caption = 'Convert Truly'
            Checked = True
            State = cbChecked
            TabOrder = 3
            OnClick = CheckPanekitTrulyClick
          end
          object CheckPanekitDamper: TCheckBox
            Left = 5
            Top = 30
            Width = 60
            Height = 15
            AllowGrayed = True
            Caption = 'damper'
            TabOrder = 2
            OnClick = CheckPanekitTrulyClick
          end
        end
        object MemoPanekit: TMemo
          Left = 0
          Top = 286
          Width = 165
          Height = 120
          Align = alBottom
          Font.Charset = SHIFTJIS_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = #65325#65331' '#12468#12471#12483#12463
          Font.Style = []
          ImeMode = imDisable
          ParentFont = False
          ReadOnly = True
          ScrollBars = ssVertical
          TabOrder = 2
          WantReturns = False
        end
      end
    end
    object EditKeyTest: TEdit
      Left = 60
      Top = 455
      Width = 110
      Height = 20
      Anchors = [akLeft, akRight, akBottom]
      ImeMode = imDisable
      ReadOnly = True
      TabOrder = 1
      Text = 'KeyTest'
      OnKeyDown = EditKeyTestKeyDown
      OnKeyUp = EditKeyTestKeyUp
    end
    object CheckAnime: TCheckBox
      Left = 5
      Top = 460
      Width = 55
      Height = 15
      Anchors = [akLeft, akBottom]
      Caption = 'Anime'
      Checked = True
      State = cbChecked
      TabOrder = 2
      OnClick = CheckAnimeClick
    end
  end
  object Timer1: TTimer
    Interval = 25
    OnTimer = Timer1Timer
    Left = 10
    Top = 50
  end
  object OpenDialog: TOpenDialog
    FileName = '*.rcd'
    Filter = 'RigidChipFiles(*.rcd,*.txt)|*.rcd;*.txt|All Files(*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Load'
    Left = 10
    Top = 85
  end
  object TimerReload: TTimer
    Enabled = False
    Interval = 500
    OnTimer = TimerReloadTimer
    Left = 45
    Top = 50
  end
  object ColorDialog: TColorDialog
    Ctl3D = True
    Left = 80
    Top = 85
  end
  object SaveDialog: TSaveDialog
    FileName = '*.rcd'
    Filter = 'RigidChipFiles(*.rcd,*.txt)|*.rcd;*.txt|All Files(*.*)|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Title = 'Save'
    Left = 45
    Top = 85
  end
  object MainMenu1: TMainMenu
    AutoHotkeys = maManual
    Left = 10
    Top = 15
    object KeyFile: TMenuItem
      Caption = '&File'
      object KeyNew: TMenuItem
        Caption = '&New'
        ShortCut = 16462
        OnClick = KeyNewClick
      end
      object KeyOpen: TMenuItem
        Caption = '&Open...'
        ShortCut = 16463
        OnClick = KeyOpenClick
      end
      object KeySave: TMenuItem
        Caption = '&Save'
        ShortCut = 16467
        OnClick = KeySaveClick
      end
      object KeySaveAs: TMenuItem
        Caption = 'Save&As...'
        OnClick = KeySaveAsClick
      end
      object KeyFileBar1: TMenuItem
        Caption = '-'
      end
      object KeyImport: TMenuItem
        Caption = 'Import &Panekit...'
        OnClick = KeyImportClick
      end
      object KeyFileBar2: TMenuItem
        Caption = '-'
      end
      object KeyExit: TMenuItem
        Caption = 'E&xit'
        ShortCut = 32883
        OnClick = KeyExitClick
      end
    end
    object KeyEdit: TMenuItem
      Caption = '&Edit'
      OnClick = KeyEditClick
      object KeyUndo: TMenuItem
        Caption = '&UnDelete'
        OnClick = KeyUndoClick
      end
      object KeyEditBar1: TMenuItem
        Caption = '-'
      end
      object KeyCut: TMenuItem
        Caption = 'Cu&t'
        OnClick = KeyCutClick
      end
      object KeyCopy: TMenuItem
        Caption = '&Copy'
        OnClick = KeyCopyClick
      end
      object KeyPaste: TMenuItem
        Caption = 'Paste'
        OnClick = KeyPasteClick
      end
      object KeyDelete: TMenuItem
        Caption = '&Delete'
        OnClick = KeyDeleteClick
      end
      object KeyDeleteOne: TMenuItem
        Caption = 'Delete only &one'
        OnClick = ButtonMinusClick
      end
    end
    object KeyTool: TMenuItem
      Caption = '&Tool'
      object KeyRCLoad: TMenuItem
        Caption = 'RigidChips &Load'
        ShortCut = 16466
        OnClick = KeyRCLoadClick
      end
      object KeyAutoReload: TMenuItem
        Caption = 'Auto&Reload'
        ShortCut = 16469
        OnClick = KeyAutoReloadClick
      end
      object KeyConvertCowl: TMenuItem
        Caption = 'ConverTo&Cowl'
        OnClick = KeyConvertCowlClick
      end
    end
    object KeyOption: TMenuItem
      Caption = '&Option'
      object KeySelectAdd: TMenuItem
        Caption = 'Select &AddChip'
        Checked = True
        OnClick = KeySelectAddClick
      end
      object KeyShowVoidOptions: TMenuItem
        Caption = 'Show &void options'
        Checked = True
        OnClick = KeyShowVoidOptionsClick
      end
      object KeyShowCGravity: TMenuItem
        Caption = 'Show CGravity'
        OnClick = KeyShowCGravityClick
      end
      object KeyHide: TMenuItem
        Caption = '&Hide'
        object KeyHideCowl: TMenuItem
          Caption = '&Cowl'
          OnClick = HideClick
        end
        object KeyHideGhost: TMenuItem
          Caption = '&Ghost'
          Checked = True
          OnClick = HideClick
        end
        object KeyHideBalloon: TMenuItem
          Caption = '&Balloon'
          OnClick = HideClick
        end
      end
      object KeyOptSave: TMenuItem
        Caption = '&Save'
        OnClick = KeyOptSaveClick
        object KeySaveObfuscate: TMenuItem
          Caption = 'Obfuscate'
          OnClick = KeySaveOptionClick
        end
        object KeySpaceBlock: TMenuItem
          Caption = 'Space after BlockType'
          OnClick = KeySaveOptionClick
        end
        object KeyNLBlock: TMenuItem
          Caption = 'New line after BlockType'
          OnClick = KeySaveOptionClick
        end
        object KeySpaceOpt: TMenuItem
          Caption = 'Space after Options'
          OnClick = KeySaveOptionClick
        end
        object KeyNLOpt: TMenuItem
          Caption = 'New line after Options'
          OnClick = KeySaveOptionClick
        end
        object KeySpaceComma: TMenuItem
          Caption = 'Space after comma'
          OnClick = KeySaveOptionClick
        end
        object KeyNoSubNoNL: TMenuItem
          Caption = 'No sub chip, no new line'
          OnClick = KeySaveOptionClick
        end
      end
      object KeyDefaultView: TMenuItem
        Caption = 'Default &View'
        object KeyDefViewThis: TMenuItem
          Caption = 'Use &this view'
          OnClick = KeyDefViewThisClick
        end
        object KeyDefViewInitilize: TMenuItem
          Caption = '&Initilize'
          OnClick = KeyDefViewInitilizeClick
        end
      end
    end
    object Help1: TMenuItem
      Caption = '&Help'
      object KeyAbout: TMenuItem
        Caption = '&About...'
        OnClick = KeyAboutClick
      end
    end
  end
  object PopupMenu1: TPopupMenu
    AutoHotkeys = maManual
    OnPopup = KeyEditClick
    Left = 45
    Top = 15
    object KeyOrigin: TMenuItem
      Caption = '&Origin'
      OnClick = KeyOriginClick
    end
    object KeyBar1: TMenuItem
      Caption = '-'
    end
    object KeyCut2: TMenuItem
      Caption = 'Cu&t'
      OnClick = KeyCutClick
    end
    object KeyCopy2: TMenuItem
      Caption = '&Copy'
      OnClick = KeyCopyClick
    end
    object KeyPaste2: TMenuItem
      Caption = '&Paste'
      OnClick = KeyPasteClick
    end
    object KeyDelete2: TMenuItem
      Caption = '&Delete'
      OnClick = KeyDeleteClick
    end
    object KeyDeleteOne2: TMenuItem
      Caption = 'Delete only &one'
      OnClick = ButtonMinusClick
    end
    object KeyBar2: TMenuItem
      Caption = '-'
    end
    object KeyConvertCowlS: TMenuItem
      Caption = 'ConvertToCowl'
      OnClick = KeyConvertCowlSClick
    end
  end
  object PopupMenuOptions: TPopupMenu
    OnPopup = PopupMenuOptionsPopup
    Left = 115
    Top = 15
    object KeyOptionsAngle: TMenuItem
      Caption = 'angle'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsPower: TMenuItem
      Caption = 'power'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsBrake: TMenuItem
      Caption = 'brake'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsOption: TMenuItem
      Caption = 'option'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsColor: TMenuItem
      Caption = 'color'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsEffect: TMenuItem
      Caption = 'effect'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsSpring: TMenuItem
      Caption = 'spring'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsDamper: TMenuItem
      Caption = 'damper'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsName: TMenuItem
      Caption = 'name'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsUser1: TMenuItem
      Caption = 'user1'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsUser2: TMenuItem
      Caption = 'user2'
      OnClick = KeyOptionsClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object KeyOptionUp: TMenuItem
      Caption = '&Up'
      OnClick = KeyOptionUpClick
    end
    object KeyOptionDown: TMenuItem
      Caption = '&Down'
      OnClick = KeyOptionDownClick
    end
    object KeyOptionDelete: TMenuItem
      Caption = 'D&elete'
      OnClick = KeyOptionDeleteClick
    end
    object KeyOptionsSort: TMenuItem
      Caption = '&Sort'
      OnClick = KeyOptionsSortClick
    end
  end
  object PopupMenuAdd: TPopupMenu
    OnPopup = PopupMenuAddPopup
    Left = 80
    Top = 15
    object KeyAddChip: TMenuItem
      Caption = 'Chip'
      OnClick = KeyAddChipClick
    end
    object KeyAddFrame: TMenuItem
      Caption = 'Frame'
      OnClick = KeyAddChipClick
    end
    object KeyAddWeight: TMenuItem
      Caption = 'Weight'
      OnClick = KeyAddChipClick
    end
    object KeyAddWheel: TMenuItem
      Caption = 'Wheel'
      OnClick = KeyAddChipClick
    end
    object KeyAddRLW: TMenuItem
      Caption = 'RLW'
      OnClick = KeyAddChipClick
    end
    object KeyAddJet: TMenuItem
      Caption = 'Jet'
      OnClick = KeyAddChipClick
    end
    object KeyAddRudder: TMenuItem
      Caption = 'Rudder'
      OnClick = KeyAddChipClick
    end
    object KeyAddRudderF: TMenuItem
      Caption = 'RudderF'
      OnClick = KeyAddChipClick
    end
    object KeyAddTrim: TMenuItem
      Caption = 'Trim'
      OnClick = KeyAddChipClick
    end
    object KeyAddTrimF: TMenuItem
      Caption = 'TrimF'
      OnClick = KeyAddChipClick
    end
    object KeyAddArm: TMenuItem
      Caption = 'Arm'
      OnClick = KeyAddChipClick
    end
    object KeyAddCowl: TMenuItem
      Caption = 'Cowl'
      OnClick = KeyAddChipClick
    end
    object KeyAddBar1: TMenuItem
      Caption = '-'
    end
    object KeyPasteDirection: TMenuItem
      Caption = '&Paste'
      OnClick = KeyPasteClick
    end
  end
  object OpenDialogPanekit: TOpenDialog
    Filter = 'PS'#12513#12514#12522#12501#12449#12452#12523'|*.ps;*.psx;*.mem;*.mc;*.mc?|'#12377#12409#12390#12398#12501#12449#12452#12523'(*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Load PANEKIT'
    Left = 115
    Top = 85
  end
end
