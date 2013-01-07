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


#include "pager.h"

#include <time.h>
#include "atoms.h"
#include "notifier.h"
#include "icolor.h"

#include <sstream>
#include <algorithm>
using namespace std;

#include <X11/Xutil.h>

#ifndef ulong 
typedef unsigned long ulong; 
#endif 
 


Pager::Pager():
    m_background(NULL),
    m_buffer_icons(NULL),
    selected_window(0)
{

    m_wm      = WM::Instance();
    m_config  = IConfig::Instance();

    // FIXME: mathias gumz <akira at fluxbox dot org> :
    // this is quite hacky, but no better solution yet in my mind.
    // problem is mostly that config->*values* are used not as a reference for
    // initialising but for real-calculating .. which might be not so
    // clever... one has to think about this in more detail some day.
    m_config->setIconMinWidth(m_config->getIconMinWidth() * m_wm->getHeadCount());
    m_config->setIconMaxWidth(m_config->getIconMaxWidth() * m_wm->getHeadCount());

    m_dpy     = m_wm->display();

    m_current_workspace = m_wm->getCurrentWorkspaceID();
    m_workspaces        = m_wm->getWorkspaceCount();




    /* calc window size */
    m_window_width = (m_config->getIconSpacing()  + m_config->getIconMinWidth()) * m_workspaces +
                     (m_config->getIconMaxWidth() - m_config->getIconMinWidth()) + m_config->getIconSpacing();
    m_window_height = m_config->getIconMaxHeight();
    initWindow();

    /**
    *    Cause at least KDE ignores _NET_WM_STATE_STICKY
    *    window property for windows with defined workspace ID.
    */
    m_wm->changeWindowWorkspaceID(m_win, -1);


    /* Imlib2 stuff */
    m_vis   = DefaultVisual(m_dpy, DefaultScreen(m_dpy));
    m_depth = DefaultDepth(m_dpy, DefaultScreen(m_dpy));
    m_cm    = DefaultColormap(m_dpy, DefaultScreen(m_dpy));

    /* set our cache to 2 Mb so it doesn't have to go hit the disk as long as */
    /* the images we use use less than 2Mb of RAM (that is uncompressed) */
    imlib_set_cache_size(2048 * 1024);
    /* set the maximum number of colors to allocate for 8bpp and less to 128 */
    imlib_set_color_usage(128);
    /* dither for depths < 24bpp */
    imlib_context_set_dither(1);
    /* set the dpylay , visual, colormap and drawable we are using */
    imlib_context_set_display(m_dpy);
    imlib_context_set_visual(m_vis);
    imlib_context_set_colormap(m_cm);
    imlib_context_set_drawable(m_win);
    imlib_set_font_cache_size(512 * 1024);


    /* load font */
    m_font = NULL;
    if (m_config->getFontName().size()){
        if (m_config->getFontPath().size())
            imlib_add_path_to_font_path(m_config->getFontPath().c_str());
        m_font = imlib_load_font(m_config->getFontName().c_str());
        imlib_context_set_font(m_font);
    }


    // init Workspace vector
    m_wv.resize(m_workspaces);


    /* get background */
    init_background();


    // draw icons
    updateNetWindowList();
    XFlush(m_dpy);

    Notifier::run(m_win, 1);

}


Pager::~Pager()
{
}

void Pager::run(){

    XEvent ev;


    /* infinite event loop */
    for (;;){

        /* init our updates to empty */
        m_im_updates = imlib_updates_init();


        /* init updates, but store previous cursor position.
           'MotionNotify' changes them
           and only 'LeaveNotify' drops.
        */
        int x = m_window_update.getX();
        int y = m_window_update.getY();
        m_window_update.flush();
        m_window_update.setXY(x,y);


        do
        {
            do
            {
                XNextEvent(m_dpy, &ev);
                switch (ev.type)
                {
                case ClientMessage:
#if EVENTS_DEBUG
    cout << "ClientMessage" << endl;
#endif
                    handleClientMessage(ev.xclient);
                    break;
                case Expose:
#if EVENTS_DEBUG
    cout << "Expose" << endl;
#endif
                    // do nothing, couz after all events image will be redrawn.
                    break;
                case ButtonPress:
#if EVENTS_DEBUG
    cout << "ButtonPress" << endl;
#endif
                    handleButtonPress(ev.xbutton);
                    break;
                case ButtonRelease:
#if EVENTS_DEBUG
    cout << "ButtonRelease" << endl;
#endif

                    handleButtonRelease(ev.xbutton);
                    break;
                case MotionNotify:
#if EVENTS_DEBUG
    cout << "MotionNotify" << endl;
#endif
                    m_window_update.setXY(ev.xmotion.x, ev.xmotion.y);
                    break;
                case EnterNotify:
#if EVENTS_DEBUG
    cout << "EnterNotify" << endl;
#endif
                    m_window_update.setXY(ev.xcrossing.x, ev.xcrossing.y);
                    m_window_update.createIcons(true);
                    break;
                case LeaveNotify:
#if EVENTS_DEBUG
    cout << "LeaveNotify" << endl;
#endif
                    m_window_update.setXY(-1, -1);
                    m_window_update.createIcons(true);
                    break;
                case ConfigureNotify:
#if EVENTS_DEBUG
    cout << "ConfigureNotify" << endl;
#endif

                    if (ev.xconfigure.window == m_win){
                        init_background();
                        m_window_update.createIcons(true);
                    } else if (ev.xconfigure.window != m_win){
                        m_window_update.createIcons(true);
                    }
                    break;
                case PropertyNotify:
#if EVENTS_DEBUG
    cout << "PropertyNotify" << endl;
#endif
                    /* This event is generated from root window only:
                    we use it to check for dynamic root window background change,
                    as enlightment has used it for years.
                    */
                    if (Atoms::Instance()->rootmapId() == ev.xproperty.atom){
                        init_background();
                        m_window_update.createIcons(true);
                    }
                    break;
                default:
#if EVENTS_DEBUG
    cout << "default" << endl;
#endif
                    break;
                }

            }
            while (XPending(m_dpy));


            /*
                after mouse click on the our window Xserver sends us a 'LeaveNotify'
                event and after 'EnterNotify' event. Some times this events arrive
                to ipager with an interval which leads the end of XPending cycle
                and 'createIcon' method are called. But in the next cycle ipager get
                a 'EnterNotify' event and icons are created again.
                This brings icons twinkling.
                To avoid this do a short pause.

                nano: 0,000000001 sec

            */
            timespec t, tr;
            t.tv_sec  = 0;
            t.tv_nsec = 100000;
            nanosleep(&t, &tr);

        }
        while (XPending(m_dpy));


        /* Window updates go here */
        if (m_window_update.updateNetWindowList())
            updateNetWindowList();
        if (m_window_update.displayIcons())
            if (m_window_update.motion())
                displayIcons(m_window_update.getX(), m_window_update.getY());
            else
                displayIcons();



        /* ImLib updates go here */

        /* take all the little rectangles to redraw and merge them into */
        m_im_updates = imlib_update_append_rect(m_im_updates, 0, 0,
                                                m_window_width, m_window_height
                                                );
        /* something sane for rendering */
        m_im_updates = imlib_updates_merge_for_rendering(m_im_updates, m_window_width, m_window_height);
        for (m_im_current_update = m_im_updates;
             m_im_current_update;
             m_im_current_update = imlib_updates_get_next(m_im_current_update))
          {
             int up_x, up_y, up_w, up_h;

             /* find out where the first update is */
             imlib_updates_get_coordinates(m_im_current_update, &up_x, &up_y, &up_w, &up_h);

             /* don't blend the image onto the drawable - slower */
             imlib_context_set_blend(1);
             imlib_context_set_image(m_buffer_icons);
             /* render the image at 0, 0 */
             if (m_buffer_icons)
               imlib_render_image_on_drawable(up_x, up_y);

          }
         /* if we had updates - free them */
        if (m_im_updates)
           imlib_updates_free(m_im_updates);


    }


}
/**
* Looking for selected window
*/
void Pager::handleButtonPress(XButtonEvent ev){

    /* looking for clicked window */
    selected_window = 0;

    int active = getActive(ev.x);
    Position p = position(active, ev.x, ev.y);

    int pointer_x = ev.x - p.x;
    int pointer_y = ev.y - p.y;

    WindowList wl = getWindowListForWorkspaceID(active);
    WindowList::reverse_iterator it = wl.rbegin();
    WindowList::reverse_iterator it_end = wl.rend();
    for (; it != it_end; it++){
        int x, y;
        uint width, height;
        if (!getWindowScaledGeometry(*it, p, &x, &y, &width, &height))
            continue;

        if (m_wm->hasWindowAtom((*it), Atoms::Instance()->netWmStateShaded()))
            height = 1;

        /* */
        if ( x <= pointer_x && (x + (int) width  >= pointer_x) &&
             y <= pointer_y && (y + (int) height >= pointer_y)
        ){
            selected_window = (*it);
            selected_window_ws_id = active;
            break;
        }
    }

}
/**
* Switch workspace
*/
void Pager::handleButtonRelease(XButtonEvent ev){

    m_window_update.createIcons(true);

    int active = getActive(ev.x);

    if (0 != selected_window && selected_window_ws_id != active){
        m_wm->changeWindowWorkspaceID(selected_window, active);
        selected_window = 0;
        return;
    }

    IntVec accept_buttons = m_config->getSwitchWorkspaceButton();
    for(uint i = 0; i < accept_buttons.size(); i++)
        if (accept_buttons[i] == (int) ev.button){
            m_wm->changeCurrentWorkspaceID(active);
            break;
        }

    m_config->getScrollAction()->activate((int) ev.button);
/*
    if (m_config->getMouseScrollUp().length() && 4 == (int) ev.button){
        if (m_config->getMouseScrollUp() == "nextWorkspace"){

        } else if (m_config->getMouseScrollUp() == "prevWorkspace"){

        }

    } else if (m_config->getMouseScrollDown().length() && 5 == (int) ev.button){

    }
*/
    selected_window = 0;

}

/**
 *
 * @param ev
 */
void Pager::handleMotion(int x, int y){

    m_window_update.setXY(x, y);
    m_window_update.createIcons(true);


}

void Pager::handleClientMessage(XClientMessageEvent ev){

    Atom atom = static_cast<Atom>(ev.data.l[0]);
    if (atom == Atoms::Instance()->blackboxNotifyCurrentWorkspace()){
#if EVENTS_DEBUG
        cout << "NotifyCurrentWorkspace" << endl;
#endif
        m_current_workspace = static_cast<int> (ev.data.l[1]);
        m_window_update.createIcons(true);

    } else if (atom == Atoms::Instance()->blackboxNotifyWindowAdd()){
#if EVENTS_DEBUG
        cout << " * blackboxNotifyWindowAdd" << endl;
        cout << "   ADD: " << ev.data.l[1] << "   ws: " << ev.data.l[2] << endl;
#endif

        m_window_update.updateNetWindowList(true);
        m_window_update.createIcons(true);

        m_wwm[ev.data.l[1]] = ev.data.l[2];
        m_ws.push_back(ev.data.l[1]);


    } else if (atom== Atoms::Instance()->blackboxNotifyWindowDel()){
#if EVENTS_DEBUG
        cout << " * blackboxNotifyWindowDel" << endl;
        cout << "   Del: " << ev.data.l[1] << endl;
#endif

        m_window_update.updateNetWindowList(true);
        m_window_update.createIcons(true);

        Window win = static_cast<Window>(ev.data.l[1]);

        // remove from WindowWorkspaceList
        m_wwm.erase(win);

        /**
        * remove from a stack
        */
        WindowStack::iterator it     = m_ws.begin();
        WindowStack::iterator it_end = m_ws.end();
        for(; it != m_ws.end(); it++)
            if ((*it) == win){
                m_ws.erase(it);
                break;
            }


    } else if (atom == Atoms::Instance()->blackboxNotifyWindowRaise()){
#if EVENTS_DEBUG
        cout << " * blackboxNotifyWindowRaise" << endl;
#endif

        Window window = static_cast<Window> (ev.data.l[1]);
        if (window == m_win) // skip us
            return;

        m_window_update.createIcons(true);

        if (m_ws.size()){
            WindowStack::reverse_iterator rit = m_ws.rbegin();
            if (*rit == window) // this window is already on top
                return;
        }

        m_window_update.updateNetWindowList(true);


    } else if (atom == Atoms::Instance()->blackboxNotifyWindowLower()) {
#if EVENTS_DEBUG
        cout << " * blackboxNotifyWindowLower" << endl;
#endif

    } else if (atom == Atoms::Instance()->blackboxNotifyWindowFocus()) {
#if EVENTS_DEBUG
        cout << " * blackboxNotifyWindowFocus" << endl;
#endif

        /**
        * send window on the top of the stack
        */
        Window window = ev.data.l[1];
        WindowStack::iterator it = find(m_ws.begin(), m_ws.end(), window);
        if (it != m_ws.end()){
            m_ws.erase(it);
            m_ws.push_back(window);
        }

        /**
        * HACK: Fluxbox doesnt send the _BLACKBOX_NOTIFY_WINDOW_ADD event
        * if a new window is Stucked.
        * So there is no this window in our window stack. And we add
        * it to stack here.
        */
        //WindowStack::iterator
        it = find(m_ws.begin(), m_ws.end(), window);
        if (m_ws.end() == it){
            m_ws.push_back(window);
        }

        m_window_update.createIcons(true);


    } else if (atom == Atoms::Instance()->blackboxNotifyWorkspaceCount()) {
#if EVENTS_DEBUG
        cout << " * blackboxNotifyWorkspaceCount " << endl;
#endif
        int workspaces = (int) ev.data.l[1];
        if (workspaces != m_workspaces){

            m_wv.resize(workspaces);
            m_window_width = (m_config->getIconSpacing()  + m_config->getIconMinWidth()) * workspaces +
                             (m_config->getIconMaxWidth() - m_config->getIconMinWidth()) + m_config->getIconSpacing();
            m_workspaces = workspaces;

            XResizeWindow(m_dpy, m_win, m_window_width, m_window_height);
            init_background();

            m_window_update.createIcons(true);

        }

    } else if (atom == Atoms::Instance()->ipagerNotifier()) {
#if EVENTS_DEBUG
        cout << "  ------ notify atom ----- " << endl;
#endif


        // in case a WM dont provide any notification when workspace or
        // client list have changed
        m_current_workspace = m_wm->getCurrentWorkspaceID();
        m_window_update.updateNetWindowList(true);

        int workspaces = m_wm->getWorkspaceCount();
        if (workspaces != m_workspaces){
        // count of workspaces have been changed.

            m_wv.resize(workspaces);
            m_window_width = (m_config->getIconSpacing()  + m_config->getIconMinWidth()) * workspaces +
                             (m_config->getIconMaxWidth() - m_config->getIconMinWidth()) + m_config->getIconSpacing();
            m_workspaces = workspaces;

            XResizeWindow(m_dpy, m_win, m_window_width, m_window_height);
            init_background();

            m_window_update.createIcons(true);

        }

    } else if (atom == Atoms::Instance()->blackboxNotifyStartup()) {
#if EVENTS_DEBUG
        cout << "  ------ Fluxbox startup atom ----- " << endl;
#endif
        init_background();
        m_window_update.createIcons(true);
        m_window_update.updateNetWindowList(true);


    } else {
    // unknown Fluxbox client message?!
#if EVENTS_DEBUG
        cout << " * blackbox other. Atom: " << ev.data.l[0] << endl;
#endif
    }


}

/**
 * Initialize desklet window
 * @return Window
 */
void Pager::initWindow(){

    int x = m_config->getIPagerWindowX();
    int y = m_config->getIPagerWindowY();

    m_win = XCreateSimpleWindow(m_dpy, DefaultRootWindow(m_dpy),
                              x, y,
                              m_window_width, m_window_height,
                              0, 0, 0);
    XSelectInput(m_dpy, m_win,
                    PropertyChangeMask
                    | ButtonPressMask
                    | ButtonReleaseMask
                    | PointerMotionMask
                    | ExposureMask
                    | EnterWindowMask
                    | LeaveWindowMask
                    | StructureNotifyMask
//                     | SubstructureNotifyMask
                    );

    //
    // Fluxbox support only this two atoms
    // for WMProtocols.
    Atom wmproto[2];
    wmproto[0] = Atoms::Instance()->wmDeleteWindow();
    wmproto[1] = Atoms::Instance()->blackboxStructureMessages();
    XSetWMProtocols(m_dpy, m_win, wmproto, 2);


    /**
    *  Size, Position and Class name hints.
    */
    // size and position
    XSizeHints size_hints;
    size_hints.flags = USPosition | PPosition | USSize | PSize | PWinGravity;
    size_hints.x = x;
    size_hints.y = y;
    size_hints.width = m_window_width;
    size_hints.height = m_window_height;
    size_hints.win_gravity = NorthWestGravity;

    // window manager hints
    XWMHints wm_hints;
    wm_hints.input = True;
    wm_hints.initial_state = m_config->getInSlit() ? WithdrawnState : NormalState;
    wm_hints.flags = InputHint | StateHint;

    // class hints
    XClassHint classhints;
    classhints.res_name  = "ipager";
    classhints.res_class = "IPager";

    // window name
    char *name = "iPager";
    XTextProperty windowname;
    XStringListToTextProperty(&name, 1, &windowname);

    /// setup window properties
    XSetWMProperties(m_dpy, m_win, &windowname, 0, 0, 0,
                     &size_hints, &wm_hints, &classhints);

    XResizeWindow( m_dpy, m_win, m_window_width, m_window_height);
    XMoveWindow(m_dpy, m_win, x, y); // set pager position


    /**
    * Hide Decorations
    */
    struct {
        ulong flags, functions, decorations;
        long input_mode;
        ulong status;
    } mwmhints;

    mwmhints.flags = mwmhints.functions = 0L;
    mwmhints.decorations = 0; // No Decorations
    mwmhints.input_mode = 0L;
    mwmhints.status = 0L;
    mwmhints.flags = (1l << 1);
    XChangeProperty(m_dpy, m_win,
                    Atoms::Instance()->motifWmHints(),
                    Atoms::Instance()->motifWmHints(), 32,
                    PropModeReplace, (unsigned char *) &mwmhints, 5);



    /**
    * Skip TaskBar
    */
    XChangeProperty(m_dpy, m_win,
        Atoms::Instance()->netWmState(),
        XA_ATOM, 32, PropModeAppend,
        (unsigned char *) &(Atoms::Instance()->netWmStateSkipTaskbar()),
        1
    );

    /**
    * Skip Pager
    */
    XChangeProperty(m_dpy, m_win,
        Atoms::Instance()->netWmState(),
        XA_ATOM, 32, PropModeAppend,
        (unsigned char *) &(Atoms::Instance()->netWmStateSkipPager()),
        1
    );


    /**
    * Skip Focus
    */
    XChangeProperty(m_dpy, m_win,
        Atoms::Instance()->netWmWindowType(),
        XA_ATOM, 32, PropModeAppend,
        (unsigned char *) &(Atoms::Instance()->netWmWindowTypeDock()),
        1
    );


    /**
    * On All Workspaces
    */
    XChangeProperty(m_dpy, m_win,
        Atoms::Instance()->netWmState(),
        XA_ATOM, 32, PropModeAppend,
        (unsigned char *) &(Atoms::Instance()->netWmStateSticky()),
        1
    );

    /**
    * State Below
    */
    XChangeProperty(m_dpy, m_win,
        Atoms::Instance()->netWmState(),
        XA_ATOM, 32, PropModeAppend,
        (unsigned char *) &(Atoms::Instance()->netWmStateBelow()),
        1
    );


    XMapWindow(m_dpy, m_win);
    XFlush(m_dpy);

}


/**
* Update list of clients
*
*/
void Pager::updateNetWindowList(){

    // forget what we know
    for(int i = 0; i < m_workspaces; i++)
        m_wv[i].wl.clear();

    // get the new revelation
    WindowList wl = m_wm->clientList();

    for(WindowList::iterator it = wl.begin(); it != wl.end(); it++){
        // skip what we should to
        if (
            (*it) == m_win ||
            m_wm->hasWindowAtom((*it), Atoms::Instance()->netWmStateSkipPager()) ||
            m_wm->hasWindowAtom((*it), Atoms::Instance()->netWmStateHidden())    ||
            ( !m_config->getDisplayStickyWindows() &&
               m_wm->hasWindowAtom((*it), Atoms::Instance()->netWmStateSticky())
            ) ||
            ( !m_config->getDisplayShadedWindows() &&
               m_wm->hasWindowAtom((*it), Atoms::Instance()->netWmStateShaded())
            ) ||
            WM::getErrorFlagAndDropIt()
        )
            continue;

        // where to display
        int workspace_id = m_wm->getWindowWorkspaceID((*it));

        /**
        * Fluxbox removes workspaces but doesnt move windows from that workspace
        * to another workspace.
        */
        if (workspace_id >= m_workspaces)
            continue;

        if (workspace_id < 0){
            // display on all workspaces
            if (m_wm->hasWindowAtom((*it), Atoms::Instance()->netWmStateSticky()))
                for(int i = 0; i < m_workspaces; i++)
                    m_wv[i].wl.push_back((*it));

        } else {
            /**
            *   prefer a Fluxbox notification then _NET_WM_DESKTOP window property.
            *   cause some apps (aMule, for example) change this property directly,
            *   not by sending ClientMessage event to WM as it ought to be.
            *   It leads a window is displayed on one workspace and the properties
            *   value has another workspace id.
            */
            WindowWorkspaceMap::iterator it_wwm = m_wwm.find(*it);
            if (m_wwm.end() != it_wwm && (*it_wwm).second < m_workspaces)
                workspace_id = (*it_wwm).second;

            m_wv[workspace_id].wl.push_back((*it));
        }
    }


}

/**
* Return a list of windows for defined workspace
*
*/
WindowList Pager::getWindowListForWorkspaceID(int workspace_id){

    if (workspace_id > (int) m_wv.size() || workspace_id < 0 || workspace_id >= m_workspaces)
        return WindowList();

    // does we have fluxbox notifications?
    if (!m_ws.size())
        return m_wv[workspace_id].wl; // no

    // yes. we have.
    WindowList wl;


    // create a map of accepted _NET windows
    WindowWorkspaceMap net_windows;
    for (
        WindowList::iterator it = m_wv[workspace_id].wl.begin();
        m_wv[workspace_id].wl.end() != it;
        it++
    )
        net_windows[(*it)] = 1;

    // fill WindowList in order of stacked windows.
    // which are located on a defined workspace.
    WindowStack::iterator it = m_ws.begin();
    for (; m_ws.end() != it; it++){
        if (net_windows.find((*it)) != net_windows.end())
            wl.push_back((*it));
    }
    return wl;


}

/**
 *
 * @param x
 * @param y
 * @param calc_position
 */
void Pager::displayIcons(int x, int y){

    // Sweep the buffer image
    //
    freeImLibImage(m_buffer_icons);
    m_buffer_icons = imlib_create_image(m_window_width, m_window_height);
    imlib_context_set_image(m_buffer_icons);
    imlib_context_set_color(0,0,0,0);
    imlib_image_fill_rectangle(0,0, m_window_width, m_window_height);
    imlib_blend_image_onto_image(m_background, 0,
                                 0, 0,
                                 m_window_width, m_config->getIconMaxHeight(),
                                 0, 0,
                                 m_window_width, m_config->getIconMaxHeight()
                                 );
    imlib_context_set_image(NULL);


    // Then reload the icons onto it one by one
    //
    int active = getActive(x);
    for(int rank=0; rank < m_workspaces; rank++){
        if (rank != active)
            displayIcon(rank, x, y);
    }
    if (active > -1 && active < (int) m_wv.size())
        displayIcon(active, x, y);
}

/**
 *
 * @pager rank
 * @param x
 * @param y
 * @param calc_position
 */
void Pager::displayIcon(int rank, int x, int y) {
#ifdef DEBUG
    cout << "displayIcon" << endl;
#endif
        Position p = position(rank, x, y);
        m_wv[rank].setIconX(p.x);
        m_wv[rank].setIconY(p.y);

        int delta = m_config->getZoomRecreateIconDelta();
        // should we create an workspace icon?
        if (
            // created yet?
            ( m_wv[rank].getIconImage() == NULL ) ||

            // need update?
            ( m_window_update.createIcons() )     ||

            // old icon?
            ( m_window_update.motion() && m_wv[rank].isIconOld() ) ||

            // width
            ( m_wv[rank].getIconWidth()  < p.width  - delta     ||
                m_wv[rank].getIconWidth()  > p.width  + delta ) ||
            // height
            ( m_wv[rank].getIconHeight() < p.height - delta     ||
                m_wv[rank].getIconHeight() > p.height + delta )

        ) {
            createIcon(rank, p);
        }
        m_wv[rank].setIconActualWidth(p.width);
        m_wv[rank].setIconActualHeight(p.height);
        if (m_wv[rank].getIconImage() != NULL){
            imlib_context_set_image(m_buffer_icons);
            imlib_context_set_blend(1);
            imlib_blend_image_onto_image(m_wv[rank].getIconImage(),1,
                                     0,0,
                                     m_wv[rank].getIconWidth(),
                                     m_wv[rank].getIconHeight(),
                                     p.x, p.y,
                                     p.width, p.height
                                     );
            imlib_context_set_image(NULL);
            imlib_context_set_blend(0);
        } else
            cout << "Could not blend workspace icon on the background. Icon is NULL." << endl;

}

/**
 *
 * @param workspace_number
 */
void Pager::createIcon(int workspace_id, Position &p){
#ifdef DEBUG
    cout << "createIcon:     workspace_number=" << workspace_id << endl;
#endif

    if (workspace_id >= m_workspaces)
        return;
    // New icon image
    Imlib_Image image = NULL;
    // get colors
    IColor bgcolor, lines_color, border_color;
    if (workspace_id == m_current_workspace){
        // current workspace colors
        bgcolor      = m_config->getActiveWorkspaceBackgroundColor();
        border_color = m_config->getActiveWorkspaceBorderColor();

    } else {
        bgcolor      = m_config->getWorkspaceBackgroundColor();
        border_color = m_config->getWorkspaceBorderColor();

    }
    // store context image
    Imlib_Image context_image = imlib_context_get_image();

    // create an image as a new or as a part of the background.
    if (!bgcolor.defined()){
        imlib_context_set_image(m_background);
        image = imlib_create_cropped_image(p.x, p.y, p.width, p.height);

    } else
        image = imlib_create_image(p.width, p.height);

    imlib_context_set_image(image);
    if (image)
      imlib_image_set_has_alpha(1);

    // draw background
    imlib_context_set_color(bgcolor.red, bgcolor.green, bgcolor.blue,
                            bgcolor.defined() ? 255 : 0
                            );
    imlib_image_fill_rectangle(0, 0, p.width, p.height);

    //
    // Workspace icon border
    //
    if (0 != selected_window && selected_window_ws_id != workspace_id &&
        p.width != m_config->getIconMinWidth() // only active icon does not have min width.
    ){
        // mark current workspace icon
        IColor sel_color = m_config->getSelectionColor();
        imlib_context_set_color(sel_color.red, sel_color.green, sel_color.blue, 255);
        imlib_image_draw_rectangle(0, 0, p.width, p.height);

    } else if (border_color.defined()){
        // draw default border.
        imlib_context_set_color(border_color.red, border_color.green, border_color.blue, 255);
        imlib_image_draw_rectangle(0, 0, p.width, p.height);

    }

    /* add workspace number */
    if (m_font && m_config->getDisplayWorkspaceNumber()){

        int text_w = 0;
        int text_h = 0;

        ostringstream os;
        os << workspace_id + 1;
        imlib_get_text_size(os.str().c_str(), &text_w, &text_h);
        IColor font_color = m_config->getWorkspaceNumberColor();
        imlib_context_set_color(font_color.red, font_color.green, font_color.blue, 255);

        imlib_text_draw( (p.width  - (p.width % 2) - text_w) / 2,
                            (p.height - (p.height % 2) - text_h) / 2,
                            os.str().c_str()
                            );

    }

    WindowList wl = getWindowListForWorkspaceID(workspace_id);
    WindowList::iterator it      = wl.begin();
    WindowList::iterator it_end  = wl.end();

    bool workspace_has_window_urgency_flag = false;


    // paint windows onto the icon
    for(; it != it_end; it++){
        int x, y;
        uint width, height;
        if (!getWindowScaledGeometry((*it), p, &x, &y, &width, &height))
            continue;

        // shaded window draw as a line
        if (m_wm->hasWindowAtom((*it), Atoms::Instance()->netWmStateShaded()))
            height = 1;


        IColor fill_color, border_color;

        if (workspace_id == m_current_workspace && *it == wl.back()){
        // this window is on top
            fill_color   = m_config->getActiveWindowBackgroundColor();
            border_color = m_config->getActiveWindowBorderColor();
        } else {
            fill_color   = m_config->getWindowBackgroundColor();
            border_color = m_config->getWindowBorderColor();
        }

        if (fill_color.defined()){
        // fill window with a color
            imlib_context_set_color(fill_color.red, fill_color.green, fill_color.blue, 255);
            imlib_image_fill_rectangle(x, y, width, height);
        }

        /**
        * Draw window outlines
        */
        if (*it == selected_window && m_config->getSelectionColor().defined()) {
            imlib_context_set_color(m_config->getSelectionColor().red,
                                    m_config->getSelectionColor().green,
                                    m_config->getSelectionColor().blue,
                                    255);
        } else
            // border color is been defined anyway in the IConfig coustructor.
            imlib_context_set_color(border_color.red, border_color.green, border_color.blue, 255);
        imlib_image_draw_rectangle(x, y, width, height);


        XWMHints * wmhints = XGetWMHints(m_dpy, *it);



        // Is mouse cursor over this window in workspace icon.
        bool cursor_is_over_this_window = false;
        int cursor_x = m_window_update.getX();
        int cursor_y = m_window_update.getY();
        if (cursor_x > -1 && cursor_y > -1 &&
            cursor_x >= p.x + x &&
            cursor_x <= p.x + x + (int) width &&
            cursor_y >= p.y + y &&
            cursor_y <= p.y + y + (int) height

        )
            cursor_is_over_this_window = true;

        /**
        * Add a window icon to the workspace image
        */
        if ((m_config->getDisplayWindowIcon()  == string("yes") ||
             (m_config->getDisplayWindowIcon() == string("mouseOver") &&
              cursor_is_over_this_window
             )
            ) && height > 8 && width > 8
        ){

            if (wmhints && (wmhints->flags & IconPixmapHint) && wmhints->icon_pixmap != 0) {

                // get width, height and depth for the pixmap
                Window root;
                int  icon_x, icon_y;
                uint border_width, bpp;
                uint icon_width, icon_height;

                XGetGeometry(m_dpy, wmhints->icon_pixmap, &root,
                            &icon_x, &icon_y,
                            &icon_width, &icon_height,
                            &border_width, &bpp
                            );

                Imlib_Image icon = NULL;
                imlib_context_set_drawable(wmhints->icon_pixmap);
                icon = imlib_create_image_from_drawable(wmhints->icon_mask, icon_x, icon_y, icon_width, icon_height, 1);
                imlib_context_set_drawable(m_win);

                if ((wmhints->flags & IconMaskHint) && wmhints->icon_mask != 0){
                    // turn ON transparency
                    imlib_context_set_image(icon);
                    imlib_image_set_has_alpha(1);
                    imlib_context_set_image(image);
                }

                // Scale app icon
                uint icon_scaled_width, icon_scaled_height;
                if (icon_width > width - 4 || icon_height > height - 4){
                    float w_index = (float) icon_width / (width - 4);
                    float h_index = (float) icon_height / (height - 4);
                    if (w_index >= h_index){
                        icon_scaled_width  = (uint) (icon_width  / w_index);
                        icon_scaled_height = (uint) (icon_height / w_index);
                    } else {
                        icon_scaled_width  = (uint) (icon_width  / h_index);
                        icon_scaled_height = (uint) (icon_height / h_index);
                    }

                } else {
                    icon_scaled_width  = icon_width;
                    icon_scaled_height = icon_height;

                }


                icon_x = x + (int) ((width  - icon_scaled_width)  / 2);
                icon_y = y + (int) ((height - icon_scaled_height) / 2);
                imlib_context_set_image(image);

                if (NULL != icon){
                    imlib_context_set_blend(1);
                    imlib_blend_image_onto_image(icon, 1,
                                0, 0, // s_x, x_y
                                icon_width, icon_height, // s_w, s_h
                                icon_x, icon_y, // d_x, d_y
                                icon_scaled_width, icon_scaled_height // d_w, d_h
                                );
                    imlib_context_set_blend(0);
                    imlib_context_set_image(icon);
                    imlib_free_image();
                    imlib_context_set_image(image);
                }


            }
        } // getDisplayWindowIcon


        /// Check Urgency bit
        if (wmhints && (wmhints->flags & XUrgencyHint))
            workspace_has_window_urgency_flag = true;

        if (wmhints)
            XFree(wmhints);

    }

    if (workspace_has_window_urgency_flag){
        /**
        * Workspace has a window expecting immediate user attention.
        * Draw a selection border on the workspace icon.
        */

        if (time(NULL) % 2){
            imlib_context_set_color(m_config->getSelectionColor().red,
                                    m_config->getSelectionColor().green,
                                    m_config->getSelectionColor().blue,
                                    255);
            imlib_image_draw_rectangle(0, 0, p.width, p.height);
        }

    }


    Icon icon;
    icon.image  = image;
    icon.width  = p.width;
    icon.height = p.height;
    m_wv[workspace_id].setIcon(icon);
    imlib_context_set_image(context_image);

}



/**
*
*/
bool Pager::getWindowScaledGeometry(Window win, Position &p,
                                        int * x, int * y, uint * width, uint * height
){
    Window root_return;

    uint border_width = 0;
    uint depth        = 0;

    // get params
    XGetGeometry(m_dpy, win, &root_return,
            x, y, width, height,
            &border_width, &depth);

    XTranslateCoordinates(m_dpy, win, RootWindow(m_dpy, DefaultScreen(m_dpy)),
                            0, 0, x,y, &root_return);

    // check for X errors
    if (WM::getErrorFlagAndDropIt())
        return false;

    //
    //  to              from
    //  -----       =   -----
    //  icon_width      1024
    //

    *x      = (p.width  * (*x)) / DisplayWidth(m_dpy, DefaultScreen(m_dpy));
    *y      = (p.height * (*y)) / DisplayHeight(m_dpy, DefaultScreen(m_dpy));
    *width  = (p.width  * (*width))  / DisplayWidth(m_dpy, DefaultScreen(m_dpy));
    *height = (p.height * (*height)) / DisplayHeight(m_dpy, DefaultScreen(m_dpy));


    return true;

}

/**
 *
 * @param rank
 * @param x
 * @param base_y
 * @return
 */
Position Pager::position(int rank, int x, int base_y){

    if (base_y < 3)
        base_y = 3 - base_y;
    else
        if (base_y > m_window_height - 3)
            base_y = 3 - (m_window_height - base_y);
        else
            base_y = 0;


    Position p;

    int   active;
    float scaling;
    int   width;
    int   height;

    active  = getActive(x);
    scaling = getScaling(x);
    width  = int(scaling * m_config->getIconMaxWidth()+
                       (1-scaling) * m_config->getIconMinWidth() - base_y);
    height = int(scaling * m_config->getIconMaxHeight() +
                       (1-scaling) * m_config->getIconMinHeight() - base_y);

    x=(m_config->getIconSpacing() +
            (m_config->getIconMaxWidth() - m_config->getIconMinWidth())/2 +
            (m_config->getIconSpacing() + m_config->getIconMinWidth())*rank);

    int y=(m_config->getIconMaxHeight() - m_config->getIconMinHeight())/2;

    if (m_config->getZoomType() == string("over")){
    // zoom active workspace over others.

        if (active > -1 && active < m_workspaces && rank == active){
                x-=(width - m_config->getIconMinWidth())/2;
                y-=(height - m_config->getIconMinHeight())/2;
        }

    } else {
    // zoom active workspace AND expand space to avoid icons crossing.

        if (active > -1 && active < m_workspaces){
            if (rank > active){
                x+=(width - m_config->getIconMinWidth())/2;
            } else {
                if (rank <= active)
                    x-=(width - m_config->getIconMinWidth())/2;
                if (rank==active){
                    y-=(height - m_config->getIconMinHeight())/2;
                }
            }
        }

    }

    p.x = x;
    p.y = y;
    if (rank!=active){
        p.width  = m_config->getIconMinWidth();
        p.height = m_config->getIconMinHeight();
    } else {
        p.width  = width;
        p.height = height;
    }

    return p;

}


/**
 *
 * @param x position of the mouse pointer
 * @return number of a workspace under a mouse pointer or -1 if pointer over free space.
 */
int Pager::getActive(int x){


    int a = x - m_config->getIconSpacing() -
            ((m_config->getIconMaxWidth() - m_config->getIconMinWidth())/2);
    int b = m_config->getIconMinWidth() + m_config->getIconSpacing();
    return ((a < 0) ? (-1) : (a / b));

}
/**
 *
 * @param x
 * @return
 */
float Pager::getScaling(int x){

    int a = (x - m_config->getIconSpacing() - ((m_config->getIconMaxWidth() - m_config->getIconMinWidth())/2));
    int b = (m_config->getIconMinWidth() + m_config->getIconSpacing());
    float c = (
             ((a < 0) ? (b + a) : (a % b)) -
             ((m_config->getIconMinWidth() + m_config->getIconSpacing())/2)
            ) /
            float(m_config->getIconMinWidth() + m_config->getIconSpacing());

    if (c < 0)
        c = 0 - c;


    float scaling = 1 - 2 * c;

//     float scaling = 1 - 2 * std::abs(
//         (
//             (
//                 (a < 0) ? (b + a) : (a % b)
//             ) -
//             ((config->getIconMinWidth() + config->getIconSpacing())/2)
//         ) /
//         (config->getIconMinWidth() + config->getIconSpacing()));
// //         float(config->getIconMinWidth() + config->getIconSpacing()));

    if (scaling >= m_config->getIconMaximizeThreshold())
        scaling = 1;


    if (float(m_config->getIconMinWidth() / m_config->getIconMaxWidth()) > scaling)
        return m_config->getIconMinWidth() / m_config->getIconMaxWidth();
    return scaling;

}

bool Pager::hasWindow(Window w){

    for(int i = 0; i < m_workspaces; i++)
        for(WindowList::iterator it = m_wv[i].wl.begin();
            it != m_wv[i].wl.end();
            it++
            )
            if (w == (*it))
                return true;
    return false;
}

bool Pager::hasWindowOnAllWorkspaces(Window w){

    for(int i = 0; i < m_workspaces; i++){
        bool found = false;
        WindowList::iterator it = m_wv[i].wl.begin();
        for(; it != m_wv[i].wl.end(); it++){
            if (w == (*it)){
                found = true;
                continue;
            }
        }
        if (!found)
            return false;
    }
    return true;

}


void Pager::init_background(){

    /* save current context */
    Imlib_Image current_image = imlib_context_get_image();
    Drawable current_drawble  = imlib_context_get_drawable();
    /*  */

    Imlib_Image image = grab_background();
    if (NULL == image){
        // no background. fill with a black color
        image = imlib_create_image(m_window_width, m_window_height);
        imlib_context_set_color(0, 0, 0, 255);
        imlib_image_fill_rectangle(0, 0, m_window_width, m_window_height);
    }
    imlib_context_set_image(image);


    if (m_config->getIpagerBackgroundColor().defined()){
        IColor bgcolor = m_config->getIpagerBackgroundColor();
        imlib_context_set_color(bgcolor.red, bgcolor.green, bgcolor.blue, 255);
        imlib_image_fill_rectangle(0, 0, m_window_width, m_window_height);

    }


    if (m_config->getIpagerBackgroundImagePath().size()){

        Imlib_Image image_from_file = imlib_load_image(m_config->getIpagerBackgroundImagePath().c_str());
        if (!image_from_file){
            cerr << "Could not load background image " << m_config->getIpagerBackgroundImagePath() << endl;

        } else {

            /* if we already has background image (plain color may be) free it */
            if (NULL != image){
                imlib_context_set_image(image);
                imlib_free_image();
            }

            image = image_from_file;
            imlib_context_set_image(image);

        }

    }

    /* ipager window and a border */
    if (m_config->getIpagerBorderColor().defined()){

        IColor bcolor = m_config->getIpagerBorderColor();
        imlib_context_set_color(bcolor.red, bcolor.green, bcolor.blue, 255);
        imlib_image_draw_rectangle(0, 0, m_window_width, m_window_height);

    }


    freeImLibImage(m_background);
    imlib_context_set_image(image);
    m_background = imlib_clone_image();

    /* Restore context */
    imlib_context_set_image(current_image);
    imlib_context_set_drawable(current_drawble);
    return;
}


/**
*
*   Routine to grab the background in Imlib_Image.
*   Returns NULL if it fails, Reference to output Imlib_Image otherwise.
*
*   Routine has been taken from aDesklets.
*/
Imlib_Image Pager::grab_background(){

    /*             */
    XSetWindowAttributes XDefaultWindowAttributes = {
        ParentRelative,   /* Pixmap background_pixmap */
        0L,           /* unsigned long background_pixel */
        0,            /* Pixmap border_pixmap */
        0L,           /* unsigned long border_pixel */
        0,            /* int bit_gravity */
        0,            /* int win_gravity */
        Always,       /* int backing_store */
        0L,           /* unsigned long backing_planes */
        0L,           /* unsigned long backing_pixel */
        False,        /* Bool save_under */
        StructureNotifyMask | ExposureMask | ButtonPressMask,  /* long event_mask */
        0L,           /* long do_not_propagate_mask */
        True,         /* Bool override_redirect */
        0,            /* Colormap colormap */
        0         /* Cursor cursor */
    };



    int x, y;
    XEvent ev;
    Window src;
    XWindowAttributes attr;
    Imlib_Image background=NULL;


    if(XGetWindowAttributes(m_dpy, m_win, &attr) &&
        /* Avoid reparenting coordinates translation problem */
        XTranslateCoordinates(m_dpy, m_win, RootWindow(m_dpy, m_wm->screen()),
                0, 0, &x, &y, &src)) {
        /* The trick is to create an Overrideredirect window overlapping our
            window with background type of Parent relative and then grab it.
            It seems overkill, but:
            - XGetImage() on root get all viewable children windows embedded.
            - XCopyArea() cause potential synchronisation problems depending
            on backing store settings of root window: playing around
            with GraphicsExpose events is not quicker nor simpler.

            This idea was taken from aterm source base. See src/pixmap.c
            in core distribution (http://aterm.sourceforge.net/).
        */

        int screen_width  = DisplayWidth(m_dpy, m_wm->screen());
        int screen_hegith = DisplayHeight(m_dpy, m_wm->screen());
        if (screen_width < x + attr.width)
            attr.width = screen_width - x;
        if (attr.width <= 0){
            cerr << "X coord of ipager window (" << x << ") is out of screen." << endl;
            x = screen_width - 1;
            attr.width = 1;
        }
        if (screen_hegith < y + attr.height)
            attr.height = screen_hegith - y;
        if (attr.height <= 0){
            cerr << "Y coord of ipager window (" << y << ") is out of screen." << endl;
            y = screen_hegith - 1;
            attr.height = 1;
        }

        attr.width = m_window_width;
        attr.height = m_window_height;

        if((src=XCreateWindow(m_dpy,
                RootWindow(m_dpy, m_wm->screen()),
                x, y, attr.width, attr.height,
                0,
                CopyFromParent, CopyFromParent, CopyFromParent,
                CWBackPixmap|CWBackingStore|
                CWOverrideRedirect|CWEventMask,
                &XDefaultWindowAttributes))
        ){

            XGrabServer(m_dpy);
            XMapRaised(m_dpy,src);
            XMoveWindow(m_dpy, src, x, y);
            XSync(m_dpy,False);
            do
                XWindowEvent(m_dpy, src, ExposureMask, &ev);
            while(ev.type!=Expose);

            imlib_context_set_drawable(src);
            background=imlib_create_image_from_drawable(0,0,0,
                                attr.width,attr.height,0);
            XUngrabServer(m_dpy);
            XDestroyWindow(m_dpy,src);

            /* Make sure image has an alpha channel */
            if (NULL != background){
                imlib_context_set_image(background);
                imlib_image_set_has_alpha(1);
            }
        }

    }

    return background;

}

void Pager::freeImLibImage(Imlib_Image &image){

    if (NULL == image)
        return;
    Imlib_Image current_image = imlib_context_get_image();
    imlib_context_set_image(image);
    imlib_free_image();
    image = NULL;
    imlib_context_set_image(current_image);

}




