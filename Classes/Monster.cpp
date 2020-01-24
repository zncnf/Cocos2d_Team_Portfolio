#include "cocos_framework.h"
#include "Monster.h"

Monster::Monster(Scene* scene, Layer* layer, Hero* hero, vector<HeroUnit*> unit, Mob mob)
{
	_scene = scene;
	_layer = layer;
	_hero = hero;
	_unit = unit;

	cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("Player/effect/eff_blend_02.plist");
	switch (mob) {
	case 일반미라:
		cache->addSpriteFramesWithFile("Monster/monster/mummy.plist");
		cache->addSpriteFramesWithFile("Monster/monster/mummymove.plist");
		_monsterCode = "e07";
		_atkMaxFrame = 15;
		_atk = 20;
		_hpm = 20;
		_range = 100;
		_delay = 2.0f;
		_speed = 1.0f;
		_exp = 10;
		_money = 2;
		break;
	case 광부좀비:
		cache->addSpriteFramesWithFile("Monster/monster/Mzombie.plist");
		_monsterCode = "e13";
		_atkMaxFrame = 15;
		_atk = 10;
		_hpm = 50;
		_range = 60;
		_delay = 2.0f;
		_speed = 0.8f;
		_exp = 5;
		_money = 1;
		break;
	case 일반좀비:
		cache->addSpriteFramesWithFile("Monster/monster/Nzombie.plist");
		_monsterCode = "e01";
		_atkMaxFrame = 10;
		_atk = 20;
		_hpm = 1010;
		_range = 50;
		_delay = 2.0f;
		_speed = 0.8f;
		_exp = 5;
		_money = 1;
		break;
	case 핑크미라:
		cache->addSpriteFramesWithFile("Monster/monster/Pmummy.plist");
		cache->addSpriteFramesWithFile("Monster/monster/Pmummymove.plist");
		_monsterCode = "e27";
		_atkMaxFrame = 15;
		_atk = 20;
		_hpm = 100;
		_range = 100;
		_delay = 2.0f;
		_speed = 1.0f;
		_exp = 10;
		_money = 2;
		break;
	case 강화좀비:
		cache->addSpriteFramesWithFile("Monster/monster/Uzombie.plist");
		_monsterCode = "e21";
		_atkMaxFrame = 10;
		_atk = 10;
		_hpm = 50;
		_range = 50;
		_delay = 2.0f;
		_speed = 0.8f;
		_exp = 5;
		_money = 1;
		break;
	case 좀비킹:
		cache->addSpriteFramesWithFile("Monster/monster/Zomking.plist");
		_monsterCode = "b01";
		_atkMaxFrame = 30;
		_atk = 0;
		_hpm = 300;
		_range = 300;
		_delay = 2.0f;
		_speed = 1.0f;
		_exp = 20;
		_money = 20;
		break;
	}
	_hp = _hpm;
	_state = WALK;
	_moveChange = 1;
	_attackDelay = 0;
	_isAttack = false;
	_isRemove = false;
	_isSummonX = 0;
	_unitAttack = -1;

	int zorder = rand() % 30;
	_monster = Sprite::createWithSpriteFrameName(StringUtils::format("%s_walk_0001.png", _monsterCode));
	_monster->setPosition(1000, zorder + _hero->getHero()->getPositionY() + 17);
	_layer->addChild(_monster, _hero->getHero()->getZOrder() + 15 - zorder);
}

void Monster::MonsterMove()
{
	if (_attackDelay != 0) {
		_attackDelay += 0.01f;
		if (_attackDelay > _delay) {
			_attackDelay = 0;
			_moveChange = true;
			_state = WALK;
			log("STATE : %f / %f", _attackDelay, _delay);
		}
	}
	_isAttack = false;
	_isSummonX = 0;
	for (int i = 0; i < _unit.size(); i++) {
		if (_unit[i]->getSprite()->getPositionX() < _monster->getPositionX() &&
			_unit[i]->getSprite()->getPositionX() > _monster->getPositionX() - _range) {
			if (_state == WALK || (_state == ATTACK && _attackDelay == 0)) {
				_moveChange = true;
				_state = ATTACK;
				break;
			}
			_isAttack = true;
		}
	}
	if (_hero->getHero()->getPositionX() < _monster->getPositionX() &&
		_hero->getHero()->getPositionX() > _monster->getPositionX() - _range) {
		if (_state == WALK || (_state == ATTACK && _attackDelay == 0)) {
			_moveChange = true;
			_state = ATTACK;
		}
		_isAttack = true;
	}
	if (_state == ATTACK && !_moveChange && !_isAttack) {
		_moveChange = true;
		_state = WALK;
	}
	if (_state == WALK) {
		Walk();
	}
	if (_moveChange) {
		_moveChange = 0;
		_monster->cleanup();
		
		Vector<SpriteFrame*> frame, frame2;
		if (_state == WALK) {
			for (int i = 1; i <= 12; i++) {
				frame.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(StringUtils::format("%s_walk_%04d.png", _monsterCode, i)));
			}
			auto animation = Animation::createWithSpriteFrames(frame, 0.03f);
			auto animate = Animate::create(animation);
			auto action = RepeatForever::create(animate);
			_monster->runAction(action);
		}
		else if (_state == ATTACK)
		{
			_attackDelay++;
			for (int i = 1; i <= _atkMaxFrame; i++) {
				frame.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(StringUtils::format("%s_att_%04d.png", _monsterCode, i)));
			}
			auto animation = Animation::createWithSpriteFrames(frame, 0.03f);
			auto animate = Animate::create(animation);
			if (strcmp(_monsterCode, "b01") == 0) { //좀비킹 공격모션
				for (int i = 1; i <= 12; i++) {
					frame2.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(StringUtils::format("b01_wait_%04d.png", i)));
				}

				auto animation2 = Animation::createWithSpriteFrames(frame2, 0.03f);
				auto animate2 = Animate::create(animation2);

				auto action = RepeatForever::create(Sequence::create(
					animate,
					Spawn::create(animate2, CallFunc::create(CC_CALLBACK_0(Monster::ZomkingSummon, this)), nullptr),
					nullptr));
				_monster->runAction(action);

			}
			else {
				auto action = Sequence::create(
					animate,
					CallFunc::create(CC_CALLBACK_0(Monster::Attack, this)),

					nullptr);
				_monster->runAction(action);
			}
		}
		else if (_state == DEAD) {
			for (int i = 1; i <= 16; i++) {
				frame.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(StringUtils::format("%s_down_%04d.png", _monsterCode, i)));
			}
			auto animation = Animation::createWithSpriteFrames(frame, 0.03f);
			auto animate = Animate::create(animation);

			for (int i = 1; i <= 34; i++) {
				frame2.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(StringUtils::format("eff_die_%04d.png", i))); //eff_enemydie_%04d.png <- 몬스터
			}

			auto animation2 = Animation::createWithSpriteFrames(frame2, 0.03f);
			auto animate2 = Animate::create(animation2);

			auto action = Sequence::create(
				Spawn::create(animate, MoveBy::create(0.48, Vec2(70, 0)), nullptr),
				animate2,
				CallFunc::create(CC_CALLBACK_0(Monster::Dead, this)),
				nullptr
			);
			_monster->runAction(action);
		}
	}
}

void Monster::Hit(float atk)
{
	if (_hp == _hpm) {
		_monsterHpBar = ProgressTimer::create(Sprite::create("UI/MonsterUnitHpBar.png"));
		_monsterHpBar->setType(ProgressTimer::Type::BAR);
		_monsterHpBar->setPosition({ _monster->getContentSize().width / 2 - 3, _monster->getContentSize().height - 90 });
		_monsterHpBar->setMidpoint({ 0, 0 });
		_monsterHpBar->setBarChangeRate({ 1,0 });
		_monster->addChild(_monsterHpBar);

		_monsterHpBarBack = Sprite::create("UI/UnitHpBarBack.png");
		_monsterHpBarBack->setPosition({ _monster->getContentSize().width / 2 - 3, _monster->getContentSize().height - 90 });
		_monster->addChild(_monsterHpBarBack, -10);
	}
	if (_hp > 0) {
		_hp -= atk;
		_monsterHpBar->setPercentage((_hp / _hpm) * 100);
	}
	if (_hp <= 0 && _state != DEAD) {
		_hp = 0;
		_state = DEAD;
		_moveChange = true;
		_monster->removeChild(_monsterHpBar, 1);
		_monster->removeChild(_monsterHpBarBack, 1);
	}
}

void Monster::Walk()
{
	_monster->setPosition(_monster->getPosition() + Vec2(-_speed, 0));
	if (getMonster()->getPosition().x < 0) {
		_monster->setPositionX(1000);
	}
}

void Monster::Attack()
{
	float distance = _monster->getPositionX() - _hero->getHero()->getPositionX() < 0 ? 0 :
		_monster->getPositionX() - _hero->getHero()->getPositionX();
	int target = -1;
	float temp;
	for (int i = 0; i < _unit.size(); i++) {
		temp = _monster->getPositionX() - _unit[i]->getSprite()->getPositionX() < 0 ? 0 :
			_monster->getPositionX() - _unit[i]->getSprite()->getPositionX();
		distance = min(distance, temp);
		if (temp == distance) target = i;
	}
	switch (target) {
	case -1:
		_hero->setHp(_hero->getHp() - _atk); 
		break;
	default:
		_unitAttack = target;
		break;
	}
}

void Monster::ZomkingSummon()
{
	_isSummonX = _monster->getPositionX();

	_summon = Sprite::createWithSpriteFrameName("b01_summon_0001.png");
	_summon->setPosition(_monster->getPosition());
	_layer->addChild(_summon, _hero->getHero()->getZOrder() + 15);

	Vector<SpriteFrame*> frame;

	for (int i = 1; i <= 15; i++) {
		frame.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(StringUtils::format("b01_summon_%04d.png", i)));
	}
	auto animation = Animation::createWithSpriteFrames(frame, 0.03f);
	auto animate = Animate::create(animation);
	_summon->runAction(Sequence::create(animate, CallFunc::create(CC_CALLBACK_0(Monster::ZomkingSummonRemove, this)), nullptr));
}

void Monster::ZomkingSummonRemove()
{
	_layer->removeChild(_summon);
}

void Monster::Dead()
{
	_isRemove = true;
}

void Monster::setWalk()
{
	_moveChange = true;
	_state = WALK;
}

void Monster::setSummunPositionX(float x)
{
	_monster->setPositionX(x - 50 + rand() % 100);
}
