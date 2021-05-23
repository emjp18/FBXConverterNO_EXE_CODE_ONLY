#pragma once
#include <wx/wx.h>
#include "Scene.h"
enum
{
	ID_FILE_OPTION = 2,
	ID_VERTEX_OPTION = 3,
	ID_NORMAL_OPTION = 4,
	ID_COORDINATE_OPTION = 5,
	ID_SPACE_OPTION = 6,
	ID_FILE_PATH = 7,
	ID_EXPORT_PATH = 8,
	ID_WRITE = 9
};
class cMain : public wxFrame
{
private:
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnWrite(wxCommandEvent& event);
	void OnFilePath(wxCommandEvent& event);
	void OnExportPath(wxCommandEvent& event);
	wxRadioButton* fileOption1 = nullptr;
	wxRadioButton* fileOption2= nullptr;
	wxRadioButton* fileOption3= nullptr;
	wxRadioButton* fileOption4= nullptr;
	wxRadioButton* fileOption5= nullptr;
	wxRadioButton* fileOption6= nullptr;
	wxRadioButton* fileOption7 = nullptr;
	wxRadioButton* fileOption8 = nullptr;
	wxRadioButton* fileOption9 = nullptr;
	wxRadioButton* fileOption10 = nullptr;
	wxRadioButton* normalOption1= nullptr;
	wxRadioButton* normalOption2 = nullptr;
	wxRadioButton* coordinateOption1= nullptr;
	wxRadioButton* coordinateOption2= nullptr;
	wxRadioButton* spaceOption1= nullptr;
	wxRadioButton* spaceOption2= nullptr;
	wxFileDialog* fbxFileLocation = nullptr;
	wxDirDialog* writingLocation = nullptr;
	wxComboBox* list = nullptr;
	float m_fps;
public:
	cMain();
	virtual ~cMain();
	
};

