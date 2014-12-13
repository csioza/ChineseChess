#ifndef  _SceneStart_H_
#define _SceneStart_H_

#include "cocos2d.h"
USING_NS_CC;

class SceneStart : public Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(SceneStart);
    void update(float);
    //
    Sprite* _red;
    Sprite* _black;
    //标记是否选中了红色棋子 
    bool _selected;
};

#endif

