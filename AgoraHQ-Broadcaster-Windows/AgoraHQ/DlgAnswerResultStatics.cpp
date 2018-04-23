// DlgAnswerResultStatics.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DlgAnswerResultStatics.h"
#include "afxdialogex.h"
#include "commonFun.h"

// CDlgAnswerResultStatics �Ի���

IMPLEMENT_DYNAMIC(CDlgAnswerResultStatics, CDialogEx)

CDlgAnswerResultStatics::CDlgAnswerResultStatics(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAnswerResultStatics::IDD, pParent)
{

}

CDlgAnswerResultStatics::~CDlgAnswerResultStatics()
{
}

void CDlgAnswerResultStatics::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANSWER_STATICS, m_ltrResult);
	DDX_Control(pDX, IDC_LIST_ListOfWinners, m_ltrListofWinners);
}

BEGIN_MESSAGE_MAP(CDlgAnswerResultStatics, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgAnswerResultStatics::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CDlgAnswerResultStatics::OnBnClickedButtonCancle)
END_MESSAGE_MAP()


// CDlgAnswerResultStatics ��Ϣ�������
BOOL CDlgAnswerResultStatics::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ltrResult.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	LONG lStyle;
	lStyle = GetWindowLong(m_ltrResult.m_hWnd, GWL_STYLE);
	lStyle &= ~LVS_TYPEMASK;
	lStyle |= LVS_REPORT;
	SetWindowLong(m_ltrResult.m_hWnd, GWL_STYLE, lStyle);

	m_ltrResult.InsertColumn(0, _T("QuestionID"), LVCFMT_CENTER, 60);
	m_ltrResult.InsertColumn(1, _T("Total"), LVCFMT_CENTER, 50);
	m_ltrResult.InsertColumn(2, _T("Correct"), LVCFMT_CENTER, 50);
	m_ltrResult.InsertColumn(3, _T("Wrong"), LVCFMT_CENTER, 50);
	m_ltrResult.InsertColumn(4, _T("Answer"), LVCFMT_CENTER, 50);
	m_ltrResult.InsertColumn(5, _T("A"), LVCFMT_CENTER, 50);
	m_ltrResult.InsertColumn(6, _T("B"), LVCFMT_CENTER, 50);
	m_ltrResult.InsertColumn(7, _T("C"), LVCFMT_CENTER, 50);
	m_ltrResult.InsertColumn(8, _T("D"), LVCFMT_CENTER, 50);
	m_ltrResult.InsertColumn(9, _T("rightPercent"), LVCFMT_CENTER, 75);

	m_ltrListofWinners.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	lStyle = GetWindowLong(m_ltrResult.m_hWnd, GWL_STYLE);
	lStyle &= ~LVS_TYPEMASK;
	lStyle |= LVS_REPORT;
	SetWindowLong(m_ltrListofWinners.m_hWnd, GWL_STYLE, lStyle);
	m_ltrListofWinners.InsertColumn(0, _T("PlayerID"), LVCFMT_CENTER, 120);
	m_ltrListofWinners.InsertColumn(1, _T("PlayerName"), LVCFMT_CENTER, 330);

	return TRUE;
}

void CDlgAnswerResultStatics::OnBnClickedButtonSave()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


void CDlgAnswerResultStatics::OnBnClickedButtonCancle()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}

void CDlgAnswerResultStatics::setContext(const tagQuestionStatics &questionStatics)
{
	m_ltrListofWinners.ShowWindow(SW_HIDE);
	m_ltrResult.ShowWindow(SW_SHOW);

	CString strQuestionId = s2cs(int2str(questionStatics.nsid + 1));
	CString strTotal = s2cs(int2str(questionStatics.nTotal));
	CString strCorrect = s2cs(int2str(questionStatics.ncorrectNum));
	CString strWrong = s2cs(int2str(questionStatics.nTotal - questionStatics.ncorrectNum));
	double dCorrertPercent = 0.0f;
	if (questionStatics.nTotal)
	double dCorrertPercent = 0.0f;
	if (questionStatics.nTotal)
	dCorrertPercent = 1.00 * (questionStatics.ncorrectNum) / (questionStatics.nTotal);
	CString strCorrectPercent;
	strCorrectPercent.Format(_T("%.2f"), dCorrertPercent);
	ASSERT(5 > questionStatics.nresult);
	CString strResutl; CString strAnswerA; CString strAnswerB; CString strAnswerC; CString strAnswerD;
	if (0 == questionStatics.nresult){
		strResutl = _T("A");
		strAnswerA = s2cs(int2str(questionStatics.mapSpread.at("A")));
	}
	else if (1 == questionStatics.nresult){
		strResutl = _T("B");
		strAnswerB = s2cs(int2str(questionStatics.mapSpread.at("B")));
	}
	else if (2 == questionStatics.nresult){
		strResutl = _T("C");
		strAnswerC = s2cs(int2str(questionStatics.mapSpread.at("C")));
	}
	else if (3 == questionStatics.nresult){
		strResutl = _T("D");
		strAnswerD = s2cs(int2str(questionStatics.mapSpread.at("D")));
	}
	
	int nCount = m_ltrResult.GetItemCount();
	m_ltrResult.InsertItem(nCount, _T(""));
	m_ltrResult.SetItemText(nCount, 0, strQuestionId);
	m_ltrResult.SetItemText(nCount, 1, strTotal);
	m_ltrResult.SetItemText(nCount, 2, strCorrect);
	m_ltrResult.SetItemText(nCount, 3, strWrong);
	m_ltrResult.SetItemText(nCount, 4, strResutl);
	m_ltrResult.SetItemText(nCount, 5, strAnswerA);
	m_ltrResult.SetItemText(nCount, 6, strAnswerB);
	m_ltrResult.SetItemText(nCount, 7, strAnswerC);
	m_ltrResult.SetItemText(nCount, 8, strAnswerD);
	m_ltrResult.SetItemText(nCount, 9, strCorrectPercent);

	Invalidate(TRUE);
}

void CDlgAnswerResultStatics::setContext(const std::vector<tagListOfWinners> &vecListOfWinners)
{
	m_ltrResult.DeleteAllItems();
	m_ltrResult.ShowWindow(SW_HIDE);
	m_ltrListofWinners.DeleteAllItems();
	m_ltrListofWinners.ShowWindow(SW_SHOW);
	
	int nCount = 1;
	for (std::vector<tagListOfWinners>::const_iterator it = vecListOfWinners.begin(); vecListOfWinners.end() != it; it++){
		 int nRow = m_ltrListofWinners.InsertItem(nCount, _T(""));
		CString sPlayerId = s2cs(int2str(it->nPlayerId));
		CString sPlayerName = s2cs(it->strPlayerName);
		m_ltrListofWinners.SetItemText(nRow, 0, sPlayerId);
		m_ltrListofWinners.SetItemText(nRow, 1, sPlayerName);
		nCount++;
	}
}