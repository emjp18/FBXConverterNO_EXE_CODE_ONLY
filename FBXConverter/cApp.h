#pragma once
#include "cMain.h"

class cApp : public wxApp
{
private:
	cMain* frameOne = nullptr;

public:
	cApp();
	virtual ~cApp();

	virtual bool OnInit();
};

