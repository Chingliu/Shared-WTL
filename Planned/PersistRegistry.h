#if !defined(AFX_AtlPersistRegistry_H__20000520_D45E_294E_B21B_0080AD509054__INCLUDED_)
#define AFX_AtlPersistRegistry_H__20000520_D45E_294E_B21B_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////
//
// A simple persist class modelled after the ATL IPersistStorageImpl and 
// IPersistPropertyBagImpl. It writes data to the registry.
// This class allows you to use the BEGIN_PROP_MAP / END_PROP_MAP maps
// and have the data written to the registry.
//
// Only simple data types (integers and strings) are supported.
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2000.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//
//

#ifndef __cplusplus
   #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
   #error This file requires atlbase.h to be included first
#endif


//////////////////////////////////////////////////////////////////////////////
// Implementation

HRESULT AtlPersistRegistry_Load(ATL_PROPMAP_ENTRY* pMap, HKEY hKey, VOID *pThis)
{
   if(hKey==NULL) {
      ATLTRACE2(atlTraceCOM, 0, _T("PersistRegistry key passed in was invalid\n"));
      return E_POINTER;
   }
   CRegKey reg;
   reg.Open(hKey,NULL,KEY_READ);

   for (int i = 0; pMap[i].pclsidPropPage != NULL; i++) {
      if (pMap[i].szDesc == NULL) continue;

      USES_CONVERSION;
      LPCTSTR pstrName = OLE2T(pMap[i].szDesc);

      // If raw entry skip it - we don't handle it for streams just yet
      if (pMap[i].dwSizeData != 0) {
         void* pData = (void*) (pMap[i].dwOffsetData + (DWORD)pThis);
         // check the type - we only deal with limited set
         bool bTypeOK = false;
         switch (pMap[i].vt) {
         case VT_UI1:
         case VT_I1:
            {
               DWORD val;
               reg.QueryValue(val, pstrName);
               *((BYTE*)pData) = (unsigned char)val;
            }
            break;
         case VT_BOOL:
            {
               DWORD val;
               reg.QueryValue(val, pstrName);
               *((BOOL*)pData) = (BOOL)val;
            }
            break;
         case VT_BSTR:
            {
               TCHAR szStr[300];
               DWORD dwCnt = 300;
               reg.QueryValue(szStr, pstrName, &dwCnt);
               CComBSTR bstr(szStr);
               *((BSTR*)pData) = bstr.Detach();
            }
            break;
         case VT_I2:
         case VT_UI2:
            {
               DWORD val;
               reg.QueryValue(val, pstrName);
               *((short*)pData) = (short)val;
            }
            break;
         case VT_I4:
         case VT_UI4:
         case VT_INT:
         case VT_UINT:
            {
               DWORD val;
               reg.QueryValue(val, pstrName);
               *((long*)pData) = (long)val;
            }
            break;
         default:
            ATLTRACE2(atlTraceCOM, 0, _T("Unsupported type for property #%s\n"), pstrName);
            return E_INVALIDARG;
         }
      }
   }
   return S_OK;
}

HRESULT AtlPersistRegistry_Save(ATL_PROPMAP_ENTRY* pMap, HKEY hKey, VOID *pThis)
{
   if(hKey==NULL) {
      ATLTRACE2(atlTraceCOM, 0, _T("PersistRegistry key passed in was invalid\n"));
      return E_POINTER;
   }
   CRegKey reg;
   reg.Open(hKey,NULL,KEY_WRITE);

   for (int i = 0; pMap[i].pclsidPropPage != NULL; i++) {
      if (pMap[i].szDesc == NULL) continue;

      USES_CONVERSION;
      LPCTSTR pstrName = OLE2T(pMap[i].szDesc);

      // If raw entry skip it - we don't handle it for streams just yet
      if (pMap[i].dwSizeData != 0) {
         void* pData = (void*) (pMap[i].dwOffsetData + (DWORD)pThis);
         // check the type - we only deal with limited set
         bool bTypeOK = false;
         switch (pMap[i].vt) {
         case VT_UI1:
         case VT_I1:
            {
               unsigned char val = *((BYTE*)pData);
               reg.SetValue(val, pstrName);
            }
            break;
         case VT_BOOL:
            {
               BOOL val = *((VARIANT_BOOL*)pData);
               reg.SetValue(val, pstrName);
            }
            break;
         case VT_BSTR:
            {
               CComBSTR bstr = *((BSTR*)pData);
               LPCTSTR val = OLE2T(bstr);
               reg.SetValue(val, pstrName);
            }
            break;
         case VT_I2:
         case VT_UI2:
            {
               long val = *((short*)pData);
               reg.SetValue(val, pstrName);
            }
            break;
         case VT_I4:
         case VT_UI4:
         case VT_INT:
         case VT_UINT:
            {
               long val = *((long*)pData);
               reg.SetValue(val, pstrName);
               break;
            }
         default:
            ATLTRACE2(atlTraceCOM, 0, _T("Unsupported type for property #%s\n"), pstrName);
            return E_INVALIDARG;
         }
      }
   }
   return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// CPersistRegistry

template <class T>
class ATL_NO_VTABLE CPersistRegistry
{
public:
   STDMETHOD(Load)(HKEY hKey)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("CPersistRegistry::Load\n"));
      T* pT = static_cast<T*>(this);
      ATL_PROPMAP_ENTRY* pMap = T::GetPropertyMap();
      ATLASSERT(pMap != NULL);
      return AtlPersistRegistry_Load(pMap, hKey, pT);
   }
   STDMETHOD(Save)(HKEY hKey)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("CPersistRegistry::Save\n"));
      T* pT = static_cast<T*>(this);
      ATL_PROPMAP_ENTRY* pMap = T::GetPropertyMap();
      ATLASSERT(pMap != NULL);
      return AtlPersistRegistry_Save(pMap, hKey, pT);
   }

};


#endif // !defined(AFX_AtlPersistRegistry_H__20000520_D45E_294E_B21B_0080AD509054__INCLUDED_)
