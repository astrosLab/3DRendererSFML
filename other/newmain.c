#include <SFML/Graphics.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

float FOV = 90;
float focal;

sfVector3f point0 = {50, 200, 1};
sfVector3f point1 = {-50, 200, 1};
sfVector3f point2 = {50, 200, 1.5};
sfVector3f point3 = {-50, 200, 1.5};

sfVector3f plane[4] = {
	{50, 200, 1},
	{-50, 200, 1},
	{50, 200, 1.5},
	{-50, 200, 1.5}
};

sfVector2f translate(sfVector3f point) {
	sfVector2f newpoint;
	newpoint.x = (point.x * focal) / point.z + 400;
	newpoint.y = (point.y * focal) / point.z + 300;
	return newpoint;
}

void renderPoint(sfRenderWindow* window, sfVector3f point) {
	sfVector2f newPoint;

	newPoint.x = (point.x * focal) / point.z + 400;
	newPoint.y = (point.y * focal) / point.z + 300;

	//printf("New point: {%f, %f}\n", newPoint.x, newPoint.y);

	sfCircleShape* dot = sfCircleShape_create();
	sfCircleShape_setFillColor(dot, sfRed);

	//float radius = fmax(2, 10 / point.z);
	//sfCircleShape_setRadius(dot, radius);
	sfCircleShape_setRadius(dot, 2);
	sfCircleShape_setPosition(dot, newPoint);

	sfRenderWindow_drawCircleShape(window, dot, NULL);
	sfCircleShape_destroy(dot);
}

void renderPlane(sfRenderWindow* window, sfVector3f arr[]) {

}

int main() {
	sfVideoMode mode = {800, 600, 32};
	sfRenderWindow *window = sfRenderWindow_create(mode, "3D Renderer", sfClose, NULL);

	focal = 1 / tan(FOV / 2);

	while (sfRenderWindow_isOpen(window)) {
		sfEvent event;
		while (sfRenderWindow_pollEvent(window, &event)) {
			if (event.type == sfEvtClosed) {
				sfRenderWindow_close(window);
			}
		}

		sfRenderWindow_clear(window, sfBlack);

		renderPoint(window, point0);
		renderPoint(window, point1);
		renderPoint(window, point2);
		renderPoint(window, point3);

		sfRenderWindow_display(window);
		sleep(1);
	}
	
	sfRenderWindow_destroy(window);

	return 0;
}

