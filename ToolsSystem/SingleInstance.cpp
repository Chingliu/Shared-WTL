#include "stdafx.h"
#include "SingleInstance.h"


CSingleInstance::CSingleInstance()
{
}

bool CSingleInstance::IsExistingInstanceOf( PCTSTR szName )
{
	if( OpenMutex(MUTEX_ALL_ACCESS, FALSE, szName) == NULL )
	{
		CreateNullDaclSd();
		CreateMutex(&m_sa, FALSE, szName);
		FreeNullDaclSd();
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

void CSingleInstance::CreateNullDaclSd()
{
	m_sa.nLength = sizeof(m_sa);
	m_hsa = GlobalAlloc(GHND, SECURITY_DESCRIPTOR_MIN_LENGTH);
	m_sa.lpSecurityDescriptor = GlobalLock(m_hsa);
	m_sa.bInheritHandle = TRUE;

	ENSURE( InitializeSecurityDescriptor(m_sa.lpSecurityDescriptor, 1) );// "CNDS: cannot set security descriptor DACL\n"
	ENSURE( SetSecurityDescriptorDacl (m_sa.lpSecurityDescriptor, TRUE, NULL, FALSE) );// "CNDS: cannot initialise security descriptor\n"
}

void CSingleInstance::FreeNullDaclSd()
{
	GlobalUnlock(m_hsa);
	GlobalFree(m_hsa);
}
