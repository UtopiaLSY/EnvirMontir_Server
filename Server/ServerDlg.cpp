
// ServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CServerDlg *m_serverdlg;
bool listen_flag;
bool revdata_flag;
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


// CServerDlg 对话框



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
    , m_EditServerPort(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTOPEN, m_ButtOpenServer);
    DDX_Control(pDX, IDC_BUTTCONNECT, m_ButtConnect);
    DDX_Control(pDX, IDC_COMBO2, m_ComboClientIP);
    DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_EditServerPort);
    DDX_Control(pDX, IDC_TEMP, m_EditTemp);
    DDX_Control(pDX, IDC_HUM, m_EditHum);
    DDX_Control(pDX, IDC_CO2, m_EditCO2);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BUTTOPEN, &CServerDlg::OnBnClickedButtopen)
    ON_BN_CLICKED(IDC_BUTTCONNECT, &CServerDlg::OnBnClickedButtconnect)
END_MESSAGE_MAP()


// CServerDlg 消息处理程序

BOOL CServerDlg::OnInitDialog()
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
    m_serverdlg = this;
    client_infor = new CLIENTINFO;
    client_infor->sock = NULL;
    client_infor->startflag = FALSE;
    int serverport = 8090;
    m_EditServerPort.Format(TEXT("%d"), serverport);
    UpdateData(FALSE);
    m_ButtConnect.EnableWindow(FALSE);
    

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerDlg::OnPaint()
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
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



int CServerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialogEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  在此添加您专用的创建代码
    listenthread = NULL;
    revdatathread = NULL;
    return 0;
}


void CServerDlg::OnBnClickedButtopen()
{
    // TODO:  在此添加控件通知处理程序代码
    CString buttonstr;
    m_ButtOpenServer.GetWindowTextW(buttonstr);
    
    if (buttonstr.Compare(_T("打开服务器")) == 0)
    {
        SOCKADDR_IN serveraddr;
        int port;
        int err;

        UpdateData(TRUE);
        server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_sock == SOCKET_ERROR)
        {
            MessageBox(TEXT("创建套接字失败"));
        }
        port = _tstoi(m_EditServerPort);
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(port);
        serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        err = bind(server_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
        if (err == SOCKET_ERROR)
        {
            MessageBox(_T("bind fail"));
            closesocket(server_sock);
            return;
        }
        m_ButtOpenServer.SetWindowTextW(_T("关闭服务器"));
        listen(server_sock, 5);
        if (listenthread == NULL)
        {
            listen_flag = TRUE;
            revdata_flag = TRUE;
            listenthread = AfxBeginThread(ServerListen_Thread, this);
        }
            
        if (listenthread = NULL)
        {
            MessageBox(_T("创建线程失败\n重启软件"), _T("错误提示"));
        }
    }

    if (buttonstr.Compare(_T("关闭服务器")) == 0)
    {
        if (client_infor->sock != NULL)
        {
            shutdown(client_infor->sock, SD_BOTH);
            closesocket(client_infor->sock);
        }
            
        closesocket(server_sock);

        listen_flag = FALSE;        //关闭线程
        revdata_flag = FALSE;
        listenthread = NULL;
        revdatathread = NULL;
        m_ButtOpenServer.SetWindowTextW(_T("打开服务器"));
        m_ButtConnect.EnableWindow(FALSE);
    }
}

//监听客户端连接线程
UINT  CServerDlg::ServerListen_Thread(LPVOID param)
{
    SOCKADDR_IN clientaddr;
    CServerDlg *pthis =(CServerDlg*)param;
    
    int addrlen = sizeof(clientaddr);
    while (1)
    {
        if (listen_flag == FALSE) break;
        pthis->client_infor->sock = accept(pthis->server_sock, (SOCKADDR*)&clientaddr, &addrlen);

        if (pthis->client_infor->sock == SOCKET_ERROR)
        {
            continue;
        }
        CString showaddr;
        CHAR *addr = inet_ntoa(clientaddr.sin_addr);
        showaddr = CString(addr);
        CString port;
        port.Format(_T("%d"), ntohs(clientaddr.sin_port));
        showaddr = showaddr + ":" + port;
        pthis->m_ComboClientIP.AddString(showaddr);
        pthis->m_ComboClientIP.SetCurSel(0);
        pthis->m_ButtConnect.EnableWindow(TRUE);

        PCLIENTINFO clientparam = new CLIENTINFO;
        clientparam = pthis->client_infor;
        if (pthis->revdatathread == NULL)
            pthis->revdatathread = AfxBeginThread(ServerRevData_Thread, (LPVOID)clientparam);
    }
    pthis->listenthread = NULL;
    return 0;
}


//接收数据线程
UINT  CServerDlg::ServerRevData_Thread(LPVOID param)
{
    bool startflag;
    SOCKET sock = (SOCKET) (((PCLIENTINFO)param)->sock);
    
    int revlen = 0;
    RECVPACKET Server_RevBuff = { 0 };

    while (1)
    {
        revlen = recv(sock, (char*)&Server_RevBuff, sizeof(Server_RevBuff), 0);
        if (revlen == SOCKET_ERROR)
        {
            closesocket(m_serverdlg->server_sock);
            m_serverdlg->m_ComboClientIP.DeleteString(0);
            break;
        }
        else if (revlen == 0)               //客户端关闭
        {
            m_serverdlg->m_ComboClientIP.DeleteString(0);
            listen_flag = FALSE;
            AfxMessageBox(TEXT("客户端关闭"));
            break;
        }
        else
        {
            startflag = (((PCLIENTINFO)param)->startflag);
            if (startflag == TRUE)
            {
                m_serverdlg->ShowEnvirData((PRECVPACKET)&Server_RevBuff);
                memset((char*)&Server_RevBuff, 0, sizeof(Server_RevBuff));
            }
        }
    }
    m_serverdlg->revdatathread = NULL;
    return 0;
}



void CServerDlg::ShowEnvirData(PRECVPACKET revdata)
{
    PRECVPACKET recvpacket = new RECVPACKET;
    recvpacket = revdata;
    CString showstr;
    showstr = CString(recvpacket->revdata);

    if (memcmp(recvpacket->name, "Temperat", 8) == 0)
    {
        m_serverdlg->m_EditTemp.SetWindowTextW(showstr);
    }

    if (memcmp(recvpacket->name, "Humidity", 8) == 0)
    {
        m_serverdlg->m_EditHum.SetWindowTextW(showstr);
    }

    if (memcmp(recvpacket->name, "CO2", 3) == 0)
    {
        m_serverdlg->m_EditCO2.SetWindowTextW(showstr);
    }


}

void CServerDlg::OnBnClickedButtconnect()
{
    // TODO:  在此添加控件通知处理程序代码
    if (client_infor->sock == NULL)
    {
        MessageBox(TEXT("无客户端"), _T("提示"));
        return;
    }
    if (send(client_infor->sock, "start", 5, 0) == SOCKET_ERROR)
    {
        MessageBox(TEXT("发送出错"), _T("提示"));
        return;
    }
    
    client_infor->startflag = TRUE;
    m_ButtConnect.EnableWindow(FALSE);
}
