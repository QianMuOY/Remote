// DlgServerSet.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "DlgServerSet.h"
#include "afxdialogex.h"
#include "stdafx.h"



extern CServerSetManager __ConfigObject;  //设置全局配置文件对象

// CDlgServerSet 对话框
IMPLEMENT_DYNAMIC(CDlgServerSet, CDialog)

CDlgServerSet::CDlgServerSet(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_SERVER_SET, pParent)
	, m_CEdit_Dialog_Server_Max_Connect_Num(0)
	, m_CEdit_Dialog_Server_Listen_Port(0)
{

}

CDlgServerSet::~CDlgServerSet()
{
}

void CDlgServerSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIALOG_SERVER_MAX_CONNECT_NUM, m_CEdit_Dialog_Server_Max_Connect_Num);
	DDV_MinMaxLong(pDX, m_CEdit_Dialog_Server_Max_Connect_Num, 1, 100);
	DDX_Text(pDX, IDC_EDIT_DIALOG_SERVER_LISTEN_PORT, m_CEdit_Dialog_Server_Listen_Port);
	DDV_MinMaxLong(pDX, m_CEdit_Dialog_Server_Listen_Port, 2048, 65536);
	DDX_Control(pDX, IDC_BUTTON_DIALOG_SERVER_SET_APPLY, m_Button_Dialog_Server_Set_Apply);

	DDX_Control(pDX, IDC_BUTTON_DIALOG_SERVER_SET_APPLY, m_Button_Dialog_Server_Set_Apply);
}


BEGIN_MESSAGE_MAP(CDlgServerSet, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DIALOG_SERVER_SET_APPLY, &CDlgServerSet::OnBnClickedButtonDialogServerSetApply)
	ON_EN_CHANGE(IDC_EDIT_DIALOG_SERVER_MAX_CONNECT_NUM, &CDlgServerSet::OnEnChangeEditDialogServerMaxConnectNum)
	ON_EN_CHANGE(IDC_EDIT_DIALOG_SERVER_LISTEN_PORT, &CDlgServerSet::OnEnChangeEditDialogServerListenPort)
END_MESSAGE_MAP()


// CDlgServerSet 消息处理程序





void CDlgServerSet::OnBnClickedButtonDialogServerSetApply()
{
	// TODO: 在此添加控件通知处理程序代码
	
	UpdateData(TRUE); //更新修改的数据


	//HomeWork

	__ConfigObject.SetInt("Settings", "Port", m_CEdit_Dialog_Server_Listen_Port);
	//向ini文件中写入值
	__ConfigObject.SetInt("Settings", "MaxConnections", m_CEdit_Dialog_Server_Max_Connect_Num);
	SendMessage(WM_CLOSE);
	//修改后 显示/隐藏
	//m_Button_Dialog_Server_Set_Apply.EnableWindow(FALSE);
	//m_Button_Dialog_Server_Set_Apply.ShowWindow(SW_HIDE);        //MFC




/*	CWnd* v1 = GetDlgItem(IDC_BUTTON_SERVER_SET_APPLY);   //SDK
	v1->EnableWindow(FALSE);
	v1->ShowWindow(SW_HIDE);
*/
}


void CDlgServerSet::OnEnChangeEditDialogServerMaxConnectNum()
{
	// TODO:  在此添加控件通知处理程序代码
	//修改后 显示/隐藏
	m_Button_Dialog_Server_Set_Apply.ShowWindow(SW_NORMAL);
	m_Button_Dialog_Server_Set_Apply.EnableWindow(TRUE);
}


void CDlgServerSet::OnEnChangeEditDialogServerListenPort()
{
	// TODO:  在此添加控件通知处理程序代码
	//修改后 显示/隐藏
	m_Button_Dialog_Server_Set_Apply.ShowWindow(SW_NORMAL);
	m_Button_Dialog_Server_Set_Apply.EnableWindow(TRUE);

}


BOOL CDlgServerSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	int ListenPort = 0;
	int MaxConnections = 0;

	//将数据设置到Dlg
	ListenPort = __ConfigObject.GetInt("Settings", "Port");
	//读取ini 文件中的监听端口
	MaxConnections = __ConfigObject.GetInt("Settings", "MaxConnections");


	m_CEdit_Dialog_Server_Max_Connect_Num = MaxConnections;
	m_CEdit_Dialog_Server_Listen_Port = ListenPort;

	//由于控件Value 所以要UpdateData   ——>False     v1     控件(内存)
	//        控制                      CListContrl   v1     操作v1就是操作控件
	UpdateData(FALSE);



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}