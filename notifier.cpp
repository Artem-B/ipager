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


extern "C" {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <pthread.h>
}

#include <cstdlib>
#include <iostream>

#include "notifier.h"

using namespace std;


Display * Notifier::m_dpy           = 0;
Window    Notifier::m_win           = 0;
Atom      Notifier::ipager_notifier = 0;
int       Notifier::m_seconds       = 1;
pthread_t Notifier::m_timer_thread  = 0;


Notifier::Notifier()
{
}


Notifier::~Notifier()
{
}

void Notifier::run(Window w, int s){

    if (0 == m_dpy){
        // open new connection
        m_dpy = XOpenDisplay(getenv("DISPLAY"));
        if (NULL == m_dpy){
            cout << "Notifier::run: X Connection error" << endl;
            exit(0);
        }
        // notifier atom
        ipager_notifier = Atoms::Instance()->ipagerNotifier();
    }
    Notifier::m_win = w;
    Notifier::m_seconds = s;
    pthread_create(&m_timer_thread, NULL, &(Notifier::loop), (void*) NULL);

}

void Notifier::notify(){

    XEvent ev;
    ev.type = ClientMessage;
    ev.xclient.data.l[0] = ipager_notifier;
    ev.xclient.window    = m_win;
    ev.xclient.display   = m_dpy;
    ev.xclient.format    = 32;

    XSendEvent(m_dpy, m_win, True, 0L, &ev);
    XFlush(m_dpy);

}

void * Notifier::loop(void*){

    while (1){
        timeval timeout;
        timeout.tv_sec  = m_seconds;
        timeout.tv_usec = 0;
        select(1, NULL, NULL, NULL, &timeout);
        notify();
    }

}



