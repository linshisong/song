// LidarFullTestDlg.h : ͷ�ļ�
//

#pragma once
#include "Com/Comx.h"
#include "sdkprotocol.h"

#define MAX_COM_NUM 16
#define currentCentriod 6000
// CLidarFullTestDlg �Ի���
class CLidarFullTestDlg : public CDialog
{
// ����
public:
	CLidarFullTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LIDARFULLTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnTest();
public:
	afx_msg void OnBnClickedBtnStart();
	//int FlashCommList();
public:
	afx_msg void OnBnClickedBtnStop();
	void SetFont(int code);
	int m_code;
	int m_matchOK;
	CFont m_Font;
public:
	afx_msg void OnBnClickedBtnCheck();
	int AddDatabaseRecord(const char *sn, const char *descrept, int flag=true, int simple_echo=true);

	int GenerateSN(CString *str);
	void SetResultInfo(const char * info, int lidarResult);
	static void RxCallBackFunc(void *pParam, const unsigned char * byBuf, int dwLen);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedBtnFlash();
	afx_msg void OnBnClickedBtnOffset();

	int StartComm();
	int StartUI();
	int StopUI();
	//int CheckLidarFunc();
	int GetToday();
	void UpdateToday(int today);
	void ResetSN();
	int IncSN();
	int GenSN(char *pcSerial, char *sn);
	int GenSNEx(int serial, char *pcSerial, char *sn);
	int GetPercent();
	void SaveParam();
	void LoadParam();
public:
	CComX mCom[MAX_COM_NUM];
	void CloseAllCom();
	void TestAllCom();
	int isTestOver;
public:
	// ��ֵ
	int mThresVal;
	afx_msg void OnEnChangeEdtVal();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	afx_msg void OnBnClickedBtnOk();
	void ChangeToSensorMode(CComX* pCom);
	void TellLidarIMUCalibrate(CComX* pCom);
	void ChangeToNaviMode(CComX* pCom); 
	unsigned int Package(PackageDataStruct package);
	void TellLidarOffset(CComX* pCom,unsigned int id, s16 config);

};
