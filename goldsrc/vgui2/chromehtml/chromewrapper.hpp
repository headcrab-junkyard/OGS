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

#pragma once

#include "html/IChromeHTMLWrapper.hpp"

class CHTMLChromeController : public IHTMLChromeController
{
public:
	CHTMLChromeController();
	~CHTMLChromeController();
	
	bool Init(const char *asHTMLCacheDir, const char *asCookiePath) override;
	void Shutdown() override;
	
	bool RunFrame() override;
	
	void SetWebCookie(const char *asHostName, const char *asKey, const char *asValue, const char *asPath, RTime32 anExpires) override;
	void GetWebCookieForURL(CUtlString *asValue, const char *asURL, const char *asName) override;
	
	void SetClientBuildID(uint64_t anID) override;
	
	bool BHasPendingMessages() override;
	
	void CreateBrowser(IHTMLResponses *apBrowser, bool abPopupWindow, const char *asUserAgentIdentifier) override;
	void RemoveBrowser(IHTMLResponses *apBrowser) override;
	
	void WakeThread() override;
	
	HTMLCommandBuffer_t *GetFreeCommandBuffer(EHTMLCommands aeCmd, int anBrowser) override;
	void PushCommand(HTMLCommandBuffer_t *apCommand) override;
	
#ifdef DBGFLAG_VALIDATE
	void Validate(CValidator &aValidator, const char *asName) override;
	
	bool ChromePrepareForValidate() override;
	bool ChromeResumeFromValidate() override;
#endif // DBGFLAG_VALIDATE

	void SetCefThreadTargetFrameRate(uint32_t anFPS) override;
};