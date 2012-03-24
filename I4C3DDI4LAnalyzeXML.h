#pragma once
class I4C3DDI4LAnalyzeXML
{
public:
	I4C3DDI4LAnalyzeXML(void);
	~I4C3DDI4LAnalyzeXML(void);

	// 引数つきコンストラクタを呼ばない場合は、別途呼び出すことができます。
	BOOL LoadXML(PCTSTR szXMLUri);

	PCTSTR GetGlobalValue(PCTSTR szKey);
	PCTSTR GetSoftValue(PCTSTR szSoftName, PCTSTR szKey);

private:
	BOOL ReadGlobalTag(void);
	BOOL ReadSoftsTag(void);
};

