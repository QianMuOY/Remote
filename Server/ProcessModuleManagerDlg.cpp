// ProcessModuleManagerDlg.cpp: 实现文件
//
#include "stdafx.h"
#include "Server.h"
#include "ProcessModuleManagerDlg.h"
#include "afxdialogex.h"
#include "Windows.h"


// CProcessModuleManagerDlg 对话框

IMPLEMENT_DYNAMIC(CProcessModuleManagerDlg, CDialog)



CProcessModuleManagerDlg::CProcessModuleManagerDlg(CWnd* pParent, ULONG ProcessID, ULONG RunMode)
	: CDialog(IDD_DIALOG_PROCESS_MODULE, pParent)
{
	m_RunMode = RunMode; //运行模式
	m_ProcessID = ProcessID; //进程ID 
	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)); //Icon
	m_ProcessModuleBase = GetModuleHandle("Process.dll");  //进程模块句柄
	m_SeEnumProcessModuleList = (LPFN_SEENUMPROCESSMODULELIST)GetProcAddress(m_ProcessModuleBase, "SeEnumProcessModuleList"); //枚举
}

CProcessModuleManagerDlg::~CProcessModuleManagerDlg()
{
}

void CProcessModuleManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS_MODULE_MAIN, m_CListCtrl_Process_Module);
}
BEGIN_MESSAGE_MAP(CProcessModuleManagerDlg, CDialog)
END_MESSAGE_MAP()


// CProcessModuleManagerDlg 消息处理程序
//消息处理程序
BOOL CProcessModuleManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString v1;
	SetIcon(m_IconHwnd, FALSE);   //设置忍者图标到Dialog

	// TODO:  在此添加额外的初始化
	m_CListCtrl_Process_Module.InsertColumn(0, "模块地址", LVCFMT_LEFT, 100);
	m_CListCtrl_Process_Module.InsertColumn(1, "模块大小", LVCFMT_LEFT, 100);
	m_CListCtrl_Process_Module.InsertColumn(2, "模块完整路径", LVCFMT_LEFT, 300);
	m_CListCtrl_Process_Module.InsertColumn(2, "模块类型", LVCFMT_LEFT, 50);
	m_CListCtrl_Process_Module.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	switch (m_RunMode)   //界面处理
	{
	case PROCESS_MANAGER::SERVER_MODE:
	{
		v1.Format("%s - 进程管理", "本地");
		ShowServerProcessModuleList();
		MessageBox("helloworld", "helloworld");

		break;
	}
	case PROCESS_MANAGER::CLIENT_MODE:
	{
		v1.Format("%s - 进程管理", "远程");

		break;
	}
	default:
		break;
	}
	SetWindowText(v1);//设置对话框标题

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

BOOL CProcessModuleManagerDlg::ShowServerProcessModuleList()
{
	//枚举
	ULONG ItemCount = 0;
	vector<_PROCESS_MODULE_INFORMATION_> ProcessModuleInfo;
	if (m_SeEnumProcessModuleList == NULL)
	{
		return FALSE;
	}
	ItemCount = m_SeEnumProcessModuleList(m_ProcessID, ProcessModuleInfo);
	if (ItemCount == 0)
	{
		return FALSE;
	}
	CHAR* ModuleFullPathData = NULL;
	DWORD Offset = 0;
	CString v1;
	m_CListCtrl_Process_Module.DeleteAllItems();
	//遍历发送来的每一个字符别忘了他的数据结构啊 Id+进程名+0+完整名+0
	vector<_PROCESS_MODULE_INFORMATION_>::iterator i;
	int j = 0;
	for (i = ProcessModuleInfo.begin(); i != ProcessModuleInfo.end(); i++)
	{
		ULONG64 ModuleBase = i->ModuleBase;
		v1.Format("0x%llx", ModuleBase);
		m_CListCtrl_Process_Module.InsertItem(j, v1);       //将得到的数据加入到列表当中
		size_t ModuleSize = i->ModuleSize;
		v1.Format("0x%x", ModuleSize);
		m_CListCtrl_Process_Module.SetItemText(j, 1, v1);
		WcharToChar(&ModuleFullPathData, i->ModuleFullPathData, sizeof(i->ModuleFullPathData));
		if (ModuleFullPathData != NULL)
		{
			m_CListCtrl_Process_Module.SetItemText(j, 3, ModuleFullPathData);
			delete[] ModuleFullPathData;
			ModuleFullPathData = NULL;
		}
		if (i->ModuleType == MODULE_X64)
		{
			m_CListCtrl_Process_Module.SetItemText(j, 2, "X64");
		}
		else
		{
			m_CListCtrl_Process_Module.SetItemText(j, 2, "X86");
		}
		j++;
	}
	v1.Format("模块个数 / %d", j);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Process_Module.SetColumn(3, &lvc); //在列表中显示有多少个模块
	return  TRUE;
}
