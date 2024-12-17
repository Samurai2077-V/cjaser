#include "MyWindow.h"
#include "Math.h"
#include "DX.h"
#include "Shaders.h"
#include "GamesEngineeringBase.h"
#include "Object.h"
#include "Camera.h"
#include "Player.h"
#include"Trees.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window win;
	DXcore dx;
	shader Gun;
	shader Common;
	Vec3 TEXscale(7, 7, 7);
	Vec3 Gunscale(0.2, 0.2, 0.2);
	Vec3 move(0.f, 0.f, 0.f);
	CameraManager camera;
	GunModel GunModel;
	Plane p;
	Sphere SKYbox;
	Vec3 position(100.0f, 5.0f, -100.0f);
	GEMModelanimation TEX;
	Mesh mesh;
	TextureManager text;
	GamesEngineeringBase::Timer tim;
	win.create(1920, 1080, "FPS");
	dx.init(1920, 1080, win.hwnd, 0,dx);
	Gun.init_move("GUNVS.txt", "GUNPS.txt", dx);
	Common.init_move("CommomVS.txt", "GUNPS.txt", dx);
	p.init(dx, &text, "Textures/GRASS.jpg");
	camera.initialize(dx, text);
	GunModel.init(dx, "Model/Automatic_Carbine_5.gem", text);
	TEX.init(dx, "TRex.gem", text);
	TEX.setPosition(position);
	TEX.scaleModel(TEXscale);
	SKYbox.init(dx, 512, 512, 512, &text, "Textures/SKY.png");
	SetCursorPos(960, 540);
	GunModel.scale(Gunscale);
	std::vector<GEMModel> trees;
	std::vector<Vec3> positions = loadTreePositions("Trees.txt");
	for (const auto& pos : positions) {
		GEMModel tree;
		tree.init(dx, "Model/pine.gem", text);
		tree.scaleModel(Vec3(0.1f, 0.1f, 0.1f));
		tree.setPosition(pos);
		trees.push_back(tree);
	}

	while (true)
	{
		win.processMessages();
		dx.clear();
		float dt = tim.dt();
		ShowCursor(FALSE);  
		camera.handleInput(win.hwnd, 0.1f, dt, win.keys);
		TEX.ani("Run", dt);
		GunModel.draw(&Gun, dx, text, dt, camera);
		p.draw(&Common, dx, &text, "Textures/GRASS.jpg", camera);
		SKYbox.draw(&Common, dx, camera, &text, "Textures/SKY.png");
		for (int i = 0; i < 50; i++) {
			trees[i].draw(&Common, dx, text, camera);
		}
		TEX.chaseCamera(camera, dt, 0.7);
		TEX.draw(&Gun, dx, text, dt, camera);
		dx.present();
		if (GetAsyncKeyState(VK_ESCAPE)) {
			break;
		}
	}
}
