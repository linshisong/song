#include "stdafx.h"
#include "Comx.h"
//#include "../LogFile/LogFile.h"



DWORD WINAPI CComX::RecvThread(void* lpParameter)
{
	CComX *pGComm = (CComX *)lpParameter;
	pGComm->RecvThreadLoop();
	return NULL;
}

CComX::CComX(int MaxPackageSize, bool UsePackage)
{
#ifdef WIN32
	m_rxThread = INVALID_HANDLE_VALUE;
	m_hComHandle = INVALID_HANDLE_VALUE;

	memset(&m_oSend, 0, sizeof(OVERLAPPED));
	memset(&m_oRecv, 0, sizeof(OVERLAPPED));
	
	m_oSend.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_oRecv.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
#else
	m_rxThread = 0;
#endif
	m_RxPackageCallBack = NULL;
	m_RxBufCallBack = NULL;
	m_RecvThreadStopFlag = FALSE;

	m_bUsePackage = UsePackage;

	m_FrameHead = (unsigned char)0xAA;
	m_FrameTail = 0x55;
	m_FrameCtrl = 0xA5;

	                                                                                                 
	m_MaxPackageSize = MaxPackageSize * 2 + 2;

	m_pRxBuf = new unsigned char[m_MaxPackageSize];
	m_pTxBuf = new unsigned char[m_MaxPackageSize];
	m_RxPackageDataCount = 0;

	m_dwSysErrCode = 0;
	m_baudRate = -1;
	ResetCounters();

}

void CComX::ResetCounters()
{
	m_u64Total_Rx_Bytes = 0;
	m_u64Total_Tx_Bytes = 0;
	m_u64Total_Rx_Packages = 0;
	m_u64Total_Tx_Packages = 0;
	m_u64Total_Rx_LostPackages = 0;
}

CComX::~CComX()
{
#ifdef WIN32
	if (m_oRecv.hEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_oRecv.hEvent);
		m_oRecv.hEvent = INVALID_HANDLE_VALUE;
	}
	
	if (m_oSend.hEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_oSend.hEvent);
		m_oSend.hEvent = INVALID_HANDLE_VALUE;
	}
#endif
	
	Close();

	if (m_pRxBuf)
	{
		delete[] m_pRxBuf;
		m_pRxBuf = NULL;
	}
	if (m_pTxBuf)
	{
		delete[] m_pTxBuf;
		m_pTxBuf = NULL;
	}
}

/*******************************************************************************
* Function Name  : SetRxPackageCallBack
* Description    : 设置RxPackage回调
* Input          : func: 回调函数
				 : pParam: 回调参数
* Output         : void
* Return         : void
*******************************************************************************/
void CComX::SetRxPackageCallBack(RxCallBackFunc func, void *pParam)
{
	m_pRxPackageCallBackParam = pParam;
	m_RxPackageCallBack = func;
}

/*******************************************************************************
* Function Name  : SetRxBufCallBack
* Description    : 设置RxPackage回调
* Input          : func: 回调函数
:pParam: 回调参数
* Output         : void
* Return         : void
*******************************************************************************/
void CComX::SetRxBufCallBack(RxCallBackFunc func, void *pParam)
{
	m_pRxPBufCallBackParam = pParam;
	m_RxBufCallBack = func;
}

#ifdef WIN32
BOOL CComX::OpenWindows(int dwPortNo, int dwBaud)
{
	TCHAR sCom[100] = _T("\\\\.\\COM");

#ifdef UNICODE
	int ge = dwPortNo % 10;
	int shi = dwPortNo / 10;
	if (shi){
		sCom[7] =(TCHAR)( shi + '0');
		sCom[8] =(TCHAR)( ge + '0');
		sCom[9] = '\0';
	}
	else{
		sCom[7] =(TCHAR)( ge + '0');
		sCom[8] = '\0';
	}
#else
	TCHAR szPortNo[3];
	//_itot(dwPortNo, szPortNo, 10);
	_itot_s(dwPortNo, szPortNo, sizeof(szPortNo), 10);
	
	_tcscat_s(sCom, 10, szPortNo);
#endif
	if (m_IsOpened)
	{
		if ((dwPortNo == m_PortNo) && (dwBaud == (int)m_DCB.BaudRate))
		{
			return TRUE;
		}
		else
		{
			Close();
		}
	}

	memcpy(m_path, sCom,strlen(sCom));

	m_PortNo = dwPortNo;
	m_hComHandle = ::CreateFile(sCom,
		GENERIC_READ | GENERIC_WRITE, 0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);
	if (m_hComHandle == INVALID_HANDLE_VALUE)
	{
		//TRACE("\nComport=%d\n", m_PortNo);
		m_dwSysErrCode = GetLastError();
		m_hComHandle = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	
	PurgeComm(m_hComHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	
	if (SetupComm(m_hComHandle, m_MaxPackageSize, m_MaxPackageSize) == FALSE)
	{
		m_dwSysErrCode = GetLastError();
		CloseHandle(m_hComHandle);
		return FALSE;
	}
	
	m_DCB.DCBlength = sizeof(DCB);
	if (GetCommState(m_hComHandle, &m_DCB) == FALSE)
	{
		m_dwSysErrCode = GetLastError();
		CloseHandle(m_hComHandle);
		return FALSE;
	}
	else
	{
		m_DCB.BaudRate = dwBaud;
		m_DCB.ByteSize = 8;
		m_DCB.Parity = NOPARITY;
		m_DCB.StopBits = ONESTOPBIT;
		
		if (SetCommState(m_hComHandle, &m_DCB) == FALSE)
		{
			m_dwSysErrCode = GetLastError();
			CloseHandle(m_hComHandle);
			return FALSE;
		}
	}
	
	Sleep(60);
	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout = 1;
	TimeOuts.ReadTotalTimeoutMultiplier = 1;
	TimeOuts.ReadTotalTimeoutConstant = 1;
	
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 1000;
	TimeOuts.WriteTotalTimeoutConstant = 50000;
	
	SetCommTimeouts(m_hComHandle, &TimeOuts);
	
	m_RecvThreadStopFlag = FALSE;
	m_rxThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread, this, 0, NULL);
	m_IsOpened = TRUE;
	return TRUE;
}
#endif
/*******************************************************************************
* Function Name  : Open
* Description    : 打开串口并启动接收线程
* Input          : path: 串口地址
:baudRate: 波特率
* Output         : void
* Return         : >=0 串口fd 或者失败 <0
*******************************************************************************/
#ifndef WIN32

/*******************************************************************************
* Function Name  : getBaudrate
* Description    : 将int型波特率值转换为speet_t值的波特率
* Input          : baudrate: 波特率
* Output         : void
* Return         : speet_t 类型波特率
*******************************************************************************/
speed_t CComX::GetBaudrate(int baudrate) 
{
	switch (baudrate) {
	case 0:
		return B0;
	case 50:
		return B50;
	case 75:
		return B75;
	case 110:
		return B110;
	case 134:
		return B134;
	case 150:
		return B150;
	case 200:
		return B200;
	case 300:
		return B300;
	case 600:
		return B600;
	case 1200:
		return B1200;
	case 1800:
		return B1800;
	case 2400:
		return B2400;
	case 4800:
		return B4800;
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	case 230400:
		return B230400;
	case 460800:
		return B460800;
	case 500000:
		return B500000;
	case 576000:
		return B576000;
	case 921600:
		return B921600;
	case 1000000:
		return B1000000;
	case 1152000:
		return B1152000;
	case 1500000:
		return B1500000;
	case 2000000:
		return B2000000;
	case 2500000:
		return B2500000;
	case 3000000:
		return B3000000;
	case 3500000:
		return B3500000;
	case 4000000:
		return B4000000;
	default:
		return 0xFFFFFFFF;
	}
}

int CComX::OpenLinux(const char* path, int baudRate)
{
	if (m_IsOpened)
	{
		if ((0 == strcmp(path, m_path)) && (baudRate == m_baudRate))
		{
			return TRUE;
		}
		else
		{
			Close();
		}
	}

	speed_t speed;

	//printf("init native Check arguments\n");
	/* Check arguments */
	{
		speed = GetBaudrate(baudRate);
		if (speed == 0xFFFFFFFF) {
			/* TODO: throw an exception */
			printf("Invalid baudrate\n");
			return -1;
		}
	}

	//printf("init native Opening device!\n");
	/* Opening device */
	{
		printf("Opening serial port %s\n", path);
		//      fd = open(path_utf, O_RDWR | O_DIRECT | O_SYNC);  
		m_fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
		printf("open() fd = %d\n", m_fd);
		if (m_fd < 0) {
			/* Throw an exception */
			printf("Cannot open port %d", baudRate);
			/* TODO: throw an exception */
			return -1;
		}
	}

	//printf("init native Configure device!\n");
	/* Configure device */
	{
		struct termios cfg;
		if (tcgetattr(m_fd, &cfg)) {
			printf("Configure device tcgetattr() failed 1\n");
			close(m_fd);
			return -1;
		}

		cfmakeraw(&cfg);
		cfsetispeed(&cfg, speed);
		cfsetospeed(&cfg, speed);

		if (tcsetattr(m_fd, TCSANOW, &cfg)) {
			printf("Configure device tcsetattr() failed 2\n");
			close(m_fd);
			/* TODO: throw an exception */
			return -1;
		}
	}

	strcpy(m_path, path);	//会copy '\0'
	baudRate == m_baudRate;

	m_RecvThreadStopFlag = FALSE;

	pthread_create(&m_rxThread, NULL, RecvThread, this);
	m_IsOpened = TRUE;

	printf("OpenLinux ok, m_IsOpened = %d\n", m_IsOpened);
	return 1;
}
#endif
/*******************************************************************************
* Function Name  : Close
* Description    : 关闭串口 并关闭接收线程
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
int CComX::Close()
{
	printf("CLOSE\n");
	m_RecvThreadStopFlag = TRUE;	
	
#ifdef WIN32	
	if (m_hComHandle != INVALID_HANDLE_VALUE)
	{	
		//char buf[256];
		//sprintf(buf, "\nComport=%d\n", m_PortNo);
		//AfxMessageBox(buf);
		CloseHandle(m_hComHandle);	
		m_hComHandle = INVALID_HANDLE_VALUE;
	}
	
	m_RecvThreadStopFlag = TRUE;
	if (m_rxThread != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_rxThread, 1000);
		CloseHandle(m_rxThread);
		m_rxThread = INVALID_HANDLE_VALUE;
	}

#else
	CLOSE(m_fd);
	pthread_join(m_rxThread, NULL); 
#endif
	m_IsOpened = FALSE;
	printf("close, m_IsOpened=false\n");
	return 0;
}

/*******************************************************************************
* Function Name  : OnRecvPackage
* Description    : 接到数据包
* Input          : byBuf 收到的数据
				 : dwLen 数据长度
* Output         : void
* Return         : void
*******************************************************************************/
void CComX::OnRecvPackage(const unsigned char * byBuf, int dwLen)
{
	if (m_RxPackageCallBack)
		m_RxPackageCallBack(m_pRxPackageCallBackParam, byBuf, dwLen);
	m_u64Total_Rx_Packages++;
}

/*******************************************************************************
* Function Name  : OnRecvBuf
* Description    : 接到数据流
* Input          : byBuf 收到的数据
: dwLen 数据长度
* Output         : void
* Return         : void
*******************************************************************************/
void CComX::OnRecvBuf(const unsigned char * byBuf, int dwLen)
{
	if (m_RxBufCallBack)
		m_RxBufCallBack(m_pRxPBufCallBackParam, byBuf, dwLen);

	m_u64Total_Rx_Bytes += dwLen;
}


/*******************************************************************************
* Function Name  : RecvThreadLoop
* Description    : 接收线程
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/

void CComX::RecvThreadLoop()
{
	#define READ_BUF_LEN 1024
	printf("in loop!\n");

	unsigned char buf[READ_BUF_LEN];
	int nr;
	int dwRead;
	int bResult;
	
#ifndef WIN32
	struct timeval tv = { 0, 10 * 1000 };
	fd_set readfd;
	int rc;
#endif

	for (;;)
	{
		if (m_RecvThreadStopFlag)
		{
			printf("#########################################      RecvThreadLoop thread exit.\n");
			break;
		}

#ifdef WIN32
		nr = 0;
		ResetEvent(m_oRecv.hEvent);
		bResult = ReadFile(m_hComHandle, buf, 1024, (unsigned long*)&nr, &m_oRecv);

		char tmp[1024];
		
		if (bResult == FALSE)
		{
			m_dwSysErrCode = GetLastError();
			if (m_dwSysErrCode == ERROR_IO_PENDING)
			{
				WaitForSingleObject(m_oRecv.hEvent, INFINITE);
				GetOverlappedResult(m_hComHandle, &m_oRecv, (unsigned long*)&dwRead, FALSE);
				
				if (dwRead != 0)
				{
					sprintf(tmp, "ReadFile: %d\n", nr);
					//OutputDebugStr(tmp);
					//cout << dwRead << endl;
					OnRecvBuf(buf, dwRead);
					if (m_bUsePackage)
					{
						for (int i = 0; i < dwRead; i++)
						{
							if (AnalyzePackage(buf[i]))
								OnRecvPackage(m_pRxBuf, m_RxPackageDataCount);
						}
					}
				}
			}
		}
		else
		{			
			if (nr)
			{
				//sprintf(tmp, "ReadFile: %d\n", nr);
				//OutputDebugStr(tmp);

				OnRecvBuf(buf, nr);
				if (m_bUsePackage)
				{
					for (int i = 0; i < nr; i++)
					{
						if (AnalyzePackage(buf[i]))
							OnRecvPackage(m_pRxBuf, m_RxPackageDataCount);
					}
				}
			}
		}
#else
		FD_ZERO(&readfd);
		FD_SET(m_fd, &readfd);
		rc = select(m_fd + 1, &readfd, NULL, NULL, &tv);

		if (rc > 0)
		{
			dwRead = read(m_fd, buf, READ_BUF_LEN);
			if (dwRead > 0)				
			{
				//printf("read %d bytes\n", dwRead);
				OnRecvBuf(buf, dwRead);
				if (m_bUsePackage)
				{
					for (int i = 0; i < dwRead; i++)
					{
						if (AnalyzePackage(buf[i]))//解包中会给m_pRxBuf改值
							OnRecvPackage(m_pRxBuf, m_RxPackageDataCount);
					}
				}
			}
		}
		usleep(1*1000);
#endif
	}
	printf("exit loop!\n");
}


/*
void CComX::RecvThreadLoop()
{
#define READ_BUF_LEN 1024
	printf("in loop!\n");
	
	unsigned char buf[READ_BUF_LEN];
	int nr;
	int dwRead;
	int bResult;

	struct timeval tv = { 0, 10 * 1000 };
	fd_set readfd;
	int rc;
	
	for (;;)
	{
		if (m_RecvThreadStopFlag)
			break;
		
#ifdef WIN32

		FD_ZERO(&readfd);
		FD_SET(m_hComHandle, &readfd);
		rc = select(m_hComHandle + 1, &readfd, NULL, NULL, &tv);
		
		if (rc > 0)
		{
			bResult = ReadFile(m_hComHandle, buf, 1024, (unsigned long*)&nr, &m_oRecv);
			if (nr > 0)
			{
				OnRecvBuf(buf, dwRead);
				if (m_bUsePackage)
				{
					for (int i = 0; i < dwRead; i++)
					{
						if (AnalyzePackage(buf[i]))//解包中会给m_pRxBuf改值
							OnRecvPackage(m_pRxBuf, m_RxPackageDataCount);
					}
				}
			}
		}
#else
		FD_ZERO(&readfd);
		FD_SET(m_fd, &readfd);
		rc = select(m_fd + 1, &readfd, NULL, NULL, &tv);
		
		if (rc > 0)
		{
			dwRead = read(m_fd, buf, READ_BUF_LEN);
			if (dwRead > 0){
				OnRecvBuf(buf, dwRead);
				if (m_bUsePackage)
				{
					for (int i = 0; i < dwRead; i++)
					{
						if (AnalyzePackage(buf[i]))//解包中会给m_pRxBuf改值
							OnRecvPackage(m_pRxBuf, m_RxPackageDataCount);
					}
				}
			}
		}
#endif
	}
	printf("exit loop!\n");
}
*/
/*******************************************************************************
* Function Name  : AnalyzePackage
* Description    : 拆包
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
/*bool CComX::AnalyzePackage(unsigned char data)
{
	static char USART_LastByte = 0;
	static bool USART_BeginFlag = FALSE;
	static bool USART_CtrlFlag = FALSE;
	static int USART_RevOffset = 0;
	static char CheckSum = 0;

	if (((data == m_FrameHead) && (USART_LastByte == m_FrameHead)) || (USART_RevOffset > m_MaxPackageSize))
	{
		if (USART_RevOffset < 17 && USART_RevOffset > 0)
		{
			m_u64Total_Rx_LostPackages++;
		}
		//RESET
		USART_RevOffset = 0;
		USART_BeginFlag = TRUE;
		USART_LastByte = data;
		return FALSE;
	}
	if ((data == m_FrameTail) && (USART_LastByte == m_FrameTail) && USART_BeginFlag)
	{
		if (USART_RevOffset <= 1)
		{
			return TRUE;
		}

		USART_RevOffset--;//得到除去头尾和控制符的数据的个数



		m_RxPackageDataCount = USART_RevOffset - 1;

		CheckSum -= m_FrameTail;
		CheckSum -= m_pRxBuf[m_RxPackageDataCount];
		USART_LastByte = data;
		USART_BeginFlag = FALSE;
		if (CheckSum == m_pRxBuf[m_RxPackageDataCount])
		{
			CheckSum = 0;
			return TRUE;
		}
		m_u64Total_Rx_LostPackages++;

		CheckSum = 0;
		return FALSE;
	}
	USART_LastByte = data;
	if (USART_BeginFlag)
	{
		if (USART_CtrlFlag)
		{
			m_pRxBuf[USART_RevOffset++] = data;
			CheckSum += data;
			USART_CtrlFlag = FALSE;
			USART_LastByte = m_FrameCtrl;
		}
		else if (data == m_FrameCtrl)
		{
			USART_CtrlFlag = TRUE;
		}
		else
		{
			m_pRxBuf[USART_RevOffset++] = data;
			CheckSum += data;
		}
	}

	return FALSE;
}*/
bool CComX::AnalyzePackage(unsigned char data)
{
	static unsigned char USART_LastByte = 0;
	static bool USART_BeginFlag = FALSE;
	static bool USART_CtrlFlag = FALSE;
	static unsigned int USART_RevOffset = 0;
	static unsigned char CheckSum = 0;
	
	if (((data == m_FrameHead) && (USART_LastByte == m_FrameHead)) || (USART_RevOffset > m_MaxPackageSize))
	{
		if (USART_RevOffset < 17 && USART_RevOffset > 0)
		{
			m_u64Total_Rx_LostPackages++;
		}
		//RESET
		USART_RevOffset = 0;
		USART_BeginFlag = TRUE;
		USART_LastByte = data;
		return FALSE;
	}
	if ((data == m_FrameTail) && (USART_LastByte == m_FrameTail) && USART_BeginFlag)
	{
		if (USART_RevOffset <= 1)
		{
			return TRUE;
		}
		
		USART_RevOffset--;//μ?μ?3yè￥í·?2oí????·?μ?êy?Yμ???êy
		
		
		
		m_RxPackageDataCount = USART_RevOffset - 1;
		
		CheckSum -= m_FrameTail;
		CheckSum -= m_pRxBuf[m_RxPackageDataCount];
		USART_LastByte = data;
		USART_BeginFlag = FALSE;
		if (CheckSum == m_pRxBuf[m_RxPackageDataCount])
		{
			CheckSum = 0;
			return TRUE;
		}
		m_u64Total_Rx_LostPackages++;
		
		CheckSum = 0;
		return FALSE;
	}
	USART_LastByte = data;
	if (USART_BeginFlag)
	{
		if (USART_CtrlFlag)
		{
			m_pRxBuf[USART_RevOffset++] = data;
			CheckSum += data;
			USART_CtrlFlag = FALSE;
			USART_LastByte = m_FrameCtrl;
		}
		else if (data == m_FrameCtrl)
		{
			USART_CtrlFlag = TRUE;
		}
		else
		{
			m_pRxBuf[USART_RevOffset++] = data;
			CheckSum += data;
		}
	}
	
	return FALSE;
}

int CComX::WritePackage(unsigned char * buf, int len, int dwTimeout)
{
	int i;
	unsigned char *pBuf;
	unsigned char CheckSum = 0;

	if (buf == NULL)
	{
		return FALSE;
	}

	if (m_bUsePackage == FALSE)
		return 0;

	pBuf = m_pTxBuf;
	*pBuf++ = m_FrameHead;
	*pBuf++ = m_FrameHead;

	for (i = 0; i < len; i++)
	{
		if ((buf[i] == m_FrameCtrl) || (buf[i] == m_FrameHead) || (buf[i] == m_FrameTail))
		{
			*pBuf++ = m_FrameCtrl;
		}
		*pBuf++ = buf[i]; // contain device_addr,func_code,start_addr,read_len/write_len
		//if (bIsWrite)
		CheckSum += buf[i];
	}

	//校验和
	if ((CheckSum == m_FrameCtrl) || (CheckSum == m_FrameHead) || (CheckSum == m_FrameTail))
	{
		*pBuf++ = m_FrameCtrl;
	}
	//if (bIsWrite)
	*pBuf++ = CheckSum;

	//Send Tail USART_FRAMETAIL USART_FRAMETAIL
	*pBuf++ = m_FrameTail;
	*pBuf++ = m_FrameTail;

	len = pBuf - m_pTxBuf;
#ifdef WIN32
	int nw = WriteRawBuf(m_pTxBuf, len, dwTimeout);
#else
int nw = WriteRawBuf(m_pTxBuf, len);
#endif

	if (nw) m_u64Total_Tx_Packages++;

	return nw;
}

//写缓存函数
#ifdef WIN32
	int CComX::WriteRawBuf(const unsigned char * byBuf, int dwLen, int dwTimeout)
#else
	int CComX::WriteRawBuf(const unsigned char * byBuf, int dwLen)
#endif
{
	int nw = 0;

	if (byBuf == NULL)
	{
		printf("error, byBuf=NULL\n");
		return 0;
	}

	if (!IsOpened())
	{
		printf("error, !IsOpened()\n");
		return 0;
	}

#ifdef WIN32
	ResetEvent(m_oSend.hEvent);
	BOOL bResult = WriteFile(m_hComHandle, byBuf, dwLen, (unsigned long*)&nw, &m_oSend);
	
	if (!bResult)
	{
		m_dwSysErrCode = GetLastError();
		if (m_dwSysErrCode == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_oSend.hEvent, dwTimeout);
			GetOverlappedResult(m_hComHandle, &m_oSend, (unsigned long*)&nw, FALSE);
		}
		else
			return 0;
	}
#else
	nw = write(m_fd, byBuf, dwLen);
#endif

	m_u64Total_Tx_Bytes += nw;
	return nw;
}

void DelayUs(LONG64 Us)
{
#ifdef WIN32
	LARGE_INTEGER CurrTicks, TicksCount;
	
	QueryPerformanceFrequency(&TicksCount);
	QueryPerformanceCounter(&CurrTicks);
	
	TicksCount.QuadPart = TicksCount.QuadPart * Us / 1000000;
	TicksCount.QuadPart += CurrTicks.QuadPart;
	
	while (CurrTicks.QuadPart < TicksCount.QuadPart)
		QueryPerformanceCounter(&CurrTicks);
#else
	usleep(Us);
#endif
}

//offset为此功能寄存器在寄存器组中的偏移
void CComX::WriteRegister(unsigned char device_addr, unsigned char func_code, unsigned char register_offset, 
						  unsigned char* data, unsigned int len)
{
	printf("WriteRegister %d", len);
	
	unsigned char sendBuf[1024];

	FrameHeader *header = (FrameHeader*)sendBuf;

	header->device_addr = device_addr;
	header->func_code = func_code;
	header->offset = register_offset;
	header->len = len;
	
	int dwLen = 0;
	if (NULL == data)
	{
		dwLen = sizeof(FrameHeader); //no data and checksum		
	}
	else
	{
		dwLen = sizeof(FrameHeader)+len+1; //1 is errorcode
		memcpy(sendBuf+sizeof(FrameHeader), data, len);
		sendBuf[dwLen-1] = 0x00;		
	}	

	WritePackage(sendBuf, dwLen);

	printf("WriteRegister end, dwLen=%d\n", dwLen);
}
