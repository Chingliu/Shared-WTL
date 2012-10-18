#pragma once


//	TODO:
// -opção de poder associar a uma CWriteStream um HANDLE de arquivo,
//	ou um path de arquivo

class CWriteStream
{
public:
	CWriteStream( UINT nAllocSize=0x1000 ); //4kb

// Initializers
public:
	CAtlArray<BYTE>& AccessStream( bool reset=true );

// Data members
private:
	CAtlArray<BYTE> m_databuff;

	BYTE* m_writePos;
	UINT m_buffSize;

// Function members
private:
	void WriteSeek( int );

// Write Interface
public:
	CWriteStream& Write( const void*, UINT );
	void WriteSizedString( CString& );

	CWriteStream& operator<<( UINT& );
	CWriteStream& operator<<( DWORD& );
	CWriteStream& operator<<( CString& );
	CWriteStream& operator<<( CAtlArray<BYTE>& );
};
