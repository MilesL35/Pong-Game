struct Button_State {
	bool isDown;
	bool changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_W,
	BUTTON_S,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_ENTER,
	BUTTON_ESCAPE,

	BUTTON_COUNT //Make last item
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};