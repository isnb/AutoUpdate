//
// sproxy.exe generated file
// do not modify this file
//
// Created: 08/02/2012@21:15:2
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace SyncService
{

template <typename TClient = CSoapSocketClientT<> >
class CSyncServiceT : 
	public TClient, 
	public CSoapRootHandler
{
protected:

	const _soapmap ** GetFunctionMap();
	const _soapmap ** GetHeaderMap();
	void * GetHeaderValue();
	const wchar_t * GetNamespaceUri();
	const char * GetServiceName();
	const char * GetNamespaceUriA();
	HRESULT CallFunction(
		void *pvParam, 
		const wchar_t *wszLocalName, int cchLocalName,
		size_t nItem);
	HRESULT GetClientReader(ISAXXMLReader **ppReader);

public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	CSyncServiceT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://116.6.87.12:9999/syncwebsvc/service.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CSyncServiceT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT GetInstruction(
		int id, 
		BSTR* GetInstructionResult
	);

	HRESULT GetCanUpdate(
		int id, 
		BSTR* GetCanUpdateResult
	);

	HRESULT SaveCanUpdate(
		int id, 
		bool* SaveCanUpdateResult
	);

	HRESULT UpdateInstruction(
		int id, 
		short flag, 
		BSTR result
	);

	HRESULT Add(
		double n1, 
		double n2, 
		double* AddResult
	);
};

typedef CSyncServiceT<> CSyncService;

struct __CSyncService_GetInstruction_struct
{
	int id;
	BSTR GetInstructionResult;
};

extern __declspec(selectany) const _soapmapentry __CSyncService_GetInstruction_entries[] =
{

	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_GetInstruction_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xC92F1301, 
		"GetInstructionResult", 
		L"GetInstructionResult", 
		sizeof("GetInstructionResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSyncService_GetInstruction_struct, GetInstructionResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSyncService_GetInstruction_map =
{
	0xD0E6F791,
	"GetInstruction",
	L"GetInstructionResponse",
	sizeof("GetInstruction")-1,
	sizeof("GetInstructionResponse")-1,
	SOAPMAP_FUNC,
	__CSyncService_GetInstruction_entries,
	sizeof(__CSyncService_GetInstruction_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x349F2EE6,
	"http://Yishion.Sync",
	L"http://Yishion.Sync",
	sizeof("http://Yishion.Sync")-1
};


struct __CSyncService_GetCanUpdate_struct
{
	int id;
	BSTR GetCanUpdateResult;
};

extern __declspec(selectany) const _soapmapentry __CSyncService_GetCanUpdate_entries[] =
{

	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_GetCanUpdate_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x4DEB6C94, 
		"GetCanUpdateResult", 
		L"GetCanUpdateResult", 
		sizeof("GetCanUpdateResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSyncService_GetCanUpdate_struct, GetCanUpdateResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSyncService_GetCanUpdate_map =
{
	0x762001E4,
	"GetCanUpdate",
	L"GetCanUpdateResponse",
	sizeof("GetCanUpdate")-1,
	sizeof("GetCanUpdateResponse")-1,
	SOAPMAP_FUNC,
	__CSyncService_GetCanUpdate_entries,
	sizeof(__CSyncService_GetCanUpdate_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x349F2EE6,
	"http://Yishion.Sync",
	L"http://Yishion.Sync",
	sizeof("http://Yishion.Sync")-1
};


struct __CSyncService_SaveCanUpdate_struct
{
	int id;
	bool SaveCanUpdateResult;
};

extern __declspec(selectany) const _soapmapentry __CSyncService_SaveCanUpdate_entries[] =
{

	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_SaveCanUpdate_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xDC54D4E3, 
		"SaveCanUpdateResult", 
		L"SaveCanUpdateResult", 
		sizeof("SaveCanUpdateResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_SaveCanUpdate_struct, SaveCanUpdateResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSyncService_SaveCanUpdate_map =
{
	0x4484B9F3,
	"SaveCanUpdate",
	L"SaveCanUpdateResponse",
	sizeof("SaveCanUpdate")-1,
	sizeof("SaveCanUpdateResponse")-1,
	SOAPMAP_FUNC,
	__CSyncService_SaveCanUpdate_entries,
	sizeof(__CSyncService_SaveCanUpdate_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x349F2EE6,
	"http://Yishion.Sync",
	L"http://Yishion.Sync",
	sizeof("http://Yishion.Sync")-1
};


struct __CSyncService_UpdateInstruction_struct
{
	int id;
	short flag;
	BSTR result;
};

extern __declspec(selectany) const _soapmapentry __CSyncService_UpdateInstruction_entries[] =
{

	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_UpdateInstruction_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x0039C6FA, 
		"flag", 
		L"flag", 
		sizeof("flag")-1, 
		SOAPTYPE_SHORT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_UpdateInstruction_struct, flag),
		NULL,
		NULL,
		-1,
	},
	{
		0x1150D23F, 
		"result", 
		L"result", 
		sizeof("result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSyncService_UpdateInstruction_struct, result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSyncService_UpdateInstruction_map =
{
	0x1F0716F4,
	"UpdateInstruction",
	L"UpdateInstructionResponse",
	sizeof("UpdateInstruction")-1,
	sizeof("UpdateInstructionResponse")-1,
	SOAPMAP_FUNC,
	__CSyncService_UpdateInstruction_entries,
	sizeof(__CSyncService_UpdateInstruction_struct),
	0,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x349F2EE6,
	"http://Yishion.Sync",
	L"http://Yishion.Sync",
	sizeof("http://Yishion.Sync")-1
};


struct __CSyncService_Add_struct
{
	double n1;
	double n2;
	double AddResult;
};

extern __declspec(selectany) const _soapmapentry __CSyncService_Add_entries[] =
{

	{
		0x00000E5F, 
		"n1", 
		L"n1", 
		sizeof("n1")-1, 
		SOAPTYPE_DOUBLE, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_Add_struct, n1),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000E60, 
		"n2", 
		L"n2", 
		sizeof("n2")-1, 
		SOAPTYPE_DOUBLE, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_Add_struct, n2),
		NULL,
		NULL,
		-1,
	},
	{
		0xB5BB52A8, 
		"AddResult", 
		L"AddResult", 
		sizeof("AddResult")-1, 
		SOAPTYPE_DOUBLE, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CSyncService_Add_struct, AddResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSyncService_Add_map =
{
	0x1181BCF8,
	"Add",
	L"AddResponse",
	sizeof("Add")-1,
	sizeof("AddResponse")-1,
	SOAPMAP_FUNC,
	__CSyncService_Add_entries,
	sizeof(__CSyncService_Add_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x349F2EE6,
	"http://Yishion.Sync",
	L"http://Yishion.Sync",
	sizeof("http://Yishion.Sync")-1
};

extern __declspec(selectany) const _soapmap * __CSyncService_funcs[] =
{
	&__CSyncService_GetInstruction_map,
	&__CSyncService_GetCanUpdate_map,
	&__CSyncService_SaveCanUpdate_map,
	&__CSyncService_UpdateInstruction_map,
	&__CSyncService_Add_map,
	NULL
};

template <typename TClient>
inline HRESULT CSyncServiceT<TClient>::GetInstruction(
		int id, 
		BSTR* GetInstructionResult
	)
{
    if ( GetInstructionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CSyncService_GetInstruction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 0);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://Yishion.Sync/GetInstruction\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetInstructionResult = __params.GetInstructionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CSyncServiceT<TClient>::GetCanUpdate(
		int id, 
		BSTR* GetCanUpdateResult
	)
{
    if ( GetCanUpdateResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CSyncService_GetCanUpdate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 1);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://Yishion.Sync/GetCanUpdate\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetCanUpdateResult = __params.GetCanUpdateResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CSyncServiceT<TClient>::SaveCanUpdate(
		int id, 
		bool* SaveCanUpdateResult
	)
{
    if ( SaveCanUpdateResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CSyncService_SaveCanUpdate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 2);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://Yishion.Sync/SaveCanUpdate\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveCanUpdateResult = __params.SaveCanUpdateResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CSyncServiceT<TClient>::UpdateInstruction(
		int id, 
		short flag, 
		BSTR result
	)
{

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CSyncService_UpdateInstruction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.id = id;
	__params.flag = flag;
	__params.result = result;

	__atlsoap_hr = SetClientStruct(&__params, 3);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://Yishion.Sync/UpdateInstruction\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CSyncServiceT<TClient>::Add(
		double n1, 
		double n2, 
		double* AddResult
	)
{
    if ( AddResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CSyncService_Add_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.n1 = n1;
	__params.n2 = n2;

	__atlsoap_hr = SetClientStruct(&__params, 4);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://Yishion.Sync/Add\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*AddResult = __params.AddResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CSyncServiceT<TClient>::GetFunctionMap()
{
	return __CSyncService_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CSyncServiceT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CSyncService_GetInstruction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSyncService_GetInstruction_atlsoapheader_map = 
	{
		0xD0E6F791,
		"GetInstruction",
		L"GetInstructionResponse",
		sizeof("GetInstruction")-1,
		sizeof("GetInstructionResponse")-1,
		SOAPMAP_HEADER,
		__CSyncService_GetInstruction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x349F2EE6,
		"http://Yishion.Sync",
		L"http://Yishion.Sync",
		sizeof("http://Yishion.Sync")-1
	};

	static const _soapmapentry __CSyncService_GetCanUpdate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSyncService_GetCanUpdate_atlsoapheader_map = 
	{
		0x762001E4,
		"GetCanUpdate",
		L"GetCanUpdateResponse",
		sizeof("GetCanUpdate")-1,
		sizeof("GetCanUpdateResponse")-1,
		SOAPMAP_HEADER,
		__CSyncService_GetCanUpdate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x349F2EE6,
		"http://Yishion.Sync",
		L"http://Yishion.Sync",
		sizeof("http://Yishion.Sync")-1
	};

	static const _soapmapentry __CSyncService_SaveCanUpdate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSyncService_SaveCanUpdate_atlsoapheader_map = 
	{
		0x4484B9F3,
		"SaveCanUpdate",
		L"SaveCanUpdateResponse",
		sizeof("SaveCanUpdate")-1,
		sizeof("SaveCanUpdateResponse")-1,
		SOAPMAP_HEADER,
		__CSyncService_SaveCanUpdate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x349F2EE6,
		"http://Yishion.Sync",
		L"http://Yishion.Sync",
		sizeof("http://Yishion.Sync")-1
	};

	static const _soapmapentry __CSyncService_UpdateInstruction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSyncService_UpdateInstruction_atlsoapheader_map = 
	{
		0x1F0716F4,
		"UpdateInstruction",
		L"UpdateInstructionResponse",
		sizeof("UpdateInstruction")-1,
		sizeof("UpdateInstructionResponse")-1,
		SOAPMAP_HEADER,
		__CSyncService_UpdateInstruction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x349F2EE6,
		"http://Yishion.Sync",
		L"http://Yishion.Sync",
		sizeof("http://Yishion.Sync")-1
	};

	static const _soapmapentry __CSyncService_Add_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSyncService_Add_atlsoapheader_map = 
	{
		0x1181BCF8,
		"Add",
		L"AddResponse",
		sizeof("Add")-1,
		sizeof("AddResponse")-1,
		SOAPMAP_HEADER,
		__CSyncService_Add_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x349F2EE6,
		"http://Yishion.Sync",
		L"http://Yishion.Sync",
		sizeof("http://Yishion.Sync")-1
	};


	static const _soapmap * __CSyncService_headers[] =
	{
		&__CSyncService_GetInstruction_atlsoapheader_map,
		&__CSyncService_GetCanUpdate_atlsoapheader_map,
		&__CSyncService_SaveCanUpdate_atlsoapheader_map,
		&__CSyncService_UpdateInstruction_atlsoapheader_map,
		&__CSyncService_Add_atlsoapheader_map,
		NULL
	};
	
	return __CSyncService_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CSyncServiceT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CSyncServiceT<TClient>::GetNamespaceUri()
{
	return L"http://Yishion.Sync";
}

template <typename TClient>
ATL_NOINLINE inline const char * CSyncServiceT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CSyncServiceT<TClient>::GetNamespaceUriA()
{
	return "http://Yishion.Sync";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CSyncServiceT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CSyncServiceT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
{
	if (ppReader == NULL)
	{
		return E_INVALIDARG;
	}
	
	CComPtr<ISAXXMLReader> spReader = GetReader();
	if (spReader.p != NULL)
	{
		*ppReader = spReader.Detach();
		return S_OK;
	}
	return TClient::GetClientReader(ppReader);
}

} // namespace SyncService
