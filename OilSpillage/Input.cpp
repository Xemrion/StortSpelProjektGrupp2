#include "Input.h"
std::unique_ptr<Input> Input::instance;

Input::Input() : wWidth(0), wHeight(0)//, playerKeyboard(-1)
{
}

Input::~Input()
{
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
			return instance->mouseTracker.rightButton == Mouse::ButtonStateTracker::UP;
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
			return instance->mouseTracker.leftButton == Mouse::ButtonStateTracker::UP;
		case Keys::R_PRESS:
			return false;
		case Keys::CONFIRM:
			return !keyboardState.Enter;
		case Keys::CANCEL:
			return !keyboardState.Back;
		case Keys::ACTION_1:
			return !keyboardState.Space;
		case Keys::ACTION_2:
			return !keyboardState.LeftShift;
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
			return instance->mouseTracker.rightButton == Mouse::ButtonStateTracker::HELD;
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
			return instance->mouseTracker.leftButton == Mouse::ButtonStateTracker::HELD;
		case Keys::R_PRESS:
			return false;
		case Keys::CONFIRM:
			return keyboardState.Enter;
		case Keys::CANCEL:
			return keyboardState.Back;
		case Keys::ACTION_1:
			return keyboardState.Space;
		case Keys::ACTION_2:
			return keyboardState.LeftShift;
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
			return instance->mouseTracker.rightButton == Mouse::ButtonStateTracker::PRESSED;
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
			return instance->mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED;
		case Keys::R_PRESS:
			return false;
		case Keys::CONFIRM:
			return instance->keyboardTracker.pressed.Enter;
		case Keys::CANCEL:
			return instance->keyboardTracker.pressed.Back;
		case Keys::ACTION_1:
			return instance->keyboardTracker.pressed.Space;
		case Keys::ACTION_2:
			return instance->keyboardTracker.pressed.LeftShift;
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
			return instance->mouseTracker.rightButton == Mouse::ButtonStateTracker::RELEASED;
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
			return instance->mouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED;
		case Keys::R_PRESS:
			return false;
		case Keys::CONFIRM:
			return instance->keyboardTracker.released.Enter;
		case Keys::CANCEL:
			return instance->keyboardTracker.released.Back;
		case Keys::ACTION_1:
			return instance->keyboardTracker.released.Space;
		case Keys::ACTION_2:
			return instance->keyboardTracker.released.LeftShift;
		case Keys::MENU:
			return instance->keyboardTracker.released.Escape;
		}
	}

	return false;
}

bool Input::checkButtonGamePad(Keys key, GamePad::ButtonStateTracker::ButtonState bState/*, int playerId*/)
{
	switch (key)
	{
	case Keys::L_UP:
		return instance->gamePadTrackers/*[playerId]*/.leftStickUp == bState;
	case Keys::L_DOWN:
		return instance->gamePadTrackers/*[playerId]*/.leftStickDown == bState;
	case Keys::L_LEFT:
		return instance->gamePadTrackers/*[playerId]*/.leftStickLeft == bState;
	case Keys::L_RIGHT:
		return instance->gamePadTrackers/*[playerId]*/.leftStickRight == bState;
	case Keys::L_TRIGGER:
		return instance->gamePadTrackers/*[playerId]*/.leftTrigger == bState;
	case Keys::L_SHOULDER:
		return instance->gamePadTrackers/*[playerId]*/.leftShoulder == bState;
	case Keys::L_PRESS:
		return instance->gamePadTrackers/*[playerId]*/.leftStick == bState;
	case Keys::R_UP:
		return instance->gamePadTrackers/*[playerId]*/.rightStickUp == bState;
	case Keys::R_DOWN:
		return instance->gamePadTrackers/*[playerId]*/.rightStickDown == bState;
	case Keys::R_LEFT:
		return instance->gamePadTrackers/*[playerId]*/.rightStickLeft == bState;
	case Keys::R_RIGHT:
		return instance->gamePadTrackers/*[playerId]*/.rightStickRight == bState;
	case Keys::R_TRIGGER:
		return instance->gamePadTrackers/*[playerId]*/.rightTrigger == bState;
	case Keys::R_SHOULDER:
		return instance->gamePadTrackers/*[playerId]*/.rightShoulder == bState;
	case Keys::R_PRESS:
		return instance->gamePadTrackers/*[playerId]*/.rightStick == bState;
	case Keys::CONFIRM:
		return instance->gamePadTrackers/*[playerId]*/.a == bState;
	case Keys::CANCEL:
		return instance->gamePadTrackers/*[playerId]*/.b == bState;
	case Keys::ACTION_1:
		return instance->gamePadTrackers/*[playerId]*/.x == bState;
	case Keys::ACTION_2:
		return instance->gamePadTrackers/*[playerId]*/.y == bState;
	case Keys::MENU:
		return instance->gamePadTrackers/*[playerId]*/.menu == bState;
	}

	return false;
}

bool Input::isKeyDown_DEBUG(Keyboard::Keys key)
{
	return instance->keyboard.GetState().IsKeyDown(key);
}

void Input::init(Window* window)
{
	instance = std::make_unique<Input>();
	instance->mouse.SetWindow(window->handle);
	instance->wHeight = window->height;
	instance->wWidth = window->width;

}

void Input::update()
{
	Keyboard::State keyboardState = instance->keyboard.GetState();
	Mouse::State mouseState = instance->mouse.GetState();

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

	//for (int i = 0; i < Input::PLAYER_COUNT; i++)
	{
		GamePad::State gamepadState = instance->gamePad.GetState(/*i*/0);

		if (gamepadState.IsConnected())
		{
			instance->gamePadTrackers/*[i]*/.Update(gamepadState);
		}
		else
		{
			instance->gamePadTrackers/*[i]*/.Reset();
		}
	}
}

void Input::reset()
{
	instance->keyboardTracker.Reset();

	//for (int i = 0; i < Input::PLAYER_COUNT; i++)
	//{
		instance->gamePadTrackers/*[i]*/.Reset();
	//}
}

void Input::setRumble(/*int player,*/ float leftMotor, float rightMotor, float leftTrigger, float rightTrigger)
{
	/*if (player >= Input::PLAYER_COUNT || player < 0 || player == instance->playerKeyboard) return;

	if (instance->playerKeyboard != -1 && player > instance->playerKeyboard)
	{
		player--;
	}*/

	instance->gamePad.SetVibration(/*player*/0, leftMotor, rightMotor, leftTrigger, rightTrigger);
}

void Input::resetRumble(/*int player*/)
{
	/*if (player >= Input::PLAYER_COUNT || player < 0 || player == instance->playerKeyboard) return;

	if (instance->playerKeyboard != -1 && player > instance->playerKeyboard)
	{
		player--;
	}*/

	instance->gamePad.SetVibration(/*player*/0, 0.0f, 0.0f);
}

bool Input::checkButton(Keys key, States state/*, int player*/)
{
	return checkButtonKeyboard(key, state) | checkButtonGamePad(key, static_cast<GamePad::ButtonStateTracker::ButtonState>(state));

	/*if (player >= Input::PLAYER_COUNT || player < -1) return false;

	if (player == instance->playerKeyboard)
	{
		return CheckButtonKeyboard(key, state);
	}
	
	if (instance->playerKeyboard != -1 && player > instance->playerKeyboard)
	{
		player--;
	}

	GamePad::ButtonStateTracker::ButtonState bState = static_cast<GamePad::ButtonStateTracker::ButtonState>(state);
	return CheckButtonGamePad(key, bState, player);*/
}

Vector2 Input::getDirectionL(/*int player*/)
{
	Vector2 dir;

	//if (player >= Input::PLAYER_COUNT || player < 0) return dir;

	//if (player == instance->playerKeyboard)
	{
		if (checkButtonKeyboard(Keys::L_LEFT, States::HELD)) dir.x -= 1.0f;
		if (checkButtonKeyboard(Keys::L_RIGHT, States::HELD)) dir.x += 1.0f;
		if (checkButtonKeyboard(Keys::L_UP, States::HELD)) dir.y += 1.0f;
		if (checkButtonKeyboard(Keys::L_DOWN, States::HELD)) dir.y -= 1.0f;
		
	}
	if (dir == Vector2())//else 
	{
		/*if (instance->playerKeyboard != -1 && player > instance->playerKeyboard)
		{
			player--;
		}*/

		GamePad::State gamepadState = instance->gamePad.GetState(/*player*/0);

		if (gamepadState.IsConnected())
		{
			dir = Vector2(gamepadState.thumbSticks.leftX, gamepadState.thumbSticks.leftY);
		}
	}

	dir.Normalize();
	return dir;
}

float Input::getStrengthL(/*int player*/)
{
	float strength = 0.0f;

	//if (player >= Input::PLAYER_COUNT || player < 0) return strength;

	//if (trueplayer == instance->playerKeyboard)
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
	if (strength == 0.0f)//else
	{
		/*if (instance->playerKeyboard != -1 && player > instance->playerKeyboard)
		{
			player--;
		}*/

		GamePad::State gamepadState = instance->gamePad.GetState(/*player*/0);

		if (gamepadState.IsConnected())
		{
			strength = min(Vector2(gamepadState.thumbSticks.leftX, gamepadState.thumbSticks.leftY).Length(), 1.0f);
		}
	}

	return strength;
}

Vector2 Input::getDirectionR(/*int player*/)
{
	Vector2 dir;

	//if (player >= Input::PLAYER_COUNT || player < 0) return dir;

	/*if (player == instance->playerKeyboard)
	{
		float mX = instance->mouse.GetState().x - (instance->wWidth / 2);
		float mY = instance->mouse.GetState().y - (instance->wHeight / 2);

		dir = Vector2(mX, -mY);
	}
	else
	{
		if (instance->playerKeyboard != -1 && player > instance->playerKeyboard)
		{
			player--;
		}*/

		GamePad::State gamepadState = instance->gamePad.GetState(/*player*/0);

		if (gamepadState.IsConnected())
		{
			dir = Vector2(gamepadState.thumbSticks.rightX, gamepadState.thumbSticks.rightY);
		}
	//}

	if (dir == Vector2())
	{
		float mX = instance->mouse.GetState().x - (instance->wWidth / 2);
		float mY = instance->mouse.GetState().y - (instance->wHeight / 2);

		dir = Vector2(mX, -mY);
	}

	dir.Normalize();
	return dir;
}

float Input::getStrengthR(/*int player*/)
{
	float strength = 0.0f;

	//if (player >= Input::PLAYER_COUNT || player < 0) return strength;

	/*if (player == instance->playerKeyboard)
	{
		float mX = instance->mouse.GetState().x - (instance->wWidth / 2);
		float mY = instance->mouse.GetState().y - (instance->wHeight / 2);

		strength = min(Vector2(mX, -mY).Length() / 300.0f, 1.0f);
	}
	else
	{
		if (instance->playerKeyboard != -1 && player > instance->playerKeyboard)
		{
			player--;
		}*/

		GamePad::State gamepadState = instance->gamePad.GetState(/*player*/0);

		if (gamepadState.IsConnected())
		{
			strength = min(Vector2(gamepadState.thumbSticks.rightX, gamepadState.thumbSticks.rightY).Length(), 1.0f);
		}
	//}

	if (strength == 0.0f)
	{
		float mX = instance->mouse.GetState().x - (instance->wWidth / 2);
		float mY = instance->mouse.GetState().y - (instance->wHeight / 2);

		strength = min(Vector2(mX, -mY).Length() / 300.0f, 1.0f);
	}

	return strength;
}

float Input::getStrengthRnoMouse()
{
	float strength = 0.0f;
	GamePad::State gamepadState = instance->gamePad.GetState(/*player*/0);

	if (gamepadState.IsConnected())
	{
		strength = min(Vector2(gamepadState.thumbSticks.rightX, gamepadState.thumbSticks.rightY).Length(), 1.0f);
	}
	return strength;
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

/*void Input::setKeyboardPlayerID(int player)
{
	if (player >= Input::PLAYER_COUNT || player < -1) return;

	instance->playerKeyboard = player;
}*/
