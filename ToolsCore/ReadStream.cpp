// ReadStream.cpp

#include "stdafx.h"
#include "ReadStream.h"


CReadStream::CReadStream()
{
	m_seek = 0;
}

CAtlArray<BYTE>& CReadStream::BufferStream( UINT size )
{
	m_databuff.ShrinkData();// avoids realocation
	m_databuff.SetCount( size );

	m_seek = 0;
	m_buffsize = size;
	m_readPos = m_databuff.GetData();
	return m_databuff;
}

void CReadStream::DynamicSpotCopy()
{
	m_spot = m_seek;
}
void CReadStream::DynamicEndCopy( CAtlArray<BYTE>& dynbuff )
{
	dynbuff.SetCount( m_seek-m_spot );
	memcpy( dynbuff.GetData(), m_databuff.GetData()+m_spot, m_seek-m_spot );
}

bool CReadStream::BufferUTF16LE2Text(CStringW& refstr)
{
	const CAtlArray<BYTE>& buffer = *this;
	DWORD file_size = m_buffsize;

// Reads the 2-bytes header
	static_assert( sizeof(wchar_t)==2, "Not right" );
	wchar_t header;
	if( !Read(&header) )// there should be at least 2 bytes
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

	refstr.ReleaseBufferSetLength( text_length );
	return true;
}


void CReadStream::Skip( UINT nSize )
{
	m_readPos += nSize;
	m_seek += nSize;
}

// Function members
bool CReadStream::ReadSeek( int nSize=0 )
{
	m_readPos = m_databuff.GetData()+m_seek;
	if( nSize==0 && m_seek==m_buffsize )
		return false;
	if( m_seek+nSize > m_buffsize )
		return false;

	m_seek += nSize;
	return true;
}

// Normal read
bool CReadStream::Read( void* buff, UINT nMax )
{
	if( !ReadSeek(nMax) )
		return false;
	memcpy( buff, m_readPos, nMax );
	return true;
}

bool CReadStream::ReadSizedString( CString& strref )
{
	bool ret = true;

	DWORD length;  //before the string text, exists a DWORD with the string size
	*this >> length;

	if( length==0 )
		strref.Empty();
	if( !ReadSeek(length) )
		return FALSE;
	CString readbuff( (CHAR*) m_readPos, length );
	int i = readbuff.GetLength();
	strref = readbuff;
	int j = strref.GetLength();
	return ret;
}

bool CReadStream::ReadLine( CString& lineref )
{
	bool ret = ReadSeek();
	if( ret )
	{
		BYTE* lnfeed = m_readPos;
		UINT lnsize = 0;
		while( m_buffsize>m_seek+lnsize && (*(WORD*)(lnfeed++)!='\r\n' || !++lnfeed) )
			lnsize++;
		lineref = CStringA( (CHAR*) m_readPos, lnsize );
		ReadSeek( lnfeed-m_readPos );
	}
	return ret;
}


// Operators read
CReadStream& CReadStream::operator>>( DWORD& loadref )
{
	if( ReadSeek(sizeof DWORD) )
		loadref = *(DWORD*)m_readPos;

	return *this;
}

CReadStream& CReadStream::operator>>( UINT& loadref )
{
	if( ReadSeek(sizeof UINT) )
		loadref = *(UINT*)m_readPos;

	return *this;
}

CReadStream& CReadStream::operator>>( CString& strref )
{
	if( ReadSeek() )
	{
		strref = m_readPos;  //do TCHAR conversion assuming m_readPos as CHAR
		ReadSeek( strref.GetLength()+1 );
	}

	return *this;
}

CReadStream& CReadStream::operator>>( CStringA& strref )
{
	DWORD length;  //before the string text, exists a DWORD with the string size
	*this >> length;

	if( ReadSeek(length) )
		strref.SetString( (CHAR*) m_readPos, length );

	return *this;
}
