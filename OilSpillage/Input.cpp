#include "Input.h"

Input Input::instance;

Input::Input() : keyboardState(), gamePadStates()
{
	this->playerKeyboard = -1;
}

Input::~Input()
{
}

bool Input::CheckButtonKeyboard(Keys key, States state)
{
	if (state == UP)
	{
		switch (key)
		{
		case L_UP:
			return !instance.keyboardState.W;
		case L_DOWN:
			return !instance.keyboardState.S;
		case L_LEFT:
			return !instance.keyboardState.A;
		case L_RIGHT:
			return !instance.keyboardState.D;
		case L_TRIGGER:
			return false;
		case L_SHOULDER:
			return false;
		case L_PRESS:
			return false;
		case R_UP:
			return !instance.keyboardState.Up;
		case R_DOWN:
			return !instance.keyboardState.Down;
		case R_LEFT:
			return !instance.keyboardState.Left;
		case R_RIGHT:
			return !instance.keyboardState.Right;
		case R_TRIGGER:
			return false;
		case R_SHOULDER:
			return false;
		case R_PRESS:
			return false;
		case CONFIRM:
			return !instance.keyboardState.Enter;
		case CANCEL:
			return !instance.keyboardState.Back;
		case ACTION_1:
			return !instance.keyboardState.Space;
		case ACTION_2:
			return !instance.keyboardState.LeftShift;
		case MENU:
			return !instance.keyboardState.Escape;
		}
	}
	else if (state == HELD)
	{
		switch (key)
		{
		case L_UP:
			return instance.keyboardState.W;
		case L_DOWN:
			return instance.keyboardState.S;
		case L_LEFT:
			return instance.keyboardState.A;
		case L_RIGHT:
			return instance.keyboardState.D;
		case L_TRIGGER:
			return false;
		case L_SHOULDER:
			return false;
		case L_PRESS:
			return false;
		case R_UP:
			return instance.keyboardState.Up;
		case R_DOWN:
			return instance.keyboardState.Down;
		case R_LEFT:
			return instance.keyboardState.Left;
		case R_RIGHT:
			return instance.keyboardState.Right;
		case R_TRIGGER:
			return false;
		case R_SHOULDER:
			return false;
		case R_PRESS:
			return false;
		case CONFIRM:
			return instance.keyboardState.Enter;
		case CANCEL:
			return instance.keyboardState.Back;
		case ACTION_1:
			return instance.keyboardState.Space;
		case ACTION_2:
			return instance.keyboardState.LeftShift;
		case MENU:
			return instance.keyboardState.Escape;
		}
	}
	else if (state == PRESSED)
	{
		switch (key)
		{
		case L_UP:
			return instance.keyboardTracker.pressed.W;
		case L_DOWN:
			return instance.keyboardTracker.pressed.S;
		case L_LEFT:
			return instance.keyboardTracker.pressed.A;
		case L_RIGHT:
			return instance.keyboardTracker.pressed.D;
		case L_TRIGGER:
			return false;
		case L_SHOULDER:
			return false;
		case L_PRESS:
			return false;
		case R_UP:
			return instance.keyboardTracker.pressed.Up;
		case R_DOWN:
			return instance.keyboardTracker.pressed.Down;
		case R_LEFT:
			return instance.keyboardTracker.pressed.Left;
		case R_RIGHT:
			return instance.keyboardTracker.pressed.Right;
		case R_TRIGGER:
			return false;
		case R_SHOULDER:
			return false;
		case R_PRESS:
			return false;
		case CONFIRM:
			return instance.keyboardTracker.pressed.Enter;
		case CANCEL:
			return instance.keyboardTracker.pressed.Back;
		case ACTION_1:
			return instance.keyboardTracker.pressed.Space;
		case ACTION_2:
			return instance.keyboardTracker.pressed.LeftShift;
		case MENU:
			return instance.keyboardTracker.pressed.Escape;
		}
	}
	else
	{
		switch (key)
		{
		case L_UP:
			return instance.keyboardTracker.released.W;
		case L_DOWN:
			return instance.keyboardTracker.released.S;
		case L_LEFT:
			return instance.keyboardTracker.released.A;
		case L_RIGHT:
			return instance.keyboardTracker.released.D;
		case L_TRIGGER:
			return false;
		case L_SHOULDER:
			return false;
		case L_PRESS:
			return false;
		case R_UP:
			return instance.keyboardTracker.released.Up;
		case R_DOWN:
			return instance.keyboardTracker.released.Down;
		case R_LEFT:
			return instance.keyboardTracker.released.Left;
		case R_RIGHT:
			return instance.keyboardTracker.released.Right;
		case R_TRIGGER:
			return false;
		case R_SHOULDER:
			return false;
		case R_PRESS:
			return false;
		case CONFIRM:
			return instance.keyboardTracker.released.Enter;
		case CANCEL:
			return instance.keyboardTracker.released.Back;
		case ACTION_1:
			return instance.keyboardTracker.released.Space;
		case ACTION_2:
			return instance.keyboardTracker.released.LeftShift;
		case MENU:
			return instance.keyboardTracker.released.Escape;
		}
	}

	return false;
}

bool Input::CheckButtonGamePad(Keys key, GamePad::ButtonStateTracker::ButtonState bState, int playerId)
{
	switch (key)
	{
	case L_UP:
		return instance.gamePadTrackers[playerId].leftStickUp == bState;
	case L_DOWN:
		return instance.gamePadTrackers[playerId].leftStickDown == bState;
	case L_LEFT:
		return instance.gamePadTrackers[playerId].leftStickLeft == bState;
	case L_RIGHT:
		return instance.gamePadTrackers[playerId].leftStickRight == bState;
	case L_TRIGGER:
		return instance.gamePadTrackers[playerId].leftTrigger == bState;
	case L_SHOULDER:
		return instance.gamePadTrackers[playerId].leftShoulder == bState;
	case L_PRESS:
		return instance.gamePadTrackers[playerId].leftStick == bState;
	case R_UP:
		return instance.gamePadTrackers[playerId].rightStickUp == bState;
	case R_DOWN:
		return instance.gamePadTrackers[playerId].rightStickDown == bState;
	case R_LEFT:
		return instance.gamePadTrackers[playerId].rightStickLeft == bState;
	case R_RIGHT:
		return instance.gamePadTrackers[playerId].rightStickRight == bState;
	case R_TRIGGER:
		return instance.gamePadTrackers[playerId].rightTrigger == bState;
	case R_SHOULDER:
		return instance.gamePadTrackers[playerId].rightShoulder == bState;
	case R_PRESS:
		return instance.gamePadTrackers[playerId].rightStick == bState;
	case CONFIRM:
		return instance.gamePadTrackers[playerId].a == bState;
	case CANCEL:
		return instance.gamePadTrackers[playerId].b == bState;
	case ACTION_1:
		return instance.gamePadTrackers[playerId].x == bState;
	case ACTION_2:
		return instance.gamePadTrackers[playerId].y == bState;
	case MENU:
		return instance.gamePadTrackers[playerId].menu == bState;
	}

	return false;
}

bool Input::IsKeyDown_DEBUG(Keyboard::Keys key)
{
	return instance.keyboardState.IsKeyDown(key);
}

void Input::Update()
{
	instance.keyboardState = instance.keyboard.GetState();

	if (instance.keyboard.IsConnected())
	{
		instance.keyboardTracker.Update(instance.keyboardState);
	}
	else
	{
		instance.keyboardTracker.Reset();
	}

	for (int i = 0; i < Input::PLAYER_COUNT; i++)
	{
		instance.gamePadStates[i] = instance.gamePad.GetState(0);

		if (instance.gamePadStates[i].IsConnected())
		{
			instance.gamePadTrackers[i].Update(instance.gamePadStates[i]);
		}
		else
		{
			instance.gamePadTrackers[i].Reset();
		}
	}
}

void Input::Reset()
{
	instance.keyboardTracker.Reset();

	for (int i = 0; i < Input::PLAYER_COUNT; i++)
	{
		instance.gamePadTrackers[i].Reset();
	}
}

void Input::SetRumble(int player, float leftMotor, float rightMotor, float leftTrigger, float rightTrigger)
{
	if (player >= Input::PLAYER_COUNT || player < 0 || player == instance.playerKeyboard) return;

	if (instance.playerKeyboard != -1 && player > instance.playerKeyboard)
	{
		player--;
	}

	instance.gamePad.SetVibration(player, leftMotor, rightMotor, leftTrigger, rightTrigger);
}

void Input::ResetRumble(int player)
{
	if (player >= Input::PLAYER_COUNT || player < 0 || player == instance.playerKeyboard) return;

	if (instance.playerKeyboard != -1 && player > instance.playerKeyboard)
	{
		player--;
	}

	instance.gamePad.SetVibration(0, 0.0f, 0.0f);
}

bool Input::CheckButton(Keys key, States state, int player)
{
	if (player >= Input::PLAYER_COUNT || player < 0) return false;

	if (player == instance.playerKeyboard)
	{
		return CheckButtonKeyboard(key, state);
	}
	
	if (instance.playerKeyboard != -1 && player > instance.playerKeyboard)
	{
		player--;
	}

	GamePad::ButtonStateTracker::ButtonState bState = static_cast<GamePad::ButtonStateTracker::ButtonState>(state);
	return CheckButtonGamePad(key, bState, player);
}

Vector2 Input::GetDirectionL(int player)
{
	Vector2 dir;

	if (player >= Input::PLAYER_COUNT || player < 0) return dir;

	if (player == instance.playerKeyboard)
	{
		if (CheckButtonKeyboard(L_LEFT, HELD)) dir.x -= 1.0f;
		if (CheckButtonKeyboard(L_RIGHT, HELD)) dir.x += 1.0f;
		if (CheckButtonKeyboard(L_UP, HELD)) dir.y += 1.0f;
		if (CheckButtonKeyboard(L_DOWN, HELD)) dir.y -= 1.0f;
	}
	else 
	{
		if (instance.playerKeyboard != -1 && player > instance.playerKeyboard)
		{
			player--;
		}

		if (instance.gamePadStates[player].IsConnected())
		{
			dir = Vector2(instance.gamePadStates[player].thumbSticks.leftX, instance.gamePadStates[player].thumbSticks.leftY);
		}
	}

	dir.Normalize();
	return dir;
}

float Input::GetStrengthL(int player)
{
	float strength = 0.0f;

	if (player >= Input::PLAYER_COUNT || player < 0) return strength;

	if (player == instance.playerKeyboard)
	{
		bool left = CheckButtonKeyboard(L_LEFT, HELD);
		bool right = CheckButtonKeyboard(L_RIGHT, HELD);
		bool up = CheckButtonKeyboard(L_UP, HELD);
		bool down = CheckButtonKeyboard(L_DOWN, HELD);

		strength = 1.0f;

		if ((left && right && !up && !down) || (!left && !right && up && down) || (left && right && up && down) || (!left && !right && !up && !down))
		{
			strength = 0.0f;
		}
	}
	else
	{
		if (instance.playerKeyboard != -1 && player > instance.playerKeyboard)
		{
			player--;
		}

		if (instance.gamePadStates[player].IsConnected())
		{
			strength = Vector2(instance.gamePadStates[player].thumbSticks.leftX, instance.gamePadStates[player].thumbSticks.leftY).Length();
			if (strength > 1.0f) strength = 1.0f;
		}
	}

	return strength;
}

Vector2 Input::GetDirectionR(int player)
{
	Vector2 dir;

	if (player >= Input::PLAYER_COUNT || player < 0) return dir;

	if (player == instance.playerKeyboard)
	{
		if (CheckButtonKeyboard(R_LEFT, HELD)) dir.x -= 1.0f;
		if (CheckButtonKeyboard(R_RIGHT, HELD)) dir.x += 1.0f;
		if (CheckButtonKeyboard(R_UP, HELD)) dir.y += 1.0f;
		if (CheckButtonKeyboard(R_DOWN, HELD)) dir.y -= 1.0f;
	}
	else
	{
		if (instance.playerKeyboard != -1 && player > instance.playerKeyboard)
		{
			player--;
		}

		if (instance.gamePadStates[player].IsConnected())
		{
			dir = Vector2(instance.gamePadStates[player].thumbSticks.rightX, instance.gamePadStates[player].thumbSticks.rightY);
		}
	}

	dir.Normalize();
	return dir;
}

float Input::GetStrengthR(int player)
{
	float strength = 0.0f;

	if (player >= Input::PLAYER_COUNT || player < 0) return strength;

	if (player == instance.playerKeyboard)
	{
		bool left = CheckButtonKeyboard(R_LEFT, HELD);
		bool right = CheckButtonKeyboard(R_RIGHT, HELD);
		bool up = CheckButtonKeyboard(R_UP, HELD);
		bool down = CheckButtonKeyboard(R_DOWN, HELD);

		strength = 1.0f;

		if ((left && right && !up && !down) || (!left && !right && up && down) || (left && right && up && down) || (!left && !right && !up && !down))
		{
			strength = 0.0f;
		}
	}
	else
	{
		if (instance.playerKeyboard != -1 && player > instance.playerKeyboard)
		{
			player--;
		}

		if (instance.gamePadStates[player].IsConnected())
		{
			strength = Vector2(instance.gamePadStates[player].thumbSticks.rightX, instance.gamePadStates[player].thumbSticks.rightY).Length();
			if (strength > 1.0f) strength = 1.0f;
		}
	}

	return strength;
}

void Input::SetKeyboardPlayerID(int player)
{
	if (player >= Input::PLAYER_COUNT || player < -1) return;

	instance.playerKeyboard = player;
}
