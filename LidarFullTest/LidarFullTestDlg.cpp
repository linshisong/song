// LidarFullTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LidarFullTest.h"
#include "LidarFullTestDlg.h"
//#include <inttypes.h>
#include "SDKProtocol.h"
#include "Com/Comx.h"


#define INI_FILE "D:\\LidarFullTest.ini"

CLidarFullTestDlg *pThis = NULL;
int gStartFlag = 0;
static char lidar_first=1;
int lidardata_down[10],lidardata_left[10],lidardata_up[10],lidardata_right[10];
int lidardata_right_sum=0,lidardata_left_sum=0,lidardata_up_sum=0,lidardata_down_sum=0;
int lidardata_right_aver=0,lidardata_left_aver=0,lidardata_up_aver=0,lidardata_down_aver=0;
int error_0=0,error_1=0,error_2=0,error_3=0;
int lidardata_cnt;
char start_check=0;
char send_sensor=0;
char check_move=0;
char offset_flag=0;
char offset_to_zero = 0;
char offset_ok = 0;
char offset_weitiao = 0;
float thelta_index=0;
char lidar_data_cnt = 0;
char offset_to_zero_cnt = 0;
char communicate_check = 0;
char communicate_recheck = 0;


float distanceoofUpDownLeftRight[4]={0.0,0.0,0.0,0.0};
long long GetTime()
{
	long long t = 0;
	SYSTEMTIME st;
	GetLocalTime(&st);
	t = st.wMilliseconds;
	t << 32;
	t |= time(NULL);
	return t;
}

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLidarFullTestDlg 对话框




CLidarFullTestDlg::CLidarFullTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLidarFullTestDlg::IDD, pParent)
	, mThresVal(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	isTestOver = false;
	pThis = this;
}

void CLidarFullTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_VAL, mThresVal);
}

BEGIN_MESSAGE_MAP(CLidarFullTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_STOP, &CLidarFullTestDlg::OnBnClickedBtnStop)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CLidarFullTestDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_FLASH, &CLidarFullTestDlg::OnBnClickedBtnFlash)
	ON_EN_CHANGE(IDC_EDT_VAL, &CLidarFullTestDlg::OnEnChangeEdtVal)
	ON_WM_TIMER()	
	ON_BN_CLICKED(IDC_BTN_OK, &CLidarFullTestDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_OFFSET, &CLidarFullTestDlg::OnBnClickedBtnOffset)
END_MESSAGE_MAP()

//CComX gcom;

/*
int CLidarFullTestDlg::FlashCommList()
{	
	CComboBox *pCombo = (CComboBox*)GetDlgItem(IDC_COM);
	pCombo->ResetContent();
	HANDLE hCom = NULL;
	char CommPort[256];
	for (int i = 0; i < MAX_COM_NUM; i++)
	{	
			BOOL b = gcom.OpenWindows(i+1, 115200);
		//);

		if (b)
		{
			//AfxMessageBox(CommPort);
			gcom.Close();

			sprintf(CommPort, "串口%d", i);
			pCombo->AddString(CString(CommPort));
			pCombo->SetCurSel(0);
		}
		else
		{
			continue;
		}
		//k++;//k记载成功打开的个数
	}
	return 0;
}
*/
BOOL CLidarFullTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	//Start init
	//FlashCommList();
	CEdit *pVal = (CEdit*)GetDlgItem(IDC_EDT_VAL);
	pVal->SetWindowText("0");

	LoadParam();
	//END

	mThresVal = 70;
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLidarFullTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLidarFullTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}


}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CLidarFullTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "afxinet.h" 
bool PostContent(CString strUrl, const CString &strPara, CString &strContent, CString &strDescript)  
{                           
	try
	{  
		strDescript = "提交成功完成！";  
		bool bRet = false;  
		CString strServer, strObject, strHeader, strRet;  
		unsigned short nPort;  
		DWORD dwServiceType;  
		if(!AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort))  
		{  
			strDescript = strUrl + "不是有效有网络地址！";  
			return false;  
		}  
		CInternetSession sess;//Create session  

		CHttpFile* pFile;  

		CHttpConnection *pServer = sess.GetHttpConnection(strServer, nPort);  

		if(pServer == NULL)  
		{  
			strDescript = "对不起，连接服务器失败！";  
			return false;  
		}  
		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,strObject,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);  
		if(pFile == NULL)  
		{  
			strDescript = "找不到网络地址" + strUrl;  
			return false;  
		}  

		CString strHeaders = _T("Content-Type:application/x-www-form-urlencoded;Access-Control-Allow-Origin:http://www.sogou.com");  
			pFile -> SendRequest(strHeaders, (LPTSTR)(LPCTSTR)strPara, strPara.GetLength());  

		CString strSentence;  
		DWORD dwStatus;  
		DWORD dwBuffLen = sizeof(dwStatus);  
		BOOL bSuccess = pFile->QueryInfo(  
			HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,  
			&dwStatus, &dwBuffLen);  

		if( bSuccess && dwStatus>=  200 && dwStatus<300)  
		{  
			char buffer[256];  
			memset(buffer, 0, 256);  
			int nReadCount = 0;  
			while((nReadCount = pFile->Read(buffer, 2048)) > 0)  
			{  
				strContent += buffer;  
				memset(buffer, 0, 256);  
			}  
			bRet = true;  
		}  
		else  
		{  
			strDescript = "网站服务器错误" + strUrl;  
			bRet = false;  
		}  
		////////////////////////////////////////  
		pFile->Close();  
		sess.Close();  
		return bRet;  
	}  
	catch(...)  
	{  
		int nCode = GetLastError();  
		strDescript.Format("向服务器post失败！错误号：%d", nCode);  
		return false;  
	}  
}

/*/////////////////////////////////////////////////
sn:雷达产品序列号
remark:产品描述
flag:是否插入记录
simple_echo:服务端简单回显。简单仅返回true或failed,如果非简单回显，则返回一张网页
*//////////////////////////////////////////////////
int CLidarFullTestDlg::AddDatabaseRecord(const char *sn, const char *descrept, int flag, int simple_echo)
{
	CString strUrl = "http://rs.imscv.com/mf/home/Index/AddRecord";
	CString strPara;
	strPara.Format("sn=%s&remark=%s&flag=true&simple_echo=%d", sn, descrept, simple_echo);
	CString strContent="";
	CString strDesc="";
	bool b = PostContent(strUrl, strPara, strContent, strDesc); //true false
	return b;
}

int CLidarFullTestDlg::GenerateSN(CString *str)
{
	SYSTEMTIME st;

	GetLocalTime(&st);
	CString strTime;
	strTime.Format("%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);
	*str = strTime;

	return true;
}

void CLidarFullTestDlg::OnBnClickedBtnTest()
{	
	//TRACE(strDesc);
}
/*
int CLidarFullTestDlg::StartComm()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_COM);
	CString strPort;
	pEdit->GetWindowText(strPort);

	char buf[64];
	strcpy(buf, strPort);
	int port = atoi(&buf[4]);

	int result = gcom.OpenWindows(port, 115200);
	gcom.SetRxPackageCallBack(RxCallBackFunc, this);
	return result;
}*/


int CLidarFullTestDlg::StartUI()
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_BTN_OK);
	pBtn->EnableWindow(FALSE);

	CButton *pOffset = (CButton*)GetDlgItem(IDC_OFFSET);
	pOffset->EnableWindow(FALSE);

	//CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDT_RESULT);
	//SetResultInfo("开始", 1);

	return TRUE;
}




int CLidarFullTestDlg::StopUI()
{
	char info[1024];

	if (communicate_check==0)
	{
		pThis->m_code = 0;
		pThis->SetFont(0);
		pThis->SetResultInfo("通信检测出错\r\n", 0);

		CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
		p->SetWindowText("通信出错");
		pThis->m_matchOK = 2;
		pThis->SetFont(2);
	}
	else
	{
		if (communicate_recheck)
		{
			pThis->m_code = 0;
			pThis->SetFont(0);
			pThis->SetResultInfo("通信检测出错\r\n", 0);

			CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
			p->SetWindowText("通信出错");
			pThis->m_matchOK = 2;
			pThis->SetFont(2);

		}
		else
		{
			if (check_move)
			{
				CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
				p->SetWindowText("校准失败");
				pThis->m_matchOK = 2;
				pThis->SetFont(2);
			}
			if (!offset_ok&&offset_flag)
			{

				sprintf(info, "6米处测距标定失败，请重新做标定工作\r\n");
				pThis->SetResultInfo(info, 1);

				CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
				p->SetWindowText("标定失败");
				pThis->m_matchOK = 2;
				pThis->SetFont(2);
			}
		}
	}

	communicate_check = 0;
	offset_flag = 0;
	offset_to_zero = 0;
	offset_ok = 0;
	lidar_data_cnt = 0;
	offset_to_zero_cnt = 0;
	
	KillTimer(1);
	gStartFlag = 0;//关闭线程
	pThis->CloseAllCom();

	CButton *pBtn = (CButton*)GetDlgItem(IDC_BTN_OK);
	pBtn->EnableWindow(TRUE);

	CButton *pOFFSET = (CButton*)GetDlgItem(IDC_OFFSET);
	pOFFSET->EnableWindow(TRUE);
	return TRUE;
}
/*
int CLidarFullTestDlg::CheckLidarFunc()
{
	//检测雷达地图匹配能力
	//send msg
	uint8_t buf[sizeof(CompareMapData_S) + sizeof(SdkProtocolHeader)];

	SdkProtocolHeader *header = (SdkProtocolHeader *)buf;
	header->deviceAddr = MF_COMPARE_MAP_DATA;

	CompareMapData_S *mapData = (CompareMapData_S*)&buf[sizeof(SdkProtocolHeader)];

	mapData->timeForSN = GetTime();

	gcom.WritePackage(buf, sizeof(buf));
	printf("检测雷达地图匹配能力\n");
	return TRUE;
}*/

void CLidarFullTestDlg::CloseAllCom()
{
	for (int i = 0; i < MAX_COM_NUM; i++)
	{
		mCom[i].Close();
	}
}

void CLidarFullTestDlg::ChangeToSensorMode(CComX* pCom)
{
	SdkProtocolHeader header;
	header.deviceAddr = DEVICE_MSG;
	header.functionCode = CHANGE_LIDAR_TO_SENSOR_MODE;
	pCom->WritePackage((unsigned char*)&header,sizeof(SdkProtocolHeader));
}

void CLidarFullTestDlg::ChangeToNaviMode(CComX* pCom)
{
	SdkProtocolHeader header;
	header.deviceAddr = DEVICE_MSG;
	header.functionCode =CHANGE_LIDAR_TO_PACK_MODE;
	pCom->WritePackage((unsigned char*)&header,sizeof(SdkProtocolHeader));
}

void CLidarFullTestDlg::TellLidarIMUCalibrate(CComX* pCom)
{
	SdkProtocolHeader header;
	header.deviceAddr = ALG_ADDRESS;
	header.functionCode = ALG_CONTROL_REG;
	header.startAddr = ALG_CR_ADDR_IMU_CELIBRATE;
	header.len = 0;
	pCom->WritePackage((unsigned char*)&header,sizeof(SdkProtocolHeader));
}

unsigned int CLidarFullTestDlg::Package(PackageDataStruct package)
{
	u32 j = 0;
	u32 i = 0;
	SdkProtocolHeader sdk_header;
	u8 *psdk = (u8 *)&sdk_header;
	u8 checksum = 0;

	if ((package.DataInBuff == NULL) || (package.DataOutBuff == NULL) || (package.DataOutLen == NULL))
		return PACK_FAIL;
	sdk_header.deviceAddr = LIDAR_ADDRESS;
	sdk_header.functionCode = package.DataID;
	sdk_header.startAddr = 0;
	sdk_header.len = package.DataInLen;

	*(package.DataOutBuff + i++) = P_HEADER;
	*(package.DataOutBuff + i++) = P_HEADER;

	for (j = 0; j<sizeof(SdkProtocolHeader); j++)
	{
		if ((*(psdk + j) == P_CTRL) || (*(psdk + j) == P_HEADER) || (*(psdk + j) == P_TAIL))
		{
			*(package.DataOutBuff + i++) = P_CTRL;
		}
		*(package.DataOutBuff + i++) = *(psdk + j);
		checksum += *(psdk + j);
	}

	for (j = 0; j<package.DataInLen; j++)
	{
		if ((*(package.DataInBuff + j) == P_CTRL) || (*(package.DataInBuff + j) == P_HEADER) || (*(package.DataInBuff + j) == P_TAIL))
		{
			*(package.DataOutBuff + i++) = P_CTRL;
		}
		checksum += *(package.DataInBuff + j);
		*(package.DataOutBuff + i++) = *(package.DataInBuff + j);
	}

	if ((checksum == P_CTRL) || (checksum == P_HEADER) || (checksum == P_TAIL))
	{
		*(package.DataOutBuff + i++) = P_CTRL;
	}
	*(package.DataOutBuff + i++) = checksum;

	*(package.DataOutBuff + i++) = P_TAIL;
	*(package.DataOutBuff + i++) = P_TAIL;

	*package.DataOutLen = i;

	return PACK_OK;
}

void CLidarFullTestDlg::TellLidarOffset(CComX* pCom,unsigned int id,s16 config)
{
	PackageDataStruct package;
	u8 buffer[32];
	u32 len;
	u8 i = 0;
	package.DataID = (PackageIDTypeDef)id;
	package.DataInBuff = (u8 *)&config;
	package.DataInLen = sizeof(config);
	package.DataOutBuff = buffer;
	package.DataOutLen = &len;
	//打包数据
	Package(package);
	//发送数据包有问题
	#ifdef WIN32
		int nw = pCom->WriteRawBuf(buffer, len);
	#else
		int nw = pCom->WriteRawBuf(buffer, len);
	#endif
}



DWORD WINAPI TestComPorc(void *arg)
{
	char info[1024];	
	char j;
	u8 buffer[50] = { 0xAA,0xAA,0x10,0x01,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x13,0x55,0x55 };
	//Sleep(400);
	bool opened = false;
	((CEdit*)pThis->GetDlgItem(IDC_EDT_RESULT))->SetWindowText("");
	pThis->isTestOver = false;
	for (int i = 0; i < MAX_COM_NUM; i++)
	{
		if (pThis->isTestOver)
		{
			break;
		}
		BOOL n = pThis->mCom[i].OpenWindows(i+1, 115200);
		if (n)
		{
			opened = true;
			lidar_first=1;
			start_check=0;
			lidardata_cnt=0;
			sprintf(info, "开启COM%d,并尝试连接测试...", pThis->mCom[i].m_PortNo);
			TRACE("%s\n", info);
			pThis->m_code = 1;
			pThis->SetResultInfo(info, pThis->m_code);			
			pThis->mCom[i].SetRxPackageCallBack(CLidarFullTestDlg::RxCallBackFunc, &pThis->mCom[i]);	

			if (offset_flag)//判断是否进行测距标定
			{

				
				pThis->ChangeToSensorMode(&(pThis->mCom[i])); //获取雷达数据
				
				printf("获取雷达数据中\r\n");


			}
			else
			{
				pThis->ChangeToNaviMode(&(pThis->mCom[i]));

				pThis->TellLidarIMUCalibrate(&(pThis->mCom[i]));
				
				printf("发送指令IMU校准\r\n");
			}

		}
	}
	if (opened)
	{
		if (!offset_flag)
		{
			pThis->m_code = 1;
			pThis->SetResultInfo("连接成功，开始imu校准...", 0);
		}
		else
		{
			pThis->m_code = 1;
			pThis->SetResultInfo("连接成功，开始测距标定...", 0);
		}
	}
	else
	{
		pThis->m_code = 0;
		pThis->SetFont(0);
		pThis->SetResultInfo("没有任何可以打开的串口，\r\n\t1、重试点击按钮\r\n\t2、重启程序\r\n\t3、重启雷达", 0);
		pThis->StopUI();
	}
	return 0;
}

void CLidarFullTestDlg::TestAllCom()
{
	CreateThread(NULL, 0, TestComPorc, this, 0, NULL);
}


/*
int gRun = 1;
DWORD WINAPI UICheckProc(void *arg)
{
	while (gRun)
	{
		time_t 
			if (gStartFlag)
			{

			}
			else
			{
				Sleep(100);
			}
	}
	return 0;
}
int gNow, gLast;
*/

void CLidarFullTestDlg::OnBnClickedBtnStop()
{
	
}

void CLidarFullTestDlg::SetFont(int code)
{
	int size = 0;
	if (1 == code)
	{
		m_code = code;
		size = 12;
	}
	else if(2==code)
	{
		size=20;
	}
	else
	{
		m_code = code;
		size = 13;
	}
	
	m_Font.Detach();
	m_Font.CreateFont(size, 0, 0, 0, 400, FALSE, FALSE, 0,
		GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_MODERN, "宋体");
	//设置打印窗口字体
	if(code==1||code==0)
		GetDlgItem(IDC_EDT_RESULT)->SetFont(&m_Font);

	else if(code==2)
		GetDlgItem(IDC_BTN_SIG)->SetFont(&m_Font);

	m_Font.Detach();
	return;


	/*
	LOGFONT lf;

	if (0 == code)
	lf.lfHeight = 16;
	else
	lf.lfHeight = 20;

	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfQuality = PROOF_QUALITY;

	strcpy(lf.lfFaceName, "宋体");  //设置字体

	pFont = GetDlgItem(IDC_EDT_RESULT)->GetFont();
	pFont->Detach();
	pFont->CreateFontIndirect(&lf);
	GetDlgItem(IDC_EDT_RESULT)->SetFont(pFont);
	pFont->Detach();
	*/

}

HBRUSH CLidarFullTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	if (pWnd->GetDlgCtrlID() == IDC_EDT_RESULT)
	{
		COLORREF clr;
		if (m_code == 1)
		{
			clr = RGB(0, 0, 255);
		}
		else
		{
			clr = RGB(255, 0, 0);
		}
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clr);   //设置红色的文本

		CFont cFont;
		int size = 0;
		if (0 == m_code)
			size = 32;
		else
			size = 40;

		clr = RGB(255, 255, 255);
		pDC->SetBkColor(clr);     //设置黑色的背景    
		hbr = ::CreateSolidBrush(clr);
		return hbr;  //作为约定，返回背景色对应的刷子句柄 
	}
	else if (pWnd->GetDlgCtrlID() == IDC_BTN_SIG)
	{
		COLORREF clr;
		if (m_matchOK == 1)
		{
			clr = RGB(0, 255, 0);
		}
		else if(m_matchOK == 2)
		{
			clr = RGB(255,0,0) ;
		}
		else
		{
			clr = RGB(0, 0, 255);
		}
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clr);   //设置红色的文本

	}
	else
	{
		HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
	}
	return hbr;
}

void CLidarFullTestDlg::SetResultInfo(const char * info, int lidarResult)
{
	CEdit *pEdt = (CEdit*)GetDlgItem(IDC_EDT_RESULT);
	CString str(info);
	str += "\r\n";
	int l = pEdt->GetWindowTextLength();
	pEdt->SetSel(l, l, TRUE);
	pEdt->ReplaceSel(str);
	//pEdt->GetWindowText(str);

	//str += info;
	//pEdt->SetWindowText(str);
}

int CLidarFullTestDlg::GetToday()
{	
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	return st.wDay;
}

void CLidarFullTestDlg::UpdateToday(int today)
{
	char cday[32];
	sprintf(cday, "%d", today);

	WritePrivateProfileString("SN", "day", cday, INI_FILE);
}
void CLidarFullTestDlg::ResetSN()
{
	WritePrivateProfileString("SN", "serial", "0", INI_FILE);
}
const UINT16 CRC16_Table[256] =
{
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0X0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0X5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};
UINT16 CRC16_CrcClac(UINT8 *pucMsg, UINT32 usLen, UINT16 CRC16)
{
	DWORD i;
	for(i = 0; i < usLen; i++)
		CRC16 = CRC16_Table[((CRC16 & 0xFF) ^ (*pucMsg++))] ^ (CRC16 >> 8);
	return(CRC16);
}

CString IntToHex(int val, int len)
{
	CString str;
	CString length;
	length.Format("%d", len);
	CString fmt;
	fmt += "%0";
	fmt += length;
	fmt += "X";
	str.Format(fmt, val);
	return str;
}
int axtoi(const char *p)
{
	int i = 0;
	sscanf(p, "%x", &i);
	return i;
}

/*函数说明
字符全部为16进制
年(2B)+月(1B)+日(2B)+PLACE(1B)+MODE(2B)+CRC(4B)+SERIAL(4B)
初始的CRC为4个"0"，计算出CRC后，替换这4个0
*/
int CLidarFullTestDlg::GenSNEx(int serial, char *pcSerial, char *sn)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	int Year = st.wYear;
	int Month = st.wMonth;
	int Day = st.wDay;
	int ModeTable[] = {0x7E};
	int ProductionPlaceTable[] = {0,1};
	int ModeIndex = 0;
	int ProductionPlaceIndex = 0;
	CString Sn = IntToHex(Year - 2000, 2) + IntToHex(Month, 1) + IntToHex(Day, 2);
	Sn += IntToHex(ProductionPlaceTable[ProductionPlaceIndex], 1) + IntToHex(ModeTable[ModeIndex], 2);
	Sn += "0000";
	//int p = SerialEdit->Text.ToIntDef(1);
	int p = serial;
	CString sf;
	sf.Format("%04X", p);
	strcpy(pcSerial, sf);
	Sn += sf;
	unsigned char SnBuf[8];
	for(int i = 0 ; i < 8 ; i ++)
	{
		CString sHex = "0x" + Sn.Mid(1 + i * 2, 2);
		SnBuf[7 - i] = axtoi(sHex);
	}
	UINT16 crc = CRC16_CrcClac(SnBuf, 8, 0);
	
	Sn = Sn.Mid(0, 8)+IntToHex(crc, 4)+sf;
	
	strcpy(sn, Sn);
	
	return 0;
}

int CLidarFullTestDlg::GenSN(char *pcSerial, char *sn)
{	
	int day;
	
	day = GetPrivateProfileInt("SN", "day", 0, INI_FILE);
	int today = GetToday();
	if (day != today)
	{
		UpdateToday(today);
		ResetSN();
	}
	char pcSerialTmp[32];
	GetPrivateProfileString("SN", "serial", "0", pcSerialTmp, 16, INI_FILE);

	int serial = axtoi(pcSerialTmp);
	
	//sprintf(pSerial, "%04x", i);

	//按照编码规则生成full的sn
	//strcpy(serial, pSerial);
	//strcpy(sn, pSerial);
	GenSNEx(serial,OUT pcSerial, OUT sn);
	return 0;
}

int CLidarFullTestDlg::IncSN()
{
	char pcSerial[32];
	GetPrivateProfileString("SN", "serial", "0", pcSerial, 16, INI_FILE);
	
	int serial = axtoi(pcSerial);
	sprintf(pcSerial, "%04x", ++serial);
	
	WritePrivateProfileString("SN", "serial", pcSerial, INI_FILE);
		
	return 0;
}
//test code   ////////////////////////////////////////////////////////////////
void CLidarFullTestDlg::OnBnClickedBtnStart()
{
	char sn[32], serial[32];
	GenSN(OUT serial, OUT sn);//cache sn to file
	
	if (1) //if write db ok, inc sn
	{
		IncSN();
	}
}
////////////////////////////////////////////////////////////////
int CLidarFullTestDlg::GetPercent()
{
	CEdit *pVal = (CEdit*)GetDlgItem(IDC_PERCENT);
	CString strVal;
	pVal->GetWindowText(strVal);
	return atoi(strVal);

}

#pragma pack(1)
struct Lidar2DPoint
{
  u16 distance;
  u8 confidence;
};

struct LidarDataInfo
{
  u16 Temperature;
  s16 CurrSpeed;
  s16 GivenSpeed;
  Lidar2DPoint points[LIDAR_RESOLUTION];
};
#pragma pack(pop)

void CLidarFullTestDlg::RxCallBackFunc(void *pParam, const unsigned char * byBuf, int dwLen)
{
	CComX *pCom = (CComX*)pParam;

	if (dwLen < sizeof(SdkProtocolHeader))
	{
		printf("RxCallBackFunc read len = %d\n", dwLen);
		return;
	}

	SdkProtocolHeader* header = (SdkProtocolHeader*)byBuf;
	
	int ThresValue = pThis->mThresVal;
	
	//算法板和雷达有通信
	communicate_check = 1;

	/*imu校准*/
	if(header->deviceAddr == DEVICE_MSG) 
	{
		printf("RxCallBackFunc %d %d\n", header->deviceAddr, dwLen);
		printf("RxCallBackFunc %d \n", header->functionCode);
		if(header->functionCode == IMU_CALIBRATE_SUCCESS) //表示imu已经校准好
		{	
			printf("imu校准成功啦啦啦啦啦啦啦啦啦啦\r\n");
			pThis->SetResultInfo("imu校准完成", 0);
			pThis->SetResultInfo("进行下一步测试.....", 0);
			check_move=1;
			offset_to_zero = 1;
			
			start_check=1;
			pThis->ChangeToSensorMode(pCom); //获取雷达数据
		}
	}
	else if (MF_COMPARE_MAP_DATA == header->deviceAddr)//step2
	{		
		pThis->isTestOver = true;

		//这个应该是雷达传上来的数据
		CompareMapData_S *resp = (CompareMapData_S*)&byBuf[sizeof(SdkProtocolHeader)]; 
		//雷达第一次传送的数据为零
		if(resp->result==0)
		{
			unsigned char buf[sizeof(CompareMapData_S) + sizeof(SdkProtocolHeader)];
			SdkProtocolHeader *header = (SdkProtocolHeader *)buf;
			header->deviceAddr = MF_COMPARE_MAP_DATA; //step 1
			CompareMapData_S *mapData = (CompareMapData_S*)&buf[sizeof(SdkProtocolHeader)];
			mapData->result = pThis->mThresVal;
			mapData->timeForSN = GetTime();
			pCom->WritePackage(buf, sizeof(buf));
			return;
		}
		char info[1024];
		//距离测试与地图匹配
		CEdit *up = (CEdit*)pThis->GetDlgItem(IDC_UP);
		CEdit *down = (CEdit*)pThis->GetDlgItem(IDC_DOWN);
		CEdit *left = (CEdit*)pThis->GetDlgItem(IDC_LEFT);
		CEdit *right = (CEdit*)pThis->GetDlgItem(IDC_RIGHT);
		CString sup, sdown, sleft, sright;
		float fup,fdown,fleft,fright;
		up->GetWindowText(sup);
		down->GetWindowText(sdown);
		left->GetWindowText(sleft);
		right->GetWindowText(sright);
		fup = atof(sup);
		fdown = atof(sdown);
		fleft = atof(sleft);
		fright = atof(sright);
		int percent = pThis->GetPercent();
		float error[4];
		float max_error=0;
		error[3] = fabs( fup - distanceoofUpDownLeftRight[1])  / fup *100;
		error[2] = fabs( fdown - distanceoofUpDownLeftRight[0]) / fdown *100;
		error[1] = fabs( fleft - distanceoofUpDownLeftRight[3])/ fleft *100;
		error[0] = fabs ( fright - distanceoofUpDownLeftRight[2] )/ fright *100;
		for(int count=0;count<4;count++)
		{
			if(error[count]>max_error)
			{
				max_error=error[count];
			}
		}

		if ( (error[0] > percent) || (error[1] > percent) || (error[2] > percent) || (error[3] > percent) )
		{
			sprintf(info, "测量值(%.1f %.1f %.1f %.1f)  设定值(%.1f %.1f %.1f %.1f)\r\n误差超过%d%%\r\n不能进行匹配度测试", 
				distanceoofUpDownLeftRight[1],distanceoofUpDownLeftRight[0],
				distanceoofUpDownLeftRight[3],distanceoofUpDownLeftRight[2],
				fup, fdown, fleft, fright,percent);
			pThis->SetResultInfo(info, -1);
			pThis->m_matchOK = 2;
			CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
			p->SetWindowText("误差过大");
			pThis->SetFont(2);
			pThis->SetFont(0);
			return;
			
		}
		else
		{
			sprintf(info, "距离测试正确\r\n测量值(%.1f %.1f %.1f %.1f)  设定值(%.1f %.1f %.1f %.1f)\r\n误差为%.2f%%", 
				distanceoofUpDownLeftRight[1],distanceoofUpDownLeftRight[0],
				distanceoofUpDownLeftRight[3],distanceoofUpDownLeftRight[2],
				fup, fdown, fleft, fright,max_error);
			pThis->SetResultInfo(info, 1);
		}			
		//完成
			
			

		CEdit *pVal = (CEdit*)pThis->GetDlgItem(IDC_EDT_VAL);
		CString strVal;
		pVal->GetWindowText(strVal);
		char match=atoi(strVal);
		if (resp->result > ThresValue )
		{
			//字符串转成int
			sprintf(info, "匹配值正确, 实际%d>设定%d", resp->result , atoi(strVal));
			pThis->SetResultInfo(info, 0);
			CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
			pThis->SetFont(2);
			p->SetWindowText("测试成功");
			pThis->SetFont(2);
			pThis->m_matchOK = 1;
		}
		else
		{
			sprintf(info, "匹配值错误, 实际%d<=设定%d", resp->result , atoi(strVal));
			pThis->SetResultInfo(info, 1);
			pThis->m_matchOK = 2;
			CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
			p->SetWindowText("匹配失败");
			pThis->SetFont(2);
			pThis->SetFont(0);
			return ;
		}
		pThis->Invalidate();
		char cResult[256]={0};
		pThis->SetFont(resp->result>ThresValue?1:0);
		if (resp->exists)
		{
			sprintf(cResult, "序列号(SN)文件已经存在，SN码为：%s", resp->sn);			
			pThis->SetResultInfo(cResult, 1);
		}
		else
		{
			sprintf(cResult, "序列号(SN)文件不存在");
			pThis->SetResultInfo(cResult, 0);
		}
		//响应结果值大于预设值，且坐标符合
		if (resp->result > atoi(strVal) )     //start step 3
		{
			CString str = resp->sn;

			char sn[32], serial[32];
			pThis->GenSN(OUT serial, OUT sn);//cache sn to file
			int b = pThis->AddDatabaseRecord(sn, "正常");		
			if (b) //if write db ok, inc sn
			{
				pThis->SetResultInfo("向设备写入SN...", resp->result);
				NetCompareMapData_S req;
				req.hdr.deviceAddr = SAVE_SN;
				req.hdr.functionCode = 0;
				strcpy(req.sn, sn);

				int n = pCom->WritePackage((unsigned char*)&req, sizeof(req));	//step 3			
			}			
		}
	}
	else if (SAVE_SN == header->deviceAddr) //step 4
	{
		CompareMapData_S *resp = (CompareMapData_S*)&byBuf[sizeof(SdkProtocolHeader)];
		char cResult[256];
		memset(cResult, 0, sizeof(cResult));
		if (resp->exists == 0)
		{
			pThis->SetFont(1);
			sprintf(cResult,"写入SN -->> %s 成功", resp->sn);
			pThis->IncSN();
		}
		else 
		{
			pThis->SetFont(1);
			sprintf(cResult, "设备已经存在SN码,不需要写入");
		}
		pThis->SetResultInfo(cResult, 0);	

		pThis->StopUI();
	}
	else if(LIDAR_ADDRESS == header->deviceAddr)
	{
		static char k=0;
		static int a=0;
		
		char info[1024];

		communicate_check = 1;
		
		/*if (offset_flag == 1 && offset_to_zero == 0)
			return;*/
		
		if(header->functionCode == 0)
		{
			if(start_check==0&&offset_flag==0)
				return ;
			if(header->len == sizeof(LidarDataInfo))
			{
				if (header->startAddr == 1)
					communicate_recheck = 1;
				if (lidar_data_cnt < 20)
				{
					lidar_data_cnt++;
					return;
				}
				check_move=0;
				//偏移找到雷达的数据
				LidarDataInfo *lidarData = (LidarDataInfo*)&byBuf[sizeof(SdkProtocolHeader)];
				
				/*else
				{
					if (lidarData->points[0].distance == 0 || lidarData->points[90].distance == 0 || lidarData->points[180].distance == 0 || lidarData->points[270].distance == 0)
						return;
				}*/

				printf("recv LIDAR_ADDRESS %d %d %d %d\n",lidarData->points[0].distance,
					lidarData->points[90].distance,
					lidarData->points[180].distance,
					lidarData->points[270].distance);
				/*return;*/
				

				if (lidarData->points[0].distance == 0 || lidarData->points[90].distance == 0 || lidarData->points[180].distance == 0 || lidarData->points[270].distance == 0)
				{
					if (offset_flag&&!offset_ok)
					{
						//pThis->TellLidarOffset(pCom, PACK_SET_PIXOFFSET, (short)0);		
						printf("数据不正确\r\n");
					}
					else
					{
						return;
					}
					
				}
				lidardata_up_aver = 0;
				lidardata_down_aver = 0;
				lidardata_left_aver = 0;
				lidardata_right_aver = 0;
				lidardata_down_sum = 0;
				lidardata_right_sum = 0;
				lidardata_left_sum = 0;
				lidardata_up_sum = 0;

				if(lidar_first)//判断是否已经装入十次数据
				{
					//将雷达的数据保存进数组里边
					lidardata_down[lidardata_cnt] = lidarData->points[0].distance;
					lidardata_left[lidardata_cnt] = lidarData->points[90].distance;
					lidardata_up[lidardata_cnt] = lidarData->points[180].distance;
					lidardata_right[lidardata_cnt] = lidarData->points[270].distance;
					lidardata_cnt++;
					if(lidardata_cnt==10)
					{
						for(k=0;k<10;k++)
						{
							lidardata_down_sum +=lidardata_down[k];//下
							lidardata_left_sum +=lidardata_left[k];//左
							lidardata_up_sum   +=lidardata_up[k];//上
							lidardata_right_sum+=lidardata_right[k];//右
						}
						//计算平均值
						lidardata_down_aver = lidardata_down_sum/10;
						lidardata_left_aver = lidardata_left_sum/10;
						lidardata_up_aver = lidardata_up_sum/10;
						lidardata_right_aver = lidardata_right_sum/10;

						//进行测距标定
						if(offset_flag)
						{
							if(abs(lidardata_down_aver - currentCentriod)<30)
							{ 
								sprintf(info, "6米处测距标定正确，无需再做标定\r\n");
								pThis->SetResultInfo(info, 1);

								CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
								p->SetWindowText("标定成功");					
								pThis->m_matchOK = 1;
								pThis->SetFont(2);
								offset_ok = 1;

								pThis->StopUI();
								return;						
							}
							if (!offset_to_zero)
							{
								//for(u8 i=0;i<10;i++)
								if (offset_to_zero_cnt < 5)
								{									
									pThis->TellLidarOffset(pCom, PACK_SET_PIXOFFSET, (short)0);
									offset_to_zero_cnt += 1;
								}
								else
								{
									offset_to_zero = 1;
									offset_to_zero_cnt = 0;
								}
								lidar_first = 1;
								lidardata_cnt = 0;
								
								return;
							}

							float index1,index2;
							index1=256250.0/ currentCentriod;
							index2=256250.0/ lidardata_down_aver;
							thelta_index=index1-index2;
							thelta_index *= 10;						
							//计算出偏差，然后将偏差发送给雷达
							for (int i = 0; i < 5; i++)
							{
								pThis->TellLidarOffset(pCom, PACK_SET_PIXOFFSET, (short)thelta_index);
								printf("发送标定啦啦啦啦啦啦啦啦\r\n");
							}
							offset_weitiao = 1;
						}
						else
						{
							//计算偏差
							for (k = 0; k < 10; k++)
							{
								if ((abs(lidardata_down_aver - lidardata_down[k])) > (0.03* lidardata_down_aver))
								{
									error_0++;
								}
								if ((abs(lidardata_left_aver - lidardata_left[k])) > (0.03*lidardata_left_aver))
								{
									error_1++;
								}
								if ((abs(lidardata_up_aver - lidardata_up[k])) > (0.03*lidardata_up_aver))
								{
									error_2++;
								}
								if ((abs(lidardata_right_aver - lidardata_right[k])) > (0.03*lidardata_right_aver))
								{
									error_3++;
								}
								printf("误差%d %d %d %d\r\n", error_0, error_1, error_2, error_3);
							}
							//pThis->m_code = 1;

							if (error_0 > 3 || error_1 > 3 || error_2 > 3 || error_3 > 3)
							{
								sprintf(info, "雷达数据有误,请确认周围环境\r\n");
								CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
								pThis->SetResultInfo(info, 1);
								pThis->m_matchOK = 2;
								p->SetWindowText("数据有误");
								pThis->SetFont(2);
								pThis->SetFont(0);
								pThis->StopUI();
							}
							else
							{
								//告诉算法板可以进行计算了
								distanceoofUpDownLeftRight[0] = (float)lidardata_up_aver;
								distanceoofUpDownLeftRight[1] = (float)lidardata_down_aver;
								distanceoofUpDownLeftRight[2] = (float)lidardata_left_aver;
								distanceoofUpDownLeftRight[3] = (float)lidardata_right_aver;
								unsigned char buf[sizeof(CompareMapData_S) + sizeof(SdkProtocolHeader)];
								SdkProtocolHeader *header = (SdkProtocolHeader *)buf;
								header->deviceAddr = MF_COMPARE_MAP_DATA;
								CompareMapData_S *mapData = (CompareMapData_S*)&buf[sizeof(SdkProtocolHeader)];
								mapData->result = pThis->mThresVal;
								mapData->timeForSN = GetTime();
								pCom->WritePackage(buf, sizeof(buf));

							}
						}

						
						lidar_first=0;
						start_check=0;
						error_0=0;
						error_1=0;
						error_2=0;
						error_3=0;								
					}
				}
				else
				{
					static unsigned char cnt = 0;
					
					if ((abs(lidarData->points[0].distance - currentCentriod) > 30) && (lidarData->points[0].distance != 0) && offset_flag&&offset_ok)
					{						
						offset_weitiao = 1;
						offset_ok = 0;
					}
					if (offset_ok == 0 && offset_weitiao == 1)
					{
						if ((lidarData->points[0].distance - currentCentriod) > 30)
						{
							if ((lidarData->points[0].distance - currentCentriod) > 1500)
							{
								thelta_index += 30;
								printf("微调+30\r\n");
							}
							else if ((lidarData->points[0].distance - currentCentriod) > 500)
							{
								thelta_index += 10;
								printf("微调+10\r\n");
							}
							else
							{
								thelta_index += 1;
								printf("微调+1\r\n");
							}

							for (u8 i = 0; i < 3; i++)
								pThis->TellLidarOffset(pCom, PACK_SET_PIXOFFSET, (short)thelta_index);
						}
						else if ((lidarData->points[0].distance - currentCentriod) < -30)
						{
							if ((lidarData->points[0].distance - currentCentriod) < -1500)
							{
								thelta_index -= 30; printf("微调-30\r\n");
							}
							if ((lidarData->points[0].distance - currentCentriod) < -500)
							{
								thelta_index -= 10; printf("微调-10\r\n");
							}
							else
							{
								thelta_index -= 1; printf("微调-1\r\n");
							}
							for (u8 i = 0; i < 3; i++)
								pThis->TellLidarOffset(pCom, PACK_SET_PIXOFFSET, (short)thelta_index);
							
						}
						else
						{
							if (abs(lidarData->points[0].distance - currentCentriod) < 30)
							{
								cnt += 1;
								if (cnt >= 20)
								{
									for(char offset_ok_cnt = 0;offset_ok_cnt<5;offset_ok_cnt++)
										pThis->TellLidarOffset(pCom, PACK_FLASH_CONFIG, 0x1234);									
									cnt = 0;							
									printf("\r\n\r\n校准成功\r\n\r\n");
									offset_weitiao = 0;
									offset_ok = 1;
									sprintf(info, "6米处测距标定正确\r\n");
									pThis->SetResultInfo(info, 1);

									CStatic *p = (CStatic*)pThis->GetDlgItem(IDC_BTN_SIG);
									p->SetWindowText("标定成功");
									pThis->m_matchOK = 1;
									pThis->SetFont(2);
									pThis->StopUI();
								}
							}
							else
								cnt = 0;
						}
					}
				}
			}
		}
	}
}

void CLidarFullTestDlg::OnBnClickedBtnCheck()
{
	
}


void CLidarFullTestDlg::OnBnClickedBtnClear()
{
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDT_RESULT);
	pEdit->SetWindowText("");
}


void CLidarFullTestDlg::OnBnClickedBtnFlash()
{
	//FlashCommList();
}


void CLidarFullTestDlg::OnEnChangeEdtVal()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
}

void CLidarFullTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	StopUI();	
	
	CDialog::OnTimer(nIDEvent);
}

void CLidarFullTestDlg::SaveParam()
{
	CString up, down, left, right, percent,pre_set;
	CEdit *p;
	p = (CEdit*)GetDlgItem(IDC_UP);		p->GetWindowText(up);
	p = (CEdit*)GetDlgItem(IDC_DOWN);	p->GetWindowText(down);
	p = (CEdit*)GetDlgItem(IDC_LEFT);	p->GetWindowText(left);
	p = (CEdit*)GetDlgItem(IDC_RIGHT);	p->GetWindowText(right);
	p = (CEdit*)GetDlgItem(IDC_PERCENT);	p->GetWindowText(percent);
	p = (CEdit*)GetDlgItem(IDC_EDT_VAL);	p->GetWindowText(pre_set);	

	WritePrivateProfileString("Param", "up", up, INI_FILE);
	WritePrivateProfileString("Param", "down", down, INI_FILE);
	WritePrivateProfileString("Param", "left", left, INI_FILE);
	WritePrivateProfileString("Param", "right", right, INI_FILE);
	WritePrivateProfileString("Param", "percent", percent, INI_FILE);
	WritePrivateProfileString("Param", "pre_set", pre_set, INI_FILE);
}
void CLidarFullTestDlg::LoadParam()
{
	char up[32], down[32], left[32], right[32], percent[32], pre_set[32];
	GetPrivateProfileString("Param", "up", "", up, 32, INI_FILE);
	GetPrivateProfileString("Param", "down", "", down, 32, INI_FILE);
	GetPrivateProfileString("Param", "left", "", left, 32, INI_FILE);
	GetPrivateProfileString("Param", "right", "", right, 32, INI_FILE);
	GetPrivateProfileString("Param", "percent", "", percent, 32, INI_FILE);
	GetPrivateProfileString("Param", "pre_set", "", pre_set, 32, INI_FILE);
	CEdit *p;
	p = (CEdit*)GetDlgItem(IDC_UP);		p->SetWindowText(up);
	p = (CEdit*)GetDlgItem(IDC_DOWN);	p->SetWindowText(down);
	p = (CEdit*)GetDlgItem(IDC_LEFT);	p->SetWindowText(left);
	p = (CEdit*)GetDlgItem(IDC_RIGHT);	p->SetWindowText(right);
	p = (CEdit*)GetDlgItem(IDC_PERCENT);p->SetWindowText(percent);
	p = (CEdit*)GetDlgItem(IDC_EDT_VAL);p->SetWindowText(pre_set);
}
void CLidarFullTestDlg::OnBnClickedBtnOk()
{
	gStartFlag = 1;
	SetTimer(1, 15000, NULL);

	StartUI();
	TestAllCom();
	
	SaveParam();
	return;
	/*
	FlashCommList();
	gcom.Close();
	int result = StartComm();

	if (FALSE == result)
	{
		AfxMessageBox("打开串口失败");
		return;
	}
	
	CheckLidarFunc();
	*/
}

void CLidarFullTestDlg::OnBnClickedBtnOffset()
{
	offset_flag=1;

	SetTimer(1, 30000, NULL);

	StartUI();

	TestAllCom();
	
	SaveParam();

	return;

}

