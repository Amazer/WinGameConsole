#pragma once
#include "baseEntry.h"
class drawlibEntry :
	public baseEntry
{
public:
	virtual int Init();
	virtual int Main();
	virtual int Exit();
};

