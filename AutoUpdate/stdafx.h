#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200


#ifdef _DEBUG
#   include <miniutil/bkconsole.h>
#   define DEBUG_TRACE bkconsole::printf
#else
#   define DEBUG_TRACE
#endif

#include <string.h>

#undef _ATL_MIN_CRT

#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltrace.h>
#include "Resource.h"

#define _WTL_NO_CSTRING

#include <wtl/atlapp.h>
#include <wtl/atlframe.h>
#include <wtl/atlctrls.h>
#include <wtl/atlctrlx.h>
#include <wtl/atldlgs.h>
#include <wtl/atlcrack.h>
#include <wtl/atlmisc.h>


/* ½çÃæ·â×° */
#include "tinyxml/tinyxml.h"
#include "bkwin/bklistview.h"
#include "bkwin/bklistbox.h"
#include "bkwin/bkwnd.h"
#include "bkwin/bkwnddlg.h"
#include "bkwin/bkwndtabctrl.h"
#include "bkwin/bkwndnotify.h"
#include "bkwin/bkobject.h"
#include "bkwin/bkimage.h"
#include "bkwin/bkcolor.h"
#include "bkwin/bkskin.h"
#include "bkwin/bklistbox.h"
#include "bkwin/bkwndstyle.h"
#include "bkwin/bktheme.h"
#include "bkwin/bkatldefine.h"
#include "bkwin/bkwndcmnctrl.h"
#include "bkwin/bkwndrichtxt.h"
#include "bkwin/bkwndpanel.h"
#include "wtlhelper/whwindow.h"
#include "wtlhelper/whthreadnotify.h"
#include "wtlhelper/iidl.h"
#include "wtlhelper/bkfolderbrowsectrl.h"
#include "wtlhelper/wh3statecheckimglst.h"