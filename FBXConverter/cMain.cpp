#include "cMain.h"


void cMain::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void cMain::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This programme converts FBX files, to .xfiles!\n Average normals does not work with morph targets."
        "\n Only one skeleton per file and only one animation stack per file."
        "\n You need to select fps, if you export an animation track."
        "\n If you export blend shapes you need to also export a mesh."
        "\n Skeletons don't need to export a mesh, but they need a file with the corresponding mesh.",
        "About This Programme", wxOK | wxICON_INFORMATION);
}

void cMain::OnWrite(wxCommandEvent& event)
{
    SelectedOptions options;
    options.fileOptions = Options::FileOptions::NONE;
    if (fileOption2->GetValue())
    {
        options.fileOptions |= Options::FileOptions::MESH;
    }
    if (fileOption4->GetValue())
    {
        options.fileOptions |= Options::FileOptions::MATERIAL;
    }
    if (fileOption6->GetValue())
    {
        options.fileOptions |= Options::FileOptions::SKELETON;
    }
    if (fileOption8->GetValue())
    {
        options.fileOptions |= Options::FileOptions::MORPH;
    }
    if (fileOption10->GetValue())
    {
        options.fileOptions |= Options::FileOptions::LIGHT;
    }
    if (normalOption1->GetValue())
    {
        options.normalOptions = Options::NormalOptions::REGULAR;
    }
    else
    {
        options.normalOptions = Options::NormalOptions::AVERAGE;
    }
    if (coordinateOption1->GetValue())
    {
        options.coordinateOptions = Options::CoordinateOptions::LEFTHANDED;
    }
    else
    {
        options.coordinateOptions = Options::CoordinateOptions::RIGHTHANDED;
    }
    if (spaceOption1->GetValue())
    {
        options.spaceOptions = Options::SpaceOptions::GLOBAL;
    }
    else
    {
        options.spaceOptions = Options::SpaceOptions::LOCAL;
    }
    if (fbxFileLocation->GetPath()!=wxEmptyString&& writingLocation->GetPath() != wxEmptyString)
    {
        if (strcmp("Frames Per Second", list->GetValue())!=0)
        {
            m_fps = std::stof(list->GetValue().c_str().AsChar());
        }
        else
        {
            m_fps = 0.0f;
        }
        wxString outPath = writingLocation->GetPath() + "\\" + fbxFileLocation->GetFilename().Before('.');
        char outPathChar[500];
        char inPathChar[500];
        strcpy(inPathChar, fbxFileLocation->GetPath());
        strcpy(outPathChar, outPath);
        FbxString inPath = inPathChar;
        Scene scene(inPath, m_fps);
        scene.setOptions(options);
        scene.setOutPath(outPathChar);
        scene.getRootNode();
        scene.write();
        Close(true);
    }
    else
    {
        wxLogMessage("Choose a file and a path!");
    }
   
}

void cMain::OnFilePath(wxCommandEvent& event)
{
    fbxFileLocation->ShowModal();
}

void cMain::OnExportPath(wxCommandEvent& event)
{
    writingLocation->ShowModal();
}

cMain::cMain() : wxFrame(nullptr,wxID_DEFAULT, "FBXCONVERTER",wxDefaultPosition,wxSize(600,400))
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("FBX CONVERTER!");
    Bind(wxEVT_MENU, &cMain::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &cMain::OnExit, this, wxID_EXIT);
    wxPoint pos;
    pos.x = 200;
    pos.y = 0;
    fileOption1 = new wxRadioButton(this, ID_FILE_OPTION,"Do Not Export Meshes" , wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    fileOption2 = new wxRadioButton(this, ID_FILE_OPTION, "Export Meshes", pos);
    pos.x = 0;
    pos.y = 20;
    fileOption3 = new wxRadioButton(this, ID_FILE_OPTION,"Do Not Export Materials" , pos, wxDefaultSize, wxRB_GROUP);
    pos.x = 200;
    pos.y = 20;
    fileOption4 = new wxRadioButton(this, ID_FILE_OPTION, "Export Materials", pos);
    pos.x = 0;
    pos.y = 40;
    fileOption5 = new wxRadioButton(this, ID_FILE_OPTION,"Do Not Export Skeleton" , pos, wxDefaultSize, wxRB_GROUP);
    pos.x = 200;
    pos.y = 40;
    fileOption6 = new wxRadioButton(this, ID_FILE_OPTION, "Export Skeleton", pos);
    pos.x = 0;
    pos.y = 60;
    fileOption7 = new wxRadioButton(this, ID_FILE_OPTION,"Do Not Export Blendshapes" , pos, wxDefaultSize, wxRB_GROUP);
    pos.x = 200;
    pos.y = 60;
    fileOption8 = new wxRadioButton(this, ID_FILE_OPTION,"Export Blendshapes" , pos);
    pos.x = 0;
    pos.y = 80;
    fileOption9 = new wxRadioButton(this, ID_FILE_OPTION,"Do Not Export Lights" , pos, wxDefaultSize, wxRB_GROUP);
    pos.x = 200;
    pos.y = 80;
    fileOption10 = new wxRadioButton(this, ID_FILE_OPTION, "Export Lights", pos);
    pos.x = 0;
    pos.y = 100;
    normalOption1 = new wxRadioButton(this, ID_NORMAL_OPTION, "Regular Normals", pos, wxDefaultSize, wxRB_GROUP);
    pos.x = 200;
    pos.y = 100;
    normalOption2 = new wxRadioButton(this, ID_NORMAL_OPTION, "Average Normals", pos);
    pos.x = 0;
    pos.y = 120;
    coordinateOption1 = new wxRadioButton(this, ID_COORDINATE_OPTION, "Left Handed", pos, wxDefaultSize, wxRB_GROUP);
    pos.x = 200;
    pos.y = 120;
    coordinateOption2 = new wxRadioButton(this, ID_COORDINATE_OPTION, "Right Handed", pos);
    pos.x = 0;
    pos.y = 140;
    spaceOption1 = new wxRadioButton(this, ID_SPACE_OPTION, "Global", pos, wxDefaultSize, wxRB_GROUP);
    pos.x = 200;
    pos.y = 140;
    spaceOption2 = new wxRadioButton(this, ID_SPACE_OPTION, "Local", pos);
    pos.x = 0;
    pos.y = 180;
    wxButton* filePathBtn = new wxButton(this, ID_FILE_PATH, "File Path", pos);
    pos.x = 200;
    pos.y = 180;
    wxButton* exportPathBtn = new wxButton(this, ID_EXPORT_PATH, "Export Path", pos);
    fbxFileLocation = new wxFileDialog(this, "Choose FBX file");
    writingLocation = new wxDirDialog(this, "Choose Where to Export");
    pos.x = 150;
    pos.y = 250;
    wxButton* writeBtn = new wxButton(this, ID_WRITE, "Write", pos);
    Bind(wxEVT_BUTTON, &cMain::OnWrite, this, ID_WRITE);
    Bind(wxEVT_BUTTON, &cMain::OnFilePath, this, ID_FILE_PATH);
    Bind(wxEVT_BUTTON, &cMain::OnExportPath, this, ID_EXPORT_PATH);
    wxArrayString fpsList;
    for (int i = 1; i < 101; i++)
    {
        fpsList.Add(wxString(std::to_string(i)));
    }
    pos.x = 400;
    pos.y = 0;
    wxSize listSize;
    listSize.x = 150;
    listSize.y = 100;
    list = new wxComboBox(this, wxID_DEFAULT, "Frames Per Second", pos, listSize, fpsList);
    
}

cMain::~cMain()
{
  /*  DestroyChildren();
    Destroy();*/
    fbxFileLocation->Destroy();
    writingLocation->Destroy();
}

