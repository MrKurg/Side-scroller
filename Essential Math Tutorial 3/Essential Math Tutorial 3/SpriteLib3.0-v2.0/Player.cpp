#include "Player.h"

Player::Player()
{
}

Player::Player(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite,
					AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	InitPlayer(fileName, animationJSON, width, height, sprite, controller, transform, hasPhys, body);
}

void Player::InitPlayer(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite,
	AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	//Store references to the components
	m_sprite = sprite;
	m_animController = controller;
	m_transform = transform;
	m_hasPhysics = hasPhys;
	if (hasPhys)
	{
		m_physBody = body;
	}

	//Initialize UVs
	m_animController->InitUVs(fileName);

	//Loads the texture and sets width and height
	m_sprite->LoadSprite(fileName, width, height, true, m_animController);
	m_animController->SetVAO(m_sprite->GetVAO());
	m_animController->SetTextureSize(m_sprite->GetTextureSize());

	//Loads in the animation json file
	nlohmann::json animations = File::LoadJSON(animationJSON);

	//IDLE ANIMATIONS

	//Idle Left
	m_animController->AddAnimation(animations["IdleLeft"].get<Animation>());
	//Idle Right
	m_animController->AddAnimation(animations["IdleRight"].get<Animation>());
#ifdef TOPDOWN
	//Idle Up
	m_animController->AddAnimation(animations["IdleUp"].get<Animation>());
	//Idle Down
	m_animController->AddAnimation(animations["IdleDown"].get<Animation>());
#endif

	//WALK ANIMATIONS

	//Walk Left
	m_animController->AddAnimation(animations["WalkLeft"].get<Animation>());
	//Walk Right
	m_animController->AddAnimation(animations["WalkRight"].get<Animation>());
#ifdef TOPDOWN
	//Walk Up
	m_animController->AddAnimation(animations["WalkUp"].get<Animation>());
	//Walk Down
	m_animController->AddAnimation(animations["WalkDown"].get<Animation>());
#endif

	//ATTACK ANIMATIONS

	//Attack Left
	m_animController->AddAnimation(animations["AttackLeft"].get<Animation>());
	//Attack Right
	m_animController->AddAnimation(animations["AttackRight"].get<Animation>());
#ifdef TOPDOWN
	//Attack up
	m_animController->AddAnimation(animations["AttackUp"].get<Animation>());
	//Attack Down
	m_animController->AddAnimation(animations["AttackDown"].get<Animation>());
#endif

	//Set Default Animation
	m_animController->SetActiveAnim(IDLELEFT);
}

void Player::Update()
{
	MovementUpdate();
	AnimationUpdate();
}

void Player::MovementUpdate()
{
	m_moving = false;
	auto timer = Timer::deltaTime;
	bool jump = true;

	if (m_hasPhysics)
	{
		vec3 vel = vec3(0.f, -1.f, 0.f);
		float speed = 20.f;

		if (Input::GetKey(Key::Shift))
		{
			speed *= 15.f;
		}

		if (Input::GetKeyDown(Key::Space))
		{
			jump = true;

			if (timer < 0.5f && jump == true)
			{
				timer += Timer::deltaTime;
				vel = vel + vec3(0.f, 10.f, 0.f);
				/*m_physBody->SetVelocity(vec3(0.f, 900.f, 0.f));*/
			}
			else
			{
				timer = 0;
				/*jump = false;*/
			}
		}

		if (Input::GetKey(Key::A))
		{
			vel = vel + vec3(-5.f, 0.f, 0.f);
			m_facing = LEFT;
			m_moving = true;
		}
		if (Input::GetKey(Key::D))
		{
			vel = vel + vec3(5.f, 0.f, 0.f);
			m_facing = RIGHT;
			m_moving = true;
		}

		m_physBody->SetVelocity(vel * speed);
	}
	else
	{
		//Regular Movement
		float speed = 30.f;

		if (Input::GetKey(Key::A))
		{
			m_transform->SetPositionX(m_transform->GetPositionX() - (speed * Timer::deltaTime));
			m_facing = LEFT;
			m_moving = true;
		}
		if (Input::GetKey(Key::D))
		{
			m_transform->SetPositionX(m_transform->GetPositionX() + (speed * Timer::deltaTime));
			m_facing = RIGHT;
			m_moving = true;
		}
	}
}

void Player::AnimationUpdate()
{
	int activeAnimation = 0;

	if (m_moving)
	{
		//Puts it into the WALK category
		activeAnimation = WALK;
	}
	else if (m_attacking)
	{
		activeAnimation = ATTACK;

		//Check if the attack animation is done
		if (m_animController->GetAnimation(m_animController->GetActiveAnim()).GetAnimationDone())
		{
			//Will auto set to idle
			m_locked = false;
			m_attacking = false;
			//Resets the attack animation
			m_animController->GetAnimation(m_animController->GetActiveAnim()).Reset();

			activeAnimation = IDLE;
		}
	}
	else
	{
		activeAnimation = IDLE;
	}

	SetActiveAnimation(activeAnimation + (int)m_facing);
}

void Player::SetActiveAnimation(int anim)
{
	m_animController->SetActiveAnim(anim);
}
