object Form1: TForm1
  Left = 193
  Top = 108
  AutoScroll = False
  Caption = 'RigidChips Designer'
  ClientHeight = 490
  ClientWidth = 670
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnMouseWheel = FormMouseWheel
  DesignSize = (
    670
    490)
  PixelsPerInch = 96
  TextHeight = 12
  object PaintPanelDummy: TPanel
    Left = 5
    Top = 5
    Width = 480
    Height = 480
    BevelOuter = bvNone
    Color = clBlack
    TabOrder = 5
    Visible = False
    OnMouseDown = PaintPanelMouseDown
    OnMouseMove = PaintPanelMouseMove
    OnMouseUp = PaintPanelMouseUp
    OnResize = PaintPanelResize
  end
  object PageControl1: TPageControl
    Left = 490
    Top = 5
    Width = 175
    Height = 435
    ActivePage = TabBody
    Anchors = [akTop, akRight, akBottom]
    TabIndex = 2
    TabOrder = 4
    OnChange = PageControl1Change
    object TabVal: TTabSheet
      Caption = 'Val'
      object ListVariables: TListBox
        Left = 0
        Top = 20
        Width = 167
        Height = 163
        Align = alClient
        ImeMode = imDisable
        ItemHeight = 12
        TabOrder = 0
        OnClick = ListVariablesClick
      end
      object PanelVariable: TPanel
        Left = 0
        Top = 183
        Width = 167
        Height = 225
        Align = alBottom
        BevelOuter = bvNone
        TabOrder = 2
        DesignSize = (
          167
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
          Width = 167
          Height = 20
          Anchors = [akLeft, akTop, akRight]
          ImeMode = imDisable
          TabOrder = 0
          Text = 'VAR'
        end
        object EditVarDefault: TEdit
          Left = 0
          Top = 60
          Width = 147
          Height = 20
          Anchors = [akLeft, akTop, akRight]
          ImeMode = imDisable
          TabOrder = 1
          Text = '0'
        end
        object EditVarMin: TEdit
          Left = 0
          Top = 100
          Width = 167
          Height = 20
          Anchors = [akLeft, akTop, akRight]
          ImeMode = imDisable
          TabOrder = 3
          Text = '0'
        end
        object EditVarMax: TEdit
          Left = 0
          Top = 140
          Width = 167
          Height = 20
          Anchors = [akLeft, akTop, akRight]
          ImeMode = imDisable
          TabOrder = 4
        end
        object EditVarStep: TEdit
          Left = 0
          Top = 180
          Width = 167
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
          Left = 147
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
        Width = 167
        Height = 20
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Variables'
        TabOrder = 1
        DesignSize = (
          167
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
        167
        408)
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
        Top = 230
        Width = 75
        Height = 12
        Caption = 'Variable &Name'
        FocusControl = ComboKeyVarName
      end
      object LabelKeyStep: TLabel
        Left = 1
        Top = 270
        Width = 23
        Height = 12
        Caption = '&Step'
        FocusControl = EditKeyStep
      end
      object ComboKeyNo: TComboBox
        Left = 0
        Top = 20
        Width = 167
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
        Top = 245
        Width = 167
        Height = 20
        Anchors = [akLeft, akTop, akRight]
        ImeMode = imDisable
        ItemHeight = 12
        TabOrder = 1
        Text = 'VAR'
      end
      object EditKeyStep: TEdit
        Left = 1
        Top = 285
        Width = 167
        Height = 20
        Anchors = [akLeft, akTop, akRight]
        ImeMode = imDisable
        TabOrder = 2
        Text = '0'
      end
      object ListKeyVar: TListBox
        Left = 0
        Top = 70
        Width = 167
        Height = 150
        Anchors = [akLeft, akTop, akRight]
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
        Top = 310
        Width = 70
        Height = 20
        Anchors = [akTop, akRight]
        Caption = 'OK'
        Default = True
        TabOrder = 5
        OnClick = ButtonKeyOkClick
      end
    end
    object TabBody: TTabSheet
      Caption = 'Body'
      ImageIndex = 2
      DesignSize = (
        167
        408)
      object LabelDirection: TLabel
        Left = 0
        Top = 130
        Width = 46
        Height = 12
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
        Top = 130
        Width = 48
        Height = 12
        Caption = 'Chip&Type'
        FocusControl = ComboType
      end
      object ListNorth: TListBox
        Left = 40
        Top = 0
        Width = 90
        Height = 40
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #65325#65331' '#12468#12471#12483#12463
        Font.Style = []
        ImeMode = imDisable
        ItemHeight = 12
        ParentFont = False
        PopupMenu = PopupMenuAdd
        TabOrder = 1
        OnClick = ListNorthClick
        OnContextPopup = ListNorthContextPopup
      end
      object ButtonParent: TButton
        Left = 140
        Top = 5
        Width = 20
        Height = 20
        Anchors = [akTop, akRight]
        Caption = #8593
        TabOrder = 0
        OnClick = ButtonParentClick
      end
      object ListEast: TListBox
        Left = 85
        Top = 40
        Width = 80
        Height = 40
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #65325#65331' '#12468#12471#12483#12463
        Font.Style = []
        ImeMode = imDisable
        ItemHeight = 12
        ParentFont = False
        PopupMenu = PopupMenuAdd
        TabOrder = 2
        OnClick = ListNorthClick
        OnContextPopup = ListNorthContextPopup
      end
      object ListWest: TListBox
        Left = 0
        Top = 40
        Width = 80
        Height = 40
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clWindowText
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
      object ListSouth: TListBox
        Left = 40
        Top = 80
        Width = 90
        Height = 40
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clWindowText
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
      object ComboType: TComboBox
        Left = 85
        Top = 145
        Width = 80
        Height = 20
        Style = csDropDownList
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #65325#65331' '#12468#12471#12483#12463
        Font.Style = []
        ImeMode = imDisable
        ItemHeight = 12
        ParentFont = False
        TabOrder = 6
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
        Top = 145
        Width = 80
        Height = 20
        Style = csDropDownList
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #65325#65331' '#12468#12471#12483#12463
        Font.Style = []
        ImeMode = imDisable
        ItemHeight = 12
        ParentFont = False
        TabOrder = 5
        OnChange = ComboDirectionChange
        Items.Strings = (
          'North'
          'East'
          'South'
          'West')
      end
      object OptionsEditor: TValueListEditor
        Left = 0
        Top = 173
        Width = 167
        Height = 235
        Align = alBottom
        Anchors = [akLeft, akTop, akRight, akBottom]
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
          '=')
        TabOrder = 7
        TitleCaptions.Strings = (
          'Key'
          'Value')
        OnEditButtonClick = OptionsEditorEditButtonClick
        OnSelectCell = OptionsEditorSelectCell
        OnStringsChange = OptionsEditorStringsChange
        ColWidths = (
          55
          106)
      end
      object CheckRotate: TCheckBox
        Left = 50
        Top = 130
        Width = 30
        Height = 12
        Caption = 'R'
        Checked = True
        State = cbChecked
        TabOrder = 8
      end
    end
    object TabScript: TTabSheet
      Caption = 'Script'
      ImageIndex = 3
      object PanelScript: TPanel
        Left = 0
        Top = 0
        Width = 167
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
        Width = 167
        Height = 283
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
      object ScriptOut: TListBox
        Left = 0
        Top = 308
        Width = 167
        Height = 100
        Align = alBottom
        ImeMode = imDisable
        ItemHeight = 12
        TabOrder = 2
      end
    end
  end
  object EditKeyTest: TEdit
    Left = 550
    Top = 465
    Width = 110
    Height = 20
    Anchors = [akRight, akBottom]
    ImeMode = imDisable
    ReadOnly = True
    TabOrder = 0
    Text = 'KeyTest'
    OnKeyDown = EditKeyTestKeyDown
    OnKeyUp = EditKeyTestKeyUp
  end
  object CheckBalloon: TCheckBox
    Left = 600
    Top = 445
    Width = 60
    Height = 15
    Anchors = [akRight, akBottom]
    Caption = 'Balloon'
    Checked = True
    State = cbChecked
    TabOrder = 3
    OnClick = CheckClick
  end
  object CheckGhost: TCheckBox
    Left = 545
    Top = 445
    Width = 50
    Height = 15
    Anchors = [akRight, akBottom]
    Caption = 'Ghost'
    TabOrder = 2
    OnClick = CheckClick
  end
  object CheckCowl: TCheckBox
    Left = 495
    Top = 445
    Width = 45
    Height = 15
    Anchors = [akRight, akBottom]
    Caption = 'Cowl'
    Checked = True
    State = cbChecked
    TabOrder = 1
    OnClick = CheckClick
  end
  object CheckAnime: TCheckBox
    Left = 495
    Top = 470
    Width = 55
    Height = 15
    Anchors = [akRight, akBottom]
    Caption = 'Anime'
    Checked = True
    State = cbChecked
    TabOrder = 6
    OnClick = CheckAnimeClick
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
        Caption = '&Open'
        ShortCut = 16463
        OnClick = KeyOpenClick
      end
      object KeySave: TMenuItem
        Caption = '&Save'
        ShortCut = 16467
        OnClick = KeySaveClick
      end
      object KeySaveAs: TMenuItem
        Caption = 'Save&As'
        OnClick = KeySaveAsClick
      end
      object KeyFileBar1: TMenuItem
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
        ShortCut = 16474
        OnClick = KeyUndoClick
      end
      object KeyEditBar1: TMenuItem
        Caption = '-'
      end
      object KeyCut: TMenuItem
        Caption = 'Cu&t'
        ShortCut = 16472
        OnClick = KeyCutClick
      end
      object KeyCopy: TMenuItem
        Caption = '&Copy'
        ShortCut = 16451
        OnClick = KeyCopyClick
      end
      object KeyPaste: TMenuItem
        Caption = 'Paste'
        ShortCut = 16470
        OnClick = KeyPasteClick
      end
      object KeyDelete: TMenuItem
        Caption = '&Delete'
        ShortCut = 46
        OnClick = KeyDeleteClick
      end
    end
    object KeyTool: TMenuItem
      Caption = '&Tool'
      object KeyRCLoad: TMenuItem
        Caption = 'RigidChips &Load'
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
      object KeyOptSave: TMenuItem
        Caption = '&Save'
        OnClick = KeyOptSaveClick
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
    end
    object Help1: TMenuItem
      Caption = '&Help'
      object KeyAbout: TMenuItem
        Caption = '&About'
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
      ShortCut = 16472
      OnClick = KeyCutClick
    end
    object KeyCopy2: TMenuItem
      Caption = '&Copy'
      ShortCut = 16451
      OnClick = KeyCopyClick
    end
    object KeyPaste2: TMenuItem
      Caption = '&Paste'
      ShortCut = 16470
      OnClick = KeyPasteClick
    end
    object KeyDelete2: TMenuItem
      Caption = '&Delete'
      ShortCut = 46
      OnClick = KeyDeleteClick
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
    AutoHotkeys = maManual
    OnPopup = PopupMenuOptionsPopup
    Left = 115
    Top = 15
    object KeyDelOption: TMenuItem
      Caption = '&Delete option'
      OnClick = KeyDelOptionClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object KeyOptionsColor: TMenuItem
      Caption = 'color'
      OnClick = KeyOptionsClick
    end
    object KeyOptionsAngle: TMenuItem
      Caption = 'angle'
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
    object KeyOptionsEffect: TMenuItem
      Caption = 'effect'
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
  end
  object PopupMenuAdd: TPopupMenu
    AutoHotkeys = maManual
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
  end
end
