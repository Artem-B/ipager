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

#ifndef PAGER_H
#define PAGER_H


#include <X11/Xlib.h>
#include <Imlib2.h>

#include "atoms.h"
#include "wm.h"
#include "icon.h"
#include "iconfig.h"
#include "workspace.h"

#include <cstdlib>
#include <vector>
#include <map>
using namespace std;




// an icon dimensions
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Position;

typedef map<Window, int> WindowWorkspaceMap;
typedef vector<Window> WindowStack;


/**
@author Suhanov Vadim
*/
class Pager{


    WM          * m_wm;
    IConfig     * m_config;

    /* X11 stuff */
    Display         * m_dpy;
    Window            m_win;
    Visual          * m_vis;
    Colormap          m_cm;
    int               m_depth;
    XSetWindowAttributes m_XDefaultWindowAttributes;


    /* ImLib2 stuff */
    Imlib_Updates m_im_updates, m_im_current_update;
    Imlib_Image         m_buffer;
    Imlib_Image         m_background;
    Imlib_Image         m_buffer_icons;
    Imlib_Font          m_font;
    Imlib_Color_Range   m_range;


    int     m_workspaces;
    int     m_current_workspace;
    int     m_window_width;
    int     m_window_height;


    /* Updates for window */
    class Updates {
        bool   m_display_icons;
        bool   m_update_window_list;
        bool   m_create_icons;
        int    m_coord_x;
        int    m_coord_y;

    public:
        Updates(){
            flush();
        }
        ~Updates(){};

        void flush(){
            m_coord_x = -1;
            m_coord_y = -1;
            m_update_window_list = false;
            m_create_icons       = false;
            m_display_icons      = false;
        };

        // should the window list be updated
        bool updateNetWindowList(){ return m_update_window_list; }
        bool updateNetWindowList(bool update){
            m_create_icons       = true;
            m_display_icons      = true;
            m_update_window_list = update;
            return m_update_window_list;
        }

        // should icons be recreated
        bool createIcons(){ return m_create_icons; }
        bool createIcons(bool update){
            m_display_icons = true;
            m_create_icons  = update;
            return m_create_icons;
        }

        bool motion(){ return (m_coord_x > -1 && m_coord_y > -1) ? true : false; }
        int  getX()  { return m_coord_x; }
        int  getY()  { return m_coord_y; }
        void setXY(int x, int y){ m_coord_x = x; m_coord_y = y;}

        bool displayIcons(){ return (m_display_icons || motion()); }
    };
    Updates m_window_update;


    typedef vector<Workspace> WorkspaceVector;
    WorkspaceVector m_wv;


    /**
    *  store a windows and its workspace Id here.
    *  its necessary couz some apps change its _NET_WM_DESKTOP
    *  propetry directly, not by sending ClientMessage event to WM.
    */
    WindowWorkspaceMap m_wwm;

    /* store in it a queue of focused windows */
    WindowStack m_ws;

    /* selected window */
    Window selected_window;
    int selected_window_ws_id;

    void initWindow();
    void createIcon(int, Position&);
    void displayIcon(int rank, int x, int y);
    void displayIcons(int x = 0, int y = 0);

    void handleButtonPress(XButtonEvent);
    void handleButtonRelease(XButtonEvent);
    void handleMotion(int, int);
    void handleClientMessage(XClientMessageEvent);
    void updateNetWindowList();
    WindowList getWindowListForWorkspaceID(int);

    /* */
    bool         getWindowScaledGeometry(Window, Position&,
                                            int * x, int * y, uint * width, uint * height);
    Position     position(int rank, int x, int base_y = 0);
    int          getActive(int x);
    float        getScaling(int x);
    void         init_background();
    Imlib_Image  grab_background();
    void         freeImLibImage(Imlib_Image &image);


    bool hasWindow(Window);
    bool hasWindowOnAllWorkspaces(Window);


public:
    Pager();
    ~Pager();
    void run();

};

#endif
