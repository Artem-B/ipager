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

#ifndef ICONFIG_H
#define ICONFIG_H


#include <string>
#include <vector>
using namespace std;

#include "icolor.h"
#include "actions.h"


typedef vector<int> IntVec;


/**
@author Suhanov Vadim
*/
class IConfig{
    static IConfig * instance;
    static string    config_filename;

    int     icon_spacing;
    int     icon_min_width;
    int     icon_max_width;
    int     icon_min_height;
    int     icon_max_height;
    float   icon_maximize_threshold;

    int     zoom_recreate_icon_delta;

    bool    display_sticky_windows;
    bool    display_shaded_windows;
    bool    display_workspace_number;

    /* ipager positions */
    int     ipager_window_x;
    int     ipager_window_y;

    /* ipager background */
    IColor  ipager_background_color,
            ipager_border_color;

    string  ipager_background_image,
            font_name,
            font_path;

    IColor  workspace_number_color;

    /* windows on workspaces */
    IColor  active_workspace_background_color,
            active_workspace_border_color,
            workspace_background_color,
            workspace_border_color;

    IColor  active_window_background_color,
            active_window_border_color,
            window_background_color,
            window_border_color;

    IColor  selection_color;

    /// Urgency
    bool    urgency_notify;
    string  urgency_type;
    IColor  urgency_color;
    IntVec  urgency_mark;


    IntVec  switch_workspace_button;

    string  zoom_type;

    string  display_window_icon;

    bool    in_slit;

    /*
    string  mouse_scroll_up,
            mouse_scroll_down;
    */

    ScrollAction  scroll_action;

    IConfig();

public:
    static
    IConfig * Instance(){
        if (NULL == instance) instance = new IConfig();
        return instance;
    }
    ~IConfig();
    static void setConfigFileName(string);

    int    getIconSpacing()  { return icon_spacing;    };
    int    getIconMinWidth() { return icon_min_width;  };
    int    getIconMaxWidth() { return icon_max_width;  };
    int    getIconMinHeight(){ return icon_min_height; };
    int    getIconMaxHeight(){ return icon_max_height; };
    float  getIconMaximizeThreshold(){
                                     return icon_maximize_threshold; };
    int    getZoomRecreateIconDelta(){ return zoom_recreate_icon_delta; };


    int    getIPagerWindowX() { return ipager_window_x; };
    int    getIPagerWindowY() { return ipager_window_y; };

    IColor getIpagerBackgroundColor()     { return ipager_background_color;          };
    IColor getIpagerBorderColor()         { return ipager_border_color;              };
    string getIpagerBackgroundImagePath() { return ipager_background_image;          };

    string getFontName()                  { return font_name;                        };
    string getFontPath()                  { return font_path;                        };
    IColor getWorkspaceNumberColor()      { return workspace_number_color;           };


    bool   getDisplayStickyWindows()      { return display_sticky_windows;   };
    bool   getDisplayShadedWindows()      { return display_shaded_windows;   };
    bool   getDisplayWorkspaceNumber()    { return display_workspace_number; };

    IColor getActiveWorkspaceBackgroundColor() { return active_workspace_background_color; };
    IColor getActiveWorkspaceBorderColor()     { return active_workspace_border_color;     };
    IColor getWorkspaceBackgroundColor()       { return workspace_background_color;        };
    IColor getWorkspaceBorderColor()           { return workspace_border_color;            };

    IntVec getSwitchWorkspaceButton()     { return switch_workspace_button;     };

    IColor getActiveWindowBackgroundColor() { return active_window_background_color; };
    IColor getActiveWindowBorderColor()     { return active_window_border_color;     };
    IColor getWindowBackgroundColor()       { return window_background_color;        };
    IColor getWindowBorderColor()           { return window_border_color;             };

    IColor getSelectionColor()          { return selection_color;            };

    void   setIconMinWidth(int min_width) { icon_min_width = min_width; }
    void   setIconMaxWidth(int max_width) { icon_max_width = max_width; }

    string getZoomType(){ return zoom_type; }
    string getDisplayWindowIcon(){ return display_window_icon; };

    bool   getInSlit(){ return in_slit; };

//     string getMouseScrollUp(){ return mouse_scroll_up; }
//     string getMouseScrollDown(){ return mouse_scroll_down; }

    ScrollAction * getScrollAction(){ return &scroll_action; }
};

#endif
