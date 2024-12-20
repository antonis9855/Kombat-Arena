
#include "sgg/graphics.h"
#include <stdio.h>

void draw()
{
	graphics::Brush br;


	


	
}

void update(float dt)
{

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
	