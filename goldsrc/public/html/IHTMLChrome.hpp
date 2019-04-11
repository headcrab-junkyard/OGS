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

#include "tier1/utlbuffer.h"
#include "HTMLMessages.hpp"

// Prototypes for all the protobuf messages, so we don't need to include all of protobuf
class CMsgBrowserReady;
class CMsgNeedsPaint;
class CMsgStartRequest;
class CMsgURLChanged;
class CMsgFinishedRequest;
class CMsgShowPopup;
class CMsgHidePopup;
class CMsgOpenNewTab;
class CMsgPopupHTMLWindow;
class CMsgSetHTMLTitle;
class CMsgLoadingResource;
class CMsgStatusText;
class CMsgSetCursor;
class CMsgFileLoadDialog;
class CMsgShowToolTip;
class CMsgUpdateToolTip;
class CMsgHideToolTip;
class CMsgSearchResults;
class CMsgClose;
class CMsgHorizontalScrollBarSizeResponse;
class CMsgVerticalScrollBarSizeResponse;
class CMsgGetZoomResponse;
class CMsgLinkAtPositionResponse;
class CMsgZoomToElementAtPositionResponse;
class CMsgJSAlert;
class CMsgJSConfirm;
class CMsgCanGoBackAndForward;
class CMsgOpenSteamURL;
class CMsgSizePopup;
class CMsgScalePageToValueResponse;
class CMsgRequestFullScreen;
class CMsgExitFullScreen;
class CMsgGetCookiesForURLResponse;
class CMsgNodeHasFocus;
class CMsgSavePageToJPEGResponse;
class CMsgFocusedNodeTextResponse;

/// A single IPC packet for the html thread (in and out)
struct HTMLCommandBuffer_t
{
	HTMLCommands meCmd;
	int mnBrowser;
	CUtlBuffer mBuffer;
	
#ifdef DBGFLAG_VALIDATE
	virtual void Validate(CValidator &aValidator, const tchar *asName)
	{
		VALIDATE_SCOPE();
		ValidateObj(mBuffer);
	};
#endif // DBGFLAG_VALIDATE
};

/// Callback interfaces for messages from the html thread
struct IHTMLResponses
{
	virtual ~IHTMLResponses() = default;
	
	///
	virtual void BrowserSetIndex(int anIndex) = 0;
	
	///
	virtual int BrowserGetIndex() = 0;
	
	///
	virtual void BrowserReady(const CMsgBrowserReady *apCmd) = 0;
	
	///
	virtual void BrowserNeedsPaint(const CMsgNeedsPaint *apCmd) = 0;
	
	///
	virtual void BrowserStartRequest(const CMsgStartRequest *apCmd) = 0;
	
	///
	virtual void BrowserURLChanged(const CMsgURLChanged *apCmd) = 0;
	
	///
	virtual void BrowserFinishedRequest(const CMsgFinishedRequest *apCmd) = 0;
	
	///
	virtual void BrowserShowPopup(const CMsgShowPopup *apCmd) = 0;
	
	///
	virtual void BrowserHidePopup(const CMsgHidePopup *apCmd) = 0;
	
	///
	virtual void BrowserOpenNewTab(const CMsgOpenNewTab *apCmd) = 0;
	
	///
	virtual void BrowserPopupHTMLWindow(const CMsgPopupHTMLWindow *apCmd) = 0;
	
	///
	virtual void BrowserSetHTMLTitle(const CMsgSetHTMLTitle *apCmd) = 0;
	
	///
	virtual void BrowserLoadingResource(const CMsgLoadingResource *apCmd) = 0;
	
	///
	virtual void BrowserStatusText(const CMsgStatusText *apCmd) = 0;
	
	///
	virtual void BrowserSetCursor(const CMsgSetCursor *apCmd) = 0;
	
	///
	virtual void BrowserFileLoadDialog(const CMsgFileLoadDialog *apCmd) = 0;
	
	///
	virtual void BrowserShowToolTip(const CMsgShowToolTip *apCmd) = 0;
	
	///
	virtual void BrowserUpdateToolTip(const CMsgUpdateToolTip *apCmd) = 0;
	
	///
	virtual void BrowserHideToolTip(const CMsgHideToolTip *apCmd) = 0;
	
	///
	virtual void BrowserSearchResults(const CMsgSearchResults *apCmd) = 0;
	
	///
	virtual void BrowserClose(const CMsgClose *apCmd) = 0;
	
	///
	virtual void BrowserHorizontalScrollBarSizeResponse(const CMsgHorizontalScrollBarSizeResponse *apCmd) = 0;
	
	///
	virtual void BrowserVerticalScrollBarSizeResponse(const CMsgVerticalScrollBarSizeResponse *apCmd) = 0;
	
	///
	virtual void BrowserGetZoomResponse(const CMsgGetZoomResponse *apCmd) = 0;
	
	///
	virtual void BrowserLinkAtPositionResponse(const CMsgLinkAtPositionResponse *apCmd) = 0;
	
	///
	virtual void BrowserZoomToElementAtPositionResponse(const CMsgZoomToElementAtPositionResponse *apCmd) = 0;
	
	///
	virtual void BrowserJSAlert(const CMsgJSAlert *apCmd) = 0;
	
	///
	virtual void BrowserJSConfirm(const CMsgJSConfirm *apCmd) = 0;
	
	///
	virtual void BrowserCanGoBackandForward(const CMsgCanGoBackAndForward *apCmd) = 0;
	
	///
	virtual void BrowserOpenSteamURL(const CMsgOpenSteamURL *apCmd) = 0;
	
	///
	virtual void BrowserSizePopup(const CMsgSizePopup *apCmd) = 0;
	
	///
	virtual void BrowserScalePageToValueResponse(const CMsgScalePageToValueResponse *apCmd) = 0;
	
	///
	virtual void BrowserRequestFullScreen(const CMsgRequestFullScreen *apCmd) = 0;
	
	///
	virtual void BrowserExitFullScreen(const CMsgExitFullScreen *apCmd) = 0;
	
	///
	virtual void BrowserGetCookiesForURLResponse(const CMsgGetCookiesForURLResponse *apCmd) = 0;
	
	///
	virtual void BrowserNodeGotFocus(const CMsgNodeHasFocus *apCmd) = 0;
	
	///
	virtual void BrowserSavePageToJPEGResponse(const CMsgSavePageToJPEGResponse *apCmd) = 0;
	
	///
	virtual void BrowserFocusedNodeValueResponse(const CMsgFocusedNodeTextResponse *apCmd) = 0;
};