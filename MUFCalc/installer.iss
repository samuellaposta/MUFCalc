; ─────────────────────────────────────────────────────────────────────
;  MUFCalc v2.0 — Inno Setup script
;
;  Build steps on Windows:
;    1. Build MUFCalc in Qt Creator (Release configuration).
;    2. Open a "Qt 6.x (MinGW or MSVC) Command Prompt" and run windeployqt
;       against the freshly built MUFCalc.exe to populate the deploy folder
;       with Qt DLLs, plugins, and translations:
;
;           mkdir deploy
;           copy build-MUFCalc-Release\release\MUFCalc.exe deploy\
;           windeployqt --release --no-translations deploy\MUFCalc.exe
;
;    3. Compile this script with Inno Setup 6.x (iscc.exe installer.iss
;       on the command line, or open it in Inno Setup Compiler GUI).
;
;    4. The resulting installer is written to Output\MUFCalcSetup_v2.exe.
; ─────────────────────────────────────────────────────────────────────

#define AppName       "MUFCalc"
#define AppVersion    "2.0.0"
#define AppPublisher  "Nartey Samuel Teye (Posta)"
#define AppURL        "https://github.com/samuellaposta/MUFCalc"
#define AppExeName    "MUFCalc.exe"

[Setup]
AppId={{A2C4E1B6-7F3D-4A2C-9B3E-MUFCALC-V2-2026}}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}/issues
AppUpdatesURL={#AppURL}/releases
AppCopyright=Copyright (C) 2026 {#AppPublisher}
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
OutputBaseFilename=MUFCalcSetup_v2
OutputDir=Output
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupIconFile=resources\icons\MUFCalc.ico
UninstallDisplayIcon={app}\{#AppExeName}
UninstallDisplayName={#AppName} {#AppVersion}
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
MinVersion=10.0

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; \
    GroupDescription: "Additional shortcuts:"; Flags: unchecked

[Files]
; Everything Qt's windeployqt produced into the deploy folder ships into {app}.
Source: "deploy\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#AppName}";              Filename: "{app}\{#AppExeName}"
Name: "{group}\Uninstall {#AppName}";    Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}";      Filename: "{app}\{#AppExeName}"; \
    Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExeName}"; Description: "Launch {#AppName}"; \
    Flags: nowait postinstall skipifsilent
