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

#include "wm.h"

#include <time.h>
#include "atoms.h"
#include <stdlib.h>

WM * WM::m_instance = 0;
bool WM::x_error = false;


int handleXErrors(Display *disp, XErrorEvent *xerror) {
    cout << "X ERROR: ";
    switch (xerror->error_code){
        case BadAlloc:
            cout << "BadAlloc";
            break;
        case BadDrawable:
            cout << "BadDrawable";
            break;
        case BadFont:
            cout << "BadFont";
            break;
        case BadMatch:
            cout << "BadMatch";
            break;
        case BadPixmap:
            cout << "BadPixmap";
            break;
        case BadValue:
            cout << "BadValue";
            break;
    };

    char errtxt[128];
    XGetErrorText(disp, xerror->error_code, errtxt, 128);
    cout << errtxt;
    /*
    cout  << endl
          << "  X Error of failed request:  " <<  xerror->error_code << " " << errtxt << endl
          << "  Major/minor opcode of failed request:  " << xerror->request_code << " / " << xerror->minor_code
          << "  Resource id in failed request: " << xerror->resourceid << endl;

    */

    cout << endl;
    WM::upErrorFlag();
    return 0;
}


WM::WM()
{

    /*
    if (XInitThreads() == 0){
        cerr << "Could not init Xlib to use multiple threads" << endl;
        exit(0);
    }
    */

    m_dpy = NULL;
    m_dpy = XOpenDisplay(getenv("DISPLAY"));
    if (NULL == m_dpy){
        cout << "X Connection error" << endl;
        exit(0);
    }
    m_srn = DefaultScreen(m_dpy);

#ifdef HAVE_XINERAMA
    if (XineramaIsActive(m_dpy)) {

        XineramaScreenInfo* scr_info;
        int number;

        scr_info = XineramaQueryScreens(m_dpy, &number);
        for (int i = 0; i < number; i++)
            m_heads.push_back(HeadInfo(scr_info[i]));
        XFree(scr_info);
    }
#endif /* HAVE_XINERAMA */

    // no xinerama, so at least one head
    if (m_heads.empty()) {
        m_heads.push_back(HeadInfo());
        m_heads[0].x_org = 0;
        m_heads[0].y_org = 0;
        m_heads[0].width = DisplayWidth(m_dpy, m_srn);
        m_heads[0].height = DisplayHeight(m_dpy, m_srn);
    }

    XSetErrorHandler((XErrorHandler) handleXErrors);


}


WM::~WM()
{
    if (m_dpy != NULL)
        XCloseDisplay(m_dpy);
}


/**
 *
 * @param win
 * @return index of the desktop (workspace) a window in
 */
int WM::getWindowWorkspaceID(Window win) {

    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret, unused;
    unsigned long *data = 0;

    int res = -1;

    if (XGetWindowProperty(m_dpy, win, Atoms::Instance()->netWmDesktop(),
                            0l, 10L, False, XA_CARDINAL, &type_ret,
                            &format_ret, &nitems_ret,
                            &unused,
                            (unsigned char **) &data ) == Success
    ){
        if ( data != None )
            res = static_cast<int>(*data);
        XFree((void *) data);
    }

    return res;

}

/**
 *
 * @param num of the workspace switch to
 */
void WM::changeCurrentWorkspaceID(unsigned long int num){

    XEvent e;
    unsigned long mask;

    e.xclient.type          = ClientMessage;
    e.xclient.window        = XRootWindow(m_dpy,m_srn);
    e.xclient.message_type  = Atoms::Instance()->netCurrentDesktop();
    e.xclient.format        = 32;
    e.xclient.data.l[0]     = (unsigned long) num;
    e.xclient.data.l[1]     = time(NULL);

    mask =  SubstructureRedirectMask;
    XSendEvent(m_dpy, XRootWindow(m_dpy,m_srn), False, mask, &e);

};


void WM::changeWindowWorkspaceID(Window win, int num){

    XEvent e;
    unsigned long mask;

    e.xclient.type          = ClientMessage;
    e.xclient.window        = win;
    e.xclient.message_type  = Atoms::Instance()->netWMDesktop();
    e.xclient.format        = 32;
    e.xclient.data.l[0]     = num;
    e.xclient.data.l[1]     = 2; // from pager

    mask =  SubstructureRedirectMask;
    XSendEvent(m_dpy, XRootWindow(m_dpy,m_srn), False, mask, &e);

};



int WM::getWorkspaceCount(){

    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret, unused;
    unsigned long *data = 0;

    int res = 0;

    if (XGetWindowProperty(m_dpy, RootWindow(m_dpy, m_srn), Atoms::Instance()->netNumberOfDesktops(),
                            0l, 10L, False, 0, &type_ret,
                            &format_ret, &nitems_ret,
                            &unused,
                            (unsigned char **) &data ) == Success
    ){

        if ( data != None ) res =  static_cast<int> (data[0]);
        XFree((void *) data);

    }

    return res;

}

int WM::getCurrentWorkspaceID(){

    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret, unused;
    unsigned long *data = 0;

    int res = -1;

    if (XGetWindowProperty(m_dpy, RootWindow(m_dpy, m_srn), Atoms::Instance()->netCurrentDesktop(),
                            0l, 10L, False, 0, &type_ret,
                            &format_ret, &nitems_ret,
                            &unused,
                            (unsigned char **) &data ) == Success
    ){
        if ( data != None ) res =  static_cast<int> (data[0]);
        XFree((void *) data);
    }

    return res;

}

WindowList WM::clientList(){

    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret, unused;
    unsigned long *data = 0;

    WindowList res;

    if (XGetWindowProperty(m_dpy, RootWindow(m_dpy, m_srn), Atoms::Instance()->netClientListStacking(),
                            0l, 1000, False, XA_WINDOW, &type_ret,
                            &format_ret, &nitems_ret,
                            &unused,
                            (unsigned char **) &data ) == Success
    ){

        if (type_ret == XA_WINDOW && format_ret == 32) {
            Window *wins = (Window *) data;
            for (uint i=0; i < nitems_ret; i++)
                res.push_back(static_cast<Window> (wins[i]));
        }

        XFree((void *) data);

    }

    return res;

}

bool WM::hasWindowAtom(Window win, Atom atom){

    Atom type_ret = atom;
    int format_ret;
    unsigned long nitems_ret, unused;
    unsigned long *data = 0;

    bool res = false;

    if (XGetWindowProperty(m_dpy, win, Atoms::Instance()->netWmState(),
                            0l, 10L, False, 0, &type_ret,
                            &format_ret, &nitems_ret,
                            &unused,
                            (unsigned char **) &data ) == Success
    ){
        if (data != None){
            for (uint i = 0; i < nitems_ret; i++){
                if (data[i] == atom){
                    res = true;
                    break;
                }
            }
        }
        XFree((void *) data);
    }
    return res;

}


