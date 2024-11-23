#pragma once
#include <fstream>
#include <string>
#include "GamesEngineeringBase.h"
#include "camera.h"
#include "Run.h"
#include "other.h"

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

    void draw(GamesEngineeringBase::Window& canvas, int x, int y, Camera& cm) {
        Vector pos = Vector(x, y);
        renderImg(canvas, sprite, pos, cm);
    }

    bool collide(Run::hero& h, float newx, float newy) {
        auto& t = *this;
        if (checkImageCollision(h.sprite, h.pos.x + newx, h.pos.y + newy, t.sprite, t.x, t.y))
            return true;
        return false;
    }

    unsigned int getHeight() { return sprite.height; }
    unsigned int getWidth() { return sprite.width; }
    GamesEngineeringBase::Image& getSprite() { return sprite; }
};

class world {
    int tileswide;
    int tileshigh;
    int tilewidth;
    int tileheight;
    tile** tiles;

public:
    world(std::string filename) {
        std::ifstream infile(filename);
        std::string line;
        infile >> line;
        if (line == "tileswide") infile >> tileswide;
        infile >> line;
        if (line == "tileshigh") infile >> tileshigh;
        infile >> line;
        if (line == "tilewidth") infile >> tilewidth;
        infile >> line;
        if (line == "tileheight") infile >> tileheight;

        tiles = new tile * [tileshigh];
        for (int i = 0; i < tileshigh; ++i) {
            tiles[i] = new tile[tileswide];
        }

        skipLine(infile, 3);

        for (int i = 0; i < tileshigh; i++) {
            getline(infile, line);
            int pos = 0;
            for (int j = 0; j < tileswide; j++) {
                int num = 0;
                while (line[pos] != ',' && line[pos] != '\0') {
                    num = num * 10 + line[pos] - '0';
                    tiles[i][j].value = num;
                    pos++;
                }

                std::string filename = "Resources/tiles/" + std::to_string(num) + ".png";
                tiles[i][j].load(filename);
                pos++;
            }
        }

        infile.close();
    }

    void update(Run::hero& h, float x, float y, float mapWidth, float mapHeight, Camera& cm) {
        bool collisionXDetected = false;
        bool collisionYDetected = false;

        for (int i = 0; i < tileshigh; i++) {
            for (int j = 0; j < tileswide; j++) {
                if (tiles[i][j].value <= 22 && tiles[i][j].value >= 14) {
                    if (tiles[i][j].collide(h, x, 0)) {
                        collisionXDetected = true;
                    }
                    if (tiles[i][j].collide(h, 0, y)) {
                        collisionYDetected = true;
                    }
                    if (collisionXDetected && collisionYDetected) break;
                }
            }
            if (collisionXDetected && collisionYDetected) break;
        }

        if (!collisionXDetected) h.update(x, 0, mapWidth, mapHeight, cm);
        if (!collisionYDetected) h.update(0, y, mapWidth, mapHeight, cm);
    }

    void draw(GamesEngineeringBase::Window& canvas, Camera& cm) {
        for (int i = 0; i < tileshigh; i++) {
            if (i >= 0 && i < tileshigh) {
                for (int j = 0; j < tileswide; j++) {
                    if (j >= 0 && j < tileswide) {
                        tiles[i][j].draw(canvas, tilewidth * j, tileheight * i, cm);
                        tiles[i][j].x = tilewidth * j;
                        tiles[i][j].y = tileheight * i;
                    }
                }
            }
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, Camera& cm, int wx, int wy) {
        int r = wy % getMapHeight();
        int n = wx % getMapWidth();

        int xOffsets[3] = { -getMapWidth(), 0, getMapWidth() };
        int yOffsets[3] = { -getMapHeight(), 0, getMapHeight() };
        int viewportLeft = cm.Position.x - canvas.getWidth();
        int viewportRight = cm.Position.x + canvas.getWidth();
        int viewportTop = cm.Position.y - canvas.getHeight();
        int viewportBottom = cm.Position.y + canvas.getHeight();

        for (int yOffset : yOffsets) {
            for (int xOffset : xOffsets) {
                drawTiles(canvas, cm, n, r, xOffset, yOffset, viewportLeft, viewportRight, viewportTop, viewportBottom);
            }
        }
    }

    void drawTiles(GamesEngineeringBase::Window& canvas, Camera& cm, int n, int r, int xOffset, int yOffset,
        int viewportLeft, int viewportRight, int viewportTop, int viewportBottom) {
        for (int i = 0; i < tileshigh; ++i) {
            for (int j = 0; j < tileswide; ++j) {
                int tileX = tilewidth * j + n + xOffset;
                int tileY = tileheight * i + r + yOffset;
                if (tileX + tilewidth > viewportLeft && tileX < viewportRight && tileY + tileheight > viewportTop && tileY < viewportBottom) {
                    tiles[i][j].draw(canvas, tileX, tileY, cm);
                    tiles[i][j].x = tileX;
                    tiles[i][j].y = tileY;
                }
            }
        }
    }

    int getMapWidth() {
        return tileswide * tilewidth;
    }

    int getMapHeight() {
        return tileshigh * tileheight;
    }

private:
    void skipLine(std::ifstream& infile, int lineNumber) {
        std::string line;
        for (int i = 0; i < lineNumber; i++) {
            std::getline(infile, line);
        }
    }
};
