#pragma once
#include <fstream>
#include <string>
#include "GamesEngineeringBase.h"
#include "camera.h"
#include "Run.h"
#include"CheckCollision.h"

void skip(std::ifstream& infile, int number) { //Skip some lines
    std::string line;
    for (int i = 0; i < number; i++) {
        if (!std::getline(infile, line)) {
            break;
        }
    }
}

class tile {
public:
    int x;
    int y;
    GamesEngineeringBase::Image sprite;
    int value;

    tile() {}

    void load(std::string filename) {
        sprite.load(filename);
    }

    void draw(GamesEngineeringBase::Window& canvas, int x, int y, Camera& camera) {
        Vector p = Vector(x, y);
        DrawSprite(canvas, sprite, p, camera);
    }

    bool collision(Run::player& h, float Move_x, float Move_y) {

        return CheckCollision(h.sprite, h.pos.x + Move_x, h.pos.y + Move_y, this->sprite, this->x, this->y);
    }
    
    auto getHeight() -> unsigned int {
        return sprite.height;
    }
    auto getWidth() -> unsigned int {
        return sprite.width;
    }

   
};

class world {
    int tileswide;
    int tileshigh;
    int tilewidth;
    int tileheight;
    tile** tiles;     //2D arry

public:
    world(std::string filename) {
        std::ifstream infile(filename);
        std::string words;
        infile >> words;
        if (words == "tileswide") infile >> tileswide;
        infile >> words;
        if (words == "tileshigh") infile >> tileshigh;
        infile >> words;
        if (words == "tilewidth") infile >> tilewidth;
        infile >> words;
        if (words == "tileheight") infile >> tileheight;

        tiles = new tile * [tileshigh];
        for (int i = 0; i < tileshigh; ++i) {
            tiles[i] = new tile[tileswide];
        }
        skip(infile, 3);

        for (int i = 0; i < tileshigh; i++) {
            getline(infile, words);
            int cell = 0;
            for (int j = 0; j < tileswide; j++) {
                int num = 0;
                while (words[cell] != ',' && words[cell] != '\0') {
                    num = num * 10 + words[cell] - '0';
                    tiles[i][j].value = num;
                    cell++;
                }

                std::string filename = "Resources/tiles/" + std::to_string(num) + ".png";
                tiles[i][j].load(filename);
                cell++;
            }
        }

        infile.close();
    }
    int GetMapWidth()
    {
        return tileswide * tilewidth;
    }
    int GetMapHeight()
    {
        return tileshigh * tileheight;
    }
        
    void update(Run::player& p, float x, float y, float Width, float Height, Camera& camera) {
        bool hasCollisionX = false;
        bool hasCollisionY = false;
        for (int i = 0; i < tileshigh; i++) {
            for (int j = 0; j < tileswide; j++) {
                if (tiles[i][j].value >= 14 && tiles[i][j].value <= 22) {
                    if (tiles[i][j].collision(p, x, 0)) {
                        hasCollisionX = true;
                    }
                    if (tiles[i][j].collision(p, 0, y)) {
                        hasCollisionY = true;
                    }
                    if (hasCollisionX && hasCollisionY) break;
                }
            }
            if (hasCollisionX && hasCollisionY) break;
        }

        if (!hasCollisionX)
        {
            p.update(x, 0, Width, Height, camera);
        }
        if (!hasCollisionY)
        {
            p.update(0, y, Width, Height, camera);
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, Camera& camera) {
        for (int i = 0; i < tileshigh; i++) {
            if (i >= 0 && i < tileshigh) {
                for (int j = 0; j < tileswide; j++) {
                    if (j >= 0 && j < tileswide) {
                        tiles[i][j].draw(canvas, tilewidth * j, tileheight * i, camera);
                        tiles[i][j].x = tilewidth * j;
                        tiles[i][j].y = tileheight * i;
                    }
                }
            }
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, Camera& camera, int worldx, int worldy) {
        int remainderX = worldx % GetMapWidth();
        int remainderY = worldy % GetMapHeight();
        int X[] = { -GetMapWidth(), 0, GetMapWidth() };
        int Y[] = { -GetMapHeight(), 0, GetMapHeight() };

        int Left = camera.Position.x - canvas.getWidth();

        int Right = camera.Position.x + canvas.getWidth();

        int Top = camera.Position.y - canvas.getHeight();

        int Bottom = camera.Position.y + canvas.getHeight();

        for (int i = 0; i < 3; ++i) {
            int yOffset = Y[i];
            for (int j = 0; j < 3; ++j) {
                int xOffset = X[j];
                drawTiles(canvas, camera, remainderX, remainderY, xOffset, yOffset, Left, Right, Top, Bottom);
            }
        }
    }

    void drawTiles(GamesEngineeringBase::Window& canvas, Camera& camera, int n, int r, int X, int Y,
        int Left, int Right, int Top, int Bottom) {
        for (int i = 0; i < tileshigh; ++i) {
            for (int j = 0; j < tileswide; ++j) {
                int tile_x = tilewidth * j + n + X;
                int tile_y = tileheight * i + r + Y;
                if (tile_x + tilewidth > Left && tile_x < Right && tile_y + tileheight > Top && tile_y < Bottom) {
                    tiles[i][j].draw(canvas, tile_x, tile_y, camera);
                    tiles[i][j].x = tile_x;
                    tiles[i][j].y = tile_y;
                }
            }
        }
    }

    
};
