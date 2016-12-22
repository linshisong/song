#ifndef COMX_H_
#define COMX_H_

#ifdef WIN32
	#include <Windows.h>
	//#include <WINSOCK2.H>
	#include <stdlib.h>
	#include <stdio.h>
	#include "tchar.h"
	#ifdef _MSC_VER  //如果不是Visual Studio的编译环境，则如下的函数不能得到支持
		#define _itot_s(value, buffer, sizeInCharacters, radix) _itot(value, buffer, radix)
		#define _tcscat_s(strDestination, numberOfElements, strSource) _tcscat(strDestination, strSource)
		#define _tcscpy_s(strDestination, numberOfElements, strSource) _tcscpy(strDestination, strSource)
	#endif
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <stdio.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <termios.h>  
	#include <unistd.h>  
	#include <sys/types.h>  
	#include <sys/stat.h>  
	#include <fcntl.h>  
	#include <string.h>
	#include <termios.h>		
#endif

#ifdef WIN32
	#define CLOSE closesocket
	#define SOCKET_LEN int
	#define LONG64 __int64
#else  //类型定义
	#define SOCKET_ERROR (-1)
	#define CLOSE close
	#define SOCKET_LEN socklen_t	
	#define DWORD unsigned long
	#define WORD unsigned short
	#define TRUE 1
	#define FALSE 0
	#ifndef INFINITE
		#define INFINITE 0x7fffffff
	#endif
	#define LONG64 long long
#endif

typedef void(*RxCallBackFunc)(void *pParam, const unsigned char * byBuf, int dwLen);

typedef struct _FrameHeader{
	unsigned char device_addr;
	unsigned char func_code;
	unsigned short offset;
	unsigned int len;
}FrameHeader;

class CComX
{
public:
	CComX(int dwMaxPackageSize = 4096, bool UsePackage = TRUE);
	virtual ~CComX();
public:
	void WriteRegister(unsigned char device_addr, unsigned char func_code,
		unsigned char register_offset, unsigned char* data, unsigned int len);
	void SetRxBufCallBack(RxCallBackFunc func, void *pParam);
	void SetRxPackageCallBack(RxCallBackFunc func, void *pParam);
	//int Open(const char* path, int baudRate);
	int Close();
	void EnablePackage(bool flag = TRUE) { m_bUsePackage = flag; }
	int WritePackage(unsigned char * buf, int len, int dwTimeout = INFINITE);
#ifdef WIN32
	int WriteRawBuf(const unsigned char * byBuf, int dwLen, int dwTimeout = INFINITE);
#else
	int WriteRawBuf(const unsigned char * byBuf, int dwLen);
#endif
#ifdef WIN32
	BOOL OpenWindows(int dwPortNo, int dwBaud);
#else
	int OpenLinux(const char* path, int baudRate);
#endif	
	bool IsOpened(){ return m_IsOpened; }
	char* GetDevicePath() { return m_path; }
	int m_PortNo;
private:
	
	int GetSysErrCode(){ return m_dwSysErrCode; }

	
	

	int GetBaudRate(){ return m_baudRate; }	
	

	LONG64 GetTotalRxByteCount(){ return m_u64Total_Rx_Bytes; }
	LONG64 GetTotalTxByteCount(){ return m_u64Total_Tx_Bytes; }
	LONG64 GetTotalRxPackageCount(){ return m_u64Total_Rx_Packages; }
	LONG64 GetTotalTxPackageCount(){ return m_u64Total_Tx_Packages; }
	LONG64 GetTotalRxLostPackageCount(){ return m_u64Total_Rx_LostPackages; }
	void ResetCounters();

	
	bool IsUsePackage() { return m_bUsePackage; }

	void DelayUs(LONG64 Us);
protected:
	virtual void OnRecvPackage(const unsigned char * byBuf, int dwLen);
	virtual void OnRecvBuf(const unsigned char * byBuf, int dwLen);

private:
	LONG64 m_u64Total_Rx_Bytes;
	LONG64 m_u64Total_Rx_Packages;
	LONG64 m_u64Total_Tx_Packages;
	LONG64 m_u64Total_Tx_Bytes;
	LONG64 m_u64Total_Rx_LostPackages;

	void *m_pRxPackageCallBackParam;
	void *m_pRxPBufCallBackParam;
	RxCallBackFunc m_RxPackageCallBack;
	RxCallBackFunc m_RxBufCallBack;
#ifdef WIN32
	OVERLAPPED m_oSend;
	OVERLAPPED m_oRecv;
	HANDLE m_hComHandle;
	DCB m_DCB;
	HANDLE m_rxThread;
#else
	pthread_t m_rxThread;
	speed_t GetBaudrate(int baudrate);
#endif
	unsigned char m_FrameHead, m_FrameTail, m_FrameCtrl;

	bool m_RecvThreadStopFlag;
	bool m_IsOpened;
	
	unsigned int m_MaxPackageSize;
	unsigned char * m_pRxBuf;
	unsigned char * m_pTxBuf;
	int m_RxPackageDataCount;
	
	char m_path[128];		//设备地址
	int m_baudRate;			//波特率
	int m_fd;

	int m_dwSysErrCode;
	bool m_bUsePackage;

	void RecvThreadLoop();
	

	bool AnalyzePackage(unsigned char data);
	
public:
	static DWORD WINAPI RecvThread(void* lpParameter);

};
//static void * RecvThread(void* lpParameter)


#endif

