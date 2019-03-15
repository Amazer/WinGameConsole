#pragma once
class baseEntry
{
public:
	baseEntry();
	~baseEntry();
	virtual int Init();
	virtual int Main();
	virtual int Exit();
};

