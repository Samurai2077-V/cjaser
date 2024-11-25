#pragma once
#include "GamesEngineeringBase.h"
#include <iostream>
#include <fstream>
#include<string>
#include <cmath>
#include "CheckCollision.h"
#include "camera.h"

using namespace GamesEngineeringBase;
using namespace std;

void Normalize(Vector& V) {
	float length = sqrtf(V.x * V.x + V.y * V.y);
	if (length != 0) {
		V.x /= length;
		V.y /= length;
	}
}
void DrawSprite(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Image& sprite, Vector& v, Camera& camera) {
	unsigned int spriteHeight = sprite.height;
	unsigned int spriteWidth = sprite.width;
	for (unsigned int i = 0; i < spriteHeight; i++) {
		int camera_y = v.y + i - camera.Position.y;
		if (camera_y < 0 || camera_y >= canvas.getHeight()) {
			continue; 
		}
		for (unsigned int n = 0; n < spriteWidth; n++) {
			int camera_x = v.x + n - camera.Position.x;
			if (camera_x < 0 || camera_x >= canvas.getWidth()) {
				continue; 
			}
			if (sprite.alphaAt(n, i) > 200) {
				canvas.draw(camera_x, camera_y, sprite.at(n, i));
			}
		}
	}
}


namespace Run {
	bool running = true;
	bool Triggered = false;
	float Hero_Attack_Speed = 100;
	int AOE_Number = 3;
	int Score = 0;
	const int MaxSize = 1000;

	class Health {
		float MaxHp;
		float CurrentHp;

	public:
		Health()
		{
			MaxHp = 300;
			CurrentHp = 300;
		}

		Health(float _max)
		{
			MaxHp = _max;
			CurrentHp = _max;
		}

		void takeDamage(float damage) {
			CurrentHp -= damage;
			if (CurrentHp < 0.0f) {
				CurrentHp = 0.0f;
			}
		}

		void heal(float Heal) {
			CurrentHp += Heal;
			if (CurrentHp > MaxHp) {
				CurrentHp = MaxHp;
			}
		}

		bool isDead() {
			return CurrentHp <= 0.0f;
		}

		float getHp() {
			return CurrentHp;
		}

		float getMaxHp() {
			return MaxHp;
		}
	};

	class Character {    //to create player and NPC
	protected:
		Health* HP;
		float Speed;
		string filename;

	public:
		Vector pos;
		Image sprite;

		Character(int _x, int _y, string _filename) {
			HP = new Health(00.f);
			Speed = 0.f;
			pos.x = _x;
			pos.y = _y;
			filename = _filename;
			sprite.load(filename);
		}

		void update(float _x, float _y) {
			pos.x += _x;
			pos.y += _y;
		}

		virtual void update(Window& canvas, Character& p, float dt) {
			Vector v = p.pos - pos;
			Normalize(v);

			float dis = Speed * dt * 10;

			pos.x += v.x * dis;

			pos.y += v.y * dis;
		}

		virtual void update(int _x, int _y, float Width, float Height, Camera& m) {
			pos.x += _x;
			pos.y += _y;

			if (pos.x < 0) pos.x = 0;
			if (pos.y < 0) pos.y = 0;

			if (pos.x + sprite.width > Width) pos.x = Width - sprite.width;
			if (pos.y + sprite.height > Height) pos.y = Height - sprite.height;

			m.update(pos.x, pos.y, Width, Height, sprite);
		}

		virtual void draw(Window& canvas, Camera& m) {
			DrawSprite(canvas, sprite, pos, m);
		}

		bool collide(Character& p) {
			if (CheckCollision(sprite, pos.x, pos.y, p.sprite, p.pos.x, p.pos.y))
			{
				return true;
			}
				
			else return false;
		}

		Health& getHealth() {
			return *HP;
		}


	};

	class Bullets {
		Vector v;
	public:
		Image sprite;
		Vector pos;
		float speed = 15.f;

		Bullets(string filename) { sprite.load(filename); }

		Bullets(Vector _pos, Vector _vel, string filename) {
			pos = _pos;
			v = _vel;
			sprite.load(filename);
		}

		void update(int _speed, float dt) {
			speed = _speed;
			Normalize(v);
			float dis = speed * dt * 10;
			pos.x += v.x * dis;
			pos.y += v.y * dis;
		}

		void draw(Window& canvas, Camera& m) {
			DrawSprite(canvas, sprite, pos, m);
		}

		bool collide(Character& h) {
			if (&h == nullptr) return false;
			if (this == nullptr) return false;
			if (CheckCollision(sprite, pos.x, pos.y, h.sprite, h.pos.x, h.pos.y))
			{
				return true;
			}	
			else return false;
		}

	};

	class npc1 : public Character {
	public:
		npc1(int _x, int _y, string _filename) :Character(_x, _y, _filename) {
			delete HP;
			HP = new Health(200);
			Speed = 15;
		}
	};

	class npc2 : public Character {
	public:
		npc2(int _x, int _y, string _filename) :Character(_x, _y, _filename) {
			delete HP;
			HP = new Health(250);
			Speed = 10;
		}
	};

	class npc3 : public Character {
		float Elapsed = 0.f;

	public:
		int currentSize = 0;
		Bullets* parray[MaxSize];

		npc3(int _x, int _y, string _filename) :Character(_x, _y, _filename) {
			delete HP;
			HP = new Health(100);
			Speed = 0;
		}

		void Shoot(Character& h, float dt) {
			Elapsed += dt;
			if (Elapsed > 2) {
				Bullets* B = new Bullets(pos, h.pos - pos, "Resources/NPC_Bullet2.png");
				parray[currentSize++] = B;
				Elapsed = 0.f;
			}
		}

		void deleteB(int i) {
			Bullets* A = parray[i];
			parray[i] = nullptr;
			delete A;
		}

		void update(float Width, float Height, Character& h, float dt) {
			Shoot(h, dt);

			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr) {
					parray[i]->update(parray[i]->speed, dt);

					if (parray[i]->pos.y + 2 > static_cast<int>(Height) || parray[i]->pos.x + 2 > static_cast<int>(Width) || parray[i]->pos.y - 2 < 0 || parray[i]->pos.x - 2 < 0)						
					{
						deleteB(i);
					}
				}
			}
		}

		void draw(Window& canvas, Camera& m)  {
			DrawSprite(canvas, sprite, pos, m);

			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr)
				{
					parray[i]->draw(canvas, m);
				}
					
			}
		}
	};

	class npc4 : public Character {
	public:
		npc4(int _x, int _y, string _filename) :Character(_x, _y, _filename) {
			delete HP;
			HP = new Health(200);
			Speed = 15;
		}
	};

	class Boom {
	public:
		Vector position;
		float duration;
		float elapsedTime;
	};

	class PowerUp {
		Vector pos;
		Image sprite;
	public:

		PowerUp(int x, int y) {
			pos.x = x;
			pos.y = y;
			sprite.load("Resources/Power.png");
		}

		bool collide(Character& h) {
			if (&h == nullptr) return false;
			if (this == nullptr) return false;
			if (CheckCollision(sprite, pos.x, pos.y, h.sprite, h.pos.x, h.pos.y))
				return true;
			return false;
		}

		void draw(Window& canvas, Camera& cm) {
			DrawSprite(canvas, sprite, pos, cm);
		}

	};

	class swarm {
		float timeElapsed = 0.f;
		float timeThreshold = 3.f;

	public:
		int currentSize = 0;
		Character* sarray[MaxSize];

		swarm() {
			for (int i = 0; i < MaxSize; ++i) {
				sarray[i] = nullptr;
			}
		}

		void deleteNpc(Window& canvas, int i) {
			Character* p = sarray[i];
			sarray[i] = nullptr;
			delete p;
		}

		void update(Window& canvas, float dt, Character& h, float mapWidth, float mapHeight) {
			int npcType = rand() % 4;
			CreateNpc(mapWidth, mapHeight, dt, npcType);
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr) {
					if (dynamic_cast<npc3*>(sarray[i])) {
						npc3* n3 = dynamic_cast<npc3*>(sarray[i]);
						n3->update(mapWidth, mapHeight, h, dt);
					}
					else
						sarray[i]->update(canvas, h, dt);
				}
			}
			checkCollision(canvas, h, dt);
		}

		void draw(Window& canvas, Camera& cm) {
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr)
					sarray[i]->draw(canvas, cm);
			}
		}


		void CreateNpc(float Width, float Height, float dt, int npcID) {
			timeElapsed += dt;
			if (timeElapsed > timeThreshold) {
				Character* p;
				int direction = rand() % 4;
				int x, y; 
				switch (direction) {
				case 0: x = rand() % static_cast<int>(Width); y = -100; break; 
				case 1: x = rand() % static_cast<int>(Width); y = static_cast<int>(Height) + 100; break; 
				case 2: x = -100; y = rand() % static_cast<int>(Height); break; 
				case 3: x = static_cast<int>(Width) + 100; y = rand() % static_cast<int>(Height); break; 
				}

				switch (npcID)
				{
				case0:new npc1(x, y, "Resources/NPC1.png");
				case1:new npc1(x, y, "Resources/NPC1.png");
				case2:new npc4(x, y, "Resources/NPC3.png");
				case3:new npc3(rand() % static_cast<int>(Width), rand() % static_cast<int>(Height), "Resources/NPC4.png");
				}
				


				sarray[currentSize++] = p;

				timeElapsed = 0.f;

				timeThreshold -= 0.1f;
				

			}
		}

		void checkCollision(Window& canvas, Character& h, float dt) {
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr) {
					if (dynamic_cast<npc3*>(sarray[i])) {
						npc3* npc = dynamic_cast<npc3*>(sarray[i]);
						for (int j = 0; j < npc->currentSize; j++) {
							if (npc->parray[j]->collide(h)) {
								h.getHealth().takeDamage(20);
								if (h.getHealth().isDead()) {
									cout << "You Dead!" << endl;
									running = false;
								}
								npc->deleteB(j);
							}
						}
					}
					if (sarray[i]->collide(h)) {
						timeElapsed += dt;
						if (timeElapsed > 1.f) {
							h.getHealth().takeDamage(20);
							timeElapsed = 0.f;
						}
						if (h.getHealth().isDead()) {
							cout << "You Dead!" << endl;
							running = false;
						}
					}
				}
			}
		}
	};


	int const maxBOOM = 500;

	class player : public Character {
		float timeElapsed = 0;
		float aoeTimeElapsed = 0;
		int Count = 0;
		float frameTime;
		float aoeCooldownDuration = 10.0f;
		Image MyPlayer;
		Image lightning;
	public:
		Boom activeEffects[maxBOOM];
		float efficiency = 1.8f;

		int currentSize = 0;
		Bullets* b[MaxSize];

		player(int _x, int _y, string filename) :Character(_x, _y, filename) {
			delete HP;
			HP = new Health(200.f);
			Speed = 5.f;
			pos.x = _x;
			pos.y = _y;
			lightning.load("Resources/Boom.png");
			MyPlayer.load("Resources/hero.png");
		}

		void PlayUpdate(Window& canvas, float _x, float _y, float dt, swarm& s, float Width, float Height, Camera& m) {
			NPC_Shoot(*this, dt, s);
			checkCollision(canvas, s, dt, m);
			updateEffects(canvas, m, dt);

			for (int i = 0; i < currentSize; i++) {
				if (b[i] != nullptr) {
					b[i]->update(Hero_Attack_Speed, dt);
					if (b[i]->pos.y + 2 > static_cast<int>(Height)
						|| b[i]->pos.x + 2 > static_cast<int>(Width)
						|| b[i]->pos.y - 2 < 0
						|| b[i]->pos.x - 2 < 0) {
						deleteB(i);
					}
				}
			}
		}

		void draw(Window& canvas, Camera& camera) override {
			DrawSprite(canvas, MyPlayer, pos, camera);

			for (int i = 0; i < currentSize; i++) {
				if (b[i] != nullptr)
					b[i]->draw(canvas, camera);
			}
		}

		void updateEffects(Window& canvas, Camera& m, float dt) {
			for (int i = 0; i < Count; i++) {
				activeEffects[i].elapsedTime += dt;
				if (activeEffects[i].elapsedTime < activeEffects[i].duration)
					DrawSprite(canvas, lightning, activeEffects[i].position, m);
				else {
					for (int j = i; j < Count - 1; j++) {
						activeEffects[j] = activeEffects[j + 1];
					}
					Count--;
					i--; 
				}
			}
		}

		int Radar(Character& h, swarm& s) {
			int closestIndex = -1;
			float minDistance = 1e30f;
			for (int i = 0; i < s.currentSize; i++) {
				if (s.sarray[i] != nullptr) {
					float dis = h.pos.distance(s.sarray[i]->pos);
					if (dis < minDistance) {
						minDistance = dis;
						closestIndex = i;
					}
				}
			}
			return closestIndex;
		}

		void checkCollision(Window& canvas, swarm& s, float dt, Camera& m) {
			for (int i = 0; i < currentSize; i++) {
				if (b[i] != nullptr) {
					for (int j = 0; j < s.currentSize; j++) {
						if (b[i]->collide(*s.sarray[j])) {
							deleteB(i);
							s.sarray[j]->getHealth().takeDamage(50.f);
							if (s.sarray[j]->getHealth().isDead()) {
								s.deleteNpc(canvas, j);
								Score += 1;
							}
						}
					}
				}
			}

			aoeTimeElapsed += dt;
			if (canvas.keyPressed('F') && aoeTimeElapsed >= aoeCooldownDuration) {
				
					triggerAOE(canvas, s, AOE_Number, m, dt);
					Triggered = true;
					aoeTimeElapsed = 0;
			}
			else
				Triggered = false;
		}

		void NPC_Shoot(Character& h, float dt, swarm& s) {
			timeElapsed += dt;
			Vector i = pos + Vector(sprite.width / 2, sprite.height / 2);
			int closestIndex = Radar(h, s);
			if (closestIndex != -1) {
				if (timeElapsed > efficiency) {
					int w = s.sarray[closestIndex]->sprite.width;
					int h = s.sarray[closestIndex]->sprite.height;
					Vector closest = s.sarray[closestIndex]->pos + Vector(w / 2, h / 2);
					Bullets* p = new Bullets(i, closest - i, "Resources/NPC_Bullet1.png");
					b[currentSize++] = p;
					timeElapsed = 0.f;
				}
			}
		}

		void triggerAOE(Window& canvas, swarm& s, int N, Camera& m, float dt) {
			for (int n = 0; n < N; n++) {
				int MAX = -1;
				float maxHealth = -100.f;
				for (int i = 0; i < s.currentSize; i++) {
					if (s.sarray[i] != nullptr) {
						float HP = s.sarray[i]->getHealth().getHp();
						if (HP > maxHealth) {
							maxHealth = HP;
							MAX = i;
						}
					}
				}

				if (MAX != -1) {
					Character* npc = s.sarray[MAX];
					Vector actualPos = Vector(npc->pos.x + npc->sprite.width - lightning.width / 2, npc->pos.y + npc->sprite.height - lightning.height);
					Boom boom = { actualPos, 0.2f, 0.0f };
					activeEffects[Count++] = boom;
					npc->getHealth().takeDamage(500);
					if (npc->getHealth().isDead()) {
						s.deleteNpc(canvas, MAX);
						Score += 10;
					}
				}
			}
		}

		void deleteB(int i) {
			Bullets* a = b[i];
			b[i] = nullptr;
			delete a;
		}
	};

	class Power {                    
		float ElapsedTime = 0.f;   
		float CooldownTime = 10.f;

	public:
		int currentSize = 0;
		PowerUp* pow[MaxSize];

		Power() {
			for (int i = 0; i < MaxSize; ++i) {
				pow[i] = nullptr;
			}
		}

		void update(Window& canvas, float dt, player& p) {
			CreatePower(canvas, dt);
			for (int i = 0; i < currentSize; i++) {
				if (pow[i]->collide(p)) {
					Hero_Attack_Speed += 10.f;
					p.efficiency -= 0.2f;
					AOE_Number += 1;
					cout << "Give me power!" << endl;
					DeletePower(i);
				}
			}
		}

		void draw(Window& canvas, Camera& cm) {
			for (int i = 0; i < currentSize; i++) {
				if (pow[i] != nullptr)
					pow[i]->draw(canvas, cm);
			}
		}
		void CreatePower(Window& canvas, float dt) {
			ElapsedTime += dt;
			if (ElapsedTime > CooldownTime) {
				PowerUp* p = new PowerUp(rand() % canvas.getWidth(), rand() % canvas.getHeight());
				if (p == nullptr) return;
				pow[currentSize++] = p;
				ElapsedTime = 0.f;
			}
		}
		void DeletePower(int delete_power) {
			PowerUp* c = pow[delete_power];
			pow[delete_power] = nullptr;
			delete c;
		}
	};
}