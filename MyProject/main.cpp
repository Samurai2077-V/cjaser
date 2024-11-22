#include "GamesEngineeringBase.h"
#include "Run.h"
#include "global.h"
#include <string>
#include "Save.h"
#include "Map.h"

using namespace GamesEngineeringBase;
using namespace Run;

int main() {
	Window canvas;
	canvas.create(1024, 768, "Try to live!!!");
	Timer tim;
	int frameCount = 0;
	float elapsedTime = 0.0f;
	world w("Resources/tiles.txt");
	Camera camera = Camera(canvas.getWidth(), canvas.getHeight());
	hero h = hero(canvas.getWidth() / 2, canvas.getHeight() / 2, "Resources/hero_1.png");
	swarm s;
	items itm;
	int choice;
	do {
		cout << "WASD to move    F to use skill" << endl;
		cout << "Input 1 is to start game" << endl;
		cin >> choice;
	} while (choice != 1 && choice != 2);

	if (choice == 1) level = getLevel();
	int wx = 0, wy = 0;

	if (level == 1) {
		while (running) {
			canvas.checkInput();
			canvas.clear();

			float dt = tim.dt();
			elapsedTime += dt;
			frameCount++;
			if (elapsedTime >= 1.0f) {
				float fps = frameCount / elapsedTime;
				frameCount = 0;
				elapsedTime = 0.0f;

				std::cout << " FPS: " << fps << "  Score: " << score << std::endl;
			}

			int x = 0, y = 0;
			int move = static_cast<int>((300.f * dt));
			if (canvas.keyPressed(VK_ESCAPE)) break;
			if (canvas.keyPressed('W')) y -= move;
			if (canvas.keyPressed('S')) y += move;
			if (canvas.keyPressed('A')) x -= move;
			if (canvas.keyPressed('D')) x += move;
			

			wx -= x; wy -= y;
			w.draw(canvas, camera);
			w.update(h, x, y, w.getMapWidth(), w.getMapHeight(), camera);

			s.update(canvas, dt, h, w.getMapWidth(), w.getMapHeight());
			h.hUpdate(canvas, x, y, dt, s, w.getMapWidth(), w.getMapHeight(), camera);
			itm.update(canvas, dt, h);

			itm.draw(canvas, camera);
			s.draw(canvas, camera);
			h.draw(canvas, camera);

			canvas.present();
			
		}
	}

	else if (level == 2) {
		while (running) {
			canvas.checkInput();
			canvas.clear();

			float dt = tim.dt();
			elapsedTime += dt;
			frameCount++;
			if (elapsedTime >= 1.0f) {
				float fps = frameCount / elapsedTime;
				frameCount = 0;
				elapsedTime = 0.0f;
				std::cout << " FPS: " << fps << "  Score: " << score << std::endl;
			}

			int x = 0, y = 0;
			int move = static_cast<int>((300.f * dt));
			if (canvas.keyPressed(VK_ESCAPE)) break;
			if (canvas.keyPressed('W')) y -= move;
			if (canvas.keyPressed('S')) y += move;
			if (canvas.keyPressed('A')) x -= move;
			if (canvas.keyPressed('D')) x += move;
			
			wx -= x; wy -= y;
			w.draw(canvas, camera, wx, wy);
			s.update(canvas, dt, h, w.getMapWidth(), w.getMapHeight());
			h.hUpdate(canvas, x, y, dt, s, w.getMapWidth(), w.getMapHeight(), camera);
			itm.update(canvas, dt, h);

			itm.draw(canvas, camera);
			s.draw(canvas, camera);
			h.draw(canvas, camera);
			canvas.present();
		}
	}
}