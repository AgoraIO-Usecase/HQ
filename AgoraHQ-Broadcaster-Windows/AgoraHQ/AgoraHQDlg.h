
// AgoraHQDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "AGButton.h"
class CDlgAnswer;
class CDlgConfig;
class CDlgInput;
// CAgoraHQDlg �Ի���

class CAgoraHQDlg : public CDialogEx
{
// ����
public:
	CAgoraHQDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AGORAHQ_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	LRESULT onConnectionLost(WPARAM wParam, LPARAM lParam);

	LRESULT onNewChannelName(WPARAM wParam, LPARAM lParam);
	LRESULT onInviteRemoteAudience(WPARAM wParam,LPARAM lParam);
	LRESULT onInviteCallBackAccept(WPARAM wParam, LPARAM lParam);

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

	HWND m_wndRemote;
	uid_t m_nInviteRemote;

	std::map<uid_t, HWND> m_mapRemoteView;
	AG_INVITE_REMOTEAUDIENCE m_agInviteRemoteAudience;
};