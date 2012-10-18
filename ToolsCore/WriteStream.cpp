#include "stdafx.h"
#include "WriteStream.h"


CWriteStream::CWriteStream( UINT nAllocSize )
{
	m_databuff.SetCount( nAllocSize );
	m_writePos = m_databuff.GetData();
	m_buffSize = 0;
}

CAtlArray<BYTE>& CWriteStream::AccessStream( bool reset )
{
	if( reset )
	{
		m_writePos = m_databuff.GetData();
		m_buffSize = 0;
	}
	return m_databuff;
}


void CWriteStream::WriteSeek( int nSize )
{
	m_databuff.SetCount( m_buffSize+nSize );
	m_writePos = m_databuff.GetData() + m_buffSize;
	m_buffSize += nSize;
}

CWriteStream& CWriteStream::Write( const void* buff, UINT nSize )
{
	WriteSeek( nSize );
	memcpy( m_writePos, buff, nSize );

	return *this;
}

void CWriteStream::WriteSizedString( CString& strref )
{
	DWORD length = strref.GetLength();
	*this << length;

	if( length>0 )
	{
		WriteSeek( length );
		CStringA writebuff( strref, length );
		memcpy( m_writePos, writebuff, length );
	}
}

CWriteStream& CWriteStream::operator<<( UINT& dataref )
{
	WriteSeek( sizeof UINT );
	*(UINT*) m_writePos = dataref;

	return *this;
}

CWriteStream& CWriteStream::operator<<( DWORD& dataref )
{
	WriteSeek( sizeof DWORD );
	*(DWORD*) m_writePos = dataref;

	return *this;
}

CWriteStream& CWriteStream::operator<<( CString& strref )
{
	strref += '\0';
	WriteSeek( strref.GetLength() );
	memcpy( m_writePos, CStringA(strref), strref.GetLength() );

	return *this;
}

CWriteStream& CWriteStream::operator<<( CAtlArray<BYTE>& bufref )
{
	WriteSeek( bufref.GetCount() );
	memcpy( m_writePos, bufref.GetData(), bufref.GetCount() );

	return *this;
}
