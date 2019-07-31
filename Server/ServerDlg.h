
// ServerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#define RECVMAX_SIZE        100

typedef struct _clientinfo
{
    SOCKET sock;            //客户端套接字
    bool startflag;         //开始接收
}CLIENTINFO, *PCLIENTINFO;

typedef struct _revpacket
{
    char name[10];
    char revdata[10];
}RECVPACKET, *PRECVPACKET;


// CServerDlg 对话框
class CServerDlg : public CDialogEx
{
// 构造
public:
	CServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
    CButton m_ButtOpenServer;
    CButton m_ButtConnect;
    CComboBox m_ComboClientIP;

    
private:
    CString m_EditServerPort;
    SOCKET server_sock;
    PCLIENTINFO client_infor;
    CStatic m_EditTemp;
    CStatic m_EditHum;
    CStatic m_EditCO2;
    CWinThread *listenthread;
    CWinThread *revdatathread;

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnBnClickedButtopen();
    afx_msg void OnBnClickedButtconnect();

public:
    static UINT ServerListen_Thread(LPVOID param);
    static UINT ServerRevData_Thread(LPVOID param);
    void ShowEnvirData(PRECVPACKET revdata);

    
    
};



