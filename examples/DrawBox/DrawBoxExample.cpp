//-----------------------------------------------------------------------------
//@file  
//	DrawBoxExample.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the DrawBoxExample class
//
//  Example user module of a graphical interface with mouse interactions.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.org/support/ 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015 Sensomusic
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
//     copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "DrawBoxExample.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DrawBoxExample ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule)
{
    // cast is important to call the good destructor
	delete ((DrawBoxExample*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "draw box example";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "draw box sdk module example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void DrawBoxExample::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->NumberOfParams     = 2;
	pModuleInfo->DefaultWidth		= DEFAULT_MODULEHEIGHT;
	pModuleInfo->DefaultHeight		= DEFAULT_MODULEHEIGHT;
    pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->DontProcess		= TRUE;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = TRUE;

}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------
// query system not used

//-----------------------------------------------------------------------------
// initialisation
void DrawBoxExample::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	colorBox		= sdkGetUsineColor(clCursor);

	mouseLeftButtonDown       = FALSE;
	eraseBox();

    //sdkSetEvtNbLines (arrPointsCoordsOut, ARRAY_OUT_LINES);

}

//-----------------------------------------------------------------------------
// Parameters description
void DrawBoxExample::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{	
	switch (ParamIndex) 
	{
	// Erase trajectory button input
	case 0:
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "reset";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
		pParamInfo->IsSeparator		= TRUE;
		pParamInfo->EventPtr        = &btnErase;
		break;
	// btnRandomize
	case 1:
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "randomize";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->EventPtr        = &btnRandomize;
		break;
	default:
		// do nothing
		break;
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void DrawBoxExample::onCallBack (UsineMessage *Message) 
{
	try
	{
        // filter only message specific to this user module and type MS_CHANGE
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK && Message->lParam == MSG_CHANGE)
        {
	        // WParam contains the Param Number
	        switch (Message->wParam) 
	        {
	        // Erase trajectory button input
	        case 0:
		        eraseBox();
		        break;

	        // randomize button input
	        case 1:
                onRandomize ();
		        break;

	        // default case
	        default:
		        // do nothing
		        break;
	        }
        }
	}
	catch (...)
	{
		//sdkTraceErrorChar("error");
	}
}

//----------------------------------------------------------------------------
// layout
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// create user defined layout options in the 'edit layout' panel
void DrawBoxExample::onCreateSettings()
{
	// we had our user section in the edit layout panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "box");
	sdkAddSettingLineColor( DESIGN_TAB_NAME, &colorBox, "box color" );
}

//-----------------------------------------------------------------------------
// Callback which indicate that the user has changed someting in the layout
// You'll probably need to redreaw
void DrawBoxExample::onSettingsHasChanged()
{
	// Ask to repaint the module
	sdkRepaintPanel();
} 


void DrawBoxExample::onCreateCommands()
{
    sdkAddCommandSeparator("cmd_edit");
    sdkAddCommand("cmd_reset", 0);
    sdkAddCommand("cmd_randomize", 1);
}

//-----------------------------------------------------------------------------
// paint the module panel
void DrawBoxExample::onPaint ()
{
    // we check the box top value to see if there is something to draw 
    if (boxCoords.top >= 0)
    {
        sdkFillRect (boxCoords, colorBox, 0, colorBox, 0);
    }
	
}

//-----------------------------------------------------------------------------
// mouse and multi touch
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MouseMove callback
void DrawBoxExample::onMouseMove (TShiftState Shift, float X, float Y)
{
    // we only want to draw when the left mouse button is down
	if (mouseLeftButtonDown)
	{
		updateBox (std::min(1.0f, std::max(X, 0.0f)), std::min(1.0f, std::max(Y, 0.0f)));
	}	

    // collecte mouse pos in case of zoom
    lastMousePos.x = X;
    lastMousePos.y = Y;
}

//-----------------------------------------------------------------------------
// MouseDown callback
void DrawBoxExample::onMouseDown (TMouseButton Button, TShiftState Shift, float X,float Y)
{
	if (Button == mbLeft)
	{
        float left = std::min(1.0f, std::max(X, 0.0f));
        float top = std::min(1.0f, std::max(Y, 0.0f)) ;   
        boxCoords.left = left;
        boxCoords.top = top;
        boxCoords.bottom = top;
        boxCoords.right = left;

		// we store the state of the left button
		mouseLeftButtonDown = TRUE;
	}
}

//-----------------------------------------------------------------------------
// MouseUp callback
void DrawBoxExample::onMouseUp (TMouseButton Button, TShiftState Shift, float X, float Y)
{
	mouseLeftButtonDown = FALSE;
}

//-----------------------------------------------------------------------------
// MouseWheel callback
void DrawBoxExample::onMouseWheel (TShiftState Shift, int WheelDelta)
{
    WheelDelta > 0 ? boxScale = 1.1f : boxScale = 0.9f;

    // apply the scale
    boxCoords.left      = (boxCoords.left - lastMousePos.x) * boxScale + lastMousePos.x; 
    boxCoords.top       = (boxCoords.top - lastMousePos.y) * boxScale + lastMousePos.y; 
    boxCoords.bottom    = (boxCoords.bottom - lastMousePos.y) * boxScale + lastMousePos.y;  
    boxCoords.right     = (boxCoords.right - lastMousePos.x) * boxScale + lastMousePos.x; 
    
    // limit to canvas bounds
    boxCoords.left      = std::min(1.0f, std::max(boxCoords.left, 0.0f)); 
    boxCoords.top       = std::min(1.0f, std::max(boxCoords.top, 0.0f)); 
    boxCoords.bottom    = std::min(1.0f, std::max(boxCoords.bottom, 0.0f));  
    boxCoords.right     = std::min(1.0f, std::max(boxCoords.right, 0.0f)); 
}

//-----------------------------------------------------------------------------
// usine randomize
void DrawBoxExample::onRandomize ()
{
    boxCoords.left = (float)(rand() / (float)(RAND_MAX));
    boxCoords.right = (float)(rand() / (float)(RAND_MAX));
    boxCoords.top = (float)(rand() / (float)(RAND_MAX));
    boxCoords.bottom = (float)(rand() / (float)(RAND_MAX));
    
	// Ask to repaint the module
	sdkRepaintPanel();
}

//-----------------------------------------------------------------------------
// Multi touch callbacks not used

//-------------------------------------------------------------------------
// private methodes implementation
//-----------------------------------------------------------------------
//-------------------------------------------------------------------------
void DrawBoxExample::eraseBox()
{
    // we put all box values to -1
    // in the paint methode, we'll check against the top value if it's negative
    boxCoords.left = -1;
    boxCoords.top = -1;
    boxCoords.bottom = -1;
    boxCoords.right = -1;
    
    // Ask to repaint the module
    sdkRepaintPanel();
}

//-------------------------------------------------------------------------
void DrawBoxExample::updateBox(float x, float y)
{
    boxCoords.bottom = y;
    boxCoords.right = x;
    
    // Ask to repaint the module
    sdkRepaintPanel();

}