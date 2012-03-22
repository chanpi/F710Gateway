#pragma once
class I4C3DDIAnalyzeXML
{
public:
	I4C3DDIAnalyzeXML(void);
	~I4C3DDIAnalyzeXML(void);

	// �������R���X�g���N�^���Ă΂Ȃ��ꍇ�́A�ʓr�Ăяo�����Ƃ��ł��܂��B
	BOOL LoadXML(PCTSTR szXMLUri);

	PCTSTR GetGlobalValue(PCTSTR szKey);
	PCTSTR GetSoftValue(PCTSTR szSoftName, PCTSTR szKey);

private:
	BOOL ReadGlobalTag(void);
	BOOL ReadSoftsTag(void);
};

