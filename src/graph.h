#pragma once
#include "SDL.h"
#include <thread>
#include <string>
#include <vector>
#include <mutex>
struct drawObject {
	float incrStep;
	int R;
	int G;
	int B;
	float (*calculate)(float);
	drawObject() = delete;
	drawObject(float (*func)(float), int R, int G, int B, float precision);
};

class graph {
private:
	std::thread* renderThread;
	float minY;
	float minX;
	float maxY;
	float maxX;
	std::vector<drawObject*>drawObjs;
public:
	graph(std::string n);
	~graph();
	void threadworker(bool& exit, std::string n);
	float* ConvertGraphPosToScreenPos(float x, float y, int winRes);
	float* ConvertScreenPosToGraphPos(float x, float y, int winRes);
	void DrawPointGraph(SDL_Renderer* ren, float x, float y, int w, int h, int padding);
	void AddDrawObj(float (*func)(float), int R, int G, int B, float precision);
	bool exited=false;
};
