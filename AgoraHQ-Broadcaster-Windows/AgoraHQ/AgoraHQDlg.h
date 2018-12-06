
// AgoraHQDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "AGButton.h"
class CDlgAnswer;
class CDlgConfig;
class CDlgInput;
class CDlgSetTimeBonus;
// CAgoraHQDlg 对话框

//OBS for ExtCapture
#include "ExtendObserver/ExtendAudioFrameObserver.h"
#include "ExtendObserver/ExtendVideoFrameObserver.h"
#include <list>
#include "window-basic-main-outputs.hpp"
#include "window-agora-output.hpp"
#include "BasicPreviewWnd.h"//OBS for ExtCapture
template <typename OBSRef>
struct SignalContainer {
	OBSRef ref;
	std::vector<std::shared_ptr<OBSSignal> > handlers;
};
#define DESKTOP_AUDIO_1 Str("DesktopAudioDevice1")
#define DESKTOP_AUDIO_2 Str("DesktopAudioDevice2")
#define AUX_AUDIO_1     Str("AuxAudioDevice1")
#define AUX_AUDIO_2     Str("AuxAudioDevice2")
#define AUX_AUDIO_3     Str("AuxAudioDevice3")

#define SIMPLE_ENCODER_X264                    "x264"
#define SIMPLE_ENCODER_X264_LOWCPU             "x264_lowcpu"
#define SIMPLE_ENCODER_QSV                     "qsv"
#define SIMPLE_ENCODER_NVENC                   "nvenc"
#define SIMPLE_ENCODER_AMD                     "amd"
#define PREVIEW_EDGE_SIZE 0
std::string GetDefaultVideoSavePath();

//end

class CAgoraHQDlg : public CDialogEx
{
// 构造
public:
	CAgoraHQDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CAgoraHQDlg();
// 对话框数据
	enum { IDD = 102 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonHqConfig();
	afx_msg void OnBnClickedButtonMediaParam();
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonInvitemedia();
	afx_msg void OnBnClickedButtonSettimebonus();

	LRESULT onJoinChannelSuccess(WPARAM wParam,LPARAM lParam);
	LRESULT onWarning(WPARAM wParam,LPARAM lParam);
	LRESULT onError(WPARAM wParam, LPARAM lParam);
	LRESULT onLeaveChannel(WPARAM wParam,LPARAM lParam);
	LRESULT onRequestChannelKey(WPARAM wParam,LPARAM lParam);
	LRESULT onLastMileQuality(WPARAM wParam,LPARAM lParam);
	LRESULT onFirstLocalVideoFrame(WPARAM wParam,LPARAM lParam);
	LRESULT onFirstRemoteVideoDecoded(WPARAM wParam, LPARAM lParam);
	LRESULT onFirstRmoteVideoFrame(WPARAM wParam,LPARAM lParam);
	LRESULT onUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT onUserOff(WPARAM wParam, LPARAM lParam);
	LRESULT onUserMuteVideo(WPARAM wParam,LPARAM lParam);
	LRESULT onConnectionLost(WPARAM wParam, LPARAM lParam);

	LRESULT onNewChannelName(WPARAM wParam, LPARAM lParam);
	LRESULT onInviteRemoteAudience(WPARAM wParam,LPARAM lParam);
	LRESULT onInviteCallBackAccept(WPARAM wParam, LPARAM lParam);
	LRESULT onHttpInviteStatusSuccess(WPARAM wParam, LPARAM lParam);
protected:
	void initCtrl();
	void uninitCtrl();

	void initAgoraMediaRtc();
	void uninitAgoraMediaRtc();

	void DrawClient(CDC *lpDC);

private:

	CAGButton m_btnJoinChannel;
	CAGButton m_btnMediaParam;
	CAGButton m_btnHQConfig;
	CAGButton m_btnInviteRemote;
	CAGButton m_btnSetTimeBonus;
	CStatic m_ctlShowPic;
	CStatic m_ctlRemoteWnd;
	CStatic m_ctlAnswerArea;
	CStatic m_ctlSdkVersion;

	CImageList	m_imgNetQuality;

	CDlgAnswer *m_pDlgAnswer;
	std::string m_strAppId;
	std::string m_strAppCertificatId;
	UINT m_uId;
	std::string m_strChannelName;
	
	CAgoraObject* m_lpAgoraObject;
	IRtcEngine* m_lpRtcEngine;
	int m_nLastmileQuality;

	CDlgConfig* m_pDlgConfig;
	CDlgInput* m_pDlgInputParam;
	CDlgSetTimeBonus* m_pDlgSetTimeBonus;

	HWND m_wndRemote;
	uid_t m_nInviteRemote;

	std::map<uid_t, HWND> m_mapRemoteView;
	AG_INVITE_REMOTEAUDIENCE m_agInviteRemoteAudience;

	void JoinChannel_Agora();
	void LeaveChannel_Agora();
	CRect m_rcNetQuality;
	//OBS for ExtCapture
	void JoinChannel_OBS();
	void LeaveChannel_OBS();
	friend class CBasicPreviewWnd;
	void EnableOBSCtrl(bool bEnable);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	void          OBSInit();
	config_t      *Config() const;
	obs_service_t *GetService();
	void          SetService(obs_service_t *service);
	int           GetProfilePath(char *path, size_t size, const char *file) const;
	void          ResetOutputs();
	void          InitPrimitives();
	//transition
	void InitDefaultTransitions();
	void InitTransition(obs_source_t *transition);

	void TransitionToScene(OBSScene scene, bool force = false,
		bool direct = false);
	void TransitionToScene(OBSSource scene, bool force = false,
		bool direct = false, bool quickTransition = false);
	void TransitionFullyStopped();
	bool overridingTransition = false;
	//obs_source_t *FindTransition(const char *name);
	//void CreateDefaultQuickTransitions();
	void SetTransition(OBSSource transition);
	OBSWeakSource lastScene;
	OBSWeakSource swapScene;
	OBSWeakSource programScene;
	obs_source_t *fadeTransition;
private:
	void          InitOBSPreview();
	bool          InitService();
	bool          LoadService();
	bool          InitBasicConfigDefaults();
	bool          InitBasicConfig();
	void          GetFPSCommon(uint32_t &num, uint32_t &den) const;
	void GetFPSInteger(uint32_t &num, uint32_t &den) const;
	void GetFPSFraction(uint32_t &num, uint32_t &den) const;
	void GetFPSNanoseconds(uint32_t &num, uint32_t &den) const;
	void GetConfigFPS(uint32_t &num, uint32_t &den) const;
	void CheckForSimpleModeX264Fallback();

	SIZE GetPreviewRect();
	/* OBS Callbacks */
	static void SceneReordered(void *data, calldata_t *params);
	static void SceneItemAdded(void *data, calldata_t *params);
	static void SceneItemRemoved(void *data, calldata_t *params);
	static void SceneItemSelected(void *data, calldata_t *params);
	static void SceneItemDeselected(void *data, calldata_t *params);
	static void SourceLoaded(void *data, obs_source_t *source);
	static void SourceRemoved(void *data, calldata_t *params);
	static void SourceActivated(void *data, calldata_t *params);
	static void SourceDeactivated(void *data, calldata_t *params);
	static void SourceRenamed(void *data, calldata_t *params);
public:
	int           ResetVideo();
	bool          ResetAudio();
	void          ResetAudioDevice(const char *sourceId, const char *deviceId,
		const char *deviceDesc, int channel);

	void RefreshSceneCollections();

	void          DrawBackdrop(float cx, float cy);

	inline bool IsPreviewProgramMode() const
	{
		return os_atomic_load_bool(&previewProgramMode);
	}

	void          Load(const char *file);
	void          CreateFirstRunSources();
	void          CreateDefaultScene(bool firstStart);
	void          ClearSceneData();
	void          AddScene(OBSSource source);
	void SetCurrentScene(obs_scene_t *scene, bool force = false,
		bool direct = false);
	void SetCurrentScene(OBSSource scene, bool force = false,
		bool direct = false);

	void AddSource(const char* id);
	bool StartStreaming();
	void StopStreaming();
	bool IsStreaming();
	OBSScene GetCurrentScene();
	OBSSceneItem  GetCurrentSceneItem();

	void UpdateSceneSelection(OBSSource source);
	//OBS for ExtCapture
	bool IsPreviewFixedScaling(){ return m_wndLocal.IsFixedScaling(); }
	float GetPreviewScalingAmount(){ return m_wndLocal.GetScalingAmount(); }
	//* GetParentDialog();
	//end
private:
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);
	//static void obsVideoCallback(uint8_t* data, void* param);
	//static void obsAudioCallback(struct audio_data* data, int planes, void* param);
	void DrawSceneEditing(){ m_wndLocal.DrawSceneEditing(); }
	void ResizePreview(uint32_t cx, uint32_t cy);
	void RemoveAllSource();
	void RemoveSelectedSceneItem();
	ConfigFile    basicConfig;
	OBSService service;
	std::unique_ptr<BasicOutputHandler> outputHandler;
	bool streamingStopping = false;

	gs_vertbuffer_t *box = nullptr;
	gs_vertbuffer_t *boxLeft = nullptr;
	gs_vertbuffer_t *boxTop = nullptr;
	gs_vertbuffer_t *boxRight = nullptr;
	gs_vertbuffer_t *boxBottom = nullptr;
	gs_vertbuffer_t *circle = nullptr;


	bool loaded = false;
	long disableSaving = 1;
	volatile bool previewProgramMode = false;
	//obs_scene_t  *scene;
	OBSScene default_scene;
	//设置视频输出参数
	int obs_output_x = 640;
	int obs_output_y = 360;
	int obs_videoBitrate = 400;
	std::string obs_video_fps = "15";
	int obs_fps = 15;
	VIDEO_PROFILE_TYPE obs_video_profile_type = VIDEO_PROFILE_360P_4;

	std::list<std::string> m_lstOBSSources;
	//std::vector<std::string> m_vecOBSSources;
	CBasicPreviewWnd m_wndLocal; //OBS for ExtCapture
	RECT m_rcWndLocal;
	CRect m_rcRemote;
public:

	int           previewX = 0, previewY = 0;
	int           previewCX = 0, previewCY = 0;
	float         previewScale = 0.0f;
	afx_msg void OnBnClickedButtonObsImage();
	afx_msg void OnBnClickedButtonObsCamera();

	afx_msg void OnBnClickedButtonObsFfmpegMedia();
	CButton m_chkOBS;
	CButton m_btnOBSImage;
	CButton m_btnOBSCamera;
	CButton m_btnOBSFFmpeg;
	afx_msg void OnClickedCheckObs();

	public:
		//agora
		obs_service_t *GetAgoraService();
		void InitAgoraService();
		void InitAgoraServiceSettings();
		void SetAgoraService(obs_service_t* service);
		void ResetAgoraOutput();

private:
	OBSService agoraService;
	std::unique_ptr<AgoraOutputHandler> agoraOutputHandler;

	std::string agoraColorFormat = "I420";
	//std::string obsColorFormatReplacedByAgora = "NV12";
	HWND remoteVideo = nullptr;
	std::list<uint32_t> m_lstUids;
	uint32_t remote_uid = 0;
	uint32_t loacal_uid = 0;
private:

	static void obsVideoCallback(uint8_t* data, void* param);
	static void obsAudioCallback(struct encoder_frame* data, int planes, void* param);

	CExtendVideoFrameObserver	m_exCapVideoFrameObserver;
	CExtendAudioFrameObserver	m_exCapAudioFrameObserver;
	// end agora
	CString source_tile = _T("Source Name");
	CString source_info = _T("Please input the name of source");
	// filters

};