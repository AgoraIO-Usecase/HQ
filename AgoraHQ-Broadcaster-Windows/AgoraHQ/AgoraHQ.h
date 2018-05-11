
// AgoraHQ.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
class CUrlServiceCallback;
// OBS for ExtCapture

#include <util/profiler.h>
#include <util/util.hpp>
#define INPUT_AUDIO_SOURCE  "wasapi_input_capture"
#define OUTPUT_AUDIO_SOURCE "wasapi_output_capture"

int GetConfigPath(char *path, size_t size, const char *name);
char *GetConfigPathPtr(const char *name);

int GetProgramDataPath(char *path, size_t size, const char *name);
char *GetProgramDataPathPtr(const char *name);
config_t *GetGlobalConfig();
const char *Str(const char *lookup);
const std::string app_name = "AgoraHQOBS";
class CAgoraHQDlg;
// OBS end
// CAgoraHQApp: 
// �йش����ʵ�֣������ AgoraHQ.cpp
//

class CAgoraHQApp : public CWinApp
{
public:
	CAgoraHQApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	CUrlServiceCallback* url_service_callback = nullptr;

	// OBS for ExtCapture
public:
	static void GetScaleAndCenterPos(
		int baseCX, int baseCY, int windowCX, int windowCY,
		int &x, int &y, float &scale);
	static  void GetCenterPosFromFixedScale(
		int baseCX, int baseCY, int windowCX, int windowCY,
		int &x, int &y, float scale);
	inline config_t *GlobalConfig() const { return globalConfig; }

	inline const char *GetLocale() const
	{
		return locale.c_str();
	}
	inline const char *GetString(const char *lookupVal) const
	{
		return textLookup.GetString(lookupVal);
	}

	CAgoraHQDlg* GetMainWindow();

	const char *GetRenderModule() const;
	const char *InputAudioSource() const;
	const char *OutputAudioSource() const;

private:


	bool InitGlobalConfig();
	bool InitGlobalConfigDefaults();
	bool InitLocale();
	bool MakeUserDirs();
	//
	void OBSAppInit();

	CAgoraHQDlg* m_pMainDialog;
	std::string                    locale;
	ConfigFile                     globalConfig;

	std::string opt_starting_collection;
	std::string opt_starting_profile;
	std::string opt_starting_scene;

	TextLookup                     textLookup;
	profiler_name_store_t          *profilerNameStore = nullptr;
	//OBS end
};

extern CAgoraHQApp theApp;