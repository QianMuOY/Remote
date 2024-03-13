// CreateClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "CreateClientDlg.h"
#include "afxdialogex.h"



struct _SERVER_CONNECT_INFO
{
	DWORD CheckFlag;
	char  ServerIP[20];
	int   ServerPort;
}__ServerConnectInfo = { 0x99999999,"",0 };


// CCreateClientDlg 对话框

IMPLEMENT_DYNAMIC(CCreateClientDlg, CDialog)

CCreateClientDlg::CCreateClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_CREATE_CLIENT, pParent)
	, m_CEdit_CreateClient_IP(_T("输入一个连接的IP地址"))
	, m_CEdit_CreateClient_Port(_T("输入一个端口号"))  //默认初始化
{

}

CCreateClientDlg::~CCreateClientDlg()
{
}

void CCreateClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CREATE_CLIENT_IP, m_CEdit_CreateClient_IP);
	DDX_Text(pDX, IDC_EDIT_CREATE_CLIENT_PORT, m_CEdit_CreateClient_Port);
}


BEGIN_MESSAGE_MAP(CCreateClientDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCreateClientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCreateClientDlg 消息处理程序


void CCreateClientDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CFile FileObject;

	UpdateData(TRUE);   //输入在控件上的数据更新到控件变量上
	USHORT  ServerPort = atoi(m_CEdit_CreateClient_Port); //将字符串转换
	strcpy(__ServerConnectInfo.ServerIP, m_CEdit_CreateClient_IP);
	//127 
	if (ServerPort < 0 || ServerPort>65536)
	{
		__ServerConnectInfo.ServerPort = 8888; 
	}
	else
	{
		__ServerConnectInfo.ServerPort = ServerPort;   //端口 有效端口
	}


	char v1[MAX_PATH];
	ZeroMemory(v1, MAX_PATH);

	LONGLONG BufferLength = 0;
	BYTE* BufferData = NULL;
	CString  v3;
	CString  ClientFullPathData;
	try
	{
		//此处得到未处理前的文件名 
		GetModuleFileName(NULL, v1, MAX_PATH);     //得到文件名
		v3 = v1;
		int Pos = v3.ReverseFind('\\');   //从右到左查询第一次出现的\\  C:...\\\\Debug\\

		v3 = v3.Left(Pos); //定位到\\


		ClientFullPathData = v3 + "\\Client.exe";   //得到当前未处理文件名

																	//打开文件
		FileObject.Open(ClientFullPathData, CFile::modeRead | CFile::typeBinary);

		BufferLength = FileObject.GetLength();
		BufferData = new BYTE[BufferLength];
		ZeroMemory(BufferData, BufferLength);

		//读取文件内容
		FileObject.Read(BufferData, BufferLength);
		//关闭文件
		FileObject.Close();
		//写入上线IP和端口 主要是寻找0x1234567这个标识然后写入这个位置


		//全局变量是在PE文件的Data节中
		int Offset = MemoryFind((char*)BufferData, (char*)&__ServerConnectInfo.CheckFlag,
			BufferLength, sizeof(DWORD));
		//自己设置的IP和端口保存到客户端文件中
		memcpy(BufferData + Offset, &__ServerConnectInfo, sizeof(__ServerConnectInfo));   //写操作
		//保存到文件
		FileObject.Open(ClientFullPathData, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite);
		FileObject.Write(BufferData, BufferLength);
		FileObject.Close();
		delete[] BufferData;
		MessageBox("生成成功");

	}
	catch (CMemoryException* e)
	{
		MessageBox("内存不足");
	}
	catch (CFileException* e)
	{
		MessageBox("文件操作错误");
	}
	catch (CException* e)
	{
		MessageBox("未知错误");
	}
	CDialog::OnOK();
}
//在内存中搜索CheckFlag
int CCreateClientDlg::MemoryFind(const char* BufferData, const char* KeyValue, int BufferLength, int KeyLength)
{
	int i, j;
	if (KeyLength == 0 || BufferLength == 0)
	{
		return -1;
	}
	for (i = 0; i < BufferLength; i++)
	{
		for (j = 0; j < KeyLength; j++)
		{
			if (BufferData[i + j] != KeyValue[j])
			{
				break;
			}
		}
		//0x12345678   78   56  34  12
		if (j == KeyLength)
		{
			return i;
		}
	}
	// BMK Hello
	return -1;
}
