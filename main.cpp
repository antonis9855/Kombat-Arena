
#include "sgg/graphics.h"
#include <stdio.h>
#include <string>
#include "util.h"


struct GameState
{
	std::string m_asset_path = "assets\\";
	float m_canvas_width = 800;
	float m_canvas_height = 600;

	graphics::Brush m_brush_bkrnd;
};

void draw()
{
	graphics::Brush br;

	graphics::setOrientation(90);

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

GameState state;

int main(int argc, char ** argv)
{
	graphics::createWindow(800, 600, "Hello World");

	graphics::setDrawFunction(draw);
	graphics::setUpdateFunction(update);

	graphics::setCanvasSize(state.m_canvas_width, state.m_canvas_height);
	graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);


	graphics::startMessageLoop();

	return 0;

}
	