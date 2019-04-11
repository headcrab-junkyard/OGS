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
/// @brief wrapper for HTML functionality

#pragma once

#include "tier1/interface.h"
#include "HTMLMessages.hpp"

constexpr auto CHROMEHTML_CONTROLLER_INTERFACE_VERSION{"ChromeHTML_Controller_001"};

struct HTMLCommandBuffer_t;
struct IHTMLResponses;
class CUtlString;

struct IHTMLChromeController : public IBaseInterface
{
	//virtual ~IHTMLChromeController() = default;
	
	///
	virtual bool Init(const char *asHTMLCacheDir, const char *asCookiePath) = 0;
	
	///
	virtual void Shutdown() = 0;
	
	///
	virtual bool RunFrame() = 0;
	
	/// Set a cookie in the CEF instance
	/// asPath is typically "/"
	/// If anExpires is 0 then it is a session only cookie, otherwise it is saved
	virtual void SetWebCookie(const char *asHostName, const char *asKey, const char *asValue, const char *asPath, RTime32 anExpires = 0) = 0;
	
	///
	virtual void GetWebCookieForURL(CUtlString *asValue, const char *asURL, const char *asName) = 0;
	
	///
	virtual void SetClientBuildID(uint64_t anID) = 0;
	
	///
	virtual bool BHasPendingMessages() = 0;
	
	///
	virtual void CreateBrowser(IHTMLResponses *apBrowser, bool abPopupWindow, const char *asUserAgentIdentifier) = 0;
	
	///
	virtual void RemoveBrowser(IHTMLResponses *apBrowser) = 0;
	
	///
	virtual void WakeThread() = 0;
	
	///
	virtual HTMLCommandBuffer_t *GetFreeCommandBuffer(EHTMLCommands aeCmd, int anBrowser) = 0;
	
	///
	virtual void PushCommand(HTMLCommandBuffer_t *apCommand) = 0;
	
#ifdef DBGFLAG_VALIDATE
	///
	virtual void Validate(CValidator &aValidator, const char *asName) = 0;
	
	///
	virtual bool ChromePrepareForValidate() = 0;
	
	///
	virtual bool ChromeResumeFromValidate() = 0;
#endif // DBGFLAG_VALIDATE

	///
	virtual void SetCefThreadTargetFrameRate(uint32_t anFPS) = 0;
};