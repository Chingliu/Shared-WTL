#include "stdafx.h"
#include "WtlFile.h"


CWtlFile::CWtlFile()
{
	m_hFile = NULL;
}

CWtlFile::~CWtlFile()
{
	Close();
}

BOOL CWtlFile::Open( PCTSTR lpszFileName, UINT nOpenFlags )
{
	Close();
	
	if( !::GetFullPathName( lpszFileName, MAX_PATH, m_strFileName.GetBuffer(MAX_PATH), NULL) )
		ATLASSERT( FALSE );

	// map read/write mode
	ATLASSERT((modeRead|modeWrite|modeReadWrite) == 3);
	DWORD dwAccess = 0;
	switch (nOpenFlags & 3)
	{
	case modeRead:
		dwAccess = GENERIC_READ;
		break;
	case modeWrite:
		dwAccess = GENERIC_WRITE;
		break;
	case modeReadWrite:
		dwAccess = GENERIC_READ | GENERIC_WRITE;
		break;
	default:
		ATLASSERT(FALSE);  // invalid share mode
	}

	// map share mode
	DWORD dwShareMode = 0;
	switch (nOpenFlags & 0x70)    // map compatibility mode to exclusive
	{
	default:
		ATLASSERT(FALSE);  // invalid share mode?
	case shareCompat:
	case shareExclusive:
		dwShareMode = 0;
		break;
	case shareDenyWrite:
		dwShareMode = FILE_SHARE_READ;
		break;
	case shareDenyRead:
		dwShareMode = FILE_SHARE_WRITE;
		break;
	case shareDenyNone:
		dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ;
		break;
	}

	// map creation flags
	DWORD dwCreateFlag;
	if (nOpenFlags & modeCreate)
	{
		if (nOpenFlags & modeNoTruncate)
			dwCreateFlag = OPEN_ALWAYS;
		else
			dwCreateFlag = CREATE_ALWAYS;
	}
	else
		dwCreateFlag = OPEN_EXISTING;

	// attempt file creation
	HANDLE hFile = ::CreateFile(lpszFileName, dwAccess, dwShareMode, NULL,
		dwCreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	m_hFile = hFile;

	// See if the file already exists and if it does move the file pointer 
	// to the end of the file
	if (::GetLastError()==ERROR_ALREADY_EXISTS)
		::SetFilePointer(hFile, 0, 0, FILE_END);

	return TRUE;
}

void CWtlFile::Close()
{
	if( !m_hFile || m_hFile==INVALID_HANDLE_VALUE ) return;

	::CloseHandle(m_hFile);
	m_hFile = NULL;
}

BOOL CWtlFile::Delete()
{
	return ::DeleteFile(m_strFileName);
}

DWORD CWtlFile::Read(LPVOID lpBuf, DWORD nCount)
{
	ATLASSERT(lpBuf!=NULL);
	if (nCount == 0)
		return 0;

	DWORD dwBytesRead;
	if( !::ReadFile(m_hFile, lpBuf, nCount, &dwBytesRead, NULL) ) return FALSE;
	return dwBytesRead;
}

BOOL CWtlFile::Write(LPCVOID lpBuf, DWORD nCount)
{
	ATLASSERT(lpBuf!=NULL);
	if (nCount == 0)
		return FALSE;

	DWORD dwBytesWritten;
	if( !::WriteFile(m_hFile, lpBuf, nCount, &dwBytesWritten, NULL) ) return FALSE;
	ATLASSERT( dwBytesWritten==nCount );
	return TRUE;
}

void CWtlFile::WriteEndOfLine()
{
	DWORD dwBytesWritten;
	BYTE buffCRLF[] = {'\r','\n'};
	::WriteFile(m_hFile, buffCRLF, 2, &dwBytesWritten, NULL);
	ATLASSERT( dwBytesWritten==2 );
}

BOOL CWtlFile::Flush()
{
	return ::FlushFileBuffers(m_hFile);
}

DWORD CWtlFile::Seek(LONG lOff, UINT nFrom)
{
	return ::SetFilePointer(m_hFile, lOff, NULL, (DWORD) nFrom);
}

DWORD CWtlFile::SeekToEnd()
{ 
	return Seek(0, CWtlFile::end); 
}

DWORD CWtlFile::SeekToBegin()
{ 
	return Seek(0, CWtlFile::begin);
}

DWORD CWtlFile::GetPosition()
{
	DWORD dwPos = ::SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
	ATLASSERT(dwPos!=-1);
	return dwPos;
}

void CWtlFile::SetLength(DWORD dwNewLen)
{
	Seek(dwNewLen, CWtlFile::begin);
	::SetEndOfFile(m_hFile);
}
DWORD CWtlFile::GetLength()
{
	return ::GetFileSize(m_hFile, NULL);
}
DWORD CWtlFile::GetSize()
{
	return GetLength();
}

BOOL CWtlFile::FileExists(PCTSTR pstrFileName) //http://blog.kowalczyk.info/article/Check-if-file-exists-on-Windows.html
{
	DWORD dwErrMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	DWORD dwAttribs = ::GetFileAttributes(pstrFileName);
	::SetErrorMode(dwErrMode);
	return (dwAttribs != (DWORD) -1) && (dwAttribs & FILE_ATTRIBUTE_DIRECTORY) == 0;
}


CTemporaryFile::CTemporaryFile()
{
}

CTemporaryFile::~CTemporaryFile()
{ 
	Close();
	Delete();
}

BOOL CTemporaryFile::TempFile( CString& outpath )
{
	PTSTR out = outpath.GetBuffer(MAX_PATH);
	::GetTempPath(MAX_PATH, out);
	::GetTempFileName(out, _T("BV"), 0, out);
	outpath.ReleaseBuffer();
	return Open( outpath, CWtlFile::modeWrite | CWtlFile::modeCreate | CWtlFile::shareExclusive );
}

