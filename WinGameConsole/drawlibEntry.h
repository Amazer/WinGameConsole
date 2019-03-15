#pragma once
#include "baseEntry.h"
class drawlibEntry :
	public baseEntry
{
public:
	drawlibEntry();
	~drawlibEntry();
	virtual int Init();
	virtual int Main();
	virtual int Exit();
};

