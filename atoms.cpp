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

#include "atoms.h"

#include "wm.h"


#include <iostream>
using namespace std;



Atoms * Atoms::instance = 0;

Atoms::Atoms( ){

    Display * dpy = WM::Instance()->display();


    rootmap_id       = XInternAtom (dpy, "_XROOTPMAP_ID", False);
    wm_delete_window = XInternAtom( dpy, "WM_DELETE_WINDOW",False);

    xa_wm_state      = XInternAtom(dpy, "WM_STATE", False);
    xa_wm_protocols  = XInternAtom(dpy, "WM_PROTOCOLS", False);
    xa_wm_take_focus = XInternAtom(dpy, "WM_TAKE_FOCUS", False);

    motif_wm_hints   = XInternAtom( dpy, "_MOTIF_WM_HINTS",False);



    /*    BlackBox Atoms    */
    blackbox_change_attributes =
        XInternAtom(dpy, "_BLACKBOX_CHANGE_ATTRIBUTES", False);
    blackbox_structure_messages =
        XInternAtom(dpy, "_BLACKBOX_STRUCTURE_MESSAGES", False);
    blackbox_notify_startup =
        XInternAtom(dpy, "_BLACKBOX_NOTIFY_STARTUP", False);
    blackbox_notify_window_add =
        XInternAtom(dpy, "_BLACKBOX_NOTIFY_WINDOW_ADD", False);
    blackbox_notify_window_del =
        XInternAtom(dpy, "_BLACKBOX_NOTIFY_WINDOW_DEL", False);
    blackbox_notify_current_workspace =
        XInternAtom(dpy, "_BLACKBOX_NOTIFY_CURRENT_WORKSPACE", False);
    blackbox_notify_workspace_count =
        XInternAtom(dpy, "_BLACKBOX_NOTIFY_WORKSPACE_COUNT", False);
    blackbox_notify_window_focus =
        XInternAtom(dpy, "_BLACKBOX_NOTIFY_WINDOW_FOCUS", False);
    blackbox_notify_window_raise =
        XInternAtom(dpy, "_BLACKBOX_NOTIFY_WINDOW_RAISE", False);
    blackbox_notify_window_lower =
        XInternAtom(dpy, "_BLACKBOX_NOTIFY_WINDOW_LOWER", False);
    blackbox_change_workspace =
            XInternAtom(dpy, "_BLACKBOX_CHANGE_WORKSPACE", False);
    blackbox_change_window_focus =
        XInternAtom(dpy, "_BLACKBOX_CHANGE_WINDOW_FOCUS", False);
    blackbox_cycle_window_focus =
        XInternAtom(dpy, "_BLACKBOX_CYCLE_WINDOW_FOCUS", False);
    blackbox_hints = XInternAtom(dpy, "_BLACKBOX_HINTS", False);



    /*  NET protocol Atoms   */
    net_supported            = XInternAtom(dpy, "_NET_SUPPORTED", False);
    net_client_list          = XInternAtom(dpy, "_NET_CLIENT_LIST", False);
    net_client_list_stacking = XInternAtom(dpy, "_NET_CLIENT_LIST_STACKING", False);
    net_number_of_desktops   = XInternAtom(dpy, "_NET_NUMBER_OF_DESKTOPS", False);
    net_desktop_geometry     = XInternAtom(dpy, "_NET_DESKTOP_GEOMETRY", False);
    net_desktop_viewport     = XInternAtom(dpy, "_NET_DESKTOP_VIEWPORT", False);
    net_current_desktop      = XInternAtom(dpy, "_NET_CURRENT_DESKTOP", False);
    net_desktop_names        = XInternAtom(dpy, "_NET_DESKTOP_NAMES", False);
    net_active_window        = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
    net_workarea             = XInternAtom(dpy, "_NET_WORKAREA", False);
    net_supporting_wm_check  = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False);
    net_virtual_roots        = XInternAtom(dpy, "_NET_VIRTUAL_ROOTS", False);

    net_close_window     = XInternAtom(dpy, "_NET_CLOSE_WINDOW", False);
    net_wm_moveresize    = XInternAtom(dpy, "_NET_WM_MOVERESIZE", False);

    net_properties       = XInternAtom(dpy, "_NET_PROPERTIES", False);
    net_wm_name          = XInternAtom(dpy, "_NET_WM_NAME", False);
    net_wm_desktop       = XInternAtom(dpy, "_NET_WM_DESKTOP", False);
    net_wm_window_type   = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    net_wm_state         = XInternAtom(dpy, "_NET_WM_STATE", False);
    net_wm_strut         = XInternAtom(dpy, "_NET_WM_STRUT", False);
    net_wm_icon_geometry = XInternAtom(dpy, "_NET_WM_ICON_GEOMETRY", False);
    net_wm_icon          = XInternAtom(dpy, "_NET_WM_ICON", False);
    net_wm_pid           = XInternAtom(dpy, "_NET_WM_PID", False);
    net_wm_handled_icons = XInternAtom(dpy, "_NET_WM_HANDLED_ICONS", False);

    net_wm_ping = XInternAtom(dpy, "_NET_WM_PING", False);

    net_wm_state_skip_taskbar = XInternAtom(dpy, "_NET_WM_STATE_SKIP_TASKBAR", False);
    net_wm_state_skip_pager   = XInternAtom(dpy, "_NET_WM_STATE_SKIP_PAGER", False);
    net_wm_state_hidden       = XInternAtom(dpy, "_NET_WM_STATE_HIDDEN", False);
    net_wm_state_sticky       = XInternAtom(dpy, "_NET_WM_STATE_STICKY", False);
    net_wm_state_shaded       = XInternAtom(dpy, "_NET_WM_STATE_SHADED", False);
    net_wm_state_below        = XInternAtom(dpy, "_NET_WM_STATE_BELOW", False);



    net_wm_window_type_dock   = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
    net_wm_window_type_menu   = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_MENU", False);

    ipager_notifier = XInternAtom(dpy, "_IPAGER_NOTIFIER", False);

};

