#include "Input.h"
#include <joystickapi.h>
std::unique_ptr<Input> Input::instance;

Input::Input() : wWidth(0), wHeight(0), controller(Controllers::NONE), preferGamePad(false), resetTimer(0.0f)
{
	this->updateController();

	if (this->controller != Controllers::NONE)
	{
		this->preferGamePad = true;
	}
}

Input::~Input()
{
}

void Input::updateController()
{
	if (!this->gamePad.GetState(0).IsConnected())
	{
		this->controller = Controllers::NONE;
		return;
	}

	JOYINFOEX info = {};
	JOYCAPS caps = {};

	if (joyGetPosEx(JOYSTICKID1, &info) != JOYERR_NOERROR)
	{
		joyGetDevCaps(JOYSTICKID1, &caps, sizeof(caps));
	}

	switch (caps.wMid)
	{
	case 0x054C:
		this->controller = Controllers::PLAYSTATION;
		break;
	default:
		this->controller = Controllers::XBOX;
		break;
	}
}

bool Input::anyKeyPressed()
{
	if (!this->gamePad.GetState(0).IsConnected()) return true;

	Keyboard::State kState = this->keyboard.GetState();

	if (this->keyboard.IsConnected())
	{
		for (int i = 0; i < 0xff; i++)
		{
			if (kState.IsKeyDown(static_cast<Keyboard::Keys>(i)))
			{
				return true;
			}
		}
	}

	if (this->mouse.IsConnected())
	{
		Mouse::State mState = this->mouse.GetState();
		return mState.leftButton || mState.middleButton || mState.rightButton || mState.xButton1 || mState.xButton2;
	}

	return false;
}

bool Input::anyKeyPressedGamePad()
{
	GamePad::State gState = this->gamePad.GetState(0);

	if (gState.IsConnected())
	{
		return	gState.buttons.a || gState.buttons.b || gState.buttons.back || gState.buttons.leftShoulder || gState.buttons.leftStick || gState.buttons.menu ||
				gState.buttons.rightShoulder || gState.buttons.rightStick || gState.buttons.start || gState.buttons.view || gState.buttons.x || gState.buttons.y ||
				gState.dpad.down || gState.dpad.left || gState.dpad.right || gState.dpad.up || gState.thumbSticks.leftX != 0 || gState.thumbSticks.leftY != 0 ||
				gState.thumbSticks.rightX != 0 || gState.thumbSticks.rightY != 0 || gState.triggers.left != 0 || gState.triggers.right != 0;
	}
	
	return false;
}

bool Input::checkButtonKeyboard(Keys key, States state)
{
	Keyboard::State keyboardState = instance->keyboard.GetState();
	if (state == States::UP)
	{
		switch (key)
		{
		case Keys::L_UP:
			return !keyboardState.W;
		case Keys::L_DOWN:
			return !keyboardState.S;
		case Keys::L_LEFT:
			return !keyboardState.A;
		case Keys::L_RIGHT:
			return !keyboardState.D;
		case Keys::L_TRIGGER:
			return false;
		case Keys::L_SHOULDER:
			return !keyboardState.LeftShift;
		case Keys::L_PRESS:
			return false;
		case Keys::R_UP:
			return !keyboardState.Up;
		case Keys::R_DOWN:
			return !keyboardState.Down;
		case Keys::R_LEFT:
			return !keyboardState.Left;
		case Keys::R_RIGHT:
			return !keyboardState.Right;
		case Keys::R_TRIGGER:
			return false;
		case Keys::R_SHOULDER:
			return !keyboardState.Space;
		case Keys::R_PRESS:
			return false;
		case Keys::CONFIRM:
			return !keyboardState.Enter;
		case Keys::CANCEL:
			return !keyboardState.Back;
		case Keys::ACTION_1:
			return false;
		case Keys::ACTION_2:
			return false;
		case Keys::MENU:
			return !keyboardState.Escape;
		}
	}
	else if (state == States::HELD)
	{
		switch (key)
		{
		case Keys::L_UP:
			return keyboardState.W;
		case Keys::L_DOWN:
			return keyboardState.S;
		case Keys::L_LEFT:
			return keyboardState.A;
		case Keys::L_RIGHT:
			return keyboardState.D;
		case Keys::L_TRIGGER:
			return false;
		case Keys::L_SHOULDER:
			return keyboardState.LeftShift;
		case Keys::L_PRESS:
			return false;
		case Keys::R_UP:
			return keyboardState.Up;
		case Keys::R_DOWN:
			return keyboardState.Down;
		case Keys::R_LEFT:
			return keyboardState.Left;
		case Keys::R_RIGHT:
			return keyboardState.Right;
		case Keys::R_TRIGGER:
			return false;
		case Keys::R_SHOULDER:
			return keyboardState.Space;
		case Keys::R_PRESS:
			return false;
		case Keys::CONFIRM:
			return keyboardState.Enter;
		case Keys::CANCEL:
			return keyboardState.Back;
		case Keys::ACTION_1:
			return false;
		case Keys::ACTION_2:
			return false;
		case Keys::MENU:
			return keyboardState.Escape;
		}
	}
	else if (state == States::PRESSED)
	{
		switch (key)
		{
		case Keys::L_UP:
			return instance->keyboardTracker.pressed.W;
		case Keys::L_DOWN:
			return instance->keyboardTracker.pressed.S;
		case Keys::L_LEFT:
			return instance->keyboardTracker.pressed.A;
		case Keys::L_RIGHT:
			return instance->keyboardTracker.pressed.D;
		case Keys::L_TRIGGER:
			return false;
		case Keys::L_SHOULDER:
			return instance->keyboardTracker.pressed.LeftShift;
		case Keys::L_PRESS:
			return false;
		case Keys::R_UP:
			return instance->keyboardTracker.pressed.Up;
		case Keys::R_DOWN:
			return instance->keyboardTracker.pressed.Down;
		case Keys::R_LEFT:
			return instance->keyboardTracker.pressed.Left;
		case Keys::R_RIGHT:
			return instance->keyboardTracker.pressed.Right;
		case Keys::R_TRIGGER:
			return false;
		case Keys::R_SHOULDER:
			return instance->keyboardTracker.pressed.Space;
		case Keys::R_PRESS:
			return false;
		case Keys::CONFIRM:
			return instance->keyboardTracker.pressed.Enter;
		case Keys::CANCEL:
			return instance->keyboardTracker.pressed.Back;
		case Keys::ACTION_1:
			return false;
		case Keys::ACTION_2:
			return false;
		case Keys::MENU:
			return instance->keyboardTracker.pressed.Escape;
		}
	}
	else
	{
		switch (key)
		{
		case Keys::L_UP:
			return instance->keyboardTracker.released.W;
		case Keys::L_DOWN:
			return instance->keyboardTracker.released.S;
		case Keys::L_LEFT:
			return instance->keyboardTracker.released.A;
		case Keys::L_RIGHT:
			return instance->keyboardTracker.released.D;
		case Keys::L_TRIGGER:
			return false;
		case Keys::L_SHOULDER:
			return instance->keyboardTracker.released.LeftShift;
		case Keys::L_PRESS:
			return false;
		case Keys::R_UP:
			return instance->keyboardTracker.released.Up;
		case Keys::R_DOWN:
			return instance->keyboardTracker.released.Down;
		case Keys::R_LEFT:
			return instance->keyboardTracker.released.Left;
		case Keys::R_RIGHT:
			return instance->keyboardTracker.released.Right;
		case Keys::R_TRIGGER:
			return false;
		case Keys::R_SHOULDER:
			return instance->keyboardTracker.released.Space;
		case Keys::R_PRESS:
			return false;
		case Keys::CONFIRM:
			return instance->keyboardTracker.released.Enter;
		case Keys::CANCEL:
			return instance->keyboardTracker.released.Back;
		case Keys::ACTION_1:
			return false;
		case Keys::ACTION_2:
			return false;
		case Keys::MENU:
			return instance->keyboardTracker.released.Escape;
		}
	}

	return false;
}

bool Input::checkButtonGamePad(Keys key, GamePad::ButtonStateTracker::ButtonState bState)
{
	switch (key)
	{
	case Keys::L_UP:
		return instance->gamePadTrackers.leftStickUp == bState || instance->gamePadTrackers.dpadUp == bState;
	case Keys::L_DOWN:
		return instance->gamePadTrackers.leftStickDown == bState || instance->gamePadTrackers.dpadDown == bState;
	case Keys::L_LEFT:
		return instance->gamePadTrackers.leftStickLeft == bState || instance->gamePadTrackers.dpadLeft == bState;
	case Keys::L_RIGHT:
		return instance->gamePadTrackers.leftStickRight == bState || instance->gamePadTrackers.dpadRight == bState;
	case Keys::L_TRIGGER:
		return instance->gamePadTrackers.leftTrigger == bState;
	case Keys::L_SHOULDER:
		return instance->gamePadTrackers.leftShoulder == bState;
	case Keys::L_PRESS:
		return instance->gamePadTrackers.leftStick == bState;
	case Keys::R_UP:
		return instance->gamePadTrackers.rightStickUp == bState;
	case Keys::R_DOWN:
		return instance->gamePadTrackers.rightStickDown == bState;
	case Keys::R_LEFT:
		return instance->gamePadTrackers.rightStickLeft == bState;
	case Keys::R_RIGHT:
		return instance->gamePadTrackers.rightStickRight == bState;
	case Keys::R_TRIGGER:
		return instance->gamePadTrackers.rightTrigger == bState;
	case Keys::R_SHOULDER:
		return instance->gamePadTrackers.rightShoulder == bState;
	case Keys::R_PRESS:
		return instance->gamePadTrackers.rightStick == bState;
	case Keys::CONFIRM:
		return instance->gamePadTrackers.a == bState;
	case Keys::CANCEL:
		return instance->gamePadTrackers.b == bState;
	case Keys::ACTION_1:
		return instance->gamePadTrackers.x == bState;
	case Keys::ACTION_2:
		return instance->gamePadTrackers.y == bState;
	case Keys::MENU:
		return instance->gamePadTrackers.menu == bState;
	}

	return false;
}

bool Input::isKeyDown_DEBUG(Keyboard::Keys key)
{
	return instance->keyboard.GetState().IsKeyDown(key);
}

bool Input::isKeyPressed(Keyboard::Keys key)
{
	return instance->keyboardTracker.IsKeyPressed(key);
}

void Input::init(Window* window)
{
	instance = std::make_unique<Input>();
	instance->mouse.SetWindow(window->handle);
	instance->wHeight = window->height;
	instance->wWidth = window->width;

}

void Input::update(float deltaTime)
{
	instance->resetTimer += deltaTime;

	if (instance->resetTimer >= 2.0f)
	{
		instance->updateController();
		instance->resetTimer = 0.0f;
	}

	Keyboard::State keyboardState = instance->keyboard.GetState();
	Mouse::State mouseState = instance->mouse.GetState();
	GamePad::State gamepadState = instance->gamePad.GetState(0);

	if (instance->keyboard.IsConnected())
	{
		instance->keyboardTracker.Update(keyboardState);
	}
	else
	{
		instance->keyboardTracker.Reset();
	}

	if (instance->mouse.IsConnected())
	{
		instance->mouseTracker.Update(mouseState);
	}
	else
	{
		instance->mouseTracker.Reset();
	}

	if (gamepadState.IsConnected())
	{
		instance->gamePadTrackers.Update(gamepadState);
	}
	else
	{
		instance->gamePadTrackers.Reset();
	}

	if (instance->anyKeyPressed())
	{
		instance->preferGamePad = false;
	}
	if (instance->anyKeyPressedGamePad())
	{
		instance->preferGamePad = true;
	}
}

void Input::reset()
{
	instance->keyboardTracker.Reset();
	instance->mouseTracker.Reset();
	instance->gamePadTrackers.Reset();
}

void Input::setRumble(float leftMotor, float rightMotor, float leftTrigger, float rightTrigger)
{
	instance->gamePad.SetVibration(0, leftMotor, rightMotor, leftTrigger, rightTrigger);
}

void Input::resetRumble()
{
	instance->gamePad.SetVibration(0, 0.0f, 0.0f);
}

bool Input::checkButton(Keys key, States state)
{
	return checkButtonKeyboard(key, state) | checkButtonGamePad(key, static_cast<GamePad::ButtonStateTracker::ButtonState>(state));
}

Vector2 Input::getDirectionL()
{
	Vector2 dir;

	if (!instance->preferGamePad)
	{
		if (checkButtonKeyboard(Keys::L_LEFT, States::HELD)) dir.x -= 1.0f;
		if (checkButtonKeyboard(Keys::L_RIGHT, States::HELD)) dir.x += 1.0f;
		if (checkButtonKeyboard(Keys::L_UP, States::HELD)) dir.y += 1.0f;
		if (checkButtonKeyboard(Keys::L_DOWN, States::HELD)) dir.y -= 1.0f;
	}
	else 
	{
		GamePad::State gamepadState = instance->gamePad.GetState(0);

		if (gamepadState.IsConnected())
		{
			dir = Vector2(gamepadState.thumbSticks.leftX, gamepadState.thumbSticks.leftY);
		}
	}

	dir.Normalize();
	return dir;
}

float Input::getStrengthL()
{
	float strength = 0.0f;

	if (!instance->preferGamePad)
	{
		bool left = checkButtonKeyboard(Keys::L_LEFT, States::HELD);
		bool right = checkButtonKeyboard(Keys::L_RIGHT, States::HELD);
		bool up = checkButtonKeyboard(Keys::L_UP, States::HELD);
		bool down = checkButtonKeyboard(Keys::L_DOWN, States::HELD);

		strength = 1.0f;

		if ((left && right && !up && !down) || (!left && !right && up && down) || (left && right && up && down) || (!left && !right && !up && !down))
		{
			strength = 0.0f;
		}
	}
	else
	{
		GamePad::State gamepadState = instance->gamePad.GetState(0);

		if (gamepadState.IsConnected())
		{
			strength = min(Vector2(gamepadState.thumbSticks.leftX, gamepadState.thumbSticks.leftY).Length(), 1.0f);
		}
	}

	return strength;
}

Vector2 Input::getDirectionR()
{
	Vector2 dir;

	if (!instance->preferGamePad)
	{
		float mX = instance->mouse.GetState().x - (instance->wWidth / 2);
		float mY = instance->mouse.GetState().y - (instance->wHeight / 2);

		dir = Vector2(mX, -mY);
	}
	else
	{
		GamePad::State gamepadState = instance->gamePad.GetState(0);

		if (gamepadState.IsConnected())
		{
			dir = Vector2(gamepadState.thumbSticks.rightX, gamepadState.thumbSticks.rightY);
		}
	}

	dir.Normalize();
	return dir;
}

float Input::getStrengthR()
{
	float strength = 0.0f;

	if (!instance->preferGamePad)
	{
		strength = checkButtonMouse(MouseKeys::LEFT, States::HELD) ? 1.0f : 0.0f;
	}
	else
	{
		GamePad::State gamepadState = instance->gamePad.GetState(0);

		if (gamepadState.IsConnected())
		{
			strength = min(Vector2(gamepadState.thumbSticks.rightX, gamepadState.thumbSticks.rightY).Length(), 1.0f);
		}
	}
	
	return strength;
}

void Input::setWindowSize(int width, int height)
{
	if (instance.get())
	{
		instance->wWidth = width;
		instance->wHeight = height;
	}
}

bool Input::checkButtonMouse(MouseKeys key, States state)
{
	Mouse::ButtonStateTracker::ButtonState buttonState = static_cast<Mouse::ButtonStateTracker::ButtonState>(state);

	switch (key)
	{
	case MouseKeys::LEFT:
		return instance->mouseTracker.leftButton == buttonState;
	case MouseKeys::MIDDLE:
		return instance->mouseTracker.middleButton == buttonState;
	case MouseKeys::RIGHT:
		return instance->mouseTracker.rightButton == buttonState;
	case MouseKeys::X_BUTTON1:
		return instance->mouseTracker.xButton1 == buttonState;
	case MouseKeys::X_BUTTON2:
		return instance->mouseTracker.xButton2 == buttonState;
	}

	return false;
}

Vector2 Input::getMousePosition()
{
	return Vector2(instance->mouse.GetState().x, instance->mouse.GetState().y);
}

Controllers Input::getControllerID()
{
	return instance->preferGamePad ? instance->controller : Controllers::NONE;
}