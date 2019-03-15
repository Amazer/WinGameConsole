#pragma once
// 添加baseEntry,
// 以后，每次的测试都可以独立写在类中，在GameEntry中调用。
class baseEntry
{
public:
	baseEntry();
	~baseEntry();
	virtual int Init() = 0;
	virtual int Main() = 0;
	virtual int Exit() = 0;
};

