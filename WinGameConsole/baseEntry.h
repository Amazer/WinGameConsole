#pragma once
// ���baseEntry,
// �Ժ�ÿ�εĲ��Զ����Զ���д�����У���GameEntry�е��á�
class baseEntry
{
public:
	baseEntry();
	~baseEntry();
	virtual int Init();
	virtual int Main();
	virtual int Exit();
};

