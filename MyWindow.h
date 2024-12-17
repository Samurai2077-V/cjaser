#pragma once
#include<Windows.h>
#include<string.h>
#include<iostream>
class Window
{
public:
	HWND hwnd;
	HINSTANCE hinstance;
	std::string name;
	int width;
	int height;
	bool keys[256];
	int mousex;
	int mousey;
	bool mouseButtons[3];
	void create(int window_width, int window_height, const std::string window_name, int window_x = 0, int window_y = 0);
	void processMessages();
	void updateMouse(int x, int y) {
		mousex = x;
		mousey = y;
	}
};



