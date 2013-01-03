
#include <iostream>
#include <map>
using namespace std;

#include "wm.h"

class Action {
public:
    Action(){};
    virtual ~Action(){};
    virtual bool activate() = 0;
};


class NextWorkspaceAction: public Action {
public:
    ~NextWorkspaceAction(){};
    bool activate(){
        int cur   = WM::Instance()->getCurrentWorkspaceID();
        int total = WM::Instance()->getWorkspaceCount();
        int to    = cur + 1;
        to = (to >= 0 && to < total) ? to : 0;
        WM::Instance()->changeCurrentWorkspaceID(to);
        return true;
    };
};


class PrevWorkspaceAction: public Action {
public:
    ~PrevWorkspaceAction(){};
    bool activate(){
        int cur   = WM::Instance()->getCurrentWorkspaceID();
        int total = WM::Instance()->getWorkspaceCount();
        int to    = cur - 1;
        to = (to >= 0 && to < total) ? to : (total - 1);
        WM::Instance()->changeCurrentWorkspaceID(to);
        return true;
    };
};




class ScrollAction {
    map<int, Action*> actions;
public:
    ScrollAction(){};
    ~ScrollAction(){
        map<int, Action*>::iterator it = actions.begin();
        for(; it != actions.end(); ++it)
            delete (*it).second;
    };
    void addAction(int button, Action *action){
        actions[button] = action;
        };
    void activate(int button){
        map<int, Action*>::iterator it = actions.find(button);
        if (it != actions.end())
            (*it).second->activate();
    };

};

