#pragma once
// 添加baseEntry,
// 以后，每次的测试都可以独立写在类中，在GameEntry中调用。
class baseEntry
{
public:
	baseEntry();
	~baseEntry();
	virtual int Init();
	virtual int Main();
	virtual int Exit();
};

