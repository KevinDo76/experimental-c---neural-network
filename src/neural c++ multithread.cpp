#include <iostream>
#include <random>
#include <cmath>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include "graph.h"
//matrix datatype
//help with handing a array of numbers and matrix mul

float randomGen(float min, float max) {
	//https://stackoverflow.com/questions/19665818/generate-random-numbers-using-c11-random-library thx u <3
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(min, max);
	float num = dist(mt);
	return num;
}

template<typename type>
struct matrix {
	//properties
	int x;
	int y;
	type** list;

	matrix() : x(0), y(0) {
		list = new type * [x];
	}

	~matrix() {
		for (int x_ = 0; x_ < x; x_++) {
			delete[] list[x_];
		}
		delete[] list;
	}

	matrix(const matrix& oldmatrix) {
		x = oldmatrix.x;
		y = oldmatrix.y;
		list = new type * [x];
		for (int x_ = 0; x_ < x; x_++) {
			list[x_] = new type[y]{ 0 };
		}

		for (int y1 = 0; y1 < y; y1++) {
			for (int x1 = 0; x1 < x; x1++) {
				list[x1][y1] = oldmatrix.list[x1][y1];
			}
		}
	}

	void copy(const matrix& oldmatrix) {
		x = oldmatrix.x;
		y = oldmatrix.y;
		for (int x_ = 0; x_ < x; x_++) {
			delete[] list[x_];
		}
		delete[] list;
		list = new type * [x];
		for (int x_ = 0; x_ < x; x_++) {
			list[x_] = new type[y]{ 0 };
		}

		for (int y1 = 0; y1 < y; y1++) {
			for (int x1 = 0; x1 < x; x1++) {
				list[x1][y1] = oldmatrix.list[x1][y1];
			}
		}
	}

	matrix(int xx, int yy) : x(xx), y(yy) {
		list = new type * [x];
		for (int x_ = 0; x_ < x; x_++) {
			list[x_] = new type[y]{ 0 };
		}
	}

	void insertRow(int yy, type* toLoad) {
		for (int x_ = 0; x_ < x; x_++) {
			list[x_][yy] = toLoad[x_];
		}
	}

	void mutate(float chance, float min, float max) {
		for (int y1 = 0; y1 < y; y1++) {
			for (int x1 = 0; x1 < x; x1++) {
				if (randomGen(0, 100) <= chance) {
					list[x1][y1] += randomGen(min, max);
				}
			}
		}
	}

	void selfAdd(matrix<type>* m2) {
		if (x == m2->x && y == m2->y) {
			for (int y1 = 0; y1 < y; y1++) {
				for (int x1 = 0; x1 < x; x1++) {
					list[x1][y1] += m2->list[x1][y1];
				}
			}
		}
	}

	void runOperation(type(*func)(type)) {
		for (int y1 = 0; y1 < y; y1++) {
			for (int x1 = 0; x1 < x; x1++) {
				list[x1][y1] = func(list[x1][y1]);
			}
		}
	}

	matrix<type>* matrixMul(matrix<type>* m2) {
		matrix<type>* returnM = new matrix<type>(m2->x, y);
		for (int y1 = 0; y1 < y; y1++) {
			for (int x1 = 0; x1 < m2->x; x1++) {
				type total = 0;
				for (int x2 = 0; x2 < x; x2++) {
					total += list[x2][y1] * m2->list[x1][x2];
				}
				returnM->list[x1][y1] = total;
			}
		}
		return returnM;
	}

	void reset(int x_, int y_) {
		for (int i = 0; i < y; i++) {
			delete[] list[i];
		}
		delete[] list;
		x = x_;
		y = y_;
		list = new type * [x];
		for (int i = 0; i < x; i++) {
			list[i] = new type[y]{ 0 };
		}
	}

	void populateRandom(type min, type max) {
		for (int y1 = 0; y1 < y; y1++) {
			for (int x1 = 0; x1 < x; x1++) {
				this->list[x1][y1] = (type)randomGen(min, max);
			}
		}
	}

	void display() {
		for (int y1 = 0; y1 < y; y1++) {
			for (int x1 = 0; x1 < x; x1++) {
				std::cout << list[x1][y1];
				std::cout << ' ';
			}
			std::cout << '\n';
		}
		std::cout << '\n';
	}
};

//main neural class implementation
class NeuralNet {
public:
	static float activateFunc(float x) {
		//return 3 / (1 + std::powf(5*2.71828f, -x)) - 1.5f;//sigmoid 1/(1+e^-x)
		
		//return std::max<float>(0.f,x);
		//return x;
		//return tanhf(x);
		if (x > 0) {
			return x;
		}
		else {
			return x * 0.05f;
		}
	}
	//public properties
	int layerCount;
	int inputCount;
	int outputCount;
	int* layerConfig;
	matrix<float>* bias;
	matrix<float>* weight;
	//public methods
	NeuralNet() = delete;
	NeuralNet(const NeuralNet& oldNeu) {
		layerCount = oldNeu.layerCount;
		inputCount = oldNeu.inputCount;
		outputCount = oldNeu.outputCount;
		//copying  the config
		layerConfig = new int[layerCount];
		for (int i = 0; i < layerCount; i++) {
			layerConfig[i] = oldNeu.layerConfig[i];
		}
		//copying the weight
		weight = new matrix<float>[layerCount + 1];
		weight[0].reset(inputCount, layerConfig[0]);
		weight[0].copy(oldNeu.weight[0]);
		for (int i = 0; i < layerCount - 1; i++) {
			weight[i + 1].reset(layerConfig[i], layerConfig[i + 1]);
			weight[i + 1].copy(oldNeu.weight[i + 1]);
		}
		weight[layerCount].reset(layerConfig[layerCount - 1], outputCount);
		weight[layerCount].copy(oldNeu.weight[layerCount]);
		//copying the bias
		bias = new matrix<float>[layerCount + 1];
		for (int i = 0; i < layerCount; i++) {
			bias[i].reset(1, layerConfig[i]);
			bias[i].copy(oldNeu.bias[i]);
		}
		bias[layerCount].reset(1, outputCount);
		bias[layerCount].copy(oldNeu.bias[layerCount]);
	}

	NeuralNet(int layerCount_, int inputCount_, int outputCount_, int* layerConfig_) : layerCount(layerCount_), inputCount(inputCount_), outputCount(outputCount_) {
		//copying over the config
		layerConfig = new int[layerCount];
		for (int i = 0; i < layerCount; i++) {
			layerConfig[i] = layerConfig_[i];
		}
		//creating the weight matrix
		weight = new matrix<float>[layerCount + 1];
		weight[0].reset(inputCount, layerConfig[0]);
		//weight[0].populateRandom(-2, 2);
		for (int i = 0; i < layerCount - 1; i++) {
			weight[i + 1].reset(layerConfig[i], layerConfig[i + 1]);
			//weight[i + 1].populateRandom(-2, 2);
		}
		weight[layerCount].reset(layerConfig[layerCount - 1], outputCount);
		//weight[layerCount].populateRandom(-2, 2);
		//creating the bias matrix
		bias = new matrix<float>[layerCount + 1];
		for (int i = 0; i < layerCount; i++) {
			bias[i].reset(1, layerConfig[i]);
			//bias[i].populateRandom(-5, 5);
		}
		bias[layerCount].reset(1, outputCount);
		//bias[layerCount].populateRandom(-5, 5);
	}

	void mutate(float chance, float min, float max) {
		for (int i = 0; i < layerCount + 1; i++) {
			weight[i].mutate(chance, min, max);
		}

		for (int i = 0; i < layerCount + 1; i++) {
			bias[i].mutate(chance, min, max);
		}
	}

	matrix<float>* compute(matrix<float>& inp) {
		matrix<float>* working = new matrix<float>(inp);
		matrix<float>* tobeDelete;
		tobeDelete = working;
		working = weight[0].matrixMul(tobeDelete);
		working->selfAdd(&bias[0]);
		working->runOperation(&activateFunc);
		//working->display();
		delete tobeDelete;
		for (int i = 0; i < layerCount - 1; i++) {
			tobeDelete = working;
			working = weight[i + 1].matrixMul(tobeDelete);
			//weight[i + 1].display();
			//working->display();
			working->selfAdd(&bias[i + 1]);
			working->runOperation(&activateFunc);
			//working->display();
			delete tobeDelete;
		}
		tobeDelete = working;
		working = weight[layerCount].matrixMul(tobeDelete);
		working->selfAdd(&bias[layerCount]);
		working->runOperation(&activateFunc);
		delete tobeDelete;
		matrix<float>* final = new matrix<float>(*working);
		delete working;
		working = nullptr;
		tobeDelete = nullptr;
		return final;
	}

	~NeuralNet() {
		delete[] layerConfig;
		delete[] bias;
		delete[] weight;
	}
};
std::mutex bestNLock;
const float mutationChance = 1;
const float highestChange = 1.15f;
const float lowerChange = -1.15f;
const int batchSize = 32;
const int ThreadCount = 32;
const int layerC = 8;
const int inCount = 1;
const int outCount = 1;
graph graphObj = graph("graph");
int taskS = (int)(batchSize / ThreadCount);
int* layerConfig = new int[8] { 10, 10, 10, 10, 10, 15, 20, 25};
NeuralNet* BestNeural = new NeuralNet(layerC, inCount, outCount, layerConfig);


float parentFunc(float x) {
	return std::sinf(x);
}

void worker2(NeuralNet* NeuL[], NeuralNet* BestNeu, int taskSize, int id) {
	int startId = taskSize * id;
	int endId = std::min(taskSize * (id + 1) - 1, batchSize - 1);
	for (int i = startId; i < endId+1; i++) {
		delete NeuL[i];
		NeuL[i] = new NeuralNet(*BestNeu);
		if (i > 0) {
			NeuL[i]->mutate(mutationChance, lowerChange, highestChange);
		}
		else {
			NeuL[i]->mutate(mutationChance*0.1, lowerChange*0.0015, highestChange*0.0015);
		}
	}
}

float BestNeuResult(float x) {
	bestNLock.lock();
	matrix<float> inp = matrix<float>(1, 1);
	inp.list[0][0] = x;
	matrix<float>* result = BestNeural->compute(inp);
	float toreturn = result->list[0][0];
	delete result;
	bestNLock.unlock();
	return toreturn;
}

//NeuralNet* Neu, matrix<float>* inp, int index, matrix<float>* outList[]
void worker(NeuralNet* NeuL[], float* resultL, int taskSize, int id) {
	int startId = taskSize * id;
	int endId = std::min(taskSize * (id + 1) - 1,batchSize-1);
	for (int i = startId; i < endId + 1; i++) {
		float errorN = 0;
		for (float x = -45; x < 45; x+=1.f) {
			matrix<float>* inp = new matrix<float>(1, inCount);
			matrix<float>* result;
			inp->list[0][0] = x;
			result = NeuL[i]->compute(*inp);
			//std::cout << result->list[0][0] << " " << parentFunc(x) << "\n";;
			errorN += std::powf(abs(result->list[0][0] - parentFunc(x)),2);
			delete result;
			delete inp;
		}
		resultL[i] = std::powl(errorN,1.1);
	}
}

int main(int argc, char* argv[]) {
	BestNeural->mutate(100, lowerChange, highestChange);
	graphObj.AddDrawObj(&parentFunc, 0, 255, 0, 1);
	graphObj.AddDrawObj(&BestNeuResult, 255, 0, 0, 1);
	float* resultL = new float[batchSize];
	float lowestError = (float)0xfffffff;
	std::thread* threadL[ThreadCount];
	NeuralNet* Neural[batchSize];
	for (int i = 0; i < batchSize; i++) {
		Neural[i] = new NeuralNet(layerC, inCount, outCount, layerConfig);
		Neural[i]->mutate(100, lowerChange, highestChange);
	}

	

	while (1) {
		for (int i = 0; i < ThreadCount; i++) {
			threadL[i] = new std::thread(worker, Neural, resultL, taskS, i);
		}

		for (int i = 0; i < ThreadCount; i++) {
			threadL[i]->join();
		}
		delete threadL[0];
		threadL[0] = new std::thread(worker, Neural, resultL, taskS, ThreadCount);
		threadL[0]->join();
		for (int i = 0; i < batchSize; i++) {
			if (lowestError > resultL[i]) {
				lowestError = resultL[i];
				bestNLock.lock();
				delete BestNeural;
				BestNeural = new NeuralNet(*Neural[i]);
				bestNLock.unlock();
			}
		}
		for (int i = 0; i < ThreadCount; i++) {
			delete threadL[i];
			threadL[i] = new std::thread(worker2, Neural, BestNeural, taskS, i);
		}

		for (int i = 0; i < ThreadCount; i++) {
			threadL[i]->join();
		}
		delete threadL[0];
		threadL[0] = new std::thread(worker2, Neural, BestNeural, taskS, ThreadCount);
		threadL[0]->join();

		for (int i = 0; i < ThreadCount; i++) {
			delete threadL[i];
		}

		std::cout << "Lowest error: " << lowestError << "\n";
		if (lowestError < 0.005) {
			break;
		}
		lowestError = (float)0xfffffff;;
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (graphObj.exited) {
			break;
		}
	}

	for (float x = 0; x < 80; x+=1.f) {
		matrix<float> inp = matrix<float>(1, 1);
		inp.list[0][0] = x;
		matrix<float>* result = BestNeural->compute(inp);
		float control = parentFunc(x);
		std::cout << control << " " << result->list[0][0]<< "  " << abs(control-result->list[0][0]) << "\n";
		delete result;
	}

	for (int i = 0; i < BestNeural->layerCount + 1; i++) {
		std::cout << "Weight " << i << "\n";
		BestNeural->weight[i].display();
		std::cout << "Bias " << i << "\n";
		BestNeural->bias[i].display();
	}


	while (1) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
	return 0;
}
