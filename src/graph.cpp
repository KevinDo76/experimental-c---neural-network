#include "graph.h"
#include <thread>
#include <chrono>
#include "SDL.h"
#include <iostream>
#include <string>
#include <cmath>
#include <mutex>
template<typename t>
t clamp(t n,t min,t max) {
	if (n > max) {
		return max;
	}
	else if (n < min) {
		return min;
	}
	else {
		return n;
	}
}


void graph::threadworker(bool& exit, std::string n) {
	bool running = true;
	//std::cout << "helo" << "\n";
	SDL_Window* win = SDL_CreateWindow(const_cast<char*>(n.c_str()), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,600, 600, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);
	int padding = 10;
	int yoff = padding/2; //topYBound
	while (running && !exit) {
		//rendering
		std::this_thread::sleep_for(std::chrono::milliseconds(6));
		if (!(SDL_GetWindowFlags(win) & SDL_WINDOW_MINIMIZED)) {
			int w;
			int h;
			SDL_GL_GetDrawableSize(win, &w, &h);
			int hw = int(w / 2.f);
			int boxRes = h - padding;
			int hh = int(boxRes/ 2.f);
			//same math as ConvertGraphPosToScreenPos
			float rangeX = this->maxX - this->minX;
			float rangeY = this->maxY - this->minY;
			float XPercentage = -this->minX / rangeX;
			float YPercentage = -this->minY / rangeY;
			int halfBoxRes = boxRes / 2;
			//var
			int leftXbound = hw - halfBoxRes;
			int rightXbound = hw + halfBoxRes;
			int bottomYBound = yoff + boxRes;

			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			SDL_RenderClear(ren);
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
			SDL_RenderDrawLine(ren, clamp<int>((leftXbound)+int(XPercentage * boxRes), leftXbound, rightXbound), yoff, clamp<int>((leftXbound)+int(XPercentage * boxRes), leftXbound, rightXbound), bottomYBound);
			SDL_RenderDrawLine(ren, leftXbound, clamp<int>(h - (yoff + int(YPercentage * boxRes)), yoff, bottomYBound), hw + (boxRes / 2), clamp<int>(h - (yoff + int(YPercentage * boxRes)), yoff, bottomYBound));
			for (int i = 0; i < drawObjs.size(); i++) {
				SDL_SetRenderDrawColor(ren, drawObjs[i]->R, drawObjs[i]->G, drawObjs[i]->B, 255);
				for (float x = 0; x < boxRes; x += 0.01) {
					float* result = ConvertScreenPosToGraphPos(x, 0, boxRes);
					DrawPointGraph(ren, result[0], drawObjs[i]->calculate(result[0]), w, h, padding);
					delete[] result;
				}
			}
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

			//box render
			//ren
			SDL_RenderDrawLine(ren, rightXbound, yoff, rightXbound, bottomYBound);
			SDL_RenderDrawLine(ren, leftXbound, yoff, rightXbound, yoff);
			SDL_RenderDrawLine(ren, leftXbound, bottomYBound, hw + halfBoxRes, bottomYBound);
			SDL_RenderDrawLine(ren, leftXbound, yoff, leftXbound, bottomYBound);
			//axis render
			//outbound block
			//SDL_Fill
			SDL_RenderPresent(ren);
		}

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_WINDOWEVENT_RESIZED) {
				std::cout << "resized\n";
			}
		}
	}
	exit = true;
}

void graph::DrawPointGraph(SDL_Renderer* ren, float x, float y, int w, int h, int padding) {
	int boxRes = h - padding;
	float* resultPos = this->ConvertGraphPosToScreenPos(x, y, boxRes);
	SDL_RenderDrawPoint(ren, (int)resultPos[0] + ((w/2.f) - (h / 2)) + (padding / 2), h - (padding / 2) - (int)resultPos[1]);
	delete[] resultPos;
}

float* graph::ConvertScreenPosToGraphPos(float x, float y, int winRes) {
	float rangeX = this->maxX - this->minX;
	float rangeY = this->maxY - this->minY;
	float XPercentage = (x/winRes)*rangeX+this->minX;
	float YPercentage = (y/winRes)*rangeY+this->minY;
	float* finalPosition = new float[2] {0};
	finalPosition[0] = XPercentage;
	finalPosition[1] = YPercentage;
	return finalPosition;
}

float* graph::ConvertGraphPosToScreenPos(float x, float y, int winRes) {
	float rangeX = this->maxX - this->minX;
	float rangeY = this->maxY - this->minY;
	float XPercentage = (x - this->minX) / rangeX;
	float YPercentage = (y - this->minY) / rangeY;
	float* finalPosition = new float[2]{0};
	finalPosition[0] = XPercentage*winRes;
	finalPosition[1] = YPercentage * winRes;
	return finalPosition;
}

graph::graph(std::string n) {
	//graph::threadworker(graph::window, graph::renderer);
	graph::maxX = 60;
	graph::minX = -60;
	graph::maxY = 10;
	graph::minY = -10;
	std::cout << n << "\n";
	graph::ConvertGraphPosToScreenPos(0, 0, 10);
	graph::exited = false;
	graph::renderThread = new std::thread(&graph::threadworker, this,std::ref(graph::exited),n);
	graph::renderThread->detach();
}


void graph::AddDrawObj(float(*func)(float), int R, int G, int B, float precision) {
	drawObject* newObj = new drawObject(func, R, G, B, precision);
	drawObjs.push_back(newObj);
}

graph::~graph() {
	graph::exited = true;
	drawObjs.clear();
	delete graph::renderThread;
}

drawObject::drawObject(float (*func)(float), int R_, int G_, int B_, float precision) : R(R_), G(G_), B(B_), calculate(func), incrStep(precision) {}