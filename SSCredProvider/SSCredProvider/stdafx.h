// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
// TODO: reference additional headers your program requires here
#include "SSCredProvider.h"


#define ISSUnknown_REF_DEFINE \
protected: \
	ULONG m_ulRef; \
public: \
	STDMETHOD_(ULONG, AddRef)(); \
	STDMETHOD_(ULONG, Release)(); \
	STDMETHOD(QueryInterface)(CONST IID& iid, void **ppv); \
private:

#define ISSUnknown_REF_INIT \
	m_ulRef = 1

#define ISSUnknown_REF_IMPL(class) \
	STDMETHODIMP_(ULONG) class::AddRef() \
	{ \
	return ++m_ulRef; \
	} \
	STDMETHODIMP_(ULONG) class::Release() \
	{ \
	ULONG rv = --m_ulRef; \
	if(rv <= 0) \
		{ \
		rv = 0; \
		delete this; \
		} \
		return rv; \
	}

#define SSWA_MAX_DOMAIN 25
#define SSWA_MAX_USERNAME 25
#define SSWA_MAX_PASSWORD 25

typedef struct tagSSWACredProvCredentialInfos
{
	BOOL fCredentialPrepared;

	WCHAR szDomain[SSWA_MAX_DOMAIN];
	WCHAR szUsername[SSWA_MAX_USERNAME];
	WCHAR szPassword[SSWA_MAX_PASSWORD];


	tagSSWACredProvCredentialInfos()
	{
		Clear();
	}

	VOID Clear()
	{
		fCredentialPrepared = FALSE;

		_tcscpy_s(szDomain, TEXT(""));
		_tcscpy_s(szDomain, TEXT(""));
		_tcscpy_s(szDomain, TEXT(""));
	}

} SSWACredProvCredentialInfos;


#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Credui.lib")


#define SS_AUTO_FNC_TRACE
#define SS_LOG_V(...)
#define LL_TRACE
#define SS_DIMOF _countof
#define SS_LOG_R_SetCB(...)
#define SS_LOG_V_W(...)

#define MP_ASSERT_HERE DebugBreak()
/************************************************************************/
/* CHECK REGION Macros                                                  */
/************************************************************************/
#define SS_CHECK_BLOCK_AFTER_TAGFINISH 0x12345678

#define check_block_begin(r_type, r_v_e) r_type _ret_01_tmp = (r_v_e), _ret_01 = (r_v_e); int __disable_loop_tt = 0; 
#define check_block_finally(r_v_s) _ret_01 = (r_v_s); _tag_finish: __disable_loop_tt = SS_CHECK_BLOCK_AFTER_TAGFINISH;
#define check_block_return return _ret_01
#define check_block_return_is_success(r_v_s) (_ret_01 == (r_v_s))

#define check_block_exit(r_v_) {if(__disable_loop_tt != 0) MP_ASSERT_HERE; _ret_01 = (r_v_); goto _tag_finish;}
#define check_value(exp, r_v_s) {_ret_01_tmp = (exp); if(_ret_01_tmp != (r_v_s)) check_block_exit(_ret_01_tmp); }
#define check_value_noerr(exp, r_v_s) {if((exp) != (r_v_s)) check_block_exit(r_v_s); }

#define check_value_r_map(exp_type, exp, exp_v_s, exp_map) \
{ \
	exp_type _exp_01_tmp = (exp); \
	if(_exp_01_tmp != (exp_v_s))  \
	check_block_exit(exp_map(_exp_01_tmp)); \
}

//////////////////////////////////////////////////////////////////////////
#define check_HRESULT_begin check_block_begin(HRESULT, E_FAIL)
#define check_HRESULT_finally check_block_finally(S_OK)
#define check_HRESULT_return check_block_return
#define check_HRESULT_return_is_success check_block_return_is_success(S_OK)

#define check_HRESULT_exit check_block_exit
#define check_HRESULT_bool(exp, r_v) if(!(exp)) check_block_exit(r_v)
#define check_HRESULT(exp) check_value(exp, S_OK)
#define check_HRESULT_SUCCEEDED(exp) {_ret_01_tmp = (exp); check_HRESULT_bool(SUCCEEDED(_ret_01_tmp), _ret_01_tmp);}
