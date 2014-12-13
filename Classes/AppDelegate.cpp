#include "AppDelegate.h"
#include "SceneStart.h"

USING_NS_CC;

AppDelegate::AppDelegate() 
{
}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::createWithRect("Ding Ding",Rect(0,0,800,480),1.0f);  
        director->setOpenGLView(glview);
    }
    glview->setDesignResolutionSize(800,480,ResolutionPolicy::EXACT_FIT);
    // turn on display FPS 
    director->setDisplayStats(true);
    // set FPS. the default value is 1.0/60 if you don't call this 
    director->setAnimationInterval(1.0 / 60);
    // create a scene. it's an autorelease object 
    auto scene = SceneStart::createScene();
    // run 
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
