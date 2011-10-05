#pragma once


class CFileReport
	: CAtlException
{
};

class CWtlFile
{
public:
	// Constructors, destructor
	CWtlFile();
	~CWtlFile();

	// Operations
	BOOL Open( PCTSTR lpszFileName, UINT nOpenFlags );
	void Close();
	BOOL Delete();

	DWORD Read(LPVOID lpBuf, DWORD nCount);
	BOOL Write(LPCVOID lpBuf, DWORD nCount);
	void WriteEndOfLine();
	BOOL Flush();

	DWORD Seek(LONG lOff, UINT nFrom);
	DWORD SeekToEnd();
	DWORD SeekToBegin();
	DWORD GetPosition();

	void SetLength(DWORD dwNewLen);
	DWORD GetLength();
	DWORD GetSize();


// Static
public:
	static BOOL FileExists(PCTSTR pstrFileName);
	// TO-DO - atributos como LastAccessTime, get a file size based on file name - http://blog.kowalczyk.info/article/Get-file-size-under-windows.html

// Data members
private:
   HANDLE m_hFile;
   CString m_strFileName;

// Enums
public:
	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	enum OpenFlags {
		modeRead =         (int) 0x00000,
		modeWrite =        (int) 0x00001,
		modeReadWrite =    (int) 0x00002,
		shareCompat =      (int) 0x00000,
		shareExclusive =   (int) 0x00010,
		shareDenyWrite =   (int) 0x00020,
		shareDenyRead =    (int) 0x00030,
		shareDenyNone =    (int) 0x00040,
		modeNoInherit =    (int) 0x00080,
		modeCreate =       (int) 0x01000,
		modeNoTruncate =   (int) 0x02000,
		typeText =         (int) 0x04000, // typeText and typeBinary are
		typeBinary =       (int) 0x08000, // used in derived classes only
		};

	enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
		};
};


class CTemporaryFile
	: public CWtlFile
{
public:
	CTemporaryFile();
	~CTemporaryFile();

public:
	BOOL TempFile( CString& outpath );
};
