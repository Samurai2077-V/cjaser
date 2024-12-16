#include "MyWindow.h"
#include "DX.h"
#include "DXnew.h"
#include "Mash.h"
#include "shaders.h"
#include "GEbase.h"
#include "shaders.h"
#include "Geometry.h"
#include "Camera.h"
#include "Soldier.h"
#include "Forest.h"



int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window win;
	DXcore dx;
	DXcoreDeferred dxd;
	shader shader1;
	shader shader2;
	shader shader3;
	shader watershader;
	shader rainshader;
	Vec3 scale(5, 5, 5);
	Vec3 pinescale(0.2, 0.2, 0.2);
	Vec3 soldierscale(0.2, 0.2, 0.2);
	Vec3 move(0.f, 0.f, 0.f);
	CameraManager camera;
	Camera camera1;
	SoldierModel soldier;
	Plane plane;
	Water water;
	Sphere sphere;
	Vec3 position(200.0f, 0.0f, 200.0f);
	GEMModelanimation gemani;
	GEMModel pine;
	GEMModel pine1;
	Mash mesh;
	SimplePond pond;
	TextureManager text;
	ParticleSystem rainSystem;
	GamesEngineeringBase::Timer tim;
	
	win.create(1920, 1080, "FPS");
	dx.init(1920, 1080, win.hwnd, 0,dx);
	dxd.init(1920, 1080, win.hwnd, 0);
	shader1.init_move("AVS.txt", "tPS.txt", dx);
	shader2.init_move("NewVertixShader.txt", "tPS.txt", dx);
	shader3.init_move("VS.txt", "PS1.txt", dx);
	watershader.init_move("watervs.txt", "waterps.txt", dx);
	rainshader.init_move("rainvs.txt", "rainps.txt", dx);
	plane.init(dx, &text, "Textures/vh1jeck_2K_Basecolor.JPG");
	water.init(dx, &text, "Textures/Water.png");
	pond.init(dx, &text, "Textures/stone.jpg");
	pond.setPosition();
	camera.initialize(dx, text);
	rainSystem.init(dx, Vec3(0, 50, 0), 1000,camera);
	soldier.init(dx, "Model/Automatic_Carbine_5.gem", text);
	gemani.init(dx, "TRex.gem", text);
	gemani.setPosition(position);
	gemani.scaleModel(scale);
	gemani.setcollider(Vec3(30, 1, 35));
	
	pine.init(dx, "Model/flower1.gem", text);
	pine.scaleModel(Vec3(0.1, 0.1, 0.1));
	pine1.init(dx, "Model/flower1.gem", text);
	pine1.scaleModel(Vec3(0.1, 0.1, 0.1));
	pine1.setPosition(Vec3(5, 0, 5));
	AABB checkcollision;
	sphere.init(dx, 64, 128, 500, &text, "Textures/kloppenheim_06_puresky.png");
	SetCursorPos(960, 540);
	soldier.scale(soldierscale,camera);
	std::vector<GEMModel> trees;
	std::vector<Vec3> positions = loadTreePositions("forest.txt");
	for (const auto& pos : positions) {
		GEMModel tree;
		tree.init(dx, "Model/pine.gem", text);
		tree.scaleModel(Vec3(0.1f, 0.1f, 0.1f));
		tree.setPosition(pos);
		trees.push_back(tree);
	}

	enum RENDER_MODE { FORWARD, DEFERRED };
	RENDER_MODE currentMode = FORWARD;
	float time = 0.0f;
	

	while (true)
	{
		if (currentMode == DEFERRED) {
			win.processMessages();
			dxd.clearGBuffer();
			
			dxd.renderGeometryPass();
			sphere.draw(&shader2, dx, camera, &text, "Textures/kloppenheim_06_puresky.png");
			dxd.renderLightPass();
			dxd.present();
		}
		else {
			win.processMessages();
			dx.clear();
			float dt = tim.dt();
			
			time = time + tim.dt()*100 * 1000;
			ShowCursor(FALSE);
			dx.updatewaveBuffer(time);
			dx.updateLightBuffer();
			camera.handleInput(win.hwnd, 0.1f, dt, win.keys);
			if (soldier.isColliding(gemani)) {
				gemani.ani("attack", dt);
			}
			else {
				gemani.ani("Run", dt);
			}
			rainSystem.update(dt,camera);
			//rainSystem.draw(&rainshader, dx,camera);

			soldier.draw(&shader1, dx, text, dt, camera);
			plane.draw(&shader2, dx, &text, "Textures/vh1jeck_2K_Basecolor.JPG", camera);
			sphere.draw(&shader2, dx, camera, &text, "Textures/kloppenheim_06_puresky.png");  //"Textures/kloppenheim_06_puresky.png"
			//water.draw(&watershader, dx, &text, "Textures/Water.png", camera,dt);
			pond.draw(&shader2, dx, &text, "Textures/stone.jpg", camera);
			for (int i = 0; i < 500; i++) {
				trees[i].draw(&shader2, dx, text, camera);
			}
			gemani.chaseCamera(camera, dt, 1);
			gemani.draw(&shader1, dx, text, dt, camera);
			dx.present();
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
				ShowCursor(TRUE);
				break;
			}
		}
		

	}
}