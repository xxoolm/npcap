/***********************IMPORTANT NPCAP LICENSE TERMS***********************
 *
 * Npcap (https://npcap.com) is a Windows packet sniffing driver and library and
 * is copyright (c) 2013-2025 by Nmap Software LLC ("The Nmap Project").  All
 * rights reserved.
 *
 * Even though Npcap source code is publicly available for review, it is not
 * open source software and may not be redistributed or used in other software
 * without special permission from the Nmap Project. The standard (free) version
 * is usually limited to installation on five systems. For more details, see the
 * LICENSE file included with Npcap and also available at
 * https://github.com/nmap/npcap/blob/master/LICENSE. This header file
 * summarizes a few important aspects of the Npcap license, but is not a
 * substitute for that full Npcap license agreement.
 *
 * We fund the Npcap project by selling two types of commercial licenses to a
 * special Npcap OEM edition:
 *
 * 1) The Npcap OEM Redistribution License allows companies distribute Npcap OEM
 * within their products. Licensees generally use the Npcap OEM silent
 * installer, ensuring a seamless experience for end users. Licensees may choose
 * between a perpetual unlimited license or a quarterly term license, along with
 * options for commercial support and updates. Prices and details:
 * https://npcap.com/oem/redist.html
 *
 * 2) The Npcap OEM Internal-Use License is for organizations that wish to use
 * Npcap OEM internally, without redistribution outside their organization. This
 * allows them to bypass the 5-system usage cap of the Npcap free edition. It
 * includes commercial support and update options, and provides the extra Npcap
 * OEM features such as the silent installer for automated deployment. Prices
 * and details: https://npcap.com/oem/internal.html
 *
 * Both of these licenses include updates and support as well as a warranty.
 * Npcap OEM also includes a silent installer for unattended installation.
 * Further details about Npcap OEM are available from https://npcap.com/oem/,
 * and you are also welcome to contact us at sales@nmap.com to ask any questions
 * or set up a license for your organization.
 *
 * Free and open source software producers are also welcome to contact us for
 * redistribution requests. However, we normally recommend that such authors
 * instead ask your users to download and install Npcap themselves. It will be
 * free for them if they need 5 or fewer copies.
 *
 * If the Nmap Project (directly or through one of our commercial licensing
 * customers) has granted you additional rights to Npcap or Npcap OEM, those
 * additional rights take precedence where they conflict with the terms of the
 * license agreement.
 *
 * Since the Npcap source code is available for download and review, users
 * sometimes contribute code patches to fix bugs or add new features. By sending
 * these changes to the Nmap Project (including through direct email or our
 * mailing lists or submitting pull requests through our source code
 * repository), it is understood unless you specify otherwise that you are
 * offering the Nmap Project the unlimited, non-exclusive right to reuse,
 * modify, and relicense your code contribution so that we may (but are not
 * obligated to) incorporate it into Npcap. If you wish to specify special
 * license conditions or restrictions on your contributions, just say so when
 * you send them.
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. Warranty rights and commercial support are
 * available for the OEM Edition described above.
 *
 * Other copyright notices and attribution may appear below this license header.
 * We have kept those for attribution purposes, but any license terms granted by
 * those notices apply only to their original work, and not to any changes made
 * by the Nmap Project or to this entire file.
 *
 ***************************************************************************/
// ProtInstall.cpp : Defines the entry point for the console application.
//

#include "netcfgapi.h"
#include "ProtInstall.h"

#include "debug.h"

// Copyright And Configuration Management ----------------------------------
//
//  			 NDISPROT Software Installer - ProtInstall.cpp
//
//  				Companion Sample Code for the Article
//
//  			  "Installing NDIS Protocols Programatically"
//  				   Published on http://www.ndis.com
//
//   Copyright (c) 2004-2006 Printing Communications Associates, Inc. (PCAUSA)
//  						http://www.pcausa.com
//
// GPL software is an abomination. Far from being free, it is available ONLY
// to members of the "GPL Club". If you don't want to join the club, then GPL
// software is poison.
//
// This software IS free software under the terms of a BSD-style license:
//
// The right to use this code in your own derivative works is granted so long
// as 1.) your own derivative works include significant modifications of your
// own, 2.) you retain the above copyright notices and this paragraph in its
// entirety within sources derived from this code.
//
// This product includes software developed by PCAUSA. The name of PCAUSA
// may not be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// End ---------------------------------------------------------------------

BOOLEAN bVerbose = TRUE;

BOOLEAN bWiFiService = FALSE;

//
// Function:  ErrMsg
//
// Purpose:   Insert text for each network component type.
//
// Arguments:
//    hr  [in]  Error code.
//
// Returns:   None.
//
// Notes:
//
VOID ErrMsg(HRESULT hr, LPCTSTR  lpFmt, ...)
{
#define ERRMSG_BUF_LEN 400
	LPTSTR lpSysMsg;
	TCHAR buf[ERRMSG_BUF_LEN];
	ULONG offset;
	va_list vArgList; 

	if (hr != 0)
	{
		_stprintf_s(buf, ERRMSG_BUF_LEN, _T("Error %#lx: "), hr);
	}
	else
	{
		buf[0] = 0;
	}

	offset = (ULONG) _tcslen(buf);

	va_start(vArgList, lpFmt);

	_vstprintf_s(buf + offset, ERRMSG_BUF_LEN - offset, lpFmt, vArgList);

	va_end(vArgList);

	if (hr != 0)
	{
		const DWORD fm = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpSysMsg, 0, NULL);

		if (fm > 0 && lpSysMsg)
		{
			offset = (ULONG) _tcslen(buf);

			_stprintf_s(buf + offset, ERRMSG_BUF_LEN - offset, _T("\n\nPossible cause:\n\n"));

			offset = (ULONG) _tcslen(buf);

			_tcscat_s(buf + offset, ERRMSG_BUF_LEN - offset, lpSysMsg);

			LocalFree((HLOCAL)lpSysMsg);
		}
	}

	//_tprintf( buf );
	TRACE_PRINT1("%s", buf);

	return;
}

DWORD GetServiceInfFilePath(LPTSTR lpFilename, DWORD nSize)
{
	// Get Path to This Module
	DWORD nResult;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];

	TRACE_ENTER();

	nResult = GetModuleFileName(NULL, lpFilename, nSize);

	if (nResult == 0)
	{
		return 0;
	}

	_tsplitpath_s(lpFilename, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, NULL, 0, NULL, 0);

	_tmakepath_s(lpFilename, nSize, szDrive, szDir, NDISLWF_SERVICE_INF_FILE, _T(".inf"));
	TRACE_PRINT1("lpFilename = %s", lpFilename);

	TRACE_EXIT();

	return (DWORD)_tcslen(lpFilename);
}

DWORD GetWFPCalloutInfFilePath(LPTSTR lpFilename, DWORD nSize)
{
	// Get Path to This Module
	DWORD nResult;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];

	TRACE_ENTER();

	nResult = GetModuleFileName(NULL, lpFilename, nSize);

	if (nResult == 0)
	{
		return 0;
	}

	_tsplitpath_s(lpFilename, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, NULL, 0, NULL, 0);

	_tmakepath_s(lpFilename, nSize, szDrive, szDir, WFP_CALLOUT_INF_FILE, _T(".inf"));
	TRACE_PRINT1("lpFilename = %s", lpFilename);

	TRACE_EXIT();

	return (DWORD)_tcslen(lpFilename);
}

//
// Function:  InstallSpecifiedComponent
//
// Purpose:   Install a network component from an INF file.
//
// Arguments:
//    lpszInfFile [in]  INF file.
//    lpszPnpID   [in]  PnpID of the network component to install.
//    lpszAppName [in]  Application name.
//    pguidClass  [in]  Class GUID of the network component.
//
// Returns:   None.
//
// Notes:
//

HRESULT InstallSpecifiedComponent(LPCTSTR lpszInfFile, LPCTSTR lpszAppName, const GUID* pguidClass)
{
	INetCfg* pnc;
	LPTSTR lpszApp;
	HRESULT hr;

	TRACE_ENTER();

	hr = HrGetINetCfg(TRUE, lpszAppName, &pnc, &lpszApp);

	if (hr == S_OK)
	{
		//
		// Install the network component.
		//
		hr = HrInstallNetComponent(pnc, pguidClass, lpszInfFile);

		if ((hr == S_OK) || (hr == NETCFG_S_REBOOT))
		{
			// hr = pnc->Apply();
		}
		else
		{
			if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
			{
				ErrMsg(hr, _T("Couldn't install the network component."));
			}
		}

		HrReleaseINetCfg(pnc, TRUE);
	}
	else
	{
		if ((hr == NETCFG_E_NO_WRITE_LOCK) && lpszApp)
		{
			ErrMsg(hr, _T("%s currently holds the lock, try later."), lpszApp);

			CoTaskMemFree(lpszApp);
		}
		else
		{
			ErrMsg(hr, _T("Couldn't the get notify object interface."));
		}
	}

	TRACE_EXIT();
	return hr;
}

BOOL InstallDriver()
{
	TCHAR szFileFullPath[_MAX_PATH];
	HRESULT hr = S_OK;

	TRACE_ENTER();

	do
	{
		// Get Path to Service INF File
		// ----------------------------
		// The INF file is assumed to be in the same folder as this application...

		if (!(BOOL) GetServiceInfFilePath(szFileFullPath, MAX_PATH))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			TRACE_PRINT("Unable to get INF file path");
			break;
		}

		hr = InstallSpecifiedComponent(szFileFullPath, APP_NAME, &GUID_DEVCLASS_NETSERVICE);

		if (hr != S_OK)
		{
			ErrMsg(hr, _T("InstallSpecifiedComponent\n"));
			break;
		}

	} while (FALSE);

	TRACE_EXIT();
	SetLastError(hr);
	return SUCCEEDED(hr);
}

BOOL UninstallDriver()
{
	BOOL bSucceed = TRUE;
	INetCfg* pnc;
	LPTSTR lpszApp;
	HRESULT hr;

	TRACE_ENTER();

	hr = HrGetINetCfg(TRUE, APP_NAME, &pnc, &lpszApp);

	if (hr == S_OK)
	{
		TRACE_PRINT1("bWiFiService = %d.", bWiFiService);
		TRACE_PRINT1("HrUninstallNetComponent: executing, szComponentId = %s.", NDISLWF_SERVICE_PNP_DEVICE_ID);
		hr = HrUninstallNetComponent(pnc, NDISLWF_SERVICE_PNP_DEVICE_ID);

		if (hr != S_OK)
		{
			if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
			{
				bSucceed = FALSE;
				ErrMsg(hr, _T("Couldn't uninstall the network component."));
			}
		}

		if (bWiFiService)
		{
			TRACE_PRINT1("HrUninstallNetComponent: executing, szComponentId = %s.", NDISLWF_SERVICE_PNP_DEVICE_ID_WIFI);
			hr = HrUninstallNetComponent(pnc, NDISLWF_SERVICE_PNP_DEVICE_ID_WIFI);

			if (hr != S_OK)
			{
				if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
				{
					bSucceed = FALSE;
					ErrMsg(hr, _T("Couldn't uninstall the network component."));
				}
			}
		}

		HrReleaseINetCfg(pnc, TRUE);
	}
	else
	{
		bSucceed = FALSE;
		if ((hr == NETCFG_E_NO_WRITE_LOCK) && lpszApp)
		{
			ErrMsg(hr, _T("%s currently holds the lock, try later."), lpszApp);

			CoTaskMemFree(lpszApp);
		}
		else
		{
			ErrMsg(hr, _T("Couldn't get the notify object interface."));
		}
	}

	TRACE_EXIT();
	return bSucceed;
}

BOOL RenableBindings()
{
	CComPtr<INetCfg> netcfg;
	CComPtr<INetCfgLock> lock;
	HRESULT hr;

	TRACE_ENTER();

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		TRACE_PRINT1("CoInitializeEx: 0x%08x\n", hr);
		TRACE_EXIT();
		return 1;
	}

	BOOL bSucceed = ConnectToNetCfg(NDISLWF_SERVICE_PNP_DEVICE_ID, APP_NAME);
	if (!bSucceed)
	{
		TRACE_PRINT1("ConnectToNetCfg: error, PNP Device ID = %s.", NDISLWF_SERVICE_PNP_DEVICE_ID);
	}

	if (bWiFiService)
	{
		bSucceed = ConnectToNetCfg(NDISLWF_SERVICE_PNP_DEVICE_ID_WIFI, APP_NAME);
		if (!bSucceed)
		{
			TRACE_PRINT1("ConnectToNetCfg: error, PNP Device ID = %s.", NDISLWF_SERVICE_PNP_DEVICE_ID_WIFI);
		}
	}

	CoUninitialize();

	TRACE_EXIT();
	return bSucceed;
}
