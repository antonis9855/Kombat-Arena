
#include "sgg/graphics.h"
#include <stdio.h>

void draw()
{
	graphics::Brush br;

	graphics::setOrientation(45);

	br.texture = "assets\\boy2.png";
	br.outline_opacity = 0.0f;

	graphics::drawRect(500, 250, 200, 200, br);

	br.fill_color[0] = 0.5f;
	br.fill_color[1] = 0.0f;
	br.fill_color[2] = 0.0f;

	graphics::resetPose();
	drawText(300, 440, 70, "Hello World!", br);

	
}

void update(float dt)
{

	graphics::MouseState mouse;
	graphics::getMouseState(mouse);
	if (mouse.button_left_released)
	{
		graphics::playSound("assets\\door2.wav", 1.0f, false);
	}

}

int main(int argc, char ** argv)
{
	graphics::createWindow(800, 600, "Hello World");

	graphics::setDrawFunction(draw);
	graphics::setUpdateFunction(update);

	graphics::setCanvasSize(800, 600);
	graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);


	graphics::startMessageLoop();

	return 0;

}
	