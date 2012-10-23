#pragma once


class CAdapter
{
};


class CAdapterLinked : public CAdapter
{
public:
	template<typename TMgrPtr> /*CManagerLinked derived type*/
	CAdapterLinked(TMgrPtr* mgr)
	{
		ASSERT(mgr);
		mgr->SetAdapter( static_cast<TMgrPtr::tdAdapter*>(this) );
	};
};

class CAdapterLinkedN : public CAdapter
{
public:
	template<typename TMgrPtr> /*CManagerLinkedN derived type*/
	CAdapterLinkedN(TMgrPtr* mgr)
	{
		ASSERT(mgr);
		mgr->AddAdapter( static_cast<TMgrPtr::tdAdapter*>(this) );
	};
};
