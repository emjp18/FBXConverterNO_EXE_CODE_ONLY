#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{
}

cApp::~cApp()
{
    
    //_CrtDumpMemoryLeaks();
    //_crtBreakAlloc = 10453;
}



bool cApp::OnInit()
{
    frameOne = new cMain();
    frameOne->Show();    
    return true;
}
