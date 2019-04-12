/*
 * This file is part of OGS Engine
 * Copyright (C) 2019 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

// libcef imports
/*

cef_build_revision
cef_post_data_create
cef_post_data_element_create
cef_post_task
cef_request_create

cef_string_list_alloc
cef_string_list_append
cef_string_list_copy
cef_string_list_free
cef_string_list_size
cef_string_list_value

cef_string_map_alloc
cef_string_map_append
cef_string_map_free
cef_string_map_key
cef_string_map_size
cef_string_map_value

cef_string_multimap_alloc
cef_string_multimap_append
cef_string_multimap_free
cef_string_multimap_key
cef_string_multimap_size
cef_string_multimap_value

cef_string_userfree_utf8_free

cef_string_utf8_clear
cef_string_utf8_cmp
cef_string_utf8_set
cef_string_utf8_to_wide

cef_string_wide_clear
cef_string_wide_to_utf8
*/

#include "chromewrapper.hpp"

EXPOSE_SINGLE_INTERFACE(CHTMLChromeController, IHTMLChromeController, CHROMEHTML_CONTROLLER_INTERFACE_VERSION)

CHTMLChromeController::CHTMLChromeController() = default;
CHTMLChromeController::~CHTMLChromeController() = default;

bool CHTMLChromeController::Init(const char *asHTMLCacheDir, const char *asCookiePath)
{
	cef_initialize();
	return true;
};

void CHTMLChromeController::Shutdown()
{
	cef_shutdown();
};

bool CHTMLChromeController::RunFrame()
{
	cef_do_message_loop_work();
	return true;
};

void CHTMLChromeController::SetWebCookie(const char *asHostName, const char *asKey, const char *asValue, const char *asPath, RTime32 anExpires)
{
	cef_set_cookie();
};

void CHTMLChromeController::GetWebCookieForURL(CUtlString *asValue, const char *asURL, const char *asName)
{
	cef_visit_url_cookies();
};

void CHTMLChromeController::SetClientBuildID(uint64_t anID)
{
};

bool CHTMLChromeController::BHasPendingMessages()
{
	return false;
};

void CHTMLChromeController::CreateBrowser(IHTMLResponses *apBrowser, bool abPopupWindow, const char *asUserAgentIdentifier)
{
	cef_browser_host_create_browser_sync();
};

void CHTMLChromeController::RemoveBrowser(IHTMLResponses *apBrowser)
{
};

void CHTMLChromeController::WakeThread()
{
};

HTMLCommandBuffer_t *CHTMLChromeController::GetFreeCommandBuffer(EHTMLCommands aeCmd, int anBrowser)
{
	return nullptr;
};

void CHTMLChromeController::PushCommand(HTMLCommandBuffer_t *apCommand)
{
};

#ifdef DBGFLAG_VALIDATE
void CHTMLChromeController::Validate(CValidator &aValidator, const char *asName)
{
};

bool CHTMLChromeController::ChromePrepareForValidate()
{
	return true;
};

bool CHTMLChromeController::ChromeResumeFromValidate()
{
	return true;
};
#endif // DBGFLAG_VALIDATE

void CHTMLChromeController::SetCefThreadTargetFrameRate(uint32_t anFPS)
{
};