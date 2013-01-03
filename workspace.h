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

#ifndef WORKSPACE_H
#define WORKSPACE_H


#include <X11/Xlib.h>
#include <Imlib2.h>

#ifdef HAS_SYS_TIME_H
#include <sys/time.h>
#endif

#include "wm.h"
#include "icon.h"


/**
@author Suhanov Vadim
*/
class Workspace {
    int    m_icon_x, m_icon_y;
    int    m_icon_actual_width, m_icon_actual_height;
    Icon   m_icon;
    struct timeval m_tv;

public:
    Workspace();
    ~Workspace();
    void freeIcon();

    // position
    int  getIconX(){ return m_icon_x;   }
    int  getIconY(){ return m_icon_y;   }
    void setIconX(int x){ m_icon_x = x; }
    void setIconY(int y){ m_icon_y = y; }

    // size
    int  getIconWidth(){  return m_icon.width;     }
    int  getIconHeight(){ return m_icon.height;    }
    void setIconWidth(int w){  m_icon.width = w;   }
    void setIconHeight(int h){ m_icon.height = h;  }

    // actual size
    int  getIconActualWidth(){  return m_icon_actual_width;    }
    int  getIconActualHeight(){ return m_icon_actual_height;   }
    void setIconActualWidth(int w) { m_icon_actual_width = w;  }
    void setIconActualHeight(int h){ m_icon_actual_height = h; }

    bool isIconOld();

    Imlib_Image getIconImage(){ return m_icon.image; }
    void setIcon(Icon&);
    WindowList wl;
};


#endif
