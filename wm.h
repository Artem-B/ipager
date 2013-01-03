/***************************************************************************
 *   Copyright (C) 2005 by Suhanov Vadim                                   *
 *   suhanov_vadim@mail.ru                                                 *
 *                                                                         *
 * Permission is hereby granted, free of charge, to any person obtaining   *
 * a copy of this software and associated documentation files              *
 * (the "Software"), to deal in the Software without restriction,          *
 * including without limitation the rights to use, copy, modify, merge,    *
 * publish, distribute, sublicense, and/or sell copies of the Software,    *
 * and to permit persons to whom the Software is furnished to do so,       *
 * subject to the following conditions:                                    *
 *                                                                         *
 * The above copyright notice and this permission notice shall be included *
 * in all copies or substantial portions of the Software.                  *
 *                                                                         *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,         *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF      *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY    *
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,    *
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH           *
 * THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.              *
 *                                                                         *
 ***************************************************************************/

#ifndef WM_H
#define WM_H

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#ifdef HAVE_XINERAMA
#include <X11/extensions/Xinerama.h>
typedef XineramaScreenInfo HeadInfo;
#else
typedef struct {
   int   screen_number;
   short x_org;
   short y_org;
   short width;
   short height;
} HeadInfo;
#endif /* HAVE_XINERAMA */
}

#include <iostream>
#include <list>
#include <vector>
using namespace std;

typedef list<Window> WindowList;
typedef vector<HeadInfo> HeadList;

/**
@author vad
*/
class WM{
    static WM  * m_instance;
    WM();

    Display * m_dpy;
    int       m_srn;

    HeadList  m_heads;

    static bool x_error;

public:

    static
    WM * Instance(){
        if ( NULL == m_instance ) m_instance = new WM;
        return m_instance;
    };
    ~WM();

    Display * display(){ return m_dpy; };
    int       screen(){  return m_srn; };

    int       getHeadCount() { return m_heads.size(); }

    int    getWindowWorkspaceID(Window);
    int    getWorkspaceCount();
    int    getCurrentWorkspaceID();
    void   changeCurrentWorkspaceID(unsigned long int);
    void   changeWindowWorkspaceID(Window, int);

    WindowList   clientList();
    HeadList&    headList() { return m_heads; }

    bool         hasWindowAtom(Window win, Atom atom);

    static void upErrorFlag()  { x_error = true;  }
    static void dropErrorFlag(){ x_error = false; }
    static bool getErrorFlag() { return x_error;  }
    static bool getErrorFlagAndDropIt(){
        if (!x_error)
            return x_error;
        x_error = false;
        return true;
    };

};

#endif
