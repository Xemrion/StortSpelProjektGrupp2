#ifndef INPUT_H
#define	INPUT_H

#include <d3d11.h>
#include <SimpleMath.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <GamePad.h>
#include "window.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

enum class Controllers {
	NONE,
	XBOX,
	PLAYSTATION
};

enum class Keys {
	L_UP, //Left stick up. Keyboard: W
	L_DOWN, //Left stick down. Keyboard: S
	L_LEFT, //Left stick left. Keyboard: A
	L_RIGHT, //Left stick right. Keyboard: D
	L_TRIGGER, //Left trigger button. Keyboard: ?
	L_SHOULDER, //Left shoulder button. Keyboard: Mouse Right Click
	L_PRESS, //Left stick pressed. Keyboard: ?

	R_UP, //Right stick up. Keyboard: Up
	R_DOWN, //Right stick down. Keyboard: Down
	R_LEFT, //Right stick left. Keyboard: Left
	R_RIGHT, //Right stick right. Keyboard: Right
	R_TRIGGER, //Right trigger button. Keyboard: ?
	R_SHOULDER, //Right shoulder button. Keyboard: Mouse Left Click 
	R_PRESS, //Right stick pressed. Keyboard: ?

	CONFIRM, //A (PS: X) button. Keyboard: Enter
	CANCEL, //B (PS: O) button. Keyboard: Return
	ACTION_1, //X (PS: Square) button. Keyboard: Space
	ACTION_2, //Y (PS: Triangle) button. Keyboard: Left shift

	MENU //Menu (PS: Options) button. Keyboard: Escape
};

enum class MouseKeys {
	LEFT,
	MIDDLE,
	RIGHT,
	X_BUTTON1,
	X_BUTTON2
};

enum class States {
	UP = 0, //Released
	HELD = 1, //Pressed
	RELEASED = 2, //Released once
	PRESSED = 3 //Pressed once
};

class Input
{
private:
	static std::unique_ptr<Input> instance;

	Mouse mouse;
	Mouse::ButtonStateTracker mouseTracker;
	Keyboard keyboard;
	Keyboard::KeyboardStateTracker keyboardTracker;
	GamePad gamePad;
	GamePad::ButtonStateTracker gamePadTrackers;
	float wHeight;
	float wWidth;

	bool preferGamePad;
	float resetTimer;
	Controllers controller;

	void updateController();
	bool anyKeyPressed();
	bool anyKeyPressedGamePad();
	static bool checkButtonKeyboard(Keys key, States state);
	static bool checkButtonGamePad(Keys key, GamePad::ButtonStateTracker::ButtonState state);
public:
	Input();
	virtual ~Input();

	static bool isKeyDown_DEBUG(Keyboard::Keys key);
	static bool isKeyPressed(Keyboard::Keys key);

	static void init(Window* window);
	static void update(float deltaTime);
	static void reset();

	static void setRumble(float leftMotor, float rightMotor, float leftTrigger = 0.f, float rightTrigger = 0.f);
	static void resetRumble();
	static bool checkButton(Keys key, States state);
	static Vector2 getDirectionL();
	static float getStrengthL();
	static Vector2 getDirectionR();
	static float getStrengthR();

	static void setWindowSize(int width, int height);
	static bool checkButtonMouse(MouseKeys key, States state);
	static Vector2 getMousePosition();
	static Controllers getControllerID();
};

#endif // !INPUT_H
