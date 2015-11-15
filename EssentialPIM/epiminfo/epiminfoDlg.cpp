
// epiminfoDlg.cpp : 实现文件
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


//firebird相关数据库基本信息

const char* g_szServName = "localhost";
const char* g_szDBUserName = "SYSDBA";
const char* g_szDBPassWord = "masterkey";

//定义全局数组，作为统计时间的索引

//人物耗时索引，一维代表序号，二维代表所花时间
int g_index_cd[1000];
//分类索引，一维代表种类，二维代表计数
int g_index_category[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

int ncount = 0;

//统计总工时


//总工时
float g_fTaskAllCd = 0.0;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CepiminfoDlg 对话框



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


// CepiminfoDlg 消息处理程序

BOOL CepiminfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//初始化combo组件
	//年份初始化
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

	//月份初始化
	for (int i = 0; i < 12; i++)
	{
		CString strmonth;
		strmonth.Format("%d",i+1);
		//m_monthbegin.AddString(strmonth);
		//m_monthend.AddString(strmonth);
		m_monthbegin.InsertString(i, strmonth);
		m_monthend.InsertString(i, strmonth);
	}

	//日期初始化
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
	
	//初始化列表框
	m_infolist.InsertColumn(0, "序", LVCFMT_LEFT, 25);
	m_infolist.InsertColumn(1, "开始时间", LVCFMT_LEFT,100);
	m_infolist.InsertColumn(2, "结束时间", LVCFMT_LEFT, 100);
	m_infolist.InsertColumn(3, "主题", LVCFMT_LEFT,250);
	m_infolist.InsertColumn(4, "分类", LVCFMT_LEFT, 45);
	m_infolist.InsertColumn(5, "优先级", LVCFMT_LEFT, 40);
	m_infolist.InsertColumn(6, "完成度", LVCFMT_LEFT, 40);
	m_infolist.InsertColumn(7, "耗时/m", LVCFMT_LEFT, 50);
	

	//耗时列表
	m_list_taskcd.InsertColumn(0, "主题", LVCFMT_LEFT, 150);
	m_list_taskcd.InsertColumn(1,"耗时", LVCFMT_LEFT, 30);

//	WinExec("E:\command\PIM\epiminfo\epiminfo\bin\MsgBox.exe",SW_SHOWNORMAL);
	//开启服务器
	//ShellExecute(this->m_hWnd, "open","bin\\fbserver.exe","","",SW_HIDE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CepiminfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
		//addd
		//CPaintDC dc(this); // device context for painting
		
		

	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CepiminfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//选择相应数据库文件
void CepiminfoDlg::OnBnClickedBtnChoose()
{
	// TODO: 在此添加控件通知处理程序代码
	
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


//将char* 转成wchar_t*的实现函数如下：

//这是把asii字符转换为unicode字符，和上面相同的原理


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




//统计功能按钮模块，生产统计列表
void CepiminfoDlg::OnBnClickedBtnQuery()
{
	// TODO: 在此添加控件通知处理程序代码
	//清空列表
	m_infolist.DeleteAllItems();
	//传入时间查询参数
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

	//查询语句
	CString strQuery;
	strQuery.Format("select STARTTIME,TITLE,ENDTIME,CATEGORY, PRIORITY, COMPLETION from TASKS where STARTTIME between '%s' and '%s' order by STARTTIME", 
		strTimeBegin, strTimeEnd
					);
	// 取数据库路径
	

	//判断参数是否正确
	if (g_strFilePath.IsEmpty())
	{
		AfxMessageBox("请检测对应参数是否正确!");
		return;
	}

	
	char* szQuery = (LPSTR)(LPCTSTR)strQuery;
	char* szDBPath = (LPSTR)(LPCTSTR)g_strFilePath;
	
	//连接数据库
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

		//显示
		while (st->Fetch())
		{
			//定义列表序号
			
			CString strCount;
			strCount.Format("%d", ncount);
			m_infolist.InsertItem(ncount, strCount);

			//查询数据库，并更新到查询列表中
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
			//将日期插入表格
			m_infolist.SetItemText(ncount, 1, strStartTime);
			m_infolist.SetItemText(ncount, 2, strEndTime);
			
			//解析主题，并显示到查询列表中
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

			//解析分类
			int nCategory = 0;
			st->Get("CATEGORY", nCategory);
			//转换为对应的分类
			m_infolist.SetItemText(ncount, 4, AnlyCategory(nCategory));

			//解析优先级
			int nPriority = 0;
			CString strPriority;
			st->Get("PRIORITY",nPriority);
			strPriority.Format("%d",nPriority);
			m_infolist.SetItemText(ncount, 5, strPriority);

			//解析状态
			int nCompletion = 0;
			CString strCompletion;
			st->Get("COMPLETION", nCompletion);
			strCompletion.Format("%d", nCompletion);
			m_infolist.SetItemText(ncount, 6, strCompletion);
			
			//增加任务耗时功能
			//添加任务耗时
			
			
			int nRuleCD = GetRuleCD(pStartTimeMap, pEndTimeMap);
			//将任务耗时添加到全局数组列表中
			g_index_cd[ncount] = nRuleCD;
			CString strRuleCD;
			if (nRuleCD == -1)
			{
				strRuleCD.Format("+∞");
			}
			else
			{
				strRuleCD.Format("%0.1f", nRuleCD*1.0/60);
			}
			m_infolist.SetItemText(ncount, 7, strRuleCD);

			//统计总工时
			//判断时间是否符合有重叠
			


			//定义全局数组，在查询的时候生成一张统计表
			
			//字符串清空
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
	case 0:	szCateGory = "无";
		g_index_category[0]++;
		break;
	case 1: szCateGory = "重要";
		g_index_category[1]++;
		break;
	case 2: szCateGory = "商务";
		g_index_category[2]++;
		break;
	case 3: szCateGory = "个人";
		g_index_category[3]++;
		break;
	case 4: szCateGory = "假期";
		g_index_category[4]++;
		break;
	case 5: szCateGory = "必须出席";
		g_index_category[5]++;
		break;
	case 6: szCateGory = "需要出差";
		g_index_category[6]++;
		break;
	case 7: szCateGory = "需要准备";
		g_index_category[7]++;
		break;
	case 8: szCateGory = "周年纪念";
		g_index_category[8]++;
		break;
	case 9: szCateGory = "电话通知";
		g_index_category[9]++;
		break;
	case 10: szCateGory = "节日";
		g_index_category[10]++;
		break;
	default: szCateGory = "其他";
		g_index_category[11]++;
		break;
	}
	return szCateGory;
}

//画柱状图函数




//得到数组最大值的下标


#define RADIAN 3.1415926/180.0
//统计代码，主要生成柱状图和饼状图
//柱状图：生成耗时最多的前5项任务
//饼状图：按照分类生成统计比例
void CepiminfoDlg::OnBnClickedBtnCount()
{
	// TODO: 在此添加控件通知处理程序代码
	
	UpdateData(TRUE);
	m_list_taskcd.DeleteAllItems();
	if (g_strFilePath.IsEmpty())
	{
		AfxMessageBox("请检测对应参数是否正确!");
		return;
	}

	// 得到分类所占比例

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

	//输出任务耗时统计列表
	//显示文字
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

	



	//解析饼状图
	float rule1,rule2,rule3,rule4,rule_else;
	rule1 = g_index_category[0]*1.0/nMaxCount;
	rule2 = g_index_category[1]*1.0/nMaxCount;
	rule3 = g_index_category[3]*1.0/nMaxCount;
	rule4 = g_index_category[4]*1.0/nMaxCount;
	rule_else = 1.0 - rule1 - rule2 - rule3 -rule4;
	//得到最耗时的任务列表
	
	


	

	//按照分钟统计
	int nTaskCdWeek[7];
	int nTaskCdMonth[4];
	
	//按照分钟统计
	float fTaskHourWeek[7];
	float fTaskHourMonth[4];
	float fTaskHourNormal[100];
	
	//解析每天的工作时长
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
		//普通查询
		//得到查询天数
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
	//画笔
	CPen pen(PS_SOLID,1,RGB(0,0,0));         

	pdc->SelectObject(&pen);
	//逻辑坐标
	SetViewportOrgEx(*pdc,20,250,NULL);
	pdc->SetMapMode(MM_LOMETRIC);



	if (g_query_flag == QUERY_WEEK)
	{
		//week统计，画柱状图
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

		//柱状图文字绘制


		//横坐标轴绘制
		pdc->MoveTo(0,0);
		pdc->LineTo(850,0);
		pdc->LineTo(830,10);
		pdc->MoveTo(850,0);
		pdc->LineTo(830,-10);

		//计算柱状图高度
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
		//画柱子

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
		//month统计
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

		//柱状图文字绘制


		//横坐标轴绘制
		pdc->MoveTo(0,0);
		pdc->LineTo(850,0);
		pdc->LineTo(830,10);
		pdc->MoveTo(850,0);
		pdc->LineTo(830,-10);

		//计算柱状图高度
		float task1,task2,task3,task4;
		int h = 800;
		task1 = fTaskHourMonth[0]/80;
		task2 = fTaskHourMonth[1]/80;
		task3 = fTaskHourMonth[2]/80;
		task4 =fTaskHourMonth[3]/80;
		



		//画柱子
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
		//普通统计
		//得到查询天数
		COleDateTimeSpan nQueryDay;
		nQueryDay = m_query_timeend - m_query_timebegin;
		int nDay = nQueryDay.GetTotalDays();

		//week统计，画柱状图
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

		//柱状图文字绘制

		//横坐标轴绘制
		pdc->MoveTo(0,0);
		pdc->LineTo(1000,0);
		pdc->LineTo(1030,10);
		pdc->MoveTo(1000,0);
		pdc->LineTo(1030,-10);

		//计算柱状图高度
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



		//画柱子
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
	//横轴数据
	pdc->TextOut(100,-10,"0");
	pdc->TextOut(200,-10,"-1");
	pdc->TextOut(300,-10,"-2");
	pdc->TextOut(400,-10,"-3");
	pdc->TextOut(500,-10,"-4");
	pdc->TextOut(600,-10,"-5");
	pdc->TextOut(700,-10,"-6");
	

	//纵坐标轴绘制
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

	//柱状图文字绘制


	//横坐标轴绘制
	pdc->MoveTo(0,0);
	pdc->LineTo(850,0);
	pdc->LineTo(830,10);
	pdc->MoveTo(850,0);
	pdc->LineTo(830,-10);

	int x1,y1,x2,y2,y3,x3, x4,y4;
	int h=800,r=400;

	
	
	//计算柱状图高度
	float task1,task2,task3,task4,task5,task6;
	task1 = g_index_cd[0]*1.0/240;
	task2 = g_index_cd[1]*1.0/240;
	task3 = g_index_cd[2]*1.0/240;
	task4 = g_index_cd[3]*1.0/240;
	task5 = g_index_cd[4]*1.0/240;
	task6 = g_index_cd[5]*1.0/240;


	//画柱子

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
	
	
	

	

	//输出显示文字和方框标志
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

	pdc->TextOut(80,850,"任务耗时统计/h");
	pdc->TextOut(1000,850,"任务分类统计");
	
	int x1,y1,x2,y2,y3,x3, x4,y4;
	int r=400;
	//CRect r5(500, qi*h,550,0);
	//下面为画扇形图
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

	//扇形图文字绘制
	pdc->TextOut(2100,850, AnlyCategory(nBuf[0]));
	pdc->TextOut(2100,750, AnlyCategory(nBuf[1]));
	pdc->TextOut(2100,650, AnlyCategory(nBuf[2]));
	pdc->TextOut(2100,550, "其他");
	
	
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

//得到任务的耗时,
//返回值为消耗的小时数
//
int CepiminfoDlg::GetRuleCD(PTimeMap pStartTime, PTimeMap pEndTime)
{
	//转化为分钟数
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
		//输出间为-1，代表很长时间
		nret = -1;
	}
	return nret;
}

//实现某个月有多少天功能能
int GetMonthDay(int year,int month)
{
	int nday = 0;
	switch (month)
	{
	case 1:nday = 31;
		break;
	case 2:
		{
			//判断闰年
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

//点击实现最近一个周的查询
void CepiminfoDlg::OnBnClickedBtnWeek()
{
	// TODO: 在此添加控件通知处理程序代码
	
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
	//判断月末需要注意是否会超出月末规定月份
	//int nDay = SysTime.wDay;
	/*
	int nDayBegin,nDayEnd;
	int nMaxDayInMonth = GetMonthDay(SysTime.wYear, SysTime.wMonth);
	int nDayofWeek = SysTime.wDayOfWeek;

	int nMonthEnd = SysTime.wMonth;
	int nYearEnd = SysTime.wYear;
	//这里按照周日为 每个星期的开始计算
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


//统计最近一月
void CepiminfoDlg::OnBnClickedBtnMonth()
{
	// TODO: 在此添加控件通知处理程序代码

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



//添加比较时间先后模块
//返回值为重叠的时间

int CountTime(COleDateTime TimeCurrent_Start, COleDateTime TimeCurrent_End, COleDateTime TimeLast_Start, COleDateTime TimeLast_End)
{
	//判断任务时间是否重叠
	int  nminute = 0;
	COleDateTimeSpan timebuf;
	if (TimeLast_End <= TimeCurrent_Start)
	{
		//没有交错重叠，则正常计算
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
		//有任务时间交错，但不存在任务包含
		timebuf = TimeCurrent_End -  TimeLast_End;
		nminute = timebuf.GetTotalMinutes();
	}
	else if (TimeCurrent_End <= TimeLast_End)
	{
		//任务交错，呈包含关系
		nminute = 0;
	}
	else
	{
		AfxMessageBox("请检测查询参数是否正确!");
		return -1;
	} 
	return nminute;
	
}


//将interbase下数据库时间转换为OleDataTime时间
COleDateTime IBPPTime2OleTime(IBPP::Timestamp ibpptime)
{
	COleDateTime oletime;
	oletime.SetDateTime(ibpptime.Year(), ibpptime.Month(), ibpptime.Day(),
		ibpptime.Hours(), ibpptime.Minutes(), 0);
	return oletime;
}



//输出统计报表
void CepiminfoDlg::OnBnClickedOutQuery()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFilePathName;
	char* cFilePath;
	
	CFileDialog dlg(FALSE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	dlg.m_ofn.lpstrFilter="Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrDefExt="txt";
	if(dlg.DoModal()==IDOK)
	{
		strFilePathName=dlg.GetPathName();
		//Outputpath=(LPTSTR)(LPCTSTR)FilePathName;
		//flag=OutputInfo(head,Outputpath);
		cFilePath = (LPTSTR)(LPCTSTR)strFilePathName;

		//打开文件an
		FILE* fqueryout;
		if ((fqueryout = fopen(cFilePath, "w")) == NULL)
		{
			AfxMessageBox("无法打开文件");
			return;
		}
		fprintf(fqueryout, "\t\t\t<----------------EssentialPim时间查询报表---------------->\n\n");
		fprintf(fqueryout, "<序>\t<---开始时间--->\t<---结束时间--->\t<优先>\t<分类>\t\t<完成>\t<耗时>\t<-------主题------->\n");


		//数据库相关操作，输出统计报表相比较上述查询可以获得更多的信息 
		CString  strTimeBuf;
		CString strTimeBegin;
		GetDlgItemTextA(IDC_DATETIMEPICKER_BEGIN,strTimeBuf);

		strTimeBegin.Format("%s 0:0:0",strTimeBuf);
	

		GetDlgItemTextA(IDC_DATETIMEPICKER_END, strTimeBuf);
		CString strTimeEnd;
	
		strTimeEnd.Format("%s 0:0:0",strTimeBuf); 

		//查询语句
		CString strQuery;
		strQuery.Format("select * from TASKS where STARTTIME between '%s' and '%s' order by STARTTIME", 
			strTimeBegin, strTimeEnd
					);
		// 取数据库路径
	

		//判断参数是否正确
		if (g_strFilePath.IsEmpty())
		{
			AfxMessageBox("请检测对应参数是否正确!");
			return;
		}

	
		char* szQuery = (LPSTR)(LPCTSTR)strQuery;
		char* szDBPath = (LPSTR)(LPCTSTR)g_strFilePath;
	
		//连接数据库
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

			//显示
			while (st->Fetch())
			{
				//定义列表序号
			
				CString strCount;
				strCount.Format("%d", ncount);
				//m_infolist.InsertItem(ncount, strCount);

				//查询数据库，并更新到查询列表中
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
				//将日期插写入文件
				
				//m_infolist.SetItemText(ncount, 1, strStartTime);
				//m_infolist.SetItemText(ncount, 2, strEndTime);


				//解析主题，并显示到查询列表中
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

				//解析分类
				int nCategory = 0;
				st->Get("CATEGORY", nCategory);
				//转换为对应的分类
				//m_infolist.SetItemText(ncount, 4, AnlyCategory(nCategory));

				//解析优先级
				int nPriority = 0;
				CString strPriority;
				st->Get("PRIORITY",nPriority);
				strPriority.Format("%d",nPriority);
				//m_infolist.SetItemText(ncount, 5, strPriority);

				//解析状态
				int nCompletion = 0;
				CString strCompletion;
				st->Get("COMPLETION", nCompletion);
				strCompletion.Format("%d", nCompletion);
				//m_infolist.SetItemText(ncount, 6, strCompletion);
			
				//增加任务耗时功能
				//添加任务耗时
				int nRuleCD = GetRuleCD(pStartTimeMap, pEndTimeMap);
				//将任务耗时添加到全局数组列表中
				g_index_cd[ncount] = nRuleCD;
				CString strRuleCD;
				if (nRuleCD == -1)
				{
					strRuleCD.Format("+∞");
				}
				else
				{
					strRuleCD.Format("%d", nRuleCD);
				}
				//m_infolist.SetItemText(ncount, 7, strRuleCD);


				//增加模块，判断是否哟附加信
				//wstring wszTemp;
				//string buf;
			
				//统计总工时
		


				//输出查询报表
				//fprintf(fqueryout, "<序>\t<---开始时间--->\t<---结束时间--->\t<优先级>\t<分类>\t<完成度>\t<耗时>\t<-----主题----->\n");
				fprintf(fqueryout,"%-5s\t%-10s\t%-10s\t%-5s\t%-10s\t%-5s\t%-5s\t%-30s\n\n",
					strCount, strStartTime, strEndTime,
					strPriority,AnlyCategory(nCategory),
					strCompletion, strRuleCD, strTitle );
			
				//字符串清空
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

//帮助
void CepiminfoDlg::OnBnClickedOutAnly()
{
	 // TODO: 在此添加控件通知处理程序代码
	CAboutDlg dlg;
	dlg.DoModal();

}


//进行周统计，输出每一天的总工时的柱状图和饼状图
void CepiminfoDlg::OnBnClickedBtnWeekAnly()
{
	// TODO: 在此添加控件通知处理程序代码

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



	//查询语句
	CString strQuery;
	strQuery.Format("select STARTTIME,TITLE,ENDTIME,CATEGORY, PRIORITY, COMPLETION from TASKS where STARTTIME between '%s' and '%s' order by STARTTIME", 
		strTimeBegin, strTimeEnd
		);
	// 取数据库路径


	//判断参数是否正确
	if (g_strFilePath.IsEmpty())
	{
		AfxMessageBox("请检测对应参数是否正确!");
		return -1;
	}


	char* szQuery = (LPSTR)(LPCTSTR)strQuery;
	char* szDBPath = (LPSTR)(LPCTSTR)g_strFilePath;

	//连接数据库
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

		//查询数据库，并更新到查询列表中
		IBPP::Timestamp ibpptime_start,ibpptime_end;
		IBPP::Timestamp timemap_start_last, timemap_end_last;

		//将对应的IBPP时间转换为COleDataTime
		COleDateTime OleTimeStart_current,OleTimeEnd_current;
		COleDateTime OleTimeStart_last, OleTimeEnd_last;
		COleDateTimeSpan temp;


		while (st->Fetch())
		{
			//定义列表序

			st->Get("STARTTIME", ibpptime_start);
			OleTimeStart_current = IBPPTime2OleTime(ibpptime_start);

			st->Get("ENDTIME", ibpptime_end);
			OleTimeEnd_current = IBPPTime2OleTime(ibpptime_end);

			//如果不是第一次对比，则将传入四个参数，计算得出时间
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
				//传入4个参数计算总计时间
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

//月统计代码
void CepiminfoDlg::OnBnClickedBtnMonthAnly()
{
	// TODO: 在此添加控件通知处理程序代码


	OnBnClickedBtnMonth();
	g_query_flag = QUERY_MONTH;
	OnBnClickedBtnCount();
}
