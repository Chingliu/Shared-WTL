#pragma once


class CWtlFile
{
public:
	// Constructors, destructor
	CWtlFile();
	~CWtlFile();

	// Operations
	bool Open( PCTSTR lpszFileName, UINT nOpenFlags );
	void Close();
	bool Delete();

	DWORD Read(LPVOID lpBuf, DWORD nCount);
	bool ReadContent(CAtlArray<BYTE>& buff);
	bool Write(LPCVOID lpBuf, DWORD nCount);
	void WriteEndOfLine();
	bool Flush();

	DWORD Seek(LONG lOff, UINT nFrom);
	DWORD SeekToEnd();
	DWORD SeekToBegin();
	DWORD GetPosition();

	void SetLength(DWORD dwNewLen);
	DWORD GetLength();
	DWORD GetSize();


// Static
public:
	static bool FileExists(PCTSTR pstrFileName);
	// TO-DO - atributos como LastAccessTime, get a file size based on file name - http://blog.kowalczyk.info/article/Get-file-size-under-windows.html

// Data members
private:
   HANDLE m_hFile;
   CString m_strFileName;

// Enums
public:
	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	enum OpenFlags {
		modeRead =         (int) 0x0000,
		modeWrite =        (int) 0x0001,
		modeReadWrite =    (int) 0x0002,
		shareCompat =      (int) 0x0000, // wtf?
		shareExclusive =   (int) 0x0010,
		shareDenyWrite =   (int) 0x0020,
		shareDenyRead =    (int) 0x0030,
		shareDenyNone =    (int) 0x0040,
		modeNoInherit =    (int) 0x0080,
		modeCreate =       (int) 0x1000,
		modeNoTruncate =   (int) 0x2000,
		typeText =         (int) 0x4000, // typeText and typeBinary are
		typeBinary =       (int) 0x8000, // used in derived classes only
		};

	/*enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
		};*/
};


// poderia usar CAtlTemporaryFile do header <atlfile.h>
class CTemporaryFile
	: public CWtlFile
{
public:
	CTemporaryFile();
	~CTemporaryFile();

public:
	bool TempFile( CString& outpath );
};


class CTextFile
	: public CWtlFile
{
public:
	bool ReadUTF16LE(CStringW& refstr);
};