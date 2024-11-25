#include "GamesEngineeringBase.h"
#include "Run.h"
#include "CheckCollision.h"
#include <string>
#include "Map.h"

using namespace GamesEngineeringBase;
using namespace Run;

int main() {
	Window canvas;
	canvas.create(1024, 768, "Try to live!!!");
	Timer tim;
	int Count = 0;
	float ElapsedTime = 0.0f;
	int w_x = 0, w_y = 0;
	world w("Resources/tiles.txt");
	Camera camera = Camera(canvas.getWidth(), canvas.getHeight());
	player p = player(canvas.getWidth() / 2, canvas.getHeight() / 2, "Resources/hero.png");
	swarm npc;
	Power power;
	int Inut=0;
	while (Inut != 1) {
		cout << "WASD to move    F to use AOE Attack" << endl;
		cout << "Input 1 to start game" << endl;
		cin >> Inut;
	}
	int map;
	do {
		std::cout << "Input 1 to enter fixed map    Input 2 to enter loop map" << std::endl;
		std::cin >> map;
	} while (map != 1 && map != 2);
	if (map == 1) {
		while (running) {
			canvas.checkInput();
			canvas.clear();
			float dt = tim.dt();
			ElapsedTime += dt;
			Count++;
			if (ElapsedTime >= 1.0f) {
				cout << " FPS: " << (Count / ElapsedTime) << "  Score: " << Score << endl;
				Count = 0;
				ElapsedTime = 0.0f;
			}
			int x = 0, y = 0;
			int move = static_cast<int>((300.f * dt));
			if (canvas.keyPressed(VK_ESCAPE)) break;
			if (canvas.keyPressed('W')) y -= move;
			if (canvas.keyPressed('S')) y += move;
			if (canvas.keyPressed('A')) x -= move;
			if (canvas.keyPressed('D')) x += move;

			w_x -= x; w_y -= y;
			w.draw(canvas, camera);
			w.update(p, x, y, w.GetMapWidth(), w.GetMapHeight(), camera);

			npc.update(canvas, dt, p, w.GetMapWidth(), w.GetMapHeight());

			p.PlayUpdate(canvas, x, y, dt, npc, w.GetMapWidth(), w.GetMapHeight(), camera);

			power.update(canvas, dt, p);

			power.draw(canvas, camera);
			npc.draw(canvas, camera);
			p.draw(canvas, camera);
			canvas.present();
			
		}
	}

	else if (map == 2) {
		while (running) {
			canvas.checkInput();
			canvas.clear();
			float dt = tim.dt();
			ElapsedTime += dt;
			Count++;
			if (ElapsedTime >= 1.0f) {
				cout << " FPS: " << (Count / ElapsedTime) << "  Score: " << Score << endl;
				Count = 0;
				ElapsedTime = 0.0f;
			}
			int x = 0, y = 0;
			int move = static_cast<int>((300.f * dt));
			if (canvas.keyPressed(VK_ESCAPE)) break;
			if (canvas.keyPressed('W')) y -= move;
			if (canvas.keyPressed('S')) y += move;
			if (canvas.keyPressed('A')) x -= move;
			if (canvas.keyPressed('D')) x += move;
			w_x -= x; w_y -= y;
			w.draw(canvas, camera, w_x, w_y);

			npc.update(canvas, dt, p, w.GetMapWidth(), w.GetMapHeight());

			p.PlayUpdate(canvas, x, y, dt, npc, w.GetMapWidth(), w.GetMapHeight(), camera);

			power.update(canvas, dt, p);

			power.draw(canvas, camera);
			npc.draw(canvas, camera);
			p.draw(canvas, camera);
			canvas.present();
		}
	}
}