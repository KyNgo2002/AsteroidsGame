#include <iostream>
#include <algorithm>
#include <string>
#include "olcConsoleGameEngine.h"

using namespace std;


class OneLoneCoder_Asteroids : public olcConsoleGameEngine
{
public: 
	OneLoneCoder_Asteroids() {
		m_sAppName = L"Asteroids";
	}
	
private:

	//Structure for all objects created in this game
	struct spaceObject {
		float x;
		float y;
		float dx;
		float dy;
		int size; 
		float angle;
	};
	vector<spaceObject> Asteroids;
	vector<spaceObject> Bullets;
	spaceObject player;
	int score = 0;
	bool dead = false;
	vector<pair<float, float>> vectorModelShip;
	vector<pair<float, float>> vectorModelAsteroids;
protected:

	//initializes model of the ship and creates first asteroids
	virtual bool OnUserCreate() {
		

		vectorModelShip = {
			{0.0f, -5.0f},
			{-2.5f, 2.5f},
			{2.5f, 2.5f}
		};

		int size = 20;
		for (int i = 0; i < size; i++) {
			float radius = (float)rand()/(float)RAND_MAX * 0.4f + 0.8f;
			float a = ((float)i / (float)size) * 6.28318f;
			vectorModelAsteroids.push_back(make_pair(radius * sinf(a), radius * cosf(a)));
		}

		ResetGame();
		return true;
	}

	//Pythagoreas theorem, checks if bullets or player has collided with an asteroid
	bool isPointInsideCircle(float cx, float cy, float radius, float x, float y)
	{
		return sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) < radius;
	}

	//Resets game to initial state after player death
	void ResetGame() {
		Asteroids.clear();
		Bullets.clear();
		Asteroids.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16 , 0.0f });
		Asteroids.push_back({ 100.0f, 20.0f, -5.0f, 3.0f, (int)16 , 0.0f });
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;
		dead = false;
		score = 0;
	}



	//Updates the state of the game(player movement, asteroid movement, bullet movement etc)
	virtual bool OnUserUpdate(float fElapsedTime) {
		if (dead) {
			ResetGame();
		}
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

		//Detects if keys have been pressed
		if (m_keys[VK_LEFT].bHeld) {
			player.angle -= 6.0f * fElapsedTime;
		}
		if (m_keys[VK_RIGHT].bHeld) {
			player.angle += 6.0f * fElapsedTime;
		} 
		if (m_keys[VK_UP].bHeld) {
			player.dx += sin(player.angle) * 20.0f * fElapsedTime;
			player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
		}

		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;


		WrapCoordinates(player.x, player.y, player.x, player.y);


		if (m_keys[VK_SPACE].bReleased) {
			Bullets.push_back({ player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 0 , 0});
		}
		for (auto &a : Asteroids) {
			if (isPointInsideCircle(a.x, a.y, a.size, player.x, player.y)) {
				dead = true;
			}
		}

		//Determines asteroid movement and draws frames of asteroids
		for (auto &temp : Asteroids) {
			temp.x += temp.dx * fElapsedTime;
			temp.y += temp.dy * fElapsedTime;
			temp.angle += 0.5f * fElapsedTime;
			WrapCoordinates(temp.x, temp.y, temp.x, temp.y);
			drawWireFrame(vectorModelAsteroids, temp.x, temp.y, temp.angle, temp.size, FG_YELLOW);
		}
		vector<spaceObject> newAsteroids;


		//Determines bullet movement and provides logic behind asteroid split when hit by bullet
		for (auto &temp : Bullets) {
			temp.x += temp.dx * fElapsedTime;
			temp.y += temp.dy * fElapsedTime;
			WrapCoordinates(temp.x, temp.y, temp.x, temp.y);
			Draw(temp.x, temp.y);
			for (auto& a : Asteroids) {
				if (isPointInsideCircle(a.x, a.y, a.size, temp.x, temp.y)) {
					temp.x = -100;
					if (a.size > 4) {
						float angle = ((float)rand() / (float)RAND_MAX) * 6.28318f;
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.28318f;
						newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle), 10.0f * cosf(angle), (int)(a.size /2) , 0.0f });
						newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), (int)(a.size / 2), 0.0f });
					}
					a.x = -100;
					score += 100;

				}
			}
		}

		//Creation of new asteroids 
		for (auto a : newAsteroids) {
			Asteroids.push_back(a);
		}
		//Detects if bullets are off of the game screen and removes them from vector if so
		if (Bullets.size() > 0) {
			auto i = remove_if(Bullets.begin(), Bullets.end(), [&](spaceObject o) {return (o.x < 1 || o.y < 1 || o.x > ScreenWidth() || o.y > ScreenHeight()); });
			if (i != Bullets.end()) {
				Bullets.erase(i);
			}
		}
		//Detects if Asteroids are off of the game screen and removes them from vector if so
		if (Asteroids.size() > 0) {
			auto i = remove_if(Asteroids.begin(), Asteroids.end(), [&](spaceObject o) {return (o.x < 1 ); });
			if (i != Asteroids.end()) {
				Asteroids.erase(i);
			}
		}
		//Creates next level and places asteroids away from the player
		if (Asteroids.empty()) {
			score += 1000;
			Asteroids.clear();
			Bullets.clear();
			Asteroids.push_back({ 30.0f * sinf(player.angle - 3.14159f / 2.0f) + player.x,
											  30.0f * cosf(player.angle - 3.14159f / 2.0f) + player.y,
											  10.0f * sinf(player.angle), 10.0f * cosf(player.angle), (int)16,  0.0f });

			Asteroids.push_back({ 30.0f * sinf(player.angle + 3.14159f / 2.0f) + player.x,
											  30.0f * cosf(player.angle + 3.14159f / 2.0f) + player.y,
											  10.0f * sinf(-player.angle), 10.0f * cosf(-player.angle), (int)16, 0.0f });
		}
		drawWireFrame(vectorModelShip, player.x, player.y, player.angle);

		DrawString(2, 2, L"Score: " + to_wstring(score));
		return true;
	}

	//Uses trig and angle properties and rotation matrices in order to draw/scale/rotate/translate objects
	void drawWireFrame(const vector<pair<float, float>>& vecModelCoordinates, float x, float y, float angle = 0.0f, float scale = 1.0f, short col = FG_WHITE) {
		vector<pair<float, float>> vecTransformedCoordinates;
		int size = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(size);
		for (int i = 0; i < size; i++) {
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(angle) - vecModelCoordinates[i].second * sinf(angle);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(angle) + vecModelCoordinates[i].second * cosf(angle);
		}

		for (int i = 0; i < size; i++) {
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * scale;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * scale;
		}
		for (int i = 0; i < size; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}
		for (int i = 0; i < size; i++) {
			int j = i + 1;
			DrawLine(vecTransformedCoordinates[i % size].first, vecTransformedCoordinates[i % size].second, vecTransformedCoordinates[j % size].first,
				vecTransformedCoordinates[j % size].second, PIXEL_SOLID, col);
		}
	}

	//Moves player and asteroids back on to screen if they ever leave the game screen
	void WrapCoordinates(float ix, float iy, float &ox, float &oy) {
		ox = ix;
		oy = iy;
		if (ix < 0.0f) {
			ox = ix + (float)ScreenWidth();
		}
		if (ix >= (float)ScreenWidth()) {
			ox = ix - float(ScreenWidth());
		}
		if (iy < 0.0f) {
			oy = iy + float(ScreenHeight());
		}
		if (iy >= (float)ScreenHeight()) {
			oy = iy - float(ScreenHeight());
		}
	}

	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		float fx;
		float fy;
		WrapCoordinates(x, y, fx, fy);
		olcConsoleGameEngine::Draw(fx, fy, c, col);
	}
};



int main() {
	OneLoneCoder_Asteroids game;
	
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();

	return 0;
}