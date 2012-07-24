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

bool CWtlFile::Open( PCTSTR lpszFileName, UINT nOpenFlags )
{
	Close();
	
	if( !::GetFullPathName( lpszFileName, MAX_PATH, m_strFileName.GetBuffer(MAX_PATH), nullptr) )// will convert relative paths to absolute
		ASSERT(false);

	// map read/write mode
	ASSERT((modeRead|modeWrite|modeReadWrite) == 3);
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
		ASSERT(false);  // invalid share mode
	}

	// map share mode
	DWORD dwShareMode = 0;
	switch (nOpenFlags & 0x70)    // map compatibility mode to exclusive
	{
	default:
		ASSERT(false);  // invalid share mode?
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
	if( hFile == INVALID_HANDLE_VALUE )
		return false;
	m_hFile = hFile;

	// See if the file already exists and if it does move the file pointer 
	// to the end of the file
	if (::GetLastError()==ERROR_ALREADY_EXISTS)
		::SetFilePointer(hFile, 0, 0, FILE_END);

	return true;
}

void CWtlFile::Close()
{
	if( !m_hFile || m_hFile==INVALID_HANDLE_VALUE ) return;

	::CloseHandle(m_hFile);
	m_hFile = NULL;
}

bool CWtlFile::Delete()
{
	return ::DeleteFile(m_strFileName)!=FALSE;
}

DWORD CWtlFile::Read(LPVOID lpBuf, DWORD nCount)
{
	ASSERT(lpBuf!=NULL);
	if (nCount == 0)
		return 0;

	DWORD dwBytesRead;
	if( !::ReadFile(m_hFile, lpBuf, nCount, &dwBytesRead, NULL) ) return FALSE;
	return dwBytesRead;
}

bool CWtlFile::ReadContent(CAtlArray<BYTE>& buff)
{
	DWORD size = GetLength();
	buff.SetCount(size);
	return Read(buff.GetData(), size)==size;
}

bool CWtlFile::Write(LPCVOID lpBuf, DWORD nCount)
{
	ASSERT(lpBuf!=NULL);
	if (nCount == 0)
		return false;

	DWORD dwBytesWritten;
	if( !::WriteFile(m_hFile, lpBuf, nCount, &dwBytesWritten, NULL) ) return false;
	ASSERT( dwBytesWritten==nCount );
	return true;
}

void CWtlFile::WriteEndOfLine()
{
	DWORD dwBytesWritten;
	BYTE buffCRLF[] = {'\r','\n'};
	::WriteFile(m_hFile, buffCRLF, 2, &dwBytesWritten, NULL);
	ASSERT( dwBytesWritten==2 );
}

bool CWtlFile::Flush()
{
	return ::FlushFileBuffers(m_hFile)!=FALSE;
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
	ASSERT(dwPos!=-1);
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

bool CWtlFile::FileExists(PCTSTR pstrFileName) //http://blog.kowalczyk.info/article/Check-if-file-exists-on-Windows.html
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

bool CTemporaryFile::TempFile( CString& outpath )
{
	PTSTR out = outpath.GetBuffer(MAX_PATH);
	::GetTempPath(MAX_PATH, out);
	::GetTempFileName(out, _T("BV"), 0, out);
	outpath.ReleaseBuffer();
	return Open( outpath, CWtlFile::modeWrite | CWtlFile::modeCreate | CWtlFile::shareExclusive );
}


bool CTextFile::ReadUTF16LE(CStringW& refstr)	// Reads a UTF-16 enconded text-file, removing a possible BOM header
{												// Note:	in UTF-16 each character is 2 or 4-bytes long, this func wont work if it has 4-bytes chars =(
	DWORD file_size = GetLength();				//			Windows works with UCS-2, always 2-byte long characters
	if( file_size==0 )
		return true;

// Reads the 2-bytes header
	static_assert( sizeof(wchar_t)==2, "WTF?" );
	wchar_t header;
	if( !Read(&header, 2) )// there should be at least 2 bytes
		return false;

	int text_length = file_size/2;
	wchar_t* buff = refstr.GetBuffer( text_length );

// Checks a possible UTF-16 BOM
	if( header == L'\xFEFF' )
		text_length--;
	else
		buff++[0] = header;// no bom header

// Reads remaining text to the buffer
	file_size -= 2;
	if( !Read(buff, file_size)==file_size )
		return false;

	//int chars = size/2;
	refstr.ReleaseBufferSetLength( text_length );
	return false;
}

/*
Old version that does not removes the BOM
bool CTextFile::ReadUTF16LE(CStringW& refstr)
{
	DWORD size = GetLength();
	void* buff = refstr.GetBuffer( size/2 );
	if( !Read(buff, size)==size )
		return false;

	refstr.ReleaseBufferSetLength( size/2 );
	return false;
}
*/