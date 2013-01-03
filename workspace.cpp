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

#include "workspace.h"

#include <time.h>
#include <iostream>
using namespace std;




Workspace::Workspace(){

    m_icon.image  = NULL;
    m_icon.width  = 0;
    m_icon.height = 0;
    m_icon_x = 0;
    m_icon_y = 0;
    m_icon_actual_width  = 0;
    m_icon_actual_height = 0;


#ifdef HAS_SYS_TIME_H
    timerclear(&m_tv);
#endif

}


Workspace::~Workspace(){

    freeIcon();

}

void Workspace::freeIcon(){

    if (NULL == m_icon.image) return;

    Imlib_Image current_image = imlib_context_get_image();
    imlib_context_set_image(m_icon.image);
    imlib_free_image();
    imlib_context_set_image(current_image);
    m_icon.image = NULL;
    m_icon.width  = 0;
    m_icon.height = 0;
#ifdef HAS_SYS_TIME_H
    timerclear(&m_tv);
#endif

}

void Workspace::setIcon(Icon &i){

    freeIcon();
    m_icon = i;
#ifdef HAS_SYS_TIME_H
    // set icon birth moment
    gettimeofday(&m_tv, NULL);
#endif

}

bool Workspace::isIconOld(){

#ifdef HAS_SYS_TIME_H
    struct timeval  tv_now;
    gettimeofday(&tv_now, NULL);

    if ( ( (double) tv_now.tv_sec + (double) tv_now.tv_usec/1000000.0f ) -
         ( (double) m_tv.tv_sec + (double) m_tv.tv_usec/1000000.0f ) > 0.2
         )
         return true;
#endif

    return false;

}

