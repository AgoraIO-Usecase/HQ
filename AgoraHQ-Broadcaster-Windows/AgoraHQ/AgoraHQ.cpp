
// AgoraHQ.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "commonFun.h"
#include "UrlService/CurlService.h"
#include "UrlServiceCallback.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAgoraHQApp

BEGIN_MESSAGE_MAP(CAgoraHQApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAgoraHQApp 构造

CAgoraHQApp::CAgoraHQApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CAgoraHQApp 对象

CAgoraHQApp theApp; 
CConfigHQ gHQConfig;
CFileIO gFileApp;


// CAgoraHQApp 初始化

BOOL CAgoraHQApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	url_service_callback = new CUrlServiceCallback;
	AfxGetUrlService()->init_curl_service(url_service_callback);

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	gFileApp.openLog(getHQLogPath());
// 	CAgoraHQDlg dlg;
// 	m_pMainWnd = &dlg;
	//OBS for ExtCapture
	OBSAppInit();
	CAgoraHQDlg dlg;
	dlg.OBSInit();
	m_pMainDialog = &dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	gFileApp.close();
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CAgoraHQApp::ExitInstance()
{
	AfxGetUrlService()->clear_callback();
	if (nullptr != url_service_callback)
	{
		delete url_service_callback;
		url_service_callback = nullptr;
	}

	AfxGetUrlService()->ReleaseInstance();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////
// OBS for ExtCapture
/////////////////////////////////////////////////////////////////////////////////////
CAgoraHQDlg* CAgoraHQApp::GetMainWindow()
{
	return m_pMainDialog;
}
#define BASE_PATH "../.."
#define CONFIG_PATH BASE_PATH "/config"
const std::string untitled_name = "Untitled";
const char *Str(const char *lookup) { return theApp.GetString(lookup); }

int GetConfigPath(char *path, size_t size, const char *name)
{
	return os_get_config_path(path, size, name);
}

char *GetConfigPathPtr(const char *name)
{
	return os_get_config_path_ptr(name);
}

int GetProgramDataPath(char *path, size_t size, const char *name)
{
	return os_get_program_data_path(path, size, name);
}

char *GetProgramDataPathPtr(const char *name)
{
	return os_get_program_data_path_ptr(name);
}

config_t *GetGlobalConfig() { return theApp.GlobalConfig(); }

void convert_x264_settings(obs_data_t *data)
{
	bool use_bufsize = obs_data_get_bool(data, "use_bufsize");

	if (use_bufsize) {
		int buffer_size = (int)obs_data_get_int(data, "buffer_size");
		if (buffer_size == 0)
			obs_data_set_string(data, "rate_control", "CRF");
	}
}
void convert_14_2_encoder_setting(const char *encoder, const char *file)
{
	obs_data_t *data = obs_data_create_from_json_file_safe(file, "bak");
	obs_data_item_t *cbr_item = obs_data_item_byname(data, "cbr");
	obs_data_item_t *rc_item = obs_data_item_byname(data, "rate_control");
	bool modified = false;
	bool cbr = true;

	if (cbr_item) {
		cbr = obs_data_item_get_bool(cbr_item);
		obs_data_item_unset_user_value(cbr_item);

		obs_data_set_string(data, "rate_control", cbr ? "CBR" : "VBR");

		modified = true;
	}

	if (!rc_item && astrcmpi(encoder, "obs_x264") == 0) {
		if (!cbr_item)
			obs_data_set_string(data, "rate_control", "CBR");
		else if (!cbr)
			convert_x264_settings(data);

		modified = true;
	}

	if (modified)
		obs_data_save_json_safe(data, file, "tmp", "bak");

	obs_data_item_release(&rc_item);
	obs_data_item_release(&cbr_item);
	obs_data_release(data);
}


bool move_reconnect_settings(ConfigFile &config, const char *sec)
{
	bool changed = false;

	if (config_has_user_value(config, sec, "Reconnect")) {
		bool reconnect = config_get_bool(config, sec, "Reconnect");
		config_set_bool(config, "Output", "Reconnect", reconnect);
		changed = true;
	}
	if (config_has_user_value(config, sec, "RetryDelay")) {
		int delay = (int)config_get_uint(config, sec, "RetryDelay");
		config_set_uint(config, "Output", "RetryDelay", delay);
		changed = true;
	}
	if (config_has_user_value(config, sec, "MaxRetries")) {
		int retries = (int)config_get_uint(config, sec, "MaxRetries");
		config_set_uint(config, "Output", "MaxRetries", retries);
		changed = true;
	}

	return changed;
}

bool update_reconnect(ConfigFile &config)
{
	if (!config_has_user_value(config, "Output", "Mode"))
		return false;

	const char *mode = config_get_string(config, "Output", "Mode");
	if (!mode)
		return false;

	const char *section = (strcmp(mode, "Advanced") == 0) ?
		"AdvOut" : "SimpleOutput";

	if (move_reconnect_settings(config, section)) {
		config_remove_value(config, "SimpleOutput", "Reconnect");
		config_remove_value(config, "SimpleOutput", "RetryDelay");
		config_remove_value(config, "SimpleOutput", "MaxRetries");
		config_remove_value(config, "AdvOut", "Reconnect");
		config_remove_value(config, "AdvOut", "RetryDelay");
		config_remove_value(config, "AdvOut", "MaxRetries");
		return true;
	}

	return false;
}

bool update_ffmpeg_output(ConfigFile &config)
{
	if (config_has_user_value(config, "AdvOut", "FFOutputToFile"))
		return false;

	const char *url = config_get_string(config, "AdvOut", "FFURL");
	if (!url)
		return false;

	bool isActualURL = strstr(url, "://") != nullptr;
	if (isActualURL)
		return false;

	std::string urlStr = url;
	std::string  extension;

	for (size_t i = urlStr.length(); i > 0; i--) {
		size_t idx = i - 1;

		if (urlStr[idx] == '.') {
			extension = &urlStr[i];
		}

		if (urlStr[idx] == '\\' || urlStr[idx] == '/') {
			urlStr[idx] = 0;
			break;
		}
	}

	if (urlStr.empty() || extension.empty())
		return false;

	config_remove_value(config, "AdvOut", "FFURL");
	config_set_string(config, "AdvOut", "FFFilePath", urlStr.c_str());
	config_set_string(config, "AdvOut", "FFExtension", extension.c_str());
	config_set_bool(config, "AdvOut", "FFOutputToFile", true);
	return true;
}


void upgrade_settings(void)
{
	char path[512];
	std::string basic_profiles = app_name + "/basic/profiles";
	int pathlen = GetConfigPath(path, 512, basic_profiles.c_str());

	if (pathlen <= 0)
		return;
	if (!os_file_exists(path))
		return;

	os_dir_t *dir = os_opendir(path);
	if (!dir)
		return;

	struct os_dirent *ent = os_readdir(dir);

	while (ent) {
		if (ent->directory && strcmp(ent->d_name, ".") != 0 &&
			strcmp(ent->d_name, "..") != 0) {
			strcat(path, "/");
			strcat(path, ent->d_name);
			strcat(path, "/basic.ini");

			ConfigFile config;
			int ret;

			ret = config.Open(path, CONFIG_OPEN_EXISTING);
			if (ret == CONFIG_SUCCESS) {
				if (update_ffmpeg_output(config) || update_reconnect(config))
				{
					config_save_safe(config, "tmp",
						nullptr);
				}
			}


			if (config) {
				const char *sEnc = config_get_string(config,
					"AdvOut", "Encoder");
				const char *rEnc = config_get_string(config,
					"AdvOut", "RecEncoder");

				/* replace "cbr" option with "rate_control" for
				* each profile's encoder data */
				path[pathlen] = 0;
				strcat(path, "/");
				strcat(path, ent->d_name);
				strcat(path, "/recordEncoder.json");
				convert_14_2_encoder_setting(rEnc, path);

				path[pathlen] = 0;
				strcat(path, "/");
				strcat(path, ent->d_name);
				strcat(path, "/streamEncoder.json");
				convert_14_2_encoder_setting(sEnc, path);
			}

			path[pathlen] = 0;
		}

		ent = os_readdir(dir);
	}

	os_closedir(dir);
}



static bool do_mkdir(const char *path)
{
	if (os_mkdirs(path) == MKDIR_ERROR) {
		//OutputDebugString(NULL, _T("Failed to create directory %s"), path);
		return false;
	}

	return true;
}

//修改目录
bool CAgoraHQApp::MakeUserDirs()
{
	char path[512];
	std::string str_temp = app_name + "/basic";
	if (GetConfigPath(path, sizeof(path), str_temp.c_str()) <= 0)
		return false;
	if (!do_mkdir(path))
		return false;

	// 	str_temp = app_name + "/logs";
	// 	if (GetConfigPath(path, sizeof(path), str_temp.c_str()) <= 0)
	// 		return false;
	// 	if (!do_mkdir(path))
	// 		return false;

	str_temp = app_name + "/profiler_data";
	if (GetConfigPath(path, sizeof(path), str_temp.c_str()) <= 0)
		return false;
	if (!do_mkdir(path))
		return false;

#ifdef _WIN32
	// 	str_temp = app_name + "/crashes";
	// 	if (GetConfigPath(path, sizeof(path), str_temp.c_str()) <= 0)
	// 		return false;
	// 	if (!do_mkdir(path))
	// 
	// 		return false;

	str_temp = app_name + "/updates";
	if (GetConfigPath(path, sizeof(path), str_temp.c_str()) <= 0)
		return false;
	if (!do_mkdir(path))
		return false;
#endif

	str_temp = app_name + "/plugin_config";
	if (GetConfigPath(path, sizeof(path), str_temp.c_str()) <= 0)
		return false;
	if (!do_mkdir(path))
		return false;

	return true;
}


std::string GetProfileDirFromName(const char *name)
{
	std::string outputPath;
	os_glob_t *glob;
	char path[512];

	std::string basic_profiles = app_name + "/basic/profiles";
	if (GetConfigPath(path, sizeof(path), basic_profiles.c_str()) <= 0)
		return outputPath;

	strcat(path, "/*");

	if (os_glob(path, 0, &glob) != 0)
		return outputPath;

	for (size_t i = 0; i < glob->gl_pathc; i++) {
		struct os_globent ent = glob->gl_pathv[i];
		if (!ent.directory)
			continue;

		strcpy(path, ent.path);
		strcat(path, "/basic.ini");

		ConfigFile config;
		if (config.Open(path, CONFIG_OPEN_EXISTING) != 0)
			continue;

		const char *curName = config_get_string(config, "General",
			"Name");
		if (astrcmpi(curName, name) == 0) {
			outputPath = ent.path;
			break;
		}
	}

	os_globfree(glob);

	if (!outputPath.empty()) {
		replace(outputPath.begin(), outputPath.end(), '\\', '/');
		const char *start = strrchr(outputPath.c_str(), '/');
		if (start)
			outputPath.erase(0, start - outputPath.c_str() + 1);
	}

	return outputPath;
}


std::string GetSceneCollectionFileFromName(const char *name)
{
	std::string outputPath;
	os_glob_t *glob;
	char path[512];
	std::string basic_scenes = app_name + "/basic/scenes";
	if (GetConfigPath(path, sizeof(path), basic_scenes.c_str()) <= 0)
		return outputPath;

	strcat(path, "/*.json");

	if (os_glob(path, 0, &glob) != 0)
		return outputPath;

	for (size_t i = 0; i < glob->gl_pathc; i++) {
		struct os_globent ent = glob->gl_pathv[i];
		if (ent.directory)
			continue;

		obs_data_t *data =
			obs_data_create_from_json_file_safe(ent.path, "bak");
		const char *curName = obs_data_get_string(data, "name");

		if (astrcmpi(name, curName) == 0) {
			outputPath = ent.path;
			obs_data_release(data);
			break;
		}

		obs_data_release(data);
	}

	os_globfree(glob);

	if (!outputPath.empty()) {
		outputPath.resize(outputPath.size() - 5);
		std::replace(outputPath.begin(), outputPath.end(), '\\', '/');
		const char *start = strrchr(outputPath.c_str(), '/');
		if (start)
			outputPath.erase(0, start - outputPath.c_str() + 1);
	}

	return outputPath;
}


bool CAgoraHQApp::InitGlobalConfig()
{
	char path[512];
	bool changed = false;
	std::string global_ini = app_name + "\\global.ini";
	int len = GetConfigPath(path, sizeof(path),
		global_ini.c_str());
	if (len <= 0) {
		return false;
	}

	int errorcode = globalConfig.Open(path, CONFIG_OPEN_ALWAYS);
	if (errorcode != CONFIG_SUCCESS) {
		//OBSErrorBox(NULL, "Failed to open global.ini: %d", errorcode);
		return false;
	}

	if (!opt_starting_collection.empty()) {
		std::string path = GetSceneCollectionFileFromName(
			opt_starting_collection.c_str());
		if (!path.empty()) {
			config_set_string(globalConfig,
				"Basic", "SceneCollection",
				opt_starting_collection.c_str());
			config_set_string(globalConfig,
				"Basic", "SceneCollectionFile",
				path.c_str());
			changed = true;
		}
	}

	if (!opt_starting_profile.empty()) {
		std::string path = GetProfileDirFromName(
			opt_starting_profile.c_str());
		if (!path.empty()) {
			config_set_string(globalConfig, "Basic", "Profile",
				opt_starting_profile.c_str());
			config_set_string(globalConfig, "Basic", "ProfileDir",
				path.c_str());
			changed = true;
		}
	}

	if (!config_has_user_value(globalConfig, "General", "Pre19Defaults")) {
		uint32_t lastVersion = config_get_int(globalConfig, "General",
			"LastVersion");
		bool useOldDefaults = lastVersion &&
			lastVersion < MAKE_SEMANTIC_VERSION(19, 0, 0);

		config_set_bool(globalConfig, "General", "Pre19Defaults",
			useOldDefaults);
		changed = true;
	}

	if (!config_has_user_value(globalConfig, "General", "Pre21Defaults")) {
		uint32_t lastVersion = config_get_int(globalConfig, "General",
			"LastVersion");
		bool useOldDefaults = lastVersion &&
			lastVersion < MAKE_SEMANTIC_VERSION(21, 0, 0);

		config_set_bool(globalConfig, "General", "Pre21Defaults",
			useOldDefaults);
		changed = true;
	}

	if (changed)
		config_save_safe(globalConfig, "tmp", nullptr);

	return InitGlobalConfigDefaults();
}

#define DEFAULT_LANG "en-US"

bool CAgoraHQApp::InitGlobalConfigDefaults()
{
	config_set_default_string(globalConfig, "General", "Language",
		DEFAULT_LANG);
	config_set_default_uint(globalConfig, "General", "MaxLogs", 10);
	config_set_default_string(globalConfig, "General", "ProcessPriority",
		"Normal");
	config_set_default_bool(globalConfig, "General", "EnableAutoUpdates",
		false);//sbd

#if _WIN32
	config_set_default_string(globalConfig, "Video", "Renderer",
		"Direct3D 11");
#else
	config_set_default_string(globalConfig, "Video", "Renderer", "OpenGL");
#endif
	//add 停止推流报告
	config_set_bool(GetGlobalConfig(), "BasicWindow",
		"WarnBeforeStoppingStream", true);

	config_set_default_bool(globalConfig, "BasicWindow", "PreviewEnabled",
		true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"PreviewProgramMode", false);
	config_set_default_bool(globalConfig, "BasicWindow",
		"SceneDuplicationMode", true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"SwapScenesMode", true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"SnappingEnabled", true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"ScreenSnapping", true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"SourceSnapping", true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"CenterSnapping", false);
	config_set_default_double(globalConfig, "BasicWindow",
		"SnapDistance", 10.0);
	config_set_default_bool(globalConfig, "BasicWindow",
		"RecordWhenStreaming", false);
	config_set_default_bool(globalConfig, "BasicWindow",
		"KeepRecordingWhenStreamStops", false);
	config_set_default_bool(globalConfig, "BasicWindow",
		"SysTrayEnabled", true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"SysTrayWhenStarted", false);
	config_set_default_bool(globalConfig, "BasicWindow",
		"SaveProjectors", false);
	config_set_default_bool(globalConfig, "BasicWindow",
		"ShowTransitions", true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"ShowListboxToolbars", true);
	config_set_default_bool(globalConfig, "BasicWindow",
		"ShowStatusBar", true);

	if (!config_get_bool(globalConfig, "General", "Pre21Defaults")) {
		config_set_default_string(globalConfig, "General",
			"CurrentTheme", "Dark");
	}
	config_set_default_bool(globalConfig, "Audio", "DisableAudioDucking",
		true);
	return true;
}

bool check_path(const char* data, const char *path, std::string &output)
{
	std::ostringstream str;
	str << path << data;
	output = str.str();

	printf("Attempted path: %s\n", output.c_str());

	return os_file_exists(output.c_str());
}

bool GetDataFilePath(const char *data, std::string &output)
{
	if (check_path(data, "data/obs-studio/", output))
		return true;

	return check_path(data, OBS_DATA_PATH "/obs-studio/", output);
}

static std::vector<std::string> GetUserPreferredLocales()
{
	std::vector<std::string> result;

	ULONG num, length = 0;
	if (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num,
		nullptr, &length))
		return result;

	std::vector<wchar_t> buffer(length);
	if (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num,
		&buffer.front(), &length))
		return result;

	result.reserve(num);
	auto start = begin(buffer);
	auto end_ = end(buffer);
	decltype(start) separator;
	while ((separator = find(start, end_, 0)) != end_) {
		if (result.size() == num)
			break;

		char conv[MAX_PATH] = {};
		os_wcs_to_utf8(&*start, separator - start, conv, MAX_PATH);

		result.emplace_back(conv);

		start = separator + 1;
	}

	return result;
}

std::vector<std::pair<std::string, std::string>> GetLocaleNames()
{
	std::string path;
	if (!GetDataFilePath("locale.ini", path))
		throw "Could not find locale.ini path";

	ConfigFile ini;
	if (ini.Open(path.c_str(), CONFIG_OPEN_EXISTING) != 0)
		throw "Could not open locale.ini";

	size_t sections = config_num_sections(ini);

	std::vector<std::pair<std::string, std::string>> names;
	names.reserve(sections);
	for (size_t i = 0; i < sections; i++) {
		const char *tag = config_get_section(ini, i);
		const char *name = config_get_string(ini, tag, "Name");
		names.emplace_back(tag, name);
	}

	return names;
}


std::vector<std::string> GetPreferredLocales()
{
	std::vector<std::string> windows_locales = GetUserPreferredLocales();
	auto obs_locales = GetLocaleNames();
	auto windows_to_obs = [&obs_locales](std::string windows) {
		std::string lang_match;

		for (auto &locale_pair : obs_locales) {
			auto &locale = locale_pair.first;
			if (locale == windows.substr(0, locale.size()))
				return locale;

			if (lang_match.size())
				continue;

			if (locale.substr(0, 2) == windows.substr(0, 2))
				lang_match = locale;
		}

		return lang_match;
	};

	std::vector<std::string> result;
	result.reserve(obs_locales.size());

	for (const std::string &locale : windows_locales) {
		std::string match = windows_to_obs(locale);
		if (!match.size())
			continue;

		if (std::find(begin(result), end(result), match) != end(result))
			continue;

		result.emplace_back(match);
	}

	return result;
}


bool CAgoraHQApp::InitLocale()
{
	const char *lang = config_get_string(globalConfig, "General",
		"Language");

	locale = lang;

	std::string chinesePath;
	if (!GetDataFilePath("locale/" DEFAULT_LANG ".ini", chinesePath)) {
		//OBSErrorBox(NULL, "Failed to find locale/" DEFAULT_LANG ".ini");
		return false;
	}

	textLookup = text_lookup_create(chinesePath.c_str());
	if (!textLookup) {
		//OBSErrorBox(NULL, "Failed to create locale from file '%s'",
		//	englishPath.c_str());
		return false;
	}

	bool userLocale = config_has_user_value(globalConfig, "General",
		"Language");
	bool defaultLang = astrcmpi(lang, DEFAULT_LANG) == 0;

	if (userLocale && defaultLang)
		return true;

	if (!userLocale && defaultLang) {
		for (auto &locale_ : GetPreferredLocales()) {
			if (locale_ == lang)
				return true;

			std::stringstream file;
			file << "locale/" << locale_ << ".ini";

			std::string path;
			if (!GetDataFilePath(file.str().c_str(), path))
				continue;

			if (!text_lookup_add(textLookup, path.c_str()))
				continue;

			blog(LOG_INFO, "Using preferred locale '%s'",
				locale_.c_str());
			locale = locale_;
			return true;
		}

		return true;
	}

	std::stringstream file;
	file << "locale/" << lang << ".ini";

	std::string path;
	if (GetDataFilePath(file.str().c_str(), path)) {
		if (!text_lookup_add(textLookup, path.c_str()))
			blog(LOG_ERROR, "Failed to add locale file '%s'",
			path.c_str());
	}
	else {
		blog(LOG_ERROR, "Could not find locale file '%s'",
			file.str().c_str());
	}

	return true;
}


static void move_basic_to_profiles(void)
{
	char path[512];
	char new_path[512];
	os_glob_t *glob;

	/* if not first time use */
	std::string str_temp = app_name + "/basic";
	if (GetConfigPath(path, 512, str_temp.c_str()) <= 0)
		return;
	if (!os_file_exists(path))
		return;

	/* if the profiles directory doesn't already exist */
	str_temp = app_name + "/basic/profiles";
	if (GetConfigPath(new_path, 512, str_temp.c_str()) <= 0)
		return;
	if (os_file_exists(new_path))
		return;

	if (os_mkdir(new_path) == MKDIR_ERROR)
		return;

	strcat(new_path, "/");
	strcat(new_path, Str(untitled_name.c_str()));//Str("Untitled"));
	if (os_mkdir(new_path) == MKDIR_ERROR)
		return;

	strcat(path, "/*.*");
	if (os_glob(path, 0, &glob) != 0)
		return;

	strcpy(path, new_path);

	for (size_t i = 0; i < glob->gl_pathc; i++) {
		struct os_globent ent = glob->gl_pathv[i];
		char *file;

		if (ent.directory)
			continue;

		file = strrchr(ent.path, '/');
		if (!file++)
			continue;

		if (astrcmpi(file, "scenes.json") == 0)
			continue;

		strcpy(new_path, path);
		strcat(new_path, "/");
		strcat(new_path, file);
		os_rename(ent.path, new_path);
	}

	os_globfree(glob);
}

static void move_basic_to_scene_collections(void)
{
	char path[512];
	char new_path[512];

	std::string str_temp = app_name + "/basic";
	if (GetConfigPath(path, 512, str_temp.c_str()) <= 0)
		return;
	if (!os_file_exists(path))
		return;

	str_temp = app_name + "/basic/scenes";
	if (GetConfigPath(new_path, 512, str_temp.c_str()) <= 0)
		return;
	if (os_file_exists(new_path))
		return;

	if (os_mkdir(new_path) == MKDIR_ERROR)
		return;

	strcat(path, "/scenes.json");
	strcat(new_path, "/");
	strcat(new_path, Str(untitled_name.c_str())/*Str("Untitled")*/);
	strcat(new_path, ".json");

	os_rename(path, new_path);
}


static bool StartupOBS(const char *locale, profiler_name_store_t *store)
{
	char path[512];

	std::string puglin_config = app_name + "/plugin_config";
	if (GetConfigPath(path, sizeof(path), puglin_config.c_str()) <= 0)
		return false;

	return obs_startup(locale, path, store);
}


static bool MakeUserProfileDirs()
{
	char path[512];

	std::string basic_profiles = app_name + "/basic/profiles";
	if (GetConfigPath(path, sizeof(path), basic_profiles.c_str()) <= 0)
		return false;
	if (!do_mkdir(path))
		return false;

	std::string basic_scenes = app_name + "/basic/scenes";
	if (GetConfigPath(path, sizeof(path), basic_scenes.c_str()) <= 0)
		return false;
	if (!do_mkdir(path))
		return false;

	return true;
}
// 
// 
// static uint64_t convert_log_name(const char *name)
// {
// 	BaseLexer  lex;
// 	std::string     year, month, day, hour, minute, second;
// 
// 	lexer_start(lex, name);
// 
// 	if (!get_token(lex, year, BASETOKEN_DIGIT)) return 0;
// 	if (!expect_token(lex, "-", BASETOKEN_OTHER)) return 0;
// 	if (!get_token(lex, month, BASETOKEN_DIGIT)) return 0;
// 	if (!expect_token(lex, "-", BASETOKEN_OTHER)) return 0;
// 	if (!get_token(lex, day, BASETOKEN_DIGIT)) return 0;
// 	if (!get_token(lex, hour, BASETOKEN_DIGIT)) return 0;
// 	if (!expect_token(lex, "-", BASETOKEN_OTHER)) return 0;
// 	if (!get_token(lex, minute, BASETOKEN_DIGIT)) return 0;
// 	if (!expect_token(lex, "-", BASETOKEN_OTHER)) return 0;
// 	if (!get_token(lex, second, BASETOKEN_DIGIT)) return 0;
// 
// 	stringstream timestring;
// 	timestring << year << month << day << hour << minute << second;
// 	return std::stoull(timestring.str());
// }
// 
// 
// static void delete_oldest_file(const char *location)
// {
// 	BPtr<char>       logDir(GetConfigPathPtr(location));
// 	std::string           oldestLog;
// 	uint64_t         oldest_ts = (uint64_t)-1;
// 	struct os_dirent *entry;
// 
// 	unsigned int maxLogs = (unsigned int)config_get_uint(
// 		theApp.GlobalConfig(), "General", "MaxLogs");
// 
// 	os_dir_t *dir = os_opendir(logDir);
// 	if (dir) {
// 		unsigned int count = 0;
// 
// 		while ((entry = os_readdir(dir)) != NULL) {
// 			if (entry->directory || *entry->d_name == '.')
// 				continue;
// 
// 			uint64_t ts = convert_log_name(entry->d_name);
// 
// 			if (ts) {
// 				if (ts < oldest_ts) {
// 					oldestLog = entry->d_name;
// 					oldest_ts = ts;
// 				}
// 
// 				count++;
// 			}
// 		}
// 
// 		os_closedir(dir);
// 
// 		if (count > maxLogs) {
// 			std::stringstream delPath;
// 
// 			delPath << logDir << "/" << oldestLog;
// 			os_unlink(delPath.str().c_str());
// 		}
// 	}
// }



static auto ProfilerNameStoreRelease = [](profiler_name_store_t *store)
{
	profiler_name_store_free(store);
};

using ProfilerNameStore =
std::unique_ptr<profiler_name_store_t,
decltype(ProfilerNameStoreRelease)>;

ProfilerNameStore CreateNameStore()
{
	return ProfilerNameStore{ profiler_name_store_create(),
		ProfilerNameStoreRelease };
}

void CAgoraHQApp::OBSAppInit()
{
	upgrade_settings();
	os_inhibit_sleep_create("OBS Video/audio");
	if (!MakeUserDirs())
		throw "Failed to create required user directories";

	if (!InitGlobalConfig())
		throw "Failed to initialize global config";
	//可以不处理locale
	// 	if (!InitLocale())
	// 		throw "Failed to load locale;
	const char *lang = config_get_string(globalConfig, "General",
		"Language");

	locale = lang;

	config_set_default_string(globalConfig, "Basic", "Profile",
		Str(untitled_name.c_str())/*Str("Untitled")*/);
	config_set_default_string(globalConfig, "Basic", "ProfileDir",
		Str(untitled_name.c_str())/*Str("Untitled")*/);
	config_set_default_string(globalConfig, "Basic", "SceneCollection",
		Str(untitled_name.c_str())/*Str("Untitled")*/);
	config_set_default_string(globalConfig, "Basic", "SceneCollectionFile",
		Str(untitled_name.c_str())/*Str("Untitled")*/);

	move_basic_to_profiles();
	move_basic_to_scene_collections();

	if (!MakeUserProfileDirs())
		throw "Failed to create profile directories";

	if (!StartupOBS(locale.c_str(), nullptr))
		return;
	//delete_oldest_file();
}


const char *CAgoraHQApp::GetRenderModule() const
{
	const char *renderer = config_get_string(globalConfig, "Video",
		"Renderer");

	return (astrcmpi(renderer, "Direct3D 11") == 0) ?
	DL_D3D11 : DL_OPENGL;//宏定义
}

const char *CAgoraHQApp::InputAudioSource() const
{
	return INPUT_AUDIO_SOURCE;
}

const char *CAgoraHQApp::OutputAudioSource() const
{
	return OUTPUT_AUDIO_SOURCE;
}

void CAgoraHQApp::GetScaleAndCenterPos(
	int baseCX, int baseCY, int windowCX, int windowCY,
	int &x, int &y, float &scale)
{
	double windowAspect, baseAspect;
	int newCX, newCY;

	windowAspect = double(windowCX) / double(windowCY);
	baseAspect = double(baseCX) / double(baseCY);

	if (windowAspect > baseAspect) {
		scale = float(windowCY) / float(baseCY);
		newCX = int(double(windowCY) * baseAspect);
		newCY = windowCY;
	}
	else {
		scale = float(windowCX) / float(baseCX);
		newCX = windowCX;
		newCY = int(float(windowCX) / baseAspect);
	}

	x = windowCX / 2 - newCX / 2;
	y = windowCY / 2 - newCY / 2;
}

void CAgoraHQApp::GetCenterPosFromFixedScale(
	int baseCX, int baseCY, int windowCX, int windowCY,
	int &x, int &y, float scale)
{
	x = (float(windowCX) - float(baseCX)*scale) / 2.0f;
	y = (float(windowCY) - float(baseCY)*scale) / 2.0f;
}