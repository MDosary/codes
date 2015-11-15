
// epiminfoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "atltime.h"
#include "atlcomtime.h"

typedef struct _TIMEMAP
{
	int nyear;
	int nmonth;
	int nday;
	int hour;
	int nminute;
	int nsecond;
}TimeMap,*PTimeMap;

// CepiminfoDlg �Ի���
class CepiminfoDlg : public CDialogEx
{
// ����
public:
	CepiminfoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_EPIMINFO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CString g_strFilePath;
	

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnChoose();
//	CComboBox m_yearbegin;
//	CComboBox m_month_begin;
//	CComboBox m_day_begin;
//	CComboBox m_yearend;
//	CComboBox m_monthend;
//	CComboBox m_monthbegin;
//	CComboBox m_daybegin;
//	CComboBox m_dayend;
	CListCtrl m_infolist;
	afx_msg void OnBnClickedBtnQuery();

	char* AnlyCategory(int nCategory);
	afx_msg void OnBnClickedBtnCount();
	int GetRuleCD(PTimeMap pStartTime, PTimeMap pEndTime);
	
	afx_msg void OnBnClickedBtnWeek();
	afx_msg void OnBnClickedBtnMonth();
	//int CountTime(COleDateTime TimeCurrent_Start, COleDateTime TimeCurrent_End, COleDateTime TimeLast_Start, COleDateTime TimeLast_End);
//	CTime m_query_timebegin;
//	CTime m_query_timeend;
	COleDateTime m_query_timebegin;
	COleDateTime m_query_timeend;
	afx_msg void OnBnClickedOutQuery();
	afx_msg void OnBnClickedOutAnly();
	afx_msg void OnBnClickedBtnWeekAnly();

	
	int CountDayRuleCD(COleDateTime DataTime);
	afx_msg void OnBnClickedBtnMonthAnly();
//	CListCtrl m_taskcd_order;
	CListCtrl m_list_taskcd;
};


