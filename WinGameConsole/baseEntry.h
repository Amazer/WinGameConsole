#pragma once
// ���baseEntry,
// �Ժ�ÿ�εĲ��Զ����Զ���д�����У���GameEntry�е��á�
class baseEntry
{
public:
	baseEntry();
	~baseEntry();
	virtual int Init() = 0;
	virtual int Main() = 0;
	virtual int Exit() = 0;
};

