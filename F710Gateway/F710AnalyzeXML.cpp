#include "StdAfx.h"
#include "F710AnalyzeXML.h"
#include "F710ModulesDefs.h"
#include "Misc.h"
#include "XMLParser.h"
#include "SharedConstants.h"
#include "MemoryLeak.h"
#include <vector>
#include <map>
using namespace std;

#if UNICODE || _UNICODE
static LPCTSTR g_FILE = __FILEW__;
#else
static LPCTSTR g_FILE = __FILE__;
#endif

extern TCHAR szTitle[MAX_LOADSTRING];

static IXMLDOMElement* g_pRootElement			= NULL;
static BOOL g_bInitialized						= FALSE;
static map<PCTSTR, PCTSTR>* g_pGlobalConfig		= NULL;
typedef pair<PCTSTR, map<PCTSTR, PCTSTR>*> config_pair;
static vector<config_pair>* g_pConfigPairContainer = NULL;

static void CleanupRootElement(void);

static const PCTSTR TAG_GLOBAL		= _T("global");
static const PCTSTR TAG_SOFTS		= _T("softs");
static const PCTSTR TAG_NAME		= _T("name");

inline void SafeReleaseMap(map<PCTSTR, PCTSTR>* pMap)
{
	if (pMap != NULL) {
		CleanupStoredValues(pMap);
		pMap = NULL;
	}
}

F710AnalyzeXML::F710AnalyzeXML(void)
{
	g_pConfigPairContainer = new vector<config_pair>;
}


F710AnalyzeXML::~F710AnalyzeXML(void)
{
	CleanupRootElement();
	if (g_pConfigPairContainer) {
		g_pConfigPairContainer->clear();
		delete g_pConfigPairContainer;
		g_pConfigPairContainer = NULL;
	}
}

/**
 * @brief
 * XMLParser.dllで取得したXMLのマップオブジェクトを解放します。
 * 
 * XMLParser.dllで取得したXMLのマップオブジェクトを解放します。
 * 
 * @remarks
 * マップの取得に成功した場合には必ず本関数で解放してください。
 * 
 * @see
 * ReadGlobalTag() | ReadSoftsTag()
 */
void CleanupRootElement(void)
{
	if (g_bInitialized) {
		if (g_pConfigPairContainer) {
			int size = g_pConfigPairContainer->size();
			for (int i = 0; i < size; ++i) {
				delete (*g_pConfigPairContainer)[i].first;
				map<PCTSTR, PCTSTR>* pMap = (*g_pConfigPairContainer)[i].second;
				SafeReleaseMap(pMap);
				pMap = NULL;
			}
		}
		SafeReleaseMap(g_pGlobalConfig);
		g_pGlobalConfig		= NULL;

		UnInitialize(g_pRootElement);
		g_pRootElement = NULL;
		g_bInitialized = FALSE;
	}
}

/**
 * @brief
 * XMLをロードしオブジェクトにします。
 * 
 * @param szXMLUri
 * ロードするXMLファイル。
 * 
 * @returns
 * XMLファイルのロードに成功した場合にはTRUE、失敗した場合にはFALSEが返ります。
 * 
 * XMLParser.dllを利用し、XMLをロードしオブジェクトにします。
 */
BOOL F710AnalyzeXML::LoadXML(PCTSTR szXMLUri)
{
	CleanupRootElement();
	if (!PathFileExists(szXMLUri)) {
		{
			LoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_LOAD), GetLastError(), g_FILE, __LINE__);
		}
		return FALSE;
	}
	g_bInitialized = Initialize(&g_pRootElement, szXMLUri);
	return g_bInitialized;
}

/**
 * @brief
 * globalタグの内容を問い合わせます。
 * 
 * @param szKey
 * global/keys/keyタグのキー値。
 * 
 * @returns
 * マップ中にキーで指定した値があれば値を、なければNULLを返します。
 * 
 * globalタグの内容を格納したマップにキーを問い合わせ、値を取得します。
 * 
 * @see
 * ReadGlobalTag()
 */
PCTSTR F710AnalyzeXML::GetGlobalValue(PCTSTR szKey)
{
	if (!ReadGlobalTag()) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_TAG_GLOBAL), GetLastError(), g_FILE, __LINE__);
		return NULL;
	}

	return GetMapItem(g_pGlobalConfig, szKey);
}

/**
 * @brief
 * softsタグの内容を問い合わせます。
 * 
 * @param szKey
 * softs/soft/keys/keyタグのキー値。
 * 
 * @returns
 * マップ中にキーで指定した値があれば値を、なければNULLを返します。
 * 
 * softsタグの内容を格納したマップにキーを問い合わせ、値を取得します。
 * 
 * @see
 * ReadGlobalTag() | ReadSoftsTag()
 */
PCTSTR F710AnalyzeXML::GetSoftValue(PCTSTR szSoftName, PCTSTR szKey)
{
	if (!this->ReadSoftsTag()) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_TAG_SOFTS), GetLastError(), g_FILE, __LINE__);
		return NULL;
	}

	if (g_pConfigPairContainer) {
		int size = g_pConfigPairContainer->size();
		for (int i = 0; i < size; ++i) {
			if (_tcsicmp((*g_pConfigPairContainer)[i].first, szSoftName) == 0) {
				return GetMapItem((*g_pConfigPairContainer)[i].second, szKey);
			}
		}
	}
	return NULL;
}

/**
 * @brief
 * 設定ファイルのglobalタグの解析を行います。
 * 
 * @returns
 * 設定ファイルのglobalタグの解析に成功した場合にはTRUE、失敗した場合にはFALSEを返します。
 * 
 * XMLParser.dllを使用して設定ファイルのglobalタグの解析を行い、マップに格納します。
 * 例: <key name="log">info</key>
 * keyタグnameアトリビュートの値をマップのキーに、バリューをマップのバリューに格納します。
 * 
 * @remarks
 * マップの内容を参照するにはGetGlobalValue()を使用します。
 * 
 * @see
 * GetGlobalValue()
 */
BOOL F710AnalyzeXML::ReadGlobalTag(void)
{
	if (g_pGlobalConfig == NULL) {
		IXMLDOMNode* pGlobal = NULL;
		if (GetDOMTree(g_pRootElement, TAG_GLOBAL, &pGlobal)) {
			g_pGlobalConfig = StoreValues(pGlobal, TAG_NAME);
			FreeDOMTree(pGlobal);

		} else {
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * @brief
 * 設定ファイルのsoftsタグの解析を行います。
 * 
 * @returns
 * 設定ファイルのsoftsタグの解析に成功した場合にはTRUE、失敗した場合にはFALSEを返します。
 * 
 * XMLParser.dllを使用して設定ファイルのsoftsタグの解析を行い、マップに格納します。
 * 
 * @remarks
 * マップの内容を参照するにはGetSoftValue()を使用します。
 * 
 * @see
 * GetSoftValue()
 */
BOOL F710AnalyzeXML::ReadSoftsTag(void)
{
	if (g_pConfigPairContainer->empty()) {

		IXMLDOMNode* pSofts = NULL;
		IXMLDOMNodeList* pSoftsList = NULL;
		if (!GetDOMTree(g_pRootElement, TAG_SOFTS, &pSofts)) {
			LoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_TAG_SOFTS_DOM), GetLastError(), g_FILE, __LINE__);
			return FALSE;
		}

		LONG childCount = GetChildList(pSofts, &pSoftsList);
		for (int i = 0; i < childCount; i++) {
			IXMLDOMNode* pTempNode = NULL;
			if (GetListItem(pSoftsList, i, &pTempNode)) {
				TCHAR szSoftwareName[I4C3D_BUFFER_SIZE] = {0};
				if (GetAttribute(pTempNode, TAG_NAME, szSoftwareName, _countof(szSoftwareName))) {

					// globalタグのターゲット名と比較
					SIZE_T len = _tcslen(szSoftwareName)+1;
					TCHAR* pSoftwareName = new TCHAR[len];
					ZeroMemory(pSoftwareName, sizeof(TCHAR) * len);
					_tcscpy_s(pSoftwareName, _tcslen(szSoftwareName)+1, szSoftwareName);
					g_pConfigPairContainer->push_back(config_pair(pSoftwareName, StoreValues(pTempNode, TAG_NAME)));
					OutputDebugString(szSoftwareName);
					OutputDebugString(L" push_back\n");
				}
				FreeListItem(pTempNode);
			}
		}

		FreeChildList(pSoftsList);
		FreeDOMTree(pSofts);
	}
	return TRUE;
}