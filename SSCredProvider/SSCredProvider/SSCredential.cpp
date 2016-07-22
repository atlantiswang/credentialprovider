#include "stdafx.h"
#include "SSCredential.h"
#include "SSCredProviderUtils.h"
#include "SSProvider.h"

ISSUnknown_REF_IMPL(CSSCredential)
CSSCredential::CSSCredential()
{
	SS_AUTO_FNC_TRACE;
	SSWACredProvAddRef();

	ISSUnknown_REF_INIT;
	m_cpus = CPUS_INVALID;
	m_dwCredUIFlags = 0;
	m_pCredentialProvider = NULL;
	m_pcpce = NULL;

	int x = 0;
	for(x=0; x<_countof(m_pwszFieldStrings); x++)
	{
		m_pwszFieldStrings[x] = NULL;
		::SHStrDupW(L"", &m_pwszFieldStrings[x]);
	}

	::CoTaskMemFree(m_pwszFieldStrings[SSFI_LARGE_TEXT]);
	::SHStrDupW(L"SSCredProvider", &m_pwszFieldStrings[SSFI_LARGE_TEXT]);
	::CoTaskMemFree(m_pwszFieldStrings[SSFI_SMALL_TEXT]);
	::SHStrDupW(L"Esc Return", &m_pwszFieldStrings[SSFI_SMALL_TEXT]);
	::CoTaskMemFree(m_pwszFieldStrings[SSFI_SUBMIT_BUTTON]);
	::SHStrDupW(L"Submit", &m_pwszFieldStrings[SSFI_SUBMIT_BUTTON]);
}

CSSCredential::~CSSCredential()
{
	SS_AUTO_FNC_TRACE;
	int n = 0;
	for(n=0; n<_countof(m_pwszFieldStrings); n++)
	{
		::CoTaskMemFree(m_pwszFieldStrings[n]);
		m_pwszFieldStrings[n] = NULL;
	}

	SSWACredProvRelease();
}

STDMETHODIMP CSSCredential::QueryInterface(REFIID riid, void** ppv)
{
	SS_AUTO_FNC_TRACE;
	if(ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	if(riid != IID_IUnknown && riid != IID_ICredentialProviderCredential)
		return E_NOINTERFACE;

	*ppv = static_cast<IUnknown*>(this);
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();

	return S_OK;
}

STDMETHODIMP CSSCredential::Advise(ICredentialProviderCredentialEvents *pcpce)
{
	SS_AUTO_FNC_TRACE;
	if(m_pcpce != NULL)
	{
		m_pcpce->Release();
		m_pcpce = NULL;
	}

	m_pcpce = pcpce;
	m_pcpce->AddRef();

	return S_OK;
}

STDMETHODIMP CSSCredential::UnAdvise()
{
	SS_AUTO_FNC_TRACE;
	if(m_pcpce != NULL)
	{
		m_pcpce->Release();
		m_pcpce = NULL;
	}

	return S_OK;
}

STDMETHODIMP CSSCredential::SetSelected(BOOL *pbAutoLogon)
{
	SS_AUTO_FNC_TRACE;
	if( (m_cpus == CPUS_LOGON || m_cpus == CPUS_UNLOCK_WORKSTATION )&&
		::MessageBox(::GetForegroundWindow(), TEXT("Log On ?"), TEXT("Confirm"), MB_ICONQUESTION|MB_YESNO) == IDYES)
	{
		TryDoLogon(L"zbook-PC", L"zbook", L" ", TRUE);
		*pbAutoLogon = TRUE;
	}else
	{
		*pbAutoLogon = FALSE;
	}

	return S_OK;
}
STDMETHODIMP CSSCredential::SetDeselected()
{
	SS_AUTO_FNC_TRACE;
	return S_OK;
}

STDMETHODIMP CSSCredential::GetFieldState(DWORD index, CREDENTIAL_PROVIDER_FIELD_STATE *pcpfs
													  , CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *pcpfis)
{
	SS_AUTO_FNC_TRACE;
	if(index<SSFI_BEGIN || index >=SSFI_NUM_FILEDS)
	{
		return E_INVALIDARG;
	}

	*pcpfs = CPFS_DISPLAY_IN_SELECTED_TILE;
	*pcpfis = CPFIS_NONE;
	if(index == SSFI_USERNAME)
	{
		*pcpfis = CPFIS_FOCUSED;
	}

	if(index == SSFI_LARGE_TEXT)
	{
		*pcpfs = CPFS_DISPLAY_IN_BOTH;
	}

	if(m_cpus != CPUS_CHANGE_PASSWORD)
	{
		if(index == SSFI_NEWPASSWORD || index == SSFI_NEWPASSWORD_CONFIRM)
		{
			*pcpfs = CPFS_HIDDEN;
		}
	}

	return S_OK;
}

STDMETHODIMP CSSCredential::GetStringValue(DWORD index, PWSTR *ppwsz)
{
	SS_AUTO_FNC_TRACE;
	if(index<SSFI_BEGIN || index >= SSFI_NUM_FILEDS)
	{
		return E_INVALIDARG;
	}

	::SHStrDupW(m_pwszFieldStrings[index], ppwsz);

	return S_OK;
}

STDMETHODIMP CSSCredential::GetBitmapValue(DWORD index, HBITMAP *phbmp)
{
	SS_AUTO_FNC_TRACE;
	if(index<SSFI_BEGIN || index >= SSFI_NUM_FILEDS)
	{
		return E_INVALIDARG;
	}

	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetCheckboxValue(DWORD index, BOOL *pbChecked, PWSTR *ppwszLabel)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetComboBoxValueCount(DWORD index, DWORD *pcItems, DWORD *pdwSelectedItem)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetComboBoxValueAt(DWORD index, DWORD item, PWSTR *ppwszItem)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetSubmitButtonValue(DWORD index, DWORD *pdwAdjacentTo)
{
	SS_AUTO_FNC_TRACE;
	if(m_cpus == CPUS_CHANGE_PASSWORD)
	{
		*pdwAdjacentTo = SSFI_NEWPASSWORD_CONFIRM;
	}else
	{
		*pdwAdjacentTo = SSFI_PASSWORD;
	}
	return S_OK;
}

STDMETHODIMP CSSCredential::SetStringValue(DWORD index, PCWSTR pwz)
{
	SS_AUTO_FNC_TRACE;
	if(index < SSFI_BEGIN || index >= SSFI_NUM_FILEDS)
	{
		return S_FALSE;
	}
	::CoTaskMemFree(m_pwszFieldStrings[index]);
	::SHStrDupW(pwz, &m_pwszFieldStrings[index]);
	return S_OK;
}

STDMETHODIMP CSSCredential::SetCheckboxValue(DWORD index, BOOL bChecked)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::SetComboBoxSelectedValue(DWORD index, DWORD selItem)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::CommandLinkClicked(DWORD index)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr
														 , CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs
														 , PWSTR *ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsi)
{
	SS_AUTO_FNC_TRACE;
	LPWSTR lpPasswordEncrypted = NULL;
	LPWSTR lpPasswordNewEncrypted = NULL;
	LPWSTR lpDomainUsername = NULL;

	BYTE *pRgb = NULL;
	DWORD cbRgb = 0;

	SS_LOG_V(LL_TRACE, _T("CredInfo : %s,%s"), m_pwszFieldStrings[SSFI_USERNAME], m_pwszFieldStrings[SSFI_PASSWORD]);

	check_HRESULT_begin
	{
		if(!m_CredentialInfos.fCredentialPrepared)
		{
			if(wcslen(m_pwszFieldStrings[SSFI_USERNAME]) > 0 && wcslen(m_pwszFieldStrings[SSFI_PASSWORD]) >= 0)
			{
				check_HRESULT(SSWACPSpliterDomainAndUsername(m_pwszFieldStrings[SSFI_USERNAME]
				, m_CredentialInfos.szDomain, SS_DIMOF(m_CredentialInfos.szDomain)
					, m_CredentialInfos.szUsername, SS_DIMOF(m_CredentialInfos.szUsername)));

				_tcscpy_s(m_CredentialInfos.szPassword, m_pwszFieldStrings[SSFI_PASSWORD]);
				m_CredentialInfos.fCredentialPrepared = TRUE;
			}
		}

		SS_LOG_V(LL_TRACE, _T("CredInfoDetail : %s\\%s,%s,%d"), m_CredentialInfos.szDomain, m_CredentialInfos.szUsername
			, m_CredentialInfos.szPassword, m_CredentialInfos.fCredentialPrepared);

		check_HRESULT_bool(m_CredentialInfos.fCredentialPrepared, E_FAIL);

		DWORD cchProcted = 0;
		check_HRESULT(SSWACPTCopyStrProtectIfNecessaryW(m_CredentialInfos.szPassword, m_cpus, NULL, &cchProcted));


		lpPasswordEncrypted = (LPWSTR)CoTaskMemAlloc(cchProcted * sizeof(WCHAR));
		check_HRESULT_bool(lpPasswordEncrypted != NULL, E_FAIL);
		check_HRESULT(SSWACPTCopyStrProtectIfNecessaryW(m_CredentialInfos.szPassword, m_cpus, lpPasswordEncrypted, &cchProcted));


		if(m_cpus == CPUS_CHANGE_PASSWORD)
		{
			check_HRESULT_bool(wcslen(m_pwszFieldStrings[SSFI_NEWPASSWORD]) == wcslen(m_pwszFieldStrings[SSFI_NEWPASSWORD_CONFIRM])
				, E_FAIL);
			check_HRESULT_bool(wcscmp(m_pwszFieldStrings[SSFI_NEWPASSWORD], m_pwszFieldStrings[SSFI_NEWPASSWORD_CONFIRM]) == 0
				, E_FAIL);
			check_HRESULT(SSWACPTCopyStrProtectIfNecessaryW(m_pwszFieldStrings[SSFI_NEWPASSWORD], m_cpus, NULL, &cchProcted));
			lpPasswordNewEncrypted = (LPWSTR)CoTaskMemAlloc(cchProcted * sizeof(WCHAR));
			check_HRESULT_bool(lpPasswordNewEncrypted != NULL, E_FAIL);
			check_HRESULT(SSWACPTCopyStrProtectIfNecessaryW(m_pwszFieldStrings[SSFI_NEWPASSWORD], m_cpus, lpPasswordNewEncrypted, &cchProcted));

			KERB_CHANGEPASSWORD_REQUEST objKcr;
			check_HRESULT(SSWACPTKerbChangePasswordRequestInitW(m_CredentialInfos.szDomain, m_CredentialInfos.szUsername, lpPasswordEncrypted
				, lpPasswordNewEncrypted, &objKcr));
			check_HRESULT(SSWACPTKerbChangePasswordRequestPack(objKcr, &pcpcs->rgbSerialization, &pcpcs->cbSerialization));
		}else if(m_cpus == CPUS_CREDUI)
		{
			size_t cchLenDomain = 0, cchLenUser = 0,  cchDomainUser;
			check_HRESULT(StringCchLengthW(m_CredentialInfos.szDomain, STRSAFE_MAX_CCH, &cchLenDomain));
			check_HRESULT(StringCchLengthW(m_CredentialInfos.szUsername, STRSAFE_MAX_CCH, &cchLenUser));
			cchDomainUser = cchLenDomain + 1 + cchLenUser + 1;
			lpDomainUsername = (LPWSTR)CoTaskMemAlloc(cchDomainUser * sizeof(WCHAR));
			check_HRESULT_bool(lpDomainUsername != NULL, E_FAIL);
			swprintf_s(lpDomainUsername, cchDomainUser, L"%s\\%s", m_CredentialInfos.szDomain, m_CredentialInfos.szUsername);

			check_HRESULT_bool(!::CredPackAuthenticationBufferW((m_dwCredUIFlags & CREDUIWIN_PACK_32_WOW) ? CRED_PACK_WOW_BUFFER : 0
				, lpDomainUsername, lpPasswordEncrypted, pRgb, &cbRgb) && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER, E_FAIL);
			pRgb = (BYTE*)HeapAlloc(::GetProcessHeap(), 0, cbRgb);
			check_HRESULT_bool(pRgb != NULL, E_FAIL);
			check_HRESULT_bool(::CredPackAuthenticationBufferW((m_dwCredUIFlags & CREDUIWIN_PACK_32_WOW) ? CRED_PACK_WOW_BUFFER : 0
				, lpDomainUsername, lpPasswordEncrypted, pRgb, &cbRgb), E_FAIL);
		}else
		{

			KERB_INTERACTIVE_UNLOCK_LOGON objKIUL;
			check_HRESULT(SSWACPTKerbInteractiveUnlockLogonInitW(m_CredentialInfos.szDomain
				, m_CredentialInfos.szUsername, lpPasswordEncrypted, m_cpus
				, &objKIUL));

			check_HRESULT(SSWACPTKerbInteractiveUnlockLogonPack(objKIUL, &pcpcs->rgbSerialization, &pcpcs->cbSerialization));
		}


		ULONG ulAuthPackage = 0;
		check_HRESULT(SSWACPTRetrieveNegotiateAuthPackage(&ulAuthPackage));

		pcpcs->ulAuthenticationPackage = ulAuthPackage;
		pcpcs->clsidCredentialProvider = CLSID_SSCredProvider;

		if(m_cpus == CPUS_CREDUI)
		{
			pcpcs->rgbSerialization = pRgb;
			pcpcs->cbSerialization = cbRgb;
			pRgb = NULL;
			cbRgb = 0;
		}

		*pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED;
	}
	check_HRESULT_finally
	{

	}

	if(pRgb != NULL)
	{
		::HeapFree(::GetProcessHeap(), 0, pRgb);
		pRgb = NULL;
	}

	if(lpPasswordNewEncrypted != NULL)
	{
		CoTaskMemFree(lpPasswordNewEncrypted);
		lpPasswordNewEncrypted = NULL;
	}

	if(lpPasswordEncrypted != NULL)
	{
		CoTaskMemFree(lpPasswordEncrypted);
		lpPasswordEncrypted = NULL;
	}

	m_CredentialInfos.Clear();

	check_HRESULT_return;
}

STDMETHODIMP CSSCredential::ReportResult(NTSTATUS status, NTSTATUS subStatus, PWSTR *ppwszOptionalStatusText
													 , CREDENTIAL_PROVIDER_STATUS_ICON *pcpsi)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

HRESULT CSSCredential::Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
								  DWORD cpusFlags, ICredentialProvider *pCredentialProvider)
{
	SS_AUTO_FNC_TRACE;
	m_cpus = cpus;
	m_dwCredUIFlags = cpusFlags;
	m_pCredentialProvider = pCredentialProvider;

	return S_OK;
}

HRESULT CSSCredential::TryDoLogon(LPCWSTR lpDomain, LPCWSTR lpUsername, LPCWSTR lpPassword, BOOL fNotifyProvider /*= TRUE*/)
{
	SS_AUTO_FNC_TRACE;
	m_CredentialInfos.Clear();
	if(lpDomain != NULL)
	{
		_tcscpy_s(m_CredentialInfos.szDomain, lpDomain);
	}

	if(lpUsername != NULL)
	{
		_tcscpy_s(m_CredentialInfos.szUsername, lpUsername);
	}

	if(lpPassword != NULL)
	{
		_tcscpy_s(m_CredentialInfos.szPassword, lpPassword);
	}

	m_CredentialInfos.fCredentialPrepared = TRUE;

	if(!fNotifyProvider)
	{
		return S_OK;
	}

	if(m_pCredentialProvider == NULL)
	{
		return E_FAIL;
	}

	return ((CSSProvider*)m_pCredentialProvider)->ChangeCredentials(this);
}
