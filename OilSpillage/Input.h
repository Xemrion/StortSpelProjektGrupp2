#ifndef INPUT_H
#define	INPUT_H

#include <d3d11.h>
#include <SimpleMath.h>
#include <Keyboard.h>
#include <GamePad.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

enum Keys {
	L_UP, //Left stick up. Keyboard: W
	L_DOWN, //Left stick down. Keyboard: S
	L_LEFT, //Left stick left. Keyboard: A
	L_RIGHT, //Left stick right. Keyboard: D
	L_TRIGGER, //Left trigger button. Keyboard: ?
	L_SHOULDER, //Left shoulder button. Keyboard: ?
	L_PRESS, //Left stick pressed. Keyboard: ?

	R_UP, //Right stick up. Keyboard: Up
	R_DOWN, //Right stick down. Keyboard: Down
	R_LEFT, //Right stick left. Keyboard: Left
	R_RIGHT, //Right stick right. Keyboard: Right
	R_TRIGGER, //Right trigger button. Keyboard: ?
	R_SHOULDER, //Right shoulder button. Keyboard: ?
	R_PRESS, //Right stick pressed. Keyboard: ?

	CONFIRM, //A (PS: X) button. Keyboard: Enter
	CANCEL, //B (PS: O) button. Keyboard: Backspace
	ACTION_1, //X (PS: Square) button. Keyboard: Space
	ACTION_2, //Y (PS: Triangle) button. Keyboard: Left shift

	MENU //Menu (PS: Options) button. Keyboard: Escape
};

enum States {
	UP = 0, //Released
	HELD = 1, //Pressed
	RELEASED = 2, //Released once
	PRESSED = 3 //Pressed once
};

class Input
{
private:
	static const int PLAYER_COUNT = 2;
	static Input instance;

	int playerKeyboard;
	Keyboard keyboard;
	Keyboard::State keyboardState;
	Keyboard::KeyboardStateTracker keyboardTracker;
	GamePad gamePad;
	GamePad::State gamePadStates[PLAYER_COUNT];
	GamePad::ButtonStateTracker gamePadTrackers[PLAYER_COUNT];

	Input();

	static bool CheckButtonKeyboard(Keys key, States state);
	static bool CheckButtonGamePad(Keys key, GamePad::ButtonStateTracker::ButtonState state, int playerId);

public:
	virtual ~Input();

	static bool IsKeyDown_DEBUG(Keyboard::Keys key);

	static void Update();
	static void Reset();

	static void SetRumble(int player, float leftMotor, float rightMotor, float leftTrigger = 0.f, float rightTrigger = 0.f);
	static void ResetRumble(int player);
	static bool CheckButton(Keys key, States state, int player);
	static Vector2 GetDirectionL(int player);
	static float GetStrengthL(int player);
	static Vector2 GetDirectionR(int player);
	static float GetStrengthR(int player);

	static void SetKeyboardPlayerID(int player);
};

#endif // !INPUT_H
