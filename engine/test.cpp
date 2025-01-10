#include "engine.h"

int main() {
	Scene scene;
	
	Block* test1 = new Block(
			Vector3f(0, 2, -2),
			Vector3f(10, 1, 1),
			Color::White
	);

	scene.addBlock("test1", test1);

	while (scene.isWindowOpen()) {
		scene.update();

		// float movement = 1 * scene.getDeltaSeconds();
		// scene.getBlock("test1")->move(Vector3f(movement, 0, 0));
	}

	return 0;
}

