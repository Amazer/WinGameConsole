#pragma once
#include "baseEntry.h"
class BobEntry :
	public baseEntry
{
public:
	virtual int Init();
	virtual int Main();
	virtual int Exit();
};

