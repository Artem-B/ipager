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

#include "iconfig.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>



template <class T>
bool from_string(T &t,
                 const std::string &s,
                 std::ios_base & (*f)(std::ios_base&))
{

   std::istringstream iss(s);
   return !(iss>>f>>t).fail();

}



bool isNumer(string &str){

    for (uint pos = 0; pos < str.size(); pos++)
        if (!isdigit(str[pos]))
            return false;
    return true;

}
bool isFloat(string &str){

    bool dot = false;
    for (uint pos = 0; pos < str.size(); pos++){
        if (!isdigit(str[pos]) &&
            ('.' != str[pos] ||
                ('.' == str[pos] && true == dot)
            )
        )
            return false;
        if ('.' == str[pos])
            dot = true;
    }
    return true;

}
bool isYesOrNo(string &str){
    return
        (string("yes") == str ||
         string("no")  == str ||
         string("1")   == str ||
         string("0")   == str
        ) ? true : false;
}
bool getYesOrNo(string &str){

    return
        (string("yes") == str || string("1") == str) ?
        true : false;

}
bool isValidColor(string &str){

    if (str.size() != 7 || str[0] != '#')
        return false;

    int i;
    return from_string<int>(i,  str.substr(1, 6), std::hex) ? true : false;

}
/**
 *
 * @param str has format #RRGGBB. with # at the begin
 * @return IColor
 */
IColor parseColor(string &str){

    IColor color;
    from_string<int>(color.red,   str.substr(1, 2), std::hex);
    from_string<int>(color.green, str.substr(3, 2), std::hex);
    from_string<int>(color.blue,  str.substr(5, 2), std::hex);
    return color;

}

vector<string> getLineTokens(string &str, char delim = ','){

    vector<string> res;
    string s;
    for(uint i = 0; i < str.size(); i++){
        char ch = str[i];
        if (ch == delim){
            res.push_back(s);
            s = "";
            continue;
        }
        s.push_back(ch);
    }
    res.push_back(s);
    return res;

}


IConfig * IConfig::instance = NULL;
string IConfig::config_filename = "";


/**
* Allow to define a file to read config from.
*/
void IConfig::setConfigFileName(string s){
    if (!s.size())
        return;
    config_filename = s;

}

IConfig::IConfig():

    icon_spacing(5),
    icon_min_width(24),
    icon_max_width(48),
    icon_min_height(24),
    icon_max_height(48),
    icon_maximize_threshold(0.9),

    zoom_recreate_icon_delta(0),

    display_sticky_windows(true),
    display_shaded_windows(true),
    display_workspace_number(false),

    ipager_window_x(0),
    ipager_window_y(0),

    workspace_number_color(255,255,255),
    active_window_border_color(255,0,0),
    window_border_color(204,204,204),

    selection_color(0,255,0),
    urgency_color(0,255,0),

    zoom_type("over"),

    display_window_icon("no"),

    in_slit(false)

{

    string dirname = getenv("HOME") + string("/.ipager/");
    if (!config_filename.size())
        config_filename = dirname + string("ipager.conf");

    struct stat buf;

    /*  IPager dir */
    if (stat(dirname.c_str(), &buf) != 0) {
        if (mkdir(dirname.c_str(), 0700) != 0){
            cerr << "Could not create config directory: " << dirname << endl;
            exit(0);
        }
    }

    /* IPager config file */
    if (stat(config_filename.c_str(), &buf) != 0) {
        // make default config file
        ofstream def_config(config_filename.c_str());
        if (!def_config){
            cerr << "Could not create default config file: " << config_filename << endl;
            exit(0);
        }
        def_config
            << "icon.spacing:      5"  << endl
            << "icon.min_width:    24" << endl
            << "icon.max_width:    48" << endl
            << "icon.min_height:   24" << endl
            << "icon.max_height:   48" << endl
            << "icon.maximize_threshold: 0.9"         << endl
            << endl
            << endl
            << "#"                                    << endl
            << "# IPager window position"             << endl
            << "#"                                    << endl
            << "ipager.window.x:  0"                  << endl
            << "ipager.window.y:  0"                  << endl
            << endl
            << "# should IPager starts in slit?"      << endl
            << "ipager.in_slit: no"                   << endl
            << endl
            << endl
            << "#"                                    << endl
            << "#"                                    << endl
            << "display_sticky_windows:   yes"        << endl
            << "display_shaded_windows:   yes"        << endl
            << endl
            << "# [ yes | no | mouseOver ]"           << endl
            << "display_window_icon:      no"         << endl
            << endl
            << endl
            << endl
            << "#"                                    << endl
            << "# Button to switch workspaces"        << endl
            << "#"                                    << endl
            << "#" << endl
            << "# [ left | right | middle | any ]"    << endl
            << "# or "                                << endl
            << "# set of buttons like: "              << endl
            << "#    left, right "                    << endl
            << "#    middle, right"                   << endl
            << "#"                                    << endl
            << "switch_workspace.button: any"         << endl
            << endl
            << endl
            << "# "                                   << endl
            << "mouse.scroll.up: nextWorkspace"       << endl
            << "mouse.scroll.down: prevWorkspace"     << endl
            << endl
            << endl
            << "#"                                    << endl
            << "# Delta (in pixels)"                  << endl
            << "#"                                    << endl
            << "# when an workspace icon changes its size"       << endl
            << "# IPager compare new values and previous."       << endl
            << "# If they differ less then 'zoom.recreate_icon_delta' then"       << endl
            << "# IPager continues to use an old icon and just zoom it."          << endl
            << "# If the sizes differ more 'zoom.recreate_icon_delta's value,"    << endl
            << "# then IPager creates a new icon picture."                        << endl
            << "# it is not very efficient to create icons often."                << endl
            << "#"                                                                << endl
            << "zoom.recreate_icon_delta: 0"                                      << endl
            << endl
            << endl
            << endl
            << "# Defines style of zooming icons. Should an icon spacing be expandig or " << endl
            << "# an active workspace icon lays over other (cross them)? "        << endl
            << "#"                                                                << endl
            << "#   [zoomAndExpand | over] "                    << endl
            << "#"                                              << endl
            << "zoom.type: over"                                << endl
            << endl
            << endl
            << endl
            << "display_workspace_number: no"                   << endl
            << "workspace_number.color:   #FFFF00"              << endl
            << endl
            << "ttf_font_path: /usr/share/fonts/TTF"            << endl
            << "ttf_font: Vera/14"                              << endl
            << endl
            << endl
            << endl
            << "#"                                              << endl
            << "# Background image for IPager window"           << endl
            << "#"                                              << endl
            << "#ipager.background.image: /path/to/image.png"   << endl
            << endl
            << endl
            << "#"                                              << endl
            << "# Colors (#RRGGBB)"                             << endl
            << "#"                                              << endl
            << "ipager.background.color:"                       << endl
            << "ipager.border.color:"                           << endl
            << endl
            << "workspace.background.color:"                    << endl
            << "workspace.border.color:"                        << endl
            << "active_workspace.background.color:"             << endl
            << "active_workspace.border.color:"                 << endl
            << endl
            << endl
            << "window.background.color:        #A47D73"        << endl
            << "window.border.color:"                           << endl
            << "active_window.background.color: #F09029"        << endl
            << "active_window.border.color:     #FFFFFF"        << endl
            << endl
            << endl
            << "selection_color: #FF0000"                       << endl
            << endl
            << endl
            << endl
            ;
    }

    ifstream config_file(config_filename.c_str());
    if (!config_file){
        cerr << "Could not open config file: " << config_filename << endl;
    }

    /**
    * Default values
    */

    // What button switches workspace
    switch_workspace_button.push_back(1);



    /**
    * Read Users preference
    */
    while(config_file && !config_file.eof()){

        string linebuffer;
        getline(config_file, linebuffer);
        if ('#' == linebuffer[0] || '!' == linebuffer[0])
            continue;

        // parse line into parts
        int index = 0;
        vector <string> vec;
        string s;
        for (uint pos = 0; pos < linebuffer.size(); pos++){
            char ch = linebuffer[pos];
            if (' ' == ch || '\n' == ch || '\t' == ch)
                continue;
            if (':' == ch){
                index++;
                vec.push_back(s);
                s = "";
                continue;
            }
            s.push_back(ch);
        }
        vec.push_back(s);

        // we need keyword and value
        if (vec.size() != 2)
            continue;

        // accept params
        if (isNumer(vec[1])){

            int value = 0;
            if (!from_string<int>(value, string(vec[1]), std::dec))
                continue;

            if (string("icon.spacing") == vec[0]){
                icon_spacing = value;

            } else if (string("icon.min_width") == vec[0]){
                icon_min_width = value;

            } else if (string("icon.max_width") == vec[0]){
                icon_max_width = value;

            } else if (string("icon.min_height") == vec[0]){
                icon_min_height = value;

            } else if (string("icon.max_height") == vec[0]){
                icon_max_height = value;

            } else if (string("ipager.window.x") == vec[0]){
                ipager_window_x = value;

            } else if (string("ipager.window.y") == vec[0]){
                ipager_window_y = value;

            } else if (string("display_sticky_windows") == vec[0] && isYesOrNo(vec[1])){
                display_sticky_windows = getYesOrNo(vec[1]);

            } else if (string("display_shaded_windows") == vec[0] && isYesOrNo(vec[1])){
                display_shaded_windows = getYesOrNo(vec[1]);

            } else if (string("zoom.recreate_icon_delta") == vec[0]){
                zoom_recreate_icon_delta = value;

            }


        } else if (isFloat(vec[1])){
        // float params

            if (string("icon.maximize_threshold") == vec[0])
                icon_maximize_threshold = strtof(vec[1].c_str(), NULL);


        } else {
        // string params

            if (string("display_sticky_windows") == vec[0] && isYesOrNo(vec[1])){
                display_sticky_windows = getYesOrNo(vec[1]);

            } else if (string("display_shaded_windows") == vec[0] && isYesOrNo(vec[1])){
                display_shaded_windows = getYesOrNo(vec[1]);

            } else if (string("display_workspace_number") == vec[0] && isYesOrNo(vec[1])){
                display_workspace_number = getYesOrNo(vec[1]);

            } else if (string("active_workspace.background.color") == vec[0] && isValidColor(vec[1])){
                active_workspace_background_color = parseColor(vec[1]);

            } else if (string("active_workspace.border.color") == vec[0] && isValidColor(vec[1])){
                active_workspace_border_color = parseColor(vec[1]);

            } else if (string("workspace.background.color") == vec[0] && isValidColor(vec[1])){
                workspace_background_color = parseColor(vec[1]);

            } else if (string("workspace.border.color") == vec[0] && isValidColor(vec[1])){
                workspace_border_color = parseColor(vec[1]);

            } else if (string("switch_workspace.button") == vec[0]){

                switch_workspace_button.clear();
                vector<string> tokens = getLineTokens(vec[1]);
                for(uint i = 0; i < tokens.size(); i++){
                    if (string("left") == tokens[i]){
                        switch_workspace_button.push_back(1);
                    } else if (string("middle") == tokens[i]){
                        switch_workspace_button.push_back(2);
                    } else if (string("right") == tokens[i]){
                        switch_workspace_button.push_back(3);
                    } else if (string("any") == tokens[i]){
                        switch_workspace_button.push_back(1);
                        switch_workspace_button.push_back(2);
                        switch_workspace_button.push_back(3);
                    }
                }

            } else if (string("active_window.background.color") == vec[0] && isValidColor(vec[1])){
                active_window_background_color = parseColor(vec[1]);

            } else if (string("active_window.border.color") == vec[0] && isValidColor(vec[1])){
                active_window_border_color = parseColor(vec[1]);

            } else if (string("window.background.color") == vec[0] && isValidColor(vec[1])){
                window_background_color = parseColor(vec[1]);

            } else if (string("window.border.color") == vec[0] && isValidColor(vec[1])){
                window_border_color = parseColor(vec[1]);

            } else if (string("ipager.background.color") == vec[0] && isValidColor(vec[1])){
                ipager_background_color = parseColor(vec[1]);

            } else if (string("ipager.border.color") == vec[0] && isValidColor(vec[1])){
                ipager_border_color = parseColor(vec[1]);

            } else if (string("ipager.background.image") == vec[0]){
                ipager_background_image = vec[1];

            } else if (string("zoom.type") == vec[0]){
                zoom_type = vec[1];

            } else if (string("ttf_font") == vec[0]){
                font_name = vec[1];

            } else if (string("ttf_font_path") == vec[0]){
                font_path = vec[1];

            } else if (string("workspace_number.color") == vec[0] && isValidColor(vec[1])){
                workspace_number_color = parseColor(vec[1]);

            } else if (string("selection_color") == vec[0] && isValidColor(vec[1])){
                selection_color = parseColor(vec[1]);

            } else if (string("display_window_icon") == vec[0]){
                if (isYesOrNo(vec[1]))
                   display_window_icon = getYesOrNo(vec[1]) ? "yes" : "no";
                if (string("mouseOver") == vec[1])
                    display_window_icon = "mouseOver";

            } else if (string("ipager.in_slit") == vec[0] && isYesOrNo(vec[1])){
                in_slit = getYesOrNo(vec[1]);

            } else if (string("mouse.scroll.up") == vec[0]){

                if (string("nextWorkspace") == vec[1])
                    scroll_action.addAction(4, new NextWorkspaceAction());
                else if (string("prevWorkspace") == vec[1])
                    scroll_action.addAction(4, new PrevWorkspaceAction());

            } else if (string("mouse.scroll.down") == vec[0]){

                if (string("nextWorkspace") == vec[1])
                    scroll_action.addAction(5, new NextWorkspaceAction());
                else if (string("prevWorkspace") == vec[1])
                    scroll_action.addAction(5, new PrevWorkspaceAction());

            }


        } // end of params


    } // read file cycle


}


IConfig::~IConfig()
{
}


