#ifndef __INTRO_SCENE_H__
#define __INTRO_SCENE_H__

#include "cocos_framework.h"
#include "IntroBg.h"
#include "IntroUI.h"
#include "Hero.h"

class IntroScene : public Scene
{
 public:
  static Scene* createScene();
  virtual bool init();
  CREATE_FUNC(IntroScene);
  void onEnter();
  void onExit();
  bool onTouchBegan(Touch* touch, Event* event);
  void onTouchEnded(Touch* touch, Event* event);
 private:
  IntroBg* _introBg;
  void changeBg15AgeToVolcano(float time);
  EventListenerTouchOneByOne* _listener;
  IntroUI* _introUI;
  void titleSkyFlow(float time);
  void flippedX();
  void moveBy();
  bool _isPlay;
  bool _isPaladog;
  bool _isCampaign;
  bool _isSurvival;
  bool _isExit;
  bool _isDataSlot;
  bool _isStart;
  Label* _lvLabel;
  Label* _goldLabel;
  Label* _hourLabel;
  Label* _minitueLabel;
  Label* _secondLabel;
  Label* _startLabel;
  int _bgStageSound;
  int _startSound;
  int _okButtonSound;
  int _unitActiveSound;
};

#endif // __INTRO_SCENE_H__
