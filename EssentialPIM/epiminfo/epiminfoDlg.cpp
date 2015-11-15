
// epiminfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "epiminfo.h"
#include "epiminfoDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <assert.h>
#include  <math.h> 
//#include <xstring>
#include <string>
#include <windows.h>
#include "core/ibpp.h"

using namespace std; 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define QUERY_NORMAL 0 
#define QUERY_WEEK 1
#define QUERY_MONTH 2

int g_query_flag = 0;


//firebird������ݿ������Ϣ

const char* g_szServName = "localhost";
const char* g_szDBUserName = "SYSDBA";
const char* g_szDBPassWord = "masterkey";

//����ȫ�����飬��Ϊͳ��ʱ�������

//�����ʱ������һά������ţ���ά��������ʱ��
int g_index_cd[1000];
//����������һά�������࣬��ά�������
int g_index_category[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

int ncount = 0;

//ͳ���ܹ�ʱ


//�ܹ�ʱ
float g_fTaskAllCd = 0.0;


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CepiminfoDlg �Ի���



CepiminfoDlg::CepiminfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CepiminfoDlg::IDD, pParent)
	, m_query_timebegin(COleDateTime::GetCurrentTime())
	, m_query_timeend(COleDateTime::GetCurrentTime())
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CepiminfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_CBO_YEAR_BEGIN, m_yearbegin);
	//  DDX_Control(pDX, IDC_CBO_MONTH_BEGIN, m_month_begin);
	//  DDX_Control(pDX, IDC_CBO_DAY_BEGIN, m_day_begin);
	//DDX_Control(pDX, IDC_CBO_YEAR_END, m_yearend);
	//DDX_Control(pDX, IDC_CBO_MONTH_END, m_monthend);
	//DDX_Control(pDX, IDC_CBO_MONTH_BEGIN, m_monthbegin);
	//DDX_Control(pDX, IDC_CBO_DAY_BEGIN, m_daybegin);
	//DDX_Control(pDX, IDC_CBO_DAY_END, m_dayend);
	DDX_Control(pDX, IDC_LIST_QUERY, m_infolist);
	//  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_BEGIN, m_query_timebegin);
	//  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_query_timeend);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_BEGIN, m_query_timebegin);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_query_timeend);
	//  DDX_Control(pDX, IDC_LIST_CD_ORDER, m_taskcd_order);
	DDX_Control(pDX, IDC_LIST_CD_ORDER, m_list_taskcd);
}

BEGIN_MESSAGE_MAP(CepiminfoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CHOOSE, &CepiminfoDlg::OnBnClickedBtnChoose)
	ON_BN_CLICKED(IDC_BTN_QUERY, &CepiminfoDlg::OnBnClickedBtnQuery)
	ON_BN_CLICKED(IDC_BTN_COUNT, &CepiminfoDlg::OnBnClickedBtnCount)
	ON_BN_CLICKED(IDC_BTN_WEEK, &CepiminfoDlg::OnBnClickedBtnWeek)
	ON_BN_CLICKED(IDC_BTN_MONTH, &CepiminfoDlg::OnBnClickedBtnMonth)
	ON_BN_CLICKED(IDC_OUT_QUERY, &CepiminfoDlg::OnBnClickedOutQuery)
	ON_BN_CLICKED(IDC_OUT_ANLY, &CepiminfoDlg::OnBnClickedOutAnly)
	ON_BN_CLICKED(IDC_BTN_WEEK_ANLY, &CepiminfoDlg::OnBnClickedBtnWeekAnly)
	ON_BN_CLICKED(IDC_BTN_MONTH_ANLY, &CepiminfoDlg::OnBnClickedBtnMonthAnly)
END_MESSAGE_MAP()


// CepiminfoDlg ��Ϣ�������

BOOL CepiminfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//��ʼ��combo���
	//��ݳ�ʼ��
	/*
	m_yearbegin.AddString("2012");
	m_yearbegin.AddString("2013");
	m_yearbegin.AddString("2014");
	m_yearbegin.AddString("2015");
	m_yearbegin.AddString("2016");

	m_yearend.AddString("2012");
	m_yearend.AddString("2013");
	m_yearend.AddString("2014");
	m_yearend.AddString("2015");
	m_yearend.AddString("2016");

	//�·ݳ�ʼ��
	for (int i = 0; i < 12; i++)
	{
		CString strmonth;
		strmonth.Format("%d",i+1);
		//m_monthbegin.AddString(strmonth);
		//m_monthend.AddString(strmonth);
		m_monthbegin.InsertString(i, strmonth);
		m_monthend.InsertString(i, strmonth);
	}

	//���ڳ�ʼ��
	for (int j = 0; j <31; j++)
	{
		CString strday;
		strday.Format("%d",j+1);
		//m_daybegin.AddString(strday);
		//m_dayend.AddString(strday);
		m_daybegin.InsertString(j, strday);
		m_dayend.InsertString(j, strday);
	}
	*/
	
	//��ʼ���б��
	m_infolist.InsertColumn(0, "��", LVCFMT_LEFT, 25);
	m_infolist.InsertColumn(1, "��ʼʱ��", LVCFMT_LEFT,100);
	m_infolist.InsertColumn(2, "����ʱ��", LVCFMT_LEFT, 100);
	m_infolist.InsertColumn(3, "����", LVCFMT_LEFT,250);
	m_infolist.InsertColumn(4, "����", LVCFMT_LEFT, 45);
	m_infolist.InsertColumn(5, "���ȼ�", LVCFMT_LEFT, 40);
	m_infolist.InsertColumn(6, "��ɶ�", LVCFMT_LEFT, 40);
	m_infolist.InsertColumn(7, "��ʱ/m", LVCFMT_LEFT, 50);
	

	//��ʱ�б�
	m_list_taskcd.InsertColumn(0, "����", LVCFMT_LEFT, 150);
	m_list_taskcd.InsertColumn(1,"��ʱ", LVCFMT_LEFT, 30);

//	WinExec("E:\command\PIM\epiminfo\epiminfo\bin\MsgBox.exe",SW_SHOWNORMAL);
	//����������
	//ShellExecute(this->m_hWnd, "open","bin\\fbserver.exe","","",SW_HIDE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CepiminfoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CepiminfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
		//addd
		//CPaintDC dc(this); // device context for painting
		
		

	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CepiminfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//ѡ����Ӧ���ݿ��ļ�
void CepiminfoDlg::OnBnClickedBtnChoose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	//CString strFile ;
	CFileDialog  dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.epim)|*.epim|All Files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal())
	{
		g_strFilePath = dlgFile.GetPathName();
	}
	SetDlgItemText(IDC_EDIT_FILEPATH, g_strFilePath);
	//g_szDBPath = (LPSTR)(LPCSTR)strFilePath;

	
}



BOOL Utf82Unicode(string utf8, wstring &wstmp)
{
	//wstring wstmp;
	int nLen = MultiByteToWideChar(CP_UTF8, 0, (LPSTR)utf8.data(), utf8.size(), NULL, 0);  
	if(nLen <=0)
	{
		return FALSE;
	}
	wstmp.resize(nLen, 0);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, (LPSTR)utf8.data(), utf8.size(), (LPWSTR)wstmp.c_str(), nLen);
	if(nResult != nLen)  
	{  
		return FALSE;  
	}
	return TRUE;
}


//��char* ת��wchar_t*��ʵ�ֺ������£�

//���ǰ�asii�ַ�ת��Ϊunicode�ַ�����������ͬ��ԭ��


/*
BOOL Utf8ToAsci(char *szUtf8, char *szAnsi, DWORD dwAnsiSize)
{
	wchar_t szUnicode[1024] = {0};
	DWORD dwUnicodeSize = 1024;

	if(MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, szUnicode, dwUnicodeSize) > 1)
	{
		
		int len= WideCharToMultiByte(CP_OEMCP,0,szUnicode,-1/*wcslen(szUnicode),NULL,0,NULL,NULL);
		szAnsi = (char* )calloc(len+1, sizeof(char));
		memset(szAnsi, 0, len*sizeof(char));
		WideCharToMultiByte(CP_OEMCP, 0, szUnicode, -1, szAnsi, len, NULL, NULL);
		//char chr[124];
		//WideCharToMultiByte( CP_ACP, 0, szUnicode, -1,  
			//chr, 124, NULL, NULL );  
		return TRUE;
	}
	else
	{
		return FALSE;
	}   
}
*/
BOOL WStringToString(const std::wstring &wstr,std::string &str)
	  {    
		      int nLen = (int)wstr.length()*2;    
		      str.resize(nLen,' ');
		  
			      int nResult = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)wstr.c_str(),nLen,(LPSTR)str.c_str(),nLen,NULL,NULL);
		  
			      if (nResult == 0)
		      {
				          return FALSE;
				      }
		  
			      return TRUE;
		  }




//ͳ�ƹ��ܰ�ťģ�飬����ͳ���б�
void CepiminfoDlg::OnBnClickedBtnQuery()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//����б�
	m_infolist.DeleteAllItems();
	//����ʱ���ѯ����
	//UpdateData(TRUE);


	//time begin
	/*
	m_yearbegin.GetWindowTextA(strYear);
	m_monthbegin.GetWindowTextA(strMonth);
	m_daybegin.GetWindowTextA(strDay);
	*/

	CString  strTimeBuf;
	CString strTimeBegin;
	GetDlgItemTextA(IDC_DATETIMEPICKER_BEGIN,strTimeBuf);

	strTimeBegin.Format("%s 0:0:0",strTimeBuf);
	
	//time end
	/*
	m_yearend.GetWindowTextA(strYear);
	m_monthend.GetWindowTextA(strMonth);
	m_dayend.GetWindowTextA(strDay);
	*/

	GetDlgItemTextA(IDC_DATETIMEPICKER_END, strTimeBuf);
	CString strTimeEnd;
	
	strTimeEnd.Format("%s 0:0:0",strTimeBuf); 

	//��ѯ���
	CString strQuery;
	strQuery.Format("select STARTTIME,TITLE,ENDTIME,CATEGORY, PRIORITY, COMPLETION from TASKS where STARTTIME between '%s' and '%s' order by STARTTIME", 
		strTimeBegin, strTimeEnd
					);
	// ȡ���ݿ�·��
	

	//�жϲ����Ƿ���ȷ
	if (g_strFilePath.IsEmpty())
	{
		AfxMessageBox("�����Ӧ�����Ƿ���ȷ!");
		return;
	}

	
	char* szQuery = (LPSTR)(LPCTSTR)strQuery;
	char* szDBPath = (LPSTR)(LPCTSTR)g_strFilePath;
	
	//�������ݿ�
	IBPP::Database db;
	db = IBPP::DatabaseFactory(g_szServName, szDBPath, g_szDBUserName, g_szDBPassWord);
	db->Connect();

	IBPP::Transaction tr = IBPP::TransactionFactory(db);
	tr->Start();
	//char* szQuery = (LPSTR)(LPCTSTR)strQuery;
	try
	{
		IBPP::Statement st = IBPP::StatementFactory(db, tr);
		st->Execute(szQuery);
		//st->Execute("select STARTTIME,TITLE,ENDTIME,CATEGORY, PRIORITY, COMPLETION from TASKS where STARTTIME between '2015/7/7 0:0:0' and '2015/7/10 0:0:0' order by STARTTIME");
		ncount = 0;
		PTimeMap pStartTimeMap, pEndTimeMap;
		//pStartTimeMap = NULL;
		//pEndTimeMap = NULL;
		pStartTimeMap = (PTimeMap)malloc(sizeof(TimeMap));
		pEndTimeMap = (PTimeMap)malloc(sizeof(TimeMap));

		//��ʾ
		while (st->Fetch())
		{
			//�����б����
			
			CString strCount;
			strCount.Format("%d", ncount);
			m_infolist.InsertItem(ncount, strCount);

			//��ѯ���ݿ⣬�����µ���ѯ�б���
			IBPP::Timestamp timemap;
			CString strStartTime, strEndTime;
		

			st->Get("STARTTIME", timemap);
			pStartTimeMap->nyear = timemap.Year();
			pStartTimeMap->nmonth = timemap.Month();
			pStartTimeMap->nday = timemap.Day();
			pStartTimeMap->hour = timemap.Hours();
			pStartTimeMap->nminute = timemap.Minutes();
			pStartTimeMap->nsecond = timemap.Seconds();
			
			strStartTime.Format("%d/%d/%d  %d:%d:%d",timemap.Year(), timemap.Month(),
				timemap.Day(), timemap.Hours(), timemap.Minutes(),timemap.Seconds());

			st->Get("ENDTIME", timemap);
			pEndTimeMap->nyear = timemap.Year();
			pEndTimeMap->nmonth = timemap.Month();
			pEndTimeMap->nday = timemap.Day();
			pEndTimeMap->hour = timemap.Hours();
			pEndTimeMap->nminute = timemap.Minutes();
			pEndTimeMap->nsecond = timemap.Seconds();

			strEndTime.Format("%d/%d/%d  %d:%d:%d", timemap.Year(), timemap.Month(),
				timemap.Day(), timemap.Hours(), timemap.Minutes(),timemap.Seconds());
			//�����ڲ�����
			m_infolist.SetItemText(ncount, 1, strStartTime);
			m_infolist.SetItemText(ncount, 2, strEndTime);
			
			//�������⣬����ʾ����ѯ�б���
			//////////////////////////////////////////////////////////////////////////
			string szTemp;
			std::wstring szOut;
			//wstring wszTemp;
			//string buf;
			st->Get("TITLE", szTemp);
			Utf82Unicode(szTemp,szOut);
			WStringToString(szOut,szTemp);
			//buf = WStringToStringStd(szOut);
			CString strTitle(szTemp.c_str());
			m_infolist.SetItemText(ncount, 3, strTitle);

			//��������
			int nCategory = 0;
			st->Get("CATEGORY", nCategory);
			//ת��Ϊ��Ӧ�ķ���
			m_infolist.SetItemText(ncount, 4, AnlyCategory(nCategory));

			//�������ȼ�
			int nPriority = 0;
			CString strPriority;
			st->Get("PRIORITY",nPriority);
			strPriority.Format("%d",nPriority);
			m_infolist.SetItemText(ncount, 5, strPriority);

			//����״̬
			int nCompletion = 0;
			CString strCompletion;
			st->Get("COMPLETION", nCompletion);
			strCompletion.Format("%d", nCompletion);
			m_infolist.SetItemText(ncount, 6, strCompletion);
			
			//���������ʱ����
			//��������ʱ
			
			
			int nRuleCD = GetRuleCD(pStartTimeMap, pEndTimeMap);
			//�������ʱ��ӵ�ȫ�������б���
			g_index_cd[ncount] = nRuleCD;
			CString strRuleCD;
			if (nRuleCD == -1)
			{
				strRuleCD.Format("+��");
			}
			else
			{
				strRuleCD.Format("%0.1f", nRuleCD*1.0/60);
			}
			m_infolist.SetItemText(ncount, 7, strRuleCD);

			//ͳ���ܹ�ʱ
			//�ж�ʱ���Ƿ�������ص�
			


			//����ȫ�����飬�ڲ�ѯ��ʱ������һ��ͳ�Ʊ�
			
			//�ַ������
			strCount.Empty();
			strStartTime.Empty();
			strEndTime.Empty();
			strTitle.Empty();
			strCompletion.Empty();
			strPriority.Empty();
			strRuleCD.Empty();
			ncount++;
			

		}

		tr->Commit();
	}
	catch(IBPP::SQLException &e)
	{
		AfxMessageBox(e.what());
		tr->Rollback();
	}
}




char* CepiminfoDlg::AnlyCategory(int nCategory)
{
	char* szCateGory;
	switch (nCategory)
	{
	case 0:	szCateGory = "��";
		g_index_category[0]++;
		break;
	case 1: szCateGory = "��Ҫ";
		g_index_category[1]++;
		break;
	case 2: szCateGory = "����";
		g_index_category[2]++;
		break;
	case 3: szCateGory = "����";
		g_index_category[3]++;
		break;
	case 4: szCateGory = "����";
		g_index_category[4]++;
		break;
	case 5: szCateGory = "�����ϯ";
		g_index_category[5]++;
		break;
	case 6: szCateGory = "��Ҫ����";
		g_index_category[6]++;
		break;
	case 7: szCateGory = "��Ҫ׼��";
		g_index_category[7]++;
		break;
	case 8: szCateGory = "�������";
		g_index_category[8]++;
		break;
	case 9: szCateGory = "�绰֪ͨ";
		g_index_category[9]++;
		break;
	case 10: szCateGory = "����";
		g_index_category[10]++;
		break;
	default: szCateGory = "����";
		g_index_category[11]++;
		break;
	}
	return szCateGory;
}

//����״ͼ����




//�õ��������ֵ���±�


#define RADIAN 3.1415926/180.0
//ͳ�ƴ��룬��Ҫ������״ͼ�ͱ�״ͼ
//��״ͼ�����ɺ�ʱ����ǰ5������
//��״ͼ�����շ�������ͳ�Ʊ���
void CepiminfoDlg::OnBnClickedBtnCount()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	UpdateData(TRUE);
	m_list_taskcd.DeleteAllItems();
	if (g_strFilePath.IsEmpty())
	{
		AfxMessageBox("�����Ӧ�����Ƿ���ȷ!");
		return;
	}

	// �õ�������ռ����

	int nBuf[12];
	int nTaskCD[1000];
	memset(nBuf, 0, 12);
	memset(nTaskCD,0 ,1000);
	
	int i,j,temp1 = 0,temp2 = 0;


	for (i = 0; i <= ncount; i++)
	{
		nTaskCD[i] = i;
	}
	for (i = 0; i <= ncount; i++)
	{
		for (j = 0; j <= ncount; j++)
		{
			if (g_index_cd[i] > g_index_cd[j])
			{
				temp1 = g_index_cd[i];
				g_index_cd[i] = g_index_cd[j];
				g_index_cd[j] = temp1;

				temp2 = nTaskCD[i];
				nTaskCD[i] = nTaskCD[j];
				nTaskCD[j] = temp2;
			}
		}
	}

	//��������ʱͳ���б�
	//��ʾ����
	/*
	pdc->TextOut(670,820,m_infolist.GetItemText(nTaskCD[0],3));
	pdc->TextOut(670,740,m_infolist.GetItemText(nTaskCD[1],3));
	pdc->TextOut(670,660,m_infolist.GetItemText(nTaskCD[2],3));
	pdc->TextOut(670,580,m_infolist.GetItemText(nTaskCD[3],3));
	pdc->TextOut(670,500,m_infolist.GetItemText(nTaskCD[4],3));
	pdc->TextOut(670,420,m_infolist.GetItemText(nTaskCD[4],3));
	*/
	
	for (int i = 0; i < 9; i++)
	{
		m_list_taskcd.InsertItem(i, m_infolist.GetItemText(nTaskCD[i], 3));
		m_list_taskcd.SetItemText(i,1, m_infolist.GetItemText(nTaskCD[i],7));
	}

	
	
	temp1 = 0;
	temp2 = 0;



	int nMaxCount = 0;
	for (i = 0; i <= 11; i++)
	{
		nMaxCount += g_index_category[i];
		nBuf[i] = i;
	}
	for (i = 0; i <= 11; i++)
	{
		for (j = 0; j <= 11; j++)
		{
			if (g_index_category[i] > g_index_category[j])
			{
				temp1 = g_index_category[i];
				g_index_category[i] = g_index_category[j];
				g_index_category[j] = temp1;

				temp2 = nBuf[i];
				nBuf[i] = nBuf[j];
				nBuf[j] = temp2;

			}
		}
	}

	



	//������״ͼ
	float rule1,rule2,rule3,rule4,rule_else;
	rule1 = g_index_category[0]*1.0/nMaxCount;
	rule2 = g_index_category[1]*1.0/nMaxCount;
	rule3 = g_index_category[3]*1.0/nMaxCount;
	rule4 = g_index_category[4]*1.0/nMaxCount;
	rule_else = 1.0 - rule1 - rule2 - rule3 -rule4;
	//�õ����ʱ�������б�
	
	


	

	//���շ���ͳ��
	int nTaskCdWeek[7];
	int nTaskCdMonth[4];
	
	//���շ���ͳ��
	float fTaskHourWeek[7];
	float fTaskHourMonth[4];
	float fTaskHourNormal[100];
	
	//����ÿ��Ĺ���ʱ��
	if (g_query_flag ==  QUERY_WEEK)
	{
		COleDateTime timeDayOfWeek(COleDateTime::GetTickCount());
		for (int i = 0; i <= 6 ; i++)
		{
			//nTaskCdWeek[i] = CountDayRuleCD(timeDayOfWeek);
			fTaskHourWeek[i] = CountDayRuleCD(timeDayOfWeek)*1.0/60;
			g_fTaskAllCd += fTaskHourWeek[i];
			timeDayOfWeek -= COleDateTimeSpan(1,0,0,0);

		}
	}
	else if (g_query_flag ==  QUERY_MONTH)
	{
		COleDateTime timeDayOfMonth(COleDateTime::GetTickCount());
		memset(fTaskHourMonth, 0, sizeof(fTaskHourMonth));
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j <= 6; j++)
			{
				//nTaskCdMonth[j] = CountDayRuleCD(timeDayOfMonth);
				fTaskHourMonth[i] += CountDayRuleCD(timeDayOfMonth)*1.0/60;
				timeDayOfMonth -= COleDateTimeSpan(1,0,0,0);
			}
			g_fTaskAllCd += fTaskHourMonth[i];
		}
	}
	else
	{
		//��ͨ��ѯ
		//�õ���ѯ����
		COleDateTimeSpan nQueryDay;
		nQueryDay = m_query_timeend - m_query_timebegin;
		int nDay = nQueryDay.GetTotalDays();
		COleDateTime timeDayOfNormal = m_query_timeend;
		memset(fTaskHourNormal, 0, sizeof(fTaskHourNormal));
		for (int i = 0; i < nDay;  i++)
		{
			fTaskHourNormal[i] = CountDayRuleCD(timeDayOfNormal)*1.0/60;
			g_fTaskAllCd += fTaskHourNormal[i];
			timeDayOfNormal -= COleDateTimeSpan(1,0,0,0);
			
		}

	}
	
	CString strAllCd;
	strAllCd.Format("%0.1f", g_fTaskAllCd);
	SetDlgItemText(IDC_EDT_ALLCD, strAllCd);
	


	CRect   rectClient;   
	CWnd* pWin = GetDlgItem(IDC_PAINT);
	pWin->GetClientRect(rectClient);

	//GetDlgItem(IDC_PAINT)->GetClientRect(rectClient);  
	CDC *pdc;
	CString aa,bb;
	pdc = pWin->GetDC();
	pdc->Rectangle(rectClient);
	//pdc=GetDlgItem(IDC_PAINT)->GetDC();
	


	LOGFONT log;
	CFont font;
	log.lfHeight=0;
	log.lfWidth = 15;
	log.lfEscapement=0;
	log.lfItalic=0;
	log.lfUnderline=0;
	log.lfStrikeOut=0;
	log.lfCharSet = DEFAULT_CHARSET;
	//	pdc.SelectObject(&font); 
	font.CreateFontIndirect(&log);
	//����
	CPen pen(PS_SOLID,1,RGB(0,0,0));         

	pdc->SelectObject(&pen);
	//�߼�����
	SetViewportOrgEx(*pdc,20,250,NULL);
	pdc->SetMapMode(MM_LOMETRIC);



	if (g_query_flag == QUERY_WEEK)
	{
		//weekͳ�ƣ�����״ͼ
		pdc->TextOut(100,-10,"0");
		pdc->TextOut(200,-10,"-1");
		pdc->TextOut(300,-10,"-2");
		pdc->TextOut(400,-10,"-3");
		pdc->TextOut(500,-10,"-4");
		pdc->TextOut(600,-10,"-5");
		pdc->TextOut(700,-10,"-6");

		pdc->MoveTo(0,0);
		pdc->LineTo(0,100);

		pdc->LineTo(-10,800);
		//pdc->MoveTo(0,500);
		//pdc->LineTo(10,700);
		CString str;
		//int i;
		for(i=1;i<9;i++)
		{   
			str.Format("%d",i*2);
			pdc->TextOut(-30,i*100,str);
		}
		//pdc->TextOut(-50,1050,"%");

		//��״ͼ���ֻ���


		//�����������
		pdc->MoveTo(0,0);
		pdc->LineTo(850,0);
		pdc->LineTo(830,10);
		pdc->MoveTo(850,0);
		pdc->LineTo(830,-10);

		//������״ͼ�߶�
		float task1,task2,task3,task4,task5,task6,task7;
		int h = 800;
		task1 = fTaskHourWeek[0]/18;
		task2 = fTaskHourWeek[1]/18;
		task3 = fTaskHourWeek[2]/18;
		task4 = fTaskHourWeek[3]/18;
		task5 = fTaskHourWeek[4]/18;
		task6 = fTaskHourWeek[5]/18;
		task7 = fTaskHourWeek[6]/18;


		CString strbuf;
		//������

		pdc->SelectObject(&font); 	
		//left, top, right, bottom
		CRect r1(100,task1*h,150,0);
		CBrush br1(RGB(253,51,67));
		pdc->FillRect(r1,&br1);
		strbuf.Format("%0.1f",fTaskHourWeek[0]);
		pdc->TextOut(100,task1*h+60,strbuf);

		CRect r2(200,task2*h,250,0);
		CBrush br2(RGB(250,164,51));
		pdc->FillRect(r2,&br2);
		strbuf.Format("%0.1f",fTaskHourWeek[1]);
		pdc->TextOut(200, task2*h+60, strbuf);

		CRect r3(300,task3*h,350,0);
		CBrush br3(RGB(139,197,51));
		pdc->FillRect(r3,&br3);
		strbuf.Format("%0.1f",fTaskHourWeek[2]);
		pdc->TextOut(300, task3*h+60, strbuf);


		CRect r4(400,task4*h,450,0);
		CBrush br4(RGB(3,141,216));
		pdc->FillRect(r4,&br4);
		strbuf.Format("%0.1f",fTaskHourWeek[3]);
		pdc->TextOut(400, task4*h+60, strbuf);

		CRect r5(500,task5*h,550,0);
		CBrush br5(RGB(192,14,235));
		pdc->FillRect(r5,&br5);
		strbuf.Format("%0.1f",fTaskHourWeek[4]);
		pdc->TextOut(500, task5*h+60, strbuf);

		CRect r6(600,task6*h, 650,0);
		CBrush br6(RGB(0,139,139));
		pdc->FillRect(r6, &br6);
		strbuf.Format("%0.1f",fTaskHourWeek[5]);
		pdc->TextOut(600, task6*h+60, strbuf);

		CRect r7(700,task7*h, 750,0);
		CBrush br7(RGB(0,120,100));
		pdc->FillRect(r7, &br7);
		strbuf.Format("%0.1f",fTaskHourWeek[6]);
		pdc->TextOut(700, task7*h+60, strbuf);


	}
	else if (g_query_flag == QUERY_MONTH)
	{
		//monthͳ��
		pdc->TextOut(100,-10,"0");
		pdc->TextOut(250,-10,"-1");
		pdc->TextOut(400,-10,"-2");
		pdc->TextOut(550,-10,"-3");


		pdc->MoveTo(0,0);
		pdc->LineTo(0,100);

		pdc->LineTo(-10,800);
		//pdc->MoveTo(0,500);
		//pdc->LineTo(10,700);
		CString str;
		//int i;
		for(i=1;i<9;i++)
		{   
			str.Format("%d",i*10);
			pdc->TextOut(-30,i*100,str);
		}
		//pdc->TextOut(-50,1050,"%");

		//��״ͼ���ֻ���


		//�����������
		pdc->MoveTo(0,0);
		pdc->LineTo(850,0);
		pdc->LineTo(830,10);
		pdc->MoveTo(850,0);
		pdc->LineTo(830,-10);

		//������״ͼ�߶�
		float task1,task2,task3,task4;
		int h = 800;
		task1 = fTaskHourMonth[0]/80;
		task2 = fTaskHourMonth[1]/80;
		task3 = fTaskHourMonth[2]/80;
		task4 =fTaskHourMonth[3]/80;
		



		//������
		CString strbuf;
		pdc->SelectObject(&font); 	
		//left, top, right, bottom
		CRect r1(100,task1*h,200,0);
		CBrush br1(RGB(253,51,67));
		pdc->FillRect(r1,&br1);
		strbuf.Format("%0.1f",fTaskHourMonth[0]);
		pdc->TextOut(100,task1*h+60,strbuf);

		CRect r2(250,task2*h,350,0);
		CBrush br2(RGB(250,164,51));
		pdc->FillRect(r2,&br2);
		strbuf.Format("%0.1f",fTaskHourMonth[1]);
		pdc->TextOut(250,task2*h+60,strbuf);


		CRect r3(400,task3*h,500,0);
		CBrush br3(RGB(139,197,51));
		pdc->FillRect(r3,&br3);
		strbuf.Format("%0.1f",fTaskHourMonth[2]);
		pdc->TextOut(400,task3*h+60,strbuf);


		CRect r4(550,task4*h,650,0);
		CBrush br4(RGB(3,141,216));
		pdc->FillRect(r4,&br4);
		strbuf.Format("%0.1f",fTaskHourMonth[3]);
		pdc->TextOut(550,task4*h+60,strbuf);




	}
	else
	{
		//��ͨͳ��
		//�õ���ѯ����
		COleDateTimeSpan nQueryDay;
		nQueryDay = m_query_timeend - m_query_timebegin;
		int nDay = nQueryDay.GetTotalDays();

		//weekͳ�ƣ�����״ͼ
		pdc->TextOut(100,-10,"0");
		pdc->TextOut(200,-10,"-1");
		pdc->TextOut(300,-10,"-2");
		pdc->TextOut(400,-10,"-3");
		pdc->TextOut(500,-10,"-4");
		pdc->TextOut(600,-10,"-5");
		pdc->TextOut(700,-10,"-6");
		pdc->TextOut(800,-10,"-7");
		pdc->TextOut(900,-10,"-8");

		pdc->MoveTo(0,0);
		pdc->LineTo(0,100);

		pdc->LineTo(-10,800);
		//pdc->MoveTo(0,500);
		//pdc->LineTo(10,700);
		CString str;
		//int i;
		for(i=1;i<9;i++)
		{   
			str.Format("%d",i*2);
			pdc->TextOut(-30,i*100,str);
		}
		//pdc->TextOut(-50,1050,"%");

		//��״ͼ���ֻ���

		//�����������
		pdc->MoveTo(0,0);
		pdc->LineTo(1000,0);
		pdc->LineTo(1030,10);
		pdc->MoveTo(1000,0);
		pdc->LineTo(1030,-10);

		//������״ͼ�߶�
		float task1,task2,task3,task4,task5,task6,task7,task8,task9;
		int h = 800;
		task1 = fTaskHourNormal[0]/18;
		task2 = fTaskHourNormal[1]/18;
		task3 = fTaskHourNormal[2]/18;
		task4 = fTaskHourNormal[3]/18;
		task5 = fTaskHourNormal[4]/18;
		task6 = fTaskHourNormal[5]/18;
		task7 = fTaskHourNormal[6]/18;
		task8 = fTaskHourNormal[7]/18;
		task9 = fTaskHourNormal[8]/18;



		//������
		CString strbuf;
		pdc->SelectObject(&font); 	
		//left, top, right, bottom
		CRect r1(100,task1*h,150,0);
		CBrush br1(RGB(253,51,67));
		pdc->FillRect(r1,&br1);
		strbuf.Format("%0.1f",fTaskHourNormal[0]);
		pdc->TextOut(100,task1*h+60,strbuf);


		CRect r2(200,task2*h,250,0);
		CBrush br2(RGB(250,164,51));
		pdc->FillRect(r2,&br2);
		strbuf.Format("%0.1f",fTaskHourNormal[1]);
		pdc->TextOut(200,task2*h+60,strbuf);


		CRect r3(300,task3*h,350,0);
		CBrush br3(RGB(139,197,51));
		pdc->FillRect(r3,&br3);
		strbuf.Format("%0.1f",fTaskHourNormal[2]);
		pdc->TextOut(300,task3*h+60,strbuf);


		CRect r4(400,task4*h,450,0);
		CBrush br4(RGB(3,141,216));
		pdc->FillRect(r4,&br4);
		strbuf.Format("%0.1f",fTaskHourNormal[3]);
		pdc->TextOut(400,task4*h+60,strbuf);


		CRect r5(500,task5*h,550,0);
		CBrush br5(RGB(192,14,235));
		pdc->FillRect(r5,&br5);
		strbuf.Format("%0.1f",fTaskHourNormal[4]);
		pdc->TextOut(500,task5*h+60,strbuf);

		CRect r6(600,task6*h, 650,0);
		CBrush br6(RGB(0,139,139));
		pdc->FillRect(r6, &br6);
		strbuf.Format("%0.1f",fTaskHourNormal[5]);
		pdc->TextOut(600,task6*h+60,strbuf);

		CRect r7(700,task7*h, 750,0);
		CBrush br7(RGB(0,120,100));
		pdc->FillRect(r7, &br7);
		strbuf.Format("%0.1f",fTaskHourNormal[6]);
		pdc->TextOut(700,task7*h+60,strbuf);

		CRect r8(800,task8*h, 850,0);
		CBrush br8(RGB(0,120,100));
		pdc->FillRect(r8, &br8);
		strbuf.Format("%0.1f",fTaskHourNormal[7]);
		pdc->TextOut(800,task8*h+60,strbuf);

		CRect r9(900,task9*h, 950,0);
		CBrush br9(RGB(0,120,100));
		pdc->FillRect(r9, &br9);
		strbuf.Format("%0.1f",fTaskHourNormal[8]);
		pdc->TextOut(900,task9*h+60,strbuf);

	}

	/*
	//��������
	pdc->TextOut(100,-10,"0");
	pdc->TextOut(200,-10,"-1");
	pdc->TextOut(300,-10,"-2");
	pdc->TextOut(400,-10,"-3");
	pdc->TextOut(500,-10,"-4");
	pdc->TextOut(600,-10,"-5");
	pdc->TextOut(700,-10,"-6");
	

	//�����������
	pdc->MoveTo(0,0);
	pdc->LineTo(0,100);

	pdc->LineTo(-10,800);
	//pdc->MoveTo(0,500);
	//pdc->LineTo(10,700);
	CString str;
	//int i;
	for(i=1;i<9;i++)
	{   str.Format("%d",i*30);

		pdc->TextOut(-30,i*100,str);
	}
	//pdc->TextOut(-50,1050,"%");

	//��״ͼ���ֻ���


	//�����������
	pdc->MoveTo(0,0);
	pdc->LineTo(850,0);
	pdc->LineTo(830,10);
	pdc->MoveTo(850,0);
	pdc->LineTo(830,-10);

	int x1,y1,x2,y2,y3,x3, x4,y4;
	int h=800,r=400;

	
	
	//������״ͼ�߶�
	float task1,task2,task3,task4,task5,task6;
	task1 = g_index_cd[0]*1.0/240;
	task2 = g_index_cd[1]*1.0/240;
	task3 = g_index_cd[2]*1.0/240;
	task4 = g_index_cd[3]*1.0/240;
	task5 = g_index_cd[4]*1.0/240;
	task6 = g_index_cd[5]*1.0/240;


	//������

	pdc->SelectObject(&font); 	
	//left, top, right, bottom
	CRect r1(100,task1*h,150,0);
	CBrush br1(RGB(253,51,67));
	pdc->FillRect(r1,&br1);
	

	CRect r2(200,task2*h,250,0);
	CBrush br2(RGB(250,164,51));
	pdc->FillRect(r2,&br2);
	

	CRect r3(300,task3*h,350,0);
	CBrush br3(RGB(139,197,51));
	pdc->FillRect(r3,&br3);
	

	CRect r4(400,task4*h,450,0);
	CBrush br4(RGB(3,141,216));
	pdc->FillRect(r4,&br4);

	
	CRect r5(500,task5*h,550,0);
	CBrush br5(RGB(192,14,235));
	pdc->FillRect(r5,&br5);

	CRect r6(600,task6*h, 650,0);
	CBrush br6(RGB(0,139,139));
	pdc->FillRect(r6, &br6);
	
	
	

	

	//�����ʾ���ֺͷ����־
	CRect rblocktext1(600,780,640,820);
	pdc->FillRect(rblocktext1,&br1);

	CRect rblocktext2(600,700,640,740);
	pdc->FillRect(rblocktext2,&br2);


	CRect rblocktext3(600,620,640,660);
	pdc->FillRect(rblocktext3,&br3);


	CRect rblocktext4(600,540,640,580);
	pdc->FillRect(rblocktext4,&br4);

	CRect rblocktext5(600,460,640,500);
	pdc->FillRect(rblocktext5,&br5);

	CRect rblocktext6(600,380,640,420);
	pdc->FillRect(rblocktext6,&br6);



	*/

	pdc->TextOut(80,850,"�����ʱͳ��/h");
	pdc->TextOut(1000,850,"�������ͳ��");
	
	int x1,y1,x2,y2,y3,x3, x4,y4;
	int r=400;
	//CRect r5(500, qi*h,550,0);
	//����Ϊ������ͼ
	CBrush brfan1(RGB(211,211,211));
	CBrush brfan2(RGB(34,139,31));
	CBrush brfan3(RGB(255,215,0));
	//CBrush br10(RGB(238,229,222));
	pdc->SelectObject(&brfan3);
	x1=int(r*cos(rule3*3.14*2))+1400;
	y1=int(r*sin(rule3*3.14*2)+400);
	pdc->Pie(1200,800,2000,0,1800,400,x1,y1);


	pdc->SelectObject(&brfan2);
	x2=int(r*cos((rule2+rule3)*3.14*2))+1400;
	y2=int(r*sin((rule2+rule3)*3.14*2))+400;
	pdc->Pie(1200,800,2000,0,x1,y1,x2,y2);

	
	pdc->SelectObject(&brfan1);
	x3=int(r*cos((rule2+rule1+rule3)*3.14*2))+1400;
	y3=int(r*sin((rule2+rule1+rule3)*3.14*2))+400 ;
	pdc->Pie(1200,800,2000,0,x2,y2,x3,y3);
	
	CBrush   br9(RGB(125,30,227));
	pdc->SelectObject(&br9);
	

	CBrush brfan4(RGB(30,144,255));
	pdc->SelectObject(&brfan4);
	pdc->Pie(1200,800,2000,0,x3,y3,1800,400); 

	//����ͼ���ֻ���
	pdc->TextOut(2100,850, AnlyCategory(nBuf[0]));
	pdc->TextOut(2100,750, AnlyCategory(nBuf[1]));
	pdc->TextOut(2100,650, AnlyCategory(nBuf[2]));
	pdc->TextOut(2100,550, "����");
	
	
	CRect rfantext1(2000,800,2080,860);
	pdc->FillRect(rfantext1,&brfan1);
	

	CRect rfantext2(2000,700,2080,760);
	pdc->FillRect(rfantext2,&brfan2);
	

	CRect rfantext3(2000,600,2080,660);
	pdc->FillRect(rfantext3,&brfan3);
	

	CRect rfantext4(2000,500,2080,560);
	pdc->FillRect(rfantext4,&brfan4);
	

	memset(g_index_category, 0, 12);
	memset(g_index_cd, 0, 1000);
	g_query_flag = 0;
	g_fTaskAllCd = 0.0;
	UpdateData(FALSE);
	OnBnClickedBtnQuery();
}

//�õ�����ĺ�ʱ,
//����ֵΪ���ĵ�Сʱ��
//
int CepiminfoDlg::GetRuleCD(PTimeMap pStartTime, PTimeMap pEndTime)
{
	//ת��Ϊ������
	int nret;
	if (pStartTime->nyear == pEndTime->nyear && pStartTime->nmonth == pEndTime->nmonth)
	{
		nret = (24*pEndTime->nday + pEndTime->hour - 24*pStartTime->nday - pStartTime->hour)*60 +
			pEndTime->nminute - pStartTime->nminute;
		if (nret == 0)
		{
			nret = 30;
		}
	}
	else
	{
		//�����Ϊ-1������ܳ�ʱ��
		nret = -1;
	}
	return nret;
}

//ʵ��ĳ�����ж����칦����
int GetMonthDay(int year,int month)
{
	int nday = 0;
	switch (month)
	{
	case 1:nday = 31;
		break;
	case 2:
		{
			//�ж�����
			if(year%4==0&&year%100!=0||year%400==0) 
				nday = 29;
			else
				nday = 28;
		}
		break;
	case 3:nday = 31;
		break;
	case 4:nday = 30;
		break;
	case 5:nday = 31;
		break;
	case 6:nday = 30;
		break;
	case 7:nday = 31;
		break;
	case 8:nday = 31;
		break;
	case 9:nday = 30;
		break;
	case 10:nday = 31;
		break;
	case 11:nday = 30;
		break;
	case 12:nday = 31;
		break;

	default:
		AfxMessageBox("month error!");
		break;
	}
	return nday;
}

//���ʵ�����һ���ܵĲ�ѯ
void CepiminfoDlg::OnBnClickedBtnWeek()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	SYSTEMTIME SysTime;
	GetSystemTime(&SysTime);
	
	COleDateTime time_current;
	time_current.SetDate(SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	
	
	m_query_timebegin = time_current -  COleDateTimeSpan(6,
		0,0,0);
	m_query_timeend = time_current;
	UpdateData(FALSE);
	/*
	CString strbuf;
	strbuf.Format("%d",SysTime.wYear);
	SetDlgItemText(IDC_CBO_YEAR_BEGIN,strbuf);
	//SetDlgItemText(IDC_CBO_YEAR_END, strbuf);
	
	//timebuf =  m_query_timebegin;
	
	//int nDayOfWeek = time_current.GetDayOfWeek() - 1;
	
	//m_query_timebegin.Format("%Y%m%d");
	//SetDlgItemText(IDC_DATETIMEPICKER_BEGIN,m_query_timebegin.Format("%Y%m%d"));
	
	//m_query_timeend = time_current;

	
	//m_query_timeend.Format("%Y%m%d");
	//SetDlgItemText(IDC_DATETIMEPICKER_END, m_query_timeend.Format("%Y%m%d"));
	
	//SetDlgItemText(IDC_CBO_MONTH_END, strbuf);
	//�ж���ĩ��Ҫע���Ƿ�ᳬ����ĩ�涨�·�
	//int nDay = SysTime.wDay;
	/*
	int nDayBegin,nDayEnd;
	int nMaxDayInMonth = GetMonthDay(SysTime.wYear, SysTime.wMonth);
	int nDayofWeek = SysTime.wDayOfWeek;

	int nMonthEnd = SysTime.wMonth;
	int nYearEnd = SysTime.wYear;
	//���ﰴ������Ϊ ÿ�����ڵĿ�ʼ����
	nDayBegin = SysTime.wDay - nDayofWeek ;
	//strbuf.Format("%d", nDayBegin);
	//SetDlgItemText(IDC_CBO_DAY_BEGIN,strbuf);
	int nDayMonth = nDayBegin + 6 - nMaxDayInMonth;
	if (nDayMonth > 0)
	{
		if (++nMonthEnd > 12)
		{
			nYearEnd++;
			strbuf.Format("%d",nYearEnd);
			SetDlgItemText(IDC_CBO_YEAR_END, strbuf);

			SetDlgItemText(IDC_CBO_MONTH_END, "1");

			strbuf.Format("%d",nDayMonth);
			SetDlgItemText(IDC_CBO_DAY_END,strbuf);
		}
		else
		{
			strbuf.Format("%d",nYearEnd);
			SetDlgItemText(IDC_CBO_YEAR_END, strbuf);
			strbuf.Format("%d",++nMonthEnd);
			SetDlgItemText(IDC_CBO_MONTH_END,strbuf);
			strbuf.Format("%d",nDayMonth);
			SetDlgItemText(IDC_CBO_DAY_END,strbuf);

		}
	}
	else
	{
		strbuf.Format("%d",nYearEnd);
		SetDlgItemText(IDC_CBO_YEAR_END,strbuf);
		strbuf.Format("%d",nMonthEnd);
		SetDlgItemText(IDC_CBO_MONTH_END,strbuf);
		strbuf.Format("%d",nDayBegin+6);
		SetDlgItemText(IDC_CBO_DAY_END,strbuf);
	}
	*/

	
	OnBnClickedBtnQuery();
	//OnBnClickedBtnCount();
	//UpdateData(FALSE);

}


//ͳ�����һ��
void CepiminfoDlg::OnBnClickedBtnMonth()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	SYSTEMTIME SysTime;
	GetSystemTime(&SysTime);
	COleDateTime time_current;
	time_current.SetDate(SysTime.wYear, SysTime.wMonth, SysTime.wDay);


	m_query_timebegin = time_current - COleDateTimeSpan(30,
		0,0,0);
	m_query_timeend = time_current;
	UpdateData(FALSE);
	OnBnClickedBtnQuery();
	//OnBnClickedBtnCount();
}



//��ӱȽ�ʱ���Ⱥ�ģ��
//����ֵΪ�ص���ʱ��

int CountTime(COleDateTime TimeCurrent_Start, COleDateTime TimeCurrent_End, COleDateTime TimeLast_Start, COleDateTime TimeLast_End)
{
	//�ж�����ʱ���Ƿ��ص�
	int  nminute = 0;
	COleDateTimeSpan timebuf;
	if (TimeLast_End <= TimeCurrent_Start)
	{
		//û�н����ص�������������
		if (TimeCurrent_Start ==  TimeCurrent_End)
		{
			nminute = 30;
		}
		else
		{
			timebuf =  TimeCurrent_End - TimeCurrent_Start;
			nminute = timebuf.GetTotalMinutes();

		}
		

	} 
	else if (TimeLast_End > TimeCurrent_Start && TimeLast_End < TimeCurrent_End )
	{
		//������ʱ�佻�����������������
		timebuf = TimeCurrent_End -  TimeLast_End;
		nminute = timebuf.GetTotalMinutes();
	}
	else if (TimeCurrent_End <= TimeLast_End)
	{
		//���񽻴��ʰ�����ϵ
		nminute = 0;
	}
	else
	{
		AfxMessageBox("�����ѯ�����Ƿ���ȷ!");
		return -1;
	} 
	return nminute;
	
}


//��interbase�����ݿ�ʱ��ת��ΪOleDataTimeʱ��
COleDateTime IBPPTime2OleTime(IBPP::Timestamp ibpptime)
{
	COleDateTime oletime;
	oletime.SetDateTime(ibpptime.Year(), ibpptime.Month(), ibpptime.Day(),
		ibpptime.Hours(), ibpptime.Minutes(), 0);
	return oletime;
}



//���ͳ�Ʊ���
void CepiminfoDlg::OnBnClickedOutQuery()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strFilePathName;
	char* cFilePath;
	
	CFileDialog dlg(FALSE);///TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
	dlg.m_ofn.lpstrFilter="Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrDefExt="txt";
	if(dlg.DoModal()==IDOK)
	{
		strFilePathName=dlg.GetPathName();
		//Outputpath=(LPTSTR)(LPCTSTR)FilePathName;
		//flag=OutputInfo(head,Outputpath);
		cFilePath = (LPTSTR)(LPCTSTR)strFilePathName;

		//���ļ�an
		FILE* fqueryout;
		if ((fqueryout = fopen(cFilePath, "w")) == NULL)
		{
			AfxMessageBox("�޷����ļ�");
			return;
		}
		fprintf(fqueryout, "\t\t\t<----------------EssentialPimʱ���ѯ����---------------->\n\n");
		fprintf(fqueryout, "<��>\t<---��ʼʱ��--->\t<---����ʱ��--->\t<����>\t<����>\t\t<���>\t<��ʱ>\t<-------����------->\n");


		//���ݿ���ز��������ͳ�Ʊ�����Ƚ�������ѯ���Ի�ø������Ϣ 
		CString  strTimeBuf;
		CString strTimeBegin;
		GetDlgItemTextA(IDC_DATETIMEPICKER_BEGIN,strTimeBuf);

		strTimeBegin.Format("%s 0:0:0",strTimeBuf);
	

		GetDlgItemTextA(IDC_DATETIMEPICKER_END, strTimeBuf);
		CString strTimeEnd;
	
		strTimeEnd.Format("%s 0:0:0",strTimeBuf); 

		//��ѯ���
		CString strQuery;
		strQuery.Format("select * from TASKS where STARTTIME between '%s' and '%s' order by STARTTIME", 
			strTimeBegin, strTimeEnd
					);
		// ȡ���ݿ�·��
	

		//�жϲ����Ƿ���ȷ
		if (g_strFilePath.IsEmpty())
		{
			AfxMessageBox("�����Ӧ�����Ƿ���ȷ!");
			return;
		}

	
		char* szQuery = (LPSTR)(LPCTSTR)strQuery;
		char* szDBPath = (LPSTR)(LPCTSTR)g_strFilePath;
	
		//�������ݿ�
		IBPP::Database db;
		db = IBPP::DatabaseFactory(g_szServName, szDBPath, g_szDBUserName, g_szDBPassWord);
		db->Connect();

		IBPP::Transaction tr = IBPP::TransactionFactory(db);
		tr->Start();
		//char* szQuery = (LPSTR)(LPCTSTR)strQuery;
		try
		{
			IBPP::Statement st = IBPP::StatementFactory(db, tr);
			st->Execute(szQuery);
			//st->Execute("select STARTTIME,TITLE,ENDTIME,CATEGORY, PRIORITY, COMPLETION from TASKS where STARTTIME between '2015/7/7 0:0:0' and '2015/7/10 0:0:0' order by STARTTIME");
			ncount = 0;
			PTimeMap pStartTimeMap, pEndTimeMap;
			//pStartTimeMap = NULL;
			//pEndTimeMap = NULL;
			pStartTimeMap = (PTimeMap)malloc(sizeof(TimeMap));
			pEndTimeMap = (PTimeMap)malloc(sizeof(TimeMap));

			//��ʾ
			while (st->Fetch())
			{
				//�����б����
			
				CString strCount;
				strCount.Format("%d", ncount);
				//m_infolist.InsertItem(ncount, strCount);

				//��ѯ���ݿ⣬�����µ���ѯ�б���
				IBPP::Timestamp timemap;
				CString strStartTime, strEndTime;
		

				st->Get("STARTTIME", timemap);
				pStartTimeMap->nyear = timemap.Year();
				pStartTimeMap->nmonth = timemap.Month();
				pStartTimeMap->nday = timemap.Day();
				pStartTimeMap->hour = timemap.Hours();
				pStartTimeMap->nminute = timemap.Minutes();
				pStartTimeMap->nsecond = timemap.Seconds();
			
				strStartTime.Format("%d/%d/%d  %d:%d:%d",timemap.Year(), timemap.Month(),
					timemap.Day(), timemap.Hours(), timemap.Minutes(),timemap.Seconds());

				st->Get("ENDTIME", timemap);
				pEndTimeMap->nyear = timemap.Year();
				pEndTimeMap->nmonth = timemap.Month();
				pEndTimeMap->nday = timemap.Day();
				pEndTimeMap->hour = timemap.Hours();
				pEndTimeMap->nminute = timemap.Minutes();
				pEndTimeMap->nsecond = timemap.Seconds();

				strEndTime.Format("%d/%d/%d  %d:%d:%d", timemap.Year(), timemap.Month(),
					timemap.Day(), timemap.Hours(), timemap.Minutes(),timemap.Seconds());
				//�����ڲ�д���ļ�
				
				//m_infolist.SetItemText(ncount, 1, strStartTime);
				//m_infolist.SetItemText(ncount, 2, strEndTime);


				//�������⣬����ʾ����ѯ�б���
				//////////////////////////////////////////////////////////////////////////
				string szTemp;
				std::wstring szOut;
				//wstring wszTemp;
				//string buf;
				st->Get("TITLE", szTemp);
				Utf82Unicode(szTemp,szOut);
				WStringToString(szOut,szTemp);
				//buf = WStringToStringStd(szOut);
				CString strTitle(szTemp.c_str());
				//m_infolist.SetItemText(ncount, 3, strTitle);

				//��������
				int nCategory = 0;
				st->Get("CATEGORY", nCategory);
				//ת��Ϊ��Ӧ�ķ���
				//m_infolist.SetItemText(ncount, 4, AnlyCategory(nCategory));

				//�������ȼ�
				int nPriority = 0;
				CString strPriority;
				st->Get("PRIORITY",nPriority);
				strPriority.Format("%d",nPriority);
				//m_infolist.SetItemText(ncount, 5, strPriority);

				//����״̬
				int nCompletion = 0;
				CString strCompletion;
				st->Get("COMPLETION", nCompletion);
				strCompletion.Format("%d", nCompletion);
				//m_infolist.SetItemText(ncount, 6, strCompletion);
			
				//���������ʱ����
				//��������ʱ
				int nRuleCD = GetRuleCD(pStartTimeMap, pEndTimeMap);
				//�������ʱ��ӵ�ȫ�������б���
				g_index_cd[ncount] = nRuleCD;
				CString strRuleCD;
				if (nRuleCD == -1)
				{
					strRuleCD.Format("+��");
				}
				else
				{
					strRuleCD.Format("%d", nRuleCD);
				}
				//m_infolist.SetItemText(ncount, 7, strRuleCD);


				//����ģ�飬�ж��Ƿ�Ӵ������
				//wstring wszTemp;
				//string buf;
			
				//ͳ���ܹ�ʱ
		


				//�����ѯ����
				//fprintf(fqueryout, "<��>\t<---��ʼʱ��--->\t<---����ʱ��--->\t<���ȼ�>\t<����>\t<��ɶ�>\t<��ʱ>\t<-----����----->\n");
				fprintf(fqueryout,"%-5s\t%-10s\t%-10s\t%-5s\t%-10s\t%-5s\t%-5s\t%-30s\n\n",
					strCount, strStartTime, strEndTime,
					strPriority,AnlyCategory(nCategory),
					strCompletion, strRuleCD, strTitle );
			
				//�ַ������
				strCount.Empty();
				strStartTime.Empty();
				strEndTime.Empty();
				strTitle.Empty();
				strCompletion.Empty();
				strPriority.Empty();
				strRuleCD.Empty();
				ncount++;
				//fprintf(fqueryout,"\n");

			}

			tr->Commit();
		}
		catch(IBPP::SQLException &e)
		{
			AfxMessageBox(e.what());
			tr->Rollback();
		}

		
	}
	else
	{
		return;
	}

}

//����
void CepiminfoDlg::OnBnClickedOutAnly()
{
	 // TODO: �ڴ���ӿؼ�֪ͨ����������
	CAboutDlg dlg;
	dlg.DoModal();

}


//������ͳ�ƣ����ÿһ����ܹ�ʱ����״ͼ�ͱ�״ͼ
void CepiminfoDlg::OnBnClickedBtnWeekAnly()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	OnBnClickedBtnWeek();
	g_query_flag = QUERY_WEEK;
	OnBnClickedBtnCount();

}


int CepiminfoDlg::CountDayRuleCD(COleDateTime DataTime)
{
	
	int nAllMintue = 0;
	CString  strTimeBuf;
	CString strTimeBegin;

	strTimeBegin.Format("%d/%d/%d 0:0:0",DataTime.GetYear(), DataTime.GetMonth(), DataTime.GetDay());

	DataTime += COleDateTimeSpan(1, 0, 0, 0);



	CString strTimeEnd;

	strTimeEnd.Format("%d/%d/%d 0:0:0",DataTime.GetYear(), DataTime.GetMonth(), DataTime.GetDay()); 



	//��ѯ���
	CString strQuery;
	strQuery.Format("select STARTTIME,TITLE,ENDTIME,CATEGORY, PRIORITY, COMPLETION from TASKS where STARTTIME between '%s' and '%s' order by STARTTIME", 
		strTimeBegin, strTimeEnd
		);
	// ȡ���ݿ�·��


	//�жϲ����Ƿ���ȷ
	if (g_strFilePath.IsEmpty())
	{
		AfxMessageBox("�����Ӧ�����Ƿ���ȷ!");
		return -1;
	}


	char* szQuery = (LPSTR)(LPCTSTR)strQuery;
	char* szDBPath = (LPSTR)(LPCTSTR)g_strFilePath;

	//�������ݿ�
	IBPP::Database db;
	db = IBPP::DatabaseFactory(g_szServName, szDBPath, g_szDBUserName, g_szDBPassWord);
	db->Connect();

	IBPP::Transaction tr = IBPP::TransactionFactory(db);
	tr->Start();
	//char* szQuery = (LPSTR)(LPCTSTR)strQuery;
	try
	{
		IBPP::Statement st = IBPP::StatementFactory(db, tr);
		st->Execute(szQuery);
		//st->Execute("select STARTTIME,TITLE,ENDTIME,CATEGORY, PRIORITY, COMPLETION from TASKS where STARTTIME between '2015/7/7 0:0:0' and '2015/7/10 0:0:0' order by STARTTIME");
		ncount = 0;

		//��ѯ���ݿ⣬�����µ���ѯ�б���
		IBPP::Timestamp ibpptime_start,ibpptime_end;
		IBPP::Timestamp timemap_start_last, timemap_end_last;

		//����Ӧ��IBPPʱ��ת��ΪCOleDataTime
		COleDateTime OleTimeStart_current,OleTimeEnd_current;
		COleDateTime OleTimeStart_last, OleTimeEnd_last;
		COleDateTimeSpan temp;


		while (st->Fetch())
		{
			//�����б���

			st->Get("STARTTIME", ibpptime_start);
			OleTimeStart_current = IBPPTime2OleTime(ibpptime_start);

			st->Get("ENDTIME", ibpptime_end);
			OleTimeEnd_current = IBPPTime2OleTime(ibpptime_end);

			//������ǵ�һ�ζԱȣ��򽫴����ĸ�����������ó�ʱ��
			if (ncount == 0) 
			{
				if (OleTimeEnd_current == OleTimeStart_current)
				{
					nAllMintue = 30;
				}
				else
				{
					temp = OleTimeEnd_current - OleTimeStart_current;
					nAllMintue =  temp.GetTotalMinutes();
				}
			
			}
			else
			{
				//����4�����������ܼ�ʱ��
				nAllMintue += CountTime(OleTimeStart_current, OleTimeEnd_current, 
					OleTimeStart_last, OleTimeEnd_last);
			}


			OleTimeStart_last =  OleTimeStart_current ; 
			OleTimeEnd_last = OleTimeEnd_current;

			ncount++;

		}

		tr->Commit();
	}
	catch(IBPP::SQLException &e)
	{
		AfxMessageBox(e.what());
		tr->Rollback();
	}
	return nAllMintue;
}

//��ͳ�ƴ���
void CepiminfoDlg::OnBnClickedBtnMonthAnly()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


	OnBnClickedBtnMonth();
	g_query_flag = QUERY_MONTH;
	OnBnClickedBtnCount();
}
