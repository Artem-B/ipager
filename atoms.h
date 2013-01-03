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

#ifndef ATOMS_H
#define ATOMS_H

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
};


class Atoms {

    static Atoms *instance;
    Atoms();

    Atom    rootmap_id;

    Atom    xa_wm_state,
            xa_wm_protocols,
            xa_wm_take_focus;


    Atom    wm_delete_window;
    Atom    motif_wm_hints;

    Atom    blackbox_change_attributes,
            blackbox_structure_messages,
            blackbox_notify_startup,
            blackbox_notify_window_add,
            blackbox_notify_window_del,
            blackbox_notify_current_workspace,
            blackbox_notify_workspace_count,
            blackbox_notify_window_focus,
            blackbox_notify_window_raise,
            blackbox_notify_window_lower,
            blackbox_change_workspace,
            blackbox_change_window_focus,
            blackbox_cycle_window_focus,
            blackbox_hints;


    Atom    net_supported,
            net_client_list,
            net_client_list_stacking,
            net_number_of_desktops,
            net_desktop_geometry,
            net_desktop_viewport,
            net_current_desktop,
            net_desktop_names,
            net_active_window,
            net_workarea,
            net_supporting_wm_check,
            net_virtual_roots,
            net_close_window,
            net_wm_moveresize,
            net_properties;

    Atom
            net_wm_name,
            net_wm_desktop,
            net_wm_window_type,
            net_wm_state,
            net_wm_strut,
            net_wm_icon_geometry,
            net_wm_icon,
            net_wm_pid,
            net_wm_handled_icons,
            net_wm_ping;

    Atom    net_wm_state_skip_taskbar,
            net_wm_state_skip_pager,
            net_wm_state_hidden,
            net_wm_state_sticky,
            net_wm_state_shaded,
            net_wm_state_below;

    Atom    net_wm_window_type_dock,
            net_wm_window_type_menu;

    Atom    ipager_notifier;


public:

    static
    Atoms * Instance(){
        if ( 0 == instance )  instance = new Atoms();
        return instance;
    };
    ~Atoms();


    const Atom &rootmapId(void) const
        { return rootmap_id; };

    const Atom &xaWmState(void) const
        { return xa_wm_state; };
    const Atom &motifWmHints(void) const
        { return motif_wm_hints; };


    const Atom &wmDeleteWindow(void) const
        { return wm_delete_window; };

    /* BlackBox */
    const Atom &blackboxStructureMessages(void) const
        { return blackbox_structure_messages; };
    const Atom &blackboxNotifyCurrentWorkspace(void) const
        { return blackbox_notify_current_workspace; };
    const Atom &blackboxChangeWorkspace(void) const
        { return blackbox_change_workspace; };
    const Atom &blackboxNotifyWindowAdd(void) const
        { return blackbox_notify_window_add; };
    const Atom &blackboxNotifyWindowDel(void) const
        { return blackbox_notify_window_del; };
    const Atom &blackboxNotifyWindowRaise(void) const
        { return blackbox_notify_window_raise; };
    const Atom &blackboxNotifyWindowLower(void) const
        { return blackbox_notify_window_lower; };
    const Atom &blackboxNotifyWindowFocus(void) const
        { return blackbox_notify_window_focus; };
    const Atom &blackboxHints(void) const
        { return blackbox_hints; };
    const Atom &blackboxNotifyStartup(void) const
        { return blackbox_notify_startup; };
    const Atom &blackboxNotifyWorkspaceCount(void) const
        { return blackbox_notify_workspace_count; };

    /* NET */
    const Atom &netClientList(void) const
        { return net_client_list; };
    const Atom &netNumberOfDesktops(void) const
        { return net_number_of_desktops; };
    const Atom &netCurrentDesktop(void) const
        { return net_current_desktop; };
    const Atom &netWMDesktop(void) const
        { return net_wm_desktop; };
    const Atom &netWmState(void) const
        { return net_wm_state; };
    const Atom &netClientListStacking(void) const
        { return net_client_list_stacking; };
    const Atom &netActiveWindow(void) const
        { return net_active_window; };
    const Atom &netWmDesktop(void) const
        { return net_wm_desktop; };
    const Atom &netWmStateSkipTaskbar(void) const
        { return net_wm_state_skip_taskbar; };
    const Atom &netWmStateSkipPager(void) const
        { return net_wm_state_skip_pager; };
    const Atom &netWmStateHidden(void) const
        { return net_wm_state_hidden; };
    const Atom &netWmStateSticky(void) const
        { return net_wm_state_sticky; };
    const Atom &netWmStateShaded(void) const
        { return net_wm_state_shaded; };
    const Atom &netWmStateBelow(void) const
        { return net_wm_state_below; };


    const Atom &netWmWindowType(void) const
        { return net_wm_window_type; };
    const Atom &netWmWindowTypeDock(void) const
        { return net_wm_window_type_dock; };
    const Atom &netWmWindowTypeMenu(void) const
        { return net_wm_window_type_menu; };


    /*    */
    const Atom &ipagerNotifier(void) const
        { return ipager_notifier; };

};

#endif
