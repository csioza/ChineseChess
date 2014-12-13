#include "SceneGame.h"

SceneGame::SceneGame()
{
}

Scene* SceneGame::createScene(bool red)
{
    auto scene = Scene::create();
    auto layer = SceneGame::create(red);
    scene->addChild(layer);

    return scene;
}

//自定义create函数 
SceneGame* SceneGame::create(bool red)
{
    SceneGame* pRet = new SceneGame();
    if(pRet)
    {
        if(pRet->init(red))
        {
            pRet->autorelease();
        }
        else
        {
            pRet->release();
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    return pRet;
}

bool SceneGame::init(bool red)
{
    //调用父类Layer 
    if ( !Layer::init() )
    {
        return false;
    }
    auto winSize = Director::getInstance()->getWinSize();
    //设置棋盘的偏移值 
    this->_plateOffset = ccp(20,10);
    //设置棋子的偏移值 
    this->_stoneOffset = ccp(60, 33);
    //设置棋子的直径为46 
	this->_d = 46;
    //初始化时，没有选中棋子 
    _selectid = -1;
    //走棋时，切换棋子的颜色 
    _redTrun = true;
    //棋盘上玩家摆红旗 
    _redSide = red;
    //创建桌子 
    auto desk = Sprite::create("floor.jpg");
    this->addChild(desk);
    //设置桌子的位置 
    desk->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    //压缩桌子 
    desk->setScaleX(winSize.width / desk->getContentSize().width);
    desk->setScaleY(winSize.height / desk->getContentSize().height);
    //创建棋盘 
    auto plate = Sprite::create("background.png");
    this->addChild(plate);
    //设置描点为(0,0) 
    plate->setAnchorPoint(Vec2::ZERO);
    //设置棋盘的位置 
    plate->setPosition(_plateOffset);
    //压缩棋盘：(窗口的高度 - 偏移的y坐标 * 2) / 图片的高度 
    plate->setScale((winSize.height -_plateOffset.y *2)/ plate->getContentSize().height);
    //摆棋子 
    for(int i=0; i<32; i++)
    {
        //创建棋子 
        _s[i] = Stone::create(i, red);
        addChild(_s[i]);
        //设置棋子的初始位置为随机位置 
        _s[i]->setPosition(ccp(CCRANDOM_0_1() * winSize.width, CCRANDOM_0_1() * winSize.height));
        //隐藏棋子 
        _s[i]->setVisible(false);
    }
    //创建一个选择框 
    //当选中某个棋子的时候,选择框会套在选好的棋子上 
    _selectSprite = Sprite::create("selected.png");
    addChild(_selectSprite);
    //隐藏选择框 
    _selectSprite->setVisible(false);
    //设置选择框的优先级 
    _selectSprite->setZOrder(1000);
    //压缩选择框 
    _selectSprite->setScale(.8f);
    //创建Menu 
    auto menu = Menu::create();
    this->addChild(menu);
    //创建开始按钮 
    auto itemStart = MenuItemImage::create("start.jpg", "start.jpg", this, menu_selector(SceneGame::Start));
    menu->addChild(itemStart);
    itemStart->setPositionX(190);
    itemStart->setPositionY(120);
    //创建新局按钮 
    auto itemNew = MenuItemImage::create("new.jpg", "new.jpg", this, menu_selector(SceneGame::New));
    menu->addChild(itemNew);
    itemNew->setPositionX(itemStart->getPositionX());
    itemNew->setPositionY(itemStart->getPositionY() + 60);
    //创建悔棋按钮 
    auto item = MenuItemImage::create("regret.jpg", "regret.jpg", this, menu_selector(SceneGame::Back));
    menu->addChild(item);
    item->setPositionX(itemStart->getPositionX());
    item->setPositionY(itemStart->getPositionY() - 60);
    //创建暂停按钮 
    auto itemPause = MenuItemImage::create("pause.jpg", "pause.jpg", this, menu_selector(SceneGame::Pause));
    menu->addChild(itemPause);
    itemPause->setPositionX(itemStart->getPositionX());
    itemPause->setPositionY(itemStart->getPositionY() - 60 - 60);
    //创建难度按钮 
    auto itemDifficulty = MenuItemImage::create("difficulty.jpg", "difficulty.jpg", this, menu_selector(SceneGame::Difficulty));
    menu->addChild(itemDifficulty);
    itemDifficulty->setPositionX(itemStart->getPositionX());
    itemDifficulty->setPositionY(itemStart->getPositionY() - 60 - 60 - 60);
    //创建播放背景音乐按钮 
    auto itemVoice = MenuItemImage::create("openVolice.png", "closeVolice.png", this, menu_selector(SceneGame::Voice));
    menu->addChild(itemVoice);
    itemVoice->setPositionX(itemStart->getPositionX());
    itemVoice->setPositionY(itemStart->getPositionY() - 60 - 60 - 60 - 60);

    //CCLog("x=%lf", itemStart->getPositionX());
    //CCLog("y=%lf", itemStart->getPositionY() - 240);

    //创建一个字体,并且以此为字体的文字  
    auto label = LabelTTF::create("Voice", "Arial", 25);  
    addChild(label);
    //设置文字的位置  
    label->setPosition(ccp(winSize.width/2 + 120, winSize.height/2 - 120));
    //设置文字的颜色 
    label->setColor(ccc3(0, 0, 0));
    //创建数组 
    _steps = Array::create();
    _steps->retain();
    //创建精灵显示游戏结果 
    sprite = Sprite::create("yingjiemian.png");
    addChild(sprite);
    sprite->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    //隐藏结果 
    sprite->setVisible(false);
    visible = false;

    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch *t,Event *e){
        Object* obj = (Object*)t;
        //获取触摸点的窗口坐标 
        Point ptInWin = t->getLocation();
        if(sprite->boundingBox().containsPoint(ptInWin) && visible == true)  
        {  
            //隐藏游戏结果 
            sprite->setVisible(false);
            visible = false;
            New(obj);
        }
        int x, y;//保存触摸点的棋盘坐标 
        //通过触摸点的窗口坐标获取棋盘的x坐标和y坐标 
        if(!getClickPos(ptInWin, x, y))
        {
            return false;
        }
        //通过触摸点在棋盘中的坐标获取选中的棋子的id 
        int clickid = getStone(x, y);
        //当触摸点的位置上有棋子的时候,clickid为选中的棋子的id,表示玩家在选棋 
        //当触摸点的位置上没有棋子的时候,clickid为-1,表示玩家在走棋 

        //-1 == _selectid表示没有选中棋子 
        if(-1 == _selectid)
        {
            setSelectId(clickid);
        }
        else
        {
            //移动棋子 
            //第一个参数：移动的棋子的id 
            //第二个参数：通过触摸点的位置判断触摸点上是否有棋子 
            //第三个参数：触摸点的x坐标 
            //第四个参数：触摸点的y坐标 
            //moveStone执行了两个步骤选棋和走棋 
            //选棋子：当_selectid == clickid时，表示选定了id为_selectid的棋子 
            //走棋子：当selectid != clickid时， 表示将id为_selectid的棋子移动到(x,y)所在的位置上 
            moveStone(_selectid, clickid, x, y);
        }

        return true;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,this);

    return true;
}

//得到鼠标点击在棋盘上的坐标点 
//当鼠标点击的位置在棋盘外返回false 
//通过窗口坐标获得棋盘坐标 
bool SceneGame::getClickPos(Point ptInWin, int &x, int &y)
{
    for(x=0; x<=8; x++)
    {
        for(y=0; y<=9; y++)
        {
            //计算棋盘上的格子在窗口上的位置 
            Point ptInPlate = getStonePos(x, y);
            // CCLog("ptInPlate.x=%lf   ptInPlate.y=%lf", ptInPlate.x,  ptInPlate.y);
            //寻找与鼠标点击的位置距离小于棋子的半径的格子 
            //如果找到了,return true,否则返回 return false 
            if(ptInWin.getDistance(ptInPlate) < _d / 2)
            {
                return true;
            }
        }
    }

    return false;
}

//通过坐标的下标获取棋子的ID 
//如果坐标上没有棋子,返回-1 
int SceneGame::getStone(int x, int y)
{
    Stone* s;
    //遍历32个棋子 
    for(int i=0; i<32; i++)
    {
        s = _s[i];
        if(s->getX() == x && s->getY() == y && !s->getDead())
        {
            //得到棋子的ID 
            return s->getID();
        }
    }

	return -1;
}

//更换选择框 
void SceneGame::setSelectId(int id)
{
    if(-1 == id)
    {
        return;
    }
    //如果没有选中红棋 
    if(_s[id]->getRed() != _redTrun)
    {
        return;
    }
    //_selectid为选中的棋子的id 
    _selectid = id;
    //显示选择框 
    _selectSprite->setVisible(true);
    //选择框在被选中的棋子上显示 
    _selectSprite->setPosition(_s[_selectid]->getPosition());
}

//移动棋子 
//第一个参数：移动的棋子的id 
//第二个参数：通过触摸点的位置判断触摸点上是否有棋子 
//第三个参数：触摸点的x坐标 
//第四个参数：触摸点的y坐标 
void SceneGame::moveStone(int moveId, int killId, int x, int y)
{
    //killId != -1表示触摸点的位置上有一个棋子 
    //_s[moveId]->getRed() == _s[killId]->getRed()表示触摸点上 
    //的棋子和走棋的棋子的颜色相同 
    if(killId != -1 && _s[moveId]->getRed() == _s[killId]->getRed())
    {
        //更换选择框 
        setSelectId(killId);
        return;
    }
    //走棋规则 
    bool bCanMove =  canMove(moveId, killId, x, y);
    //如果bCanMove为false 
    //不能走棋 
    if(false == bCanMove)
    {
        return;
    }
    //走棋之前记录棋子的信息 
    //第一个参数：需要移动的棋子的id 
    //第二个参数：通过触摸点的位置判断触摸点上是否有棋子 
    //第三个参数：棋子当前的位置的x坐标 
    //第四个参数：棋子当前的位置的y坐标 
    //第五个参数：棋子移动后的位置的x坐标 
    //第六个参数：棋子移动后的位置的y坐标 
    Step* step = Step::create(moveId, killId, _s[moveId]->getX(), _s[moveId]->getY(), x, y);
    //将棋子的信息添加到数组中 
    _steps->addObject(step);
    //设置棋子的坐标(移动棋子) 
    _s[moveId]->setX(x);
    _s[moveId]->setY(y);
    //_s[moveId]->setPosition(getStonePos(x,y));
    //SetRealPos(_s[moveId]);
    //设置移动棋子时的动作 
    auto move = MoveTo::create(.5f, getStonePos(x, y));
    //动作回调 
    auto call = CCCallFuncND::create(this, callfuncND_selector(SceneGame::moveComplete), (void*)(intptr_t)killId);
    //设置动作的执行顺序(先移动棋子,后调用回调函数) 
    auto seq = Sequence::create(move, call, NULL);
    //设置移动的棋子的优先级 
    _s[moveId]->setZOrder(_s[moveId]->getZOrder() + 1);
    //执行棋子移动的动作 
    _s[moveId]->runAction(seq);
}

//将棋盘坐标转换成窗口坐标 
CCPoint SceneGame::getStonePos(int x, int y)
{
    int xx = x * _d;
    int yy = y * _d;

    return ccp(xx, yy) + _stoneOffset;
}

//实现悔棋 
void SceneGame::Back(Object*)
{
    //当数组中的元素个数为0时 
    //没走棋 
    if( 0 == _steps->count() )
    {
        return;
    }
    //获取数组中的最后一个元素 
    //获取走棋时的最后一步棋子的信息 
    Step* step = (Step*)_steps->lastObject();
    //恢复棋子的信息 
    //设置棋子走棋前的位置x坐标 
    _s[step->_moveid]->setX(step->_xFrom);
    //设置棋子走起前的位置y坐标 
    _s[step->_moveid]->setY(step->_yFrom);
    _s[step->_moveid]->setPosition(getStonePos(step->_xFrom, step->_yFrom));
    //恢复吃掉的棋子 
    if(step->_killid != -1)
    {
        //显示吃掉的棋子 
        _s[step->_killid]->setVisible(true);

        //复活吃掉的棋子 
        _s[step->_killid]->setDead(false);
    }
    //移动了一步棋后 
    //切换移动的棋子的颜色 
    _redTrun = ! _redTrun;
    //删除数组中的最后一个元素 
    //删除走棋时最后一步棋子的信息 
    _steps->removeLastObject();
}

 //实现开始 
void SceneGame::Start(Object*)
{
    //摆棋子 
    for(int i=0; i<32; i++)
    {
        //显示棋子 
        _s[i]->setVisible(true);
        //将棋子移动到棋盘上指定的位置 
        auto move = MoveTo::create(1, this->getStonePos(_s[i]->getX(), _s[i]->getY()));
        _s[i]->runAction(move);
    }
}

//实现新局 
void SceneGame::New(Object* obj)
{
    //得到窗口的大小 
    auto winSize = Director::getInstance()->getWinSize();
    //走了多少步棋就悔多少步棋子 
    for(int i = _steps->count(); i>0; i--)
    {
        Back(obj);
    }
    for(int i=0; i<32; i++)
    {
        //设置棋子的初始位置为随机位置 
        _s[i]->setPosition(ccp(CCRANDOM_0_1() * winSize.width, CCRANDOM_0_1() * winSize.height));
        //隐藏棋子 
        _s[i]->setVisible(false); 
    }
}

//实现暂停 
void SceneGame::Pause(Object*)
{
}

//实现难度 
void SceneGame::Difficulty(Object*)
{
}

//播放背景音乐 
void SceneGame::Voice(Object*)
{
    static int i = 0;
    if(0 == i)
    {
        //播放背景音乐 
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("floor.wav",true);
        i++;
    }
    else
    {
        //停止播放背景音乐 
        CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
        i--;
    }
}

void SceneGame::moveComplete(Node* movestone, void* _killid)
{
    //得到窗口的大小 
    auto winSize = Director::getInstance()->getWinSize();
    //设置优先级 
    movestone->setZOrder(movestone->getZOrder() - 1);
    int killid =  (int)(intptr_t)_killid;
    //如果触摸点上有棋子 
    if(-1 != killid)
    {
        //杀掉触摸点上的棋子 
        _s[killid]->setDead(true);
        //隐藏杀掉的棋子 
        _s[killid]->setVisible(false);
        //当杀掉将的时候,重新开始 
        if(Stone::JIANG  == _s[killid]->getType())
        {
            //显示游戏结果 
            sprite->setVisible(true);
            //设置优先级 
            sprite->setZOrder(1000);
            visible = true;
        }
    }
    //没有选中棋子 
    _selectid = -1;
    //隐藏选择框 
    _selectSprite->setVisible(false);
    //移动了一步棋后 
    //切换移动的棋子的颜色 
    _redTrun = ! _redTrun;
}

//走棋规则 
bool SceneGame::canMove(int moveid, int killid, int x, int y)
{
    //获得选中的棋子 
    Stone* s = _s[moveid];
    //棋子的类型 
    switch(s->getType())
    {
    //将的走棋规则 
    case Stone::JIANG:
        {
            return canMoveJiang(moveid, killid, x, y);
        }
        break;
    //士的走棋规则 
    case Stone::SHI:
        {
            return canMoveShi(moveid, x, y);
        }
        break;
    //相的走棋规则 
    case Stone::XIANG:
        {
            return canMoveXiang(moveid, x, y);
        }
        break;
    //车的走棋规则 
    case Stone::CHE:
        {
            return canMoveChe(moveid, x, y);
        }
        break;
    //马的走棋规则 
    case Stone::MA:
        {
            return canMoveMa(moveid, x, y);
        }
        break;
    //炮的走棋规则 
    case Stone::PAO:
        {
            return canMovePao(moveid, killid, x, y);
        }
        break;
    //兵的走棋规则 
    case Stone::BING:
        {
            return canMoveBing(moveid, x, y);
        }
        break;
    default:
        break;
    }

    return false;
}

//将的走棋规则 
bool SceneGame::canMoveJiang(int moveid, int killid, int x, int y)
{
    Stone* skill = _s[killid];
    //将的走棋规则：
    //1、一次走一格 
    //2、不能出九宫格 

    //将的对杀 
    if(skill->getType() == Stone::JIANG)
    {
        return canMoveChe(moveid, x, y);
    }
    //通过棋子的ID得到棋子 
    Stone* s = _s[moveid];
    //获得将当前的位置 
    int xo = s->getX();
    int yo = s->getY();
    //获得将走的格数 
    //(x,y)表示将走到的位置 
    int xoff = abs(xo - x);
    int yoff = abs(yo - y);
    int d = xoff*10 + yoff;
    //走将的时候有两种情况 
    //xoff=1, yoff=0：将向左或向右 
    //xoff=0, yoff=1：将向前或向后 
    if(d != 1 && d != 10)
    {
        return false;
    }
    //判断将是否出了九宫 
    //红色的将和黑色的将的x坐标的范围都是3<=x<=5 
    if(x<3 || x>5)
    {
        return false;
    }
    //如果玩家的棋子是红棋 
    if(_redSide == s->getRed())
    {
        //判断将是否出了九宫 
        if(y<0 || y>2)
        {
            return false;
        }
    }
    else//判断黑色的将的范围 
    {
        //判断将是否出了九宫 
        if(y>9 || y<7)
        {
            return false;
        }
    }

    return true;
}


//士的走棋规则 
bool SceneGame::canMoveShi(int moveid, int x, int y)
{
    //士的走棋规则：
    //1、一次走一格 
    //2、不能出九宫格 
    //3、斜着走 
    // 
    //通过棋子的ID得到棋子 
    Stone* s = _s[moveid];
    //获得相走棋前的位置 
    int xo = s->getX();
    int yo = s->getY();
    //获得相走的格数 
    //(x,y)表示将走到的位置 
    int xoff = abs(xo - x);
    int yoff = abs(yo - y);
    int d = xoff*10 + yoff;
    //士每走一步x方向走1格,y方向走1格 
    //当走的格数大于1格时 
    //返回false 
    if(d != 11)
    {
        return false;
    }
    //判断士是否出了九宫 
    //红色的士和黑色的士的x坐标的范围都是3<=x<=5 
    if(x<3 || x>5)
    {
        return false;
    }
    //如果玩家的棋子是红棋 
    if(_redSide == s->getRed())
    {
        //判断士是否出了九宫 
        if(y<0 || y>2)
        {
            return false;
        }
    }
    else//判断黑色的士的范围 
    {
        //判断士是否出了九宫 
        if(y>9 || y<7)
        {
            return false;
        }
    }

    return true;
}

//相的走棋规则 
bool SceneGame::canMoveXiang(int moveid, int x, int y)
{
    //相的走棋规则： 
    //每走一次x移动2格,y移动2格 
    //不能过河 
    // 
    //通过棋子的ID得到棋子 
    Stone* s = _s[moveid];
    //获得相走棋前的位置 
    int xo = s->getX();
    int yo = s->getY();
    //获得相走的格数 
    //(x,y)表示将走到的位置 
    int xoff = abs(xo - x);
    int yoff = abs(yo - y);
    int d = xoff*10 + yoff;
    //相每一次x方向走2格子,y方向走2格 
    //当走的格数大于2格时 
    //返回false 
    if(d != 22)
    {
        return false;
    }
    //计算两个坐标的中点坐标 
    int xm = (xo + x) / 2;
    int ym = (yo + y) / 2;
    //得到(xm,ym)上的棋子 
    int id = getStone(xm, ym);
    //当(xm,ym)上有棋子的时候 
    if(id != -1)
    {
        //不能走相 
        return false;
    }
    //限制相不能过河 
    //如果玩家的棋子是红棋 
    if(_redSide == s->getRed())
    {
        //判断相是否过了河 
        if(y > 4)
        {
            return false;
        }
    }
    else//判断黑色的相的范围 
    {
        //判断相是否过了河 
        if(y < 5)
        {
            return false;
        }
    }
    return true;
}

//车的走棋规则 
bool SceneGame::canMoveChe(int moveid, int x, int y)
{
    //通过棋子的ID得到棋子 
    Stone* s = _s[moveid];
    //获得车走棋前的位置 
    int xo = s->getX();
    int yo = s->getY();
    //当两点之间有棋子的时候车不能走 
    if(getStoneCount(xo,yo,x,y) != 0)
    {
        return false;
    }
    return true;
}

//马的走棋规则 
bool SceneGame::canMoveMa(int moveid, int x, int y)
{
    //通过棋子的ID得到棋子 
    Stone* s = _s[moveid];
    //获得马走棋前的位置 
    int xo = s->getX();
    int yo = s->getY();
    //CCLog("xo=%d", xo);
    //CCLog("yo=%d", yo);
    //获得马走的格数 
    //(x,y)表示马走到的位置 
    //马有两种情况：
    //第一种情况：马先向前或向后走1步，再向左或向右走2步 
    //第二种情况：马先向左或向右走1不，再向前或向后走2步 
    int xoff = abs(xo-x);
    int yoff = abs(yo-y);
    //CCLog("x=%d", x);
    //CCLog("y=%d", y);
    int d = xoff*10 + yoff;
    //CCLog("d=%d", d);
    if(d != 12 && d != 21)     
    {
        return false;
    }
    int xm, ym;//记录绑脚点坐标 
    if(d == 12)//当马走的是第一种情况 
    {
        xm = xo;//绑脚点的x坐标为走棋前马的x坐标 
        ym = (yo + y) / 2;//绑脚点的y坐标为走棋前马的y坐标和走棋后马的y坐标的中点坐标 
    }
    else//当马走的是第二种情况 
    {
        xm = (xo + x) / 2;//绑脚点的x坐标为走棋前马的x坐标和走棋后马的x坐标的中点坐标 
        ym = yo;//绑脚点的y坐标为走棋前马的y坐标 
    }
    //CCLog("xm=%d", xm); 
    //CCLog("ym=%d", ym); 
    //当绑脚点有棋子时,不能走 
    if(getStone(xm, ym) != -1) 
    {
        return false;
    }
    return true;
}

//炮的走棋规则 
bool SceneGame::canMovePao(int moveid, int killid, int x, int y)
{
    //通过棋子的ID得到棋子 
    Stone* s = _s[moveid];
    //获得炮走棋前的位置 
    int xo = s->getX();
    int yo = s->getY();
    //当触摸点上有一个棋子 
    //而且两点之间只有一个棋子的时候 
    //炮吃掉触摸点上的棋子 
    if(killid != -1 && this->getStoneCount(xo,yo,x,y) == 1)
    {
        return true;
    }
    if(killid == -1 && this->getStoneCount(xo, yo, x, y) == 0) 
    {
        return true;
    }
    return false;
}

//兵的走棋规则 
bool SceneGame::canMoveBing(int moveid, int x, int y)
{
    //兵的走棋规则： 
    //1、一次走一格 
    //2、前进一格后不能后退 
    //3、过河后才可以左右移动 
    //通过棋子的ID得到棋子 
    Stone* s = _s[moveid];
    //获得将当前的位置 
    int xo = s->getX();
    int yo = s->getY();
    //获得兵走的格数 
    //(x,y)表示将走到的位置 
    int xoff = abs(xo - x);
    int yoff = abs(yo - y);
    int d = xoff*10 + yoff;
    //走将的时候有两种情况 
    //xoff=1, yoff=0：将向左或向右 
    //xoff=0, yoff=1：将向前或向后 
    if(d != 1 && d != 10)
    {
        return false;
    }
    //如果玩家的棋子是红棋 
    if(_redSide == s->getRed())
    {
        //限制红色的兵不能后退 
        if(y < yo)
        {
            return false;
        }
        //红色的兵没有过河不能左右移动 
        if(yo <= 4 && y == yo)
        {
            return false;
        }
    }
    else//判断黑色的兵 
    {
        //限制黑色的兵不能后退 
        if(y > yo)
        {
            return false;
        }
        //黑色的兵没有过河不能左右移动 
        if(yo >= 5 && y == yo)
        {
            return false;
        }
    }
    return true;
}

//计算(xo,yo)和(x,y)之间的棋子数 
//如果棋子数为-1,表示(xo,yo)和(x,y)不在一条直线上 
int SceneGame::getStoneCount(int xo, int yo, int x, int y)
{
    int ret = 0;//记录两点之间的棋子的个数 
    //(xo,yo)和(x,y)不在同一条直线上 
    if(xo != x && yo != y)
    {
        return -1;
    }
    //(xo,yo)和(x,y)在同一点上 
    if(xo == x && yo == y)
    {
        return -1;
    }
    //两点在同一条竖线上 
    if(xo == x)
    {
        //min为两个点中y坐标最小的点的y坐标 
        int min = yo < y ? yo : y;
        //max为两个点中y坐标最大的点的y坐标 
        int max = yo > y ? yo : y;
        //查找同一条竖线上两点之间的棋子数 
        for(int yy=min+1; yy<max; yy++)
        {
            //当两点之间有棋子的时候 
            if(getStone(x,yy) != -1)
            {
                ++ret;//棋子数加1 
            }
        }
    }
    else//两点在同一条横线上yo == y 
    {
         //min为两个点中x坐标最小的点的x坐标 
        int min = xo < x ? xo : x;
        //max为两个点中x坐标最大的点的x坐标 
        int max = xo > x ? xo : x;
        //查找同一条竖线上两点之间的棋子数 
        for(int xx=min+1; xx<max; xx++)
        {
            //当两点之间有棋子的时候 
            if(getStone(xx,y) != -1)
            {
                ++ret;//棋子数加1 
            }
        }
    }
    //返回两点之间的棋子数 
    return ret;
}
