#include <chrono>

#include <iostream>

#include <fstream>

#include <omp.h>

#include <string>

#include <vector>

#include <cassert>

using namespace std;

int width = -1, height = -1, max_val = -1;

string type = "";

vector<unsigned char> image;

int cntСhannels() {

	int countСhannels = 1;

	if (type == "P6") {

		countСhannels = 3;

	}

	return countСhannels;

}

int getSzImage() {

	return width * height * cntСhannels();

}

void read(string input_file) {

	ifstream reader(input_file, ios::binary);

	if (!reader.is_open()) {

		cerr << "Cannot open input_file";

		exit(1);

	}

	getline(reader, type);

	if (type != "P5" && type != "P6") {

		cerr << "Incorrect type " << type << endl;

		exit(1);

	}

	reader >> width >> height >> max_val;

	string skip;

	getline(reader, skip);

	image = vector<unsigned char>(getSzImage());

	for (int i = 0; i < getSzImage(); i++) {

		reader.read((char*)&image[i], sizeof(unsigned char));

	}

	reader.close();

}

void write(string output_file) {

	ofstream writer(output_file, ios::binary);

	if (!writer.is_open()) {

		cerr << "Cannot open output_file";

		exit(1);

	}

	writer << type << endl;

	writer << width << " " << height << endl;

	writer << 255 << endl;

	for (int i = 0; i < getSzImage(); i++) {

		writer << (char)(image[i]);

	}

	writer.close();

}

bool check() {

	bool ans = true;

	for (int i = 0; i < getSzImage(); i++) {

		if (i - cntСhannels() >= 0 && image[i] != image[i - cntСhannels()]) {

			ans = false;

			break;

		}

	}

	return ans;

}

int sort[256][3];

int main(int argc, char* argv[]) {

	// считывание входных данных

	if (argc < 5) {

		cerr << "Incorrect input";

		return 0;

	}

	string argv1 = argv[1];

	int threads = atoi(argv1.c_str());

	string input_name = argv[2];

	string output_name = argv[3];

	string argv4 = argv[4];

	double coefficient = atof(argv4.c_str());

	if (!(0 <= coefficient && coefficient <= 0.5)) {

		cerr << "Incorrect coefficient";

		return 0;

	}

	int cntThreads = 4; // значение по умолчанию

#ifdef _OPENMP

	if (threads != 0) {

		cntThreads = threads;

	}

	if (!(1 <= cntThreads && cntThreads <= 2048)) {

		cerr << "Incorrect cntThreads";

		return 0;

	}

	omp_set_num_threads(cntThreads);

#endif

	read(input_name);

	// проверка что изображение состоит из одного цвета

	if (check()) {

		float tim = 0.0;

#ifdef _OPENMP

		printf("Time (%i thread(s)): %g ms\n", cntThreads, tim);

#endif

		write(output_name);

		return 0;

	}

	chrono::time_point<chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	// сортировка подсчетом

#ifdef _OPENMP

	int sz = (getSzImage() + cntThreads - 1) / cntThreads;

	vector<vector<vector<int>>> cur(cntThreads, vector<vector<int>>(256, vector<int>(3, 0)));

#pragma omp parallel

	{

		int idThread = omp_get_thread_num();

		for (int i = idThread * sz; i < min((idThread + 1) * sz, getSzImage()); i++) {

			cur[idThread][image[i]][i % cntСhannels()]++;

		}

	}

	for (int i = 0; i < cntThreads; i++) {

#pragma omp parallel for schedule(static)

		for (int j = 0; j < 256; j++) {

			sort[j][0] += cur[i][j][0];

			sort[j][1] += cur[i][j][1];

			sort[j][2] += cur[i][j][2];

		}

	}

#else

	for (int i = 0; i < getSzImage(); i++) {

		sort[image[i]][i % cntСhannels()]++;

	}

#endif

	// проверка data race

	int sum = 0;

	for (int i = 0; i < 256; i++) {

		sum += sort[i][0] + sort[i][1] + sort[i][2];

	}

	if (sum != getSzImage()) {

		cout << "Data race error";

		exit(1);

	}

	// подсчет самых светлых и темных пикселей, которые нужно проигнорировать

	// нахождение min и max значений пикселей, не считая игнорированные

	int mn = 255, mx = 0;

	for (int i = 0; i < cntСhannels(); i++) {

		int ignoreL = 0;

		int cntL = 0;

		while (cntL < getSzImage() / cntСhannels() * coefficient) {

			cntL += sort[ignoreL++][i];

		}

		int ignoreR = 255;

		int cntR = 0;

		while (cntR < getSzImage() / cntСhannels() * coefficient) {

			cntR += sort[ignoreR--][i];

		}

		while (sort[ignoreL][i] == 0) {

			ignoreL++;

		}

		while (sort[ignoreR][i] == 0) {

			ignoreR--;

		}

		mn = min(mn, ignoreL);

		mx = max(mx, ignoreR);

	}

	// изменение исходной картинки

#pragma omp parallel for schedule(static, 100)

	for (int i = 0; i < getSzImage(); i++) {

		int x = (image[i] - mn) * (255.0 / (mx - mn));

		x = max(x, 0);

		x = min(x, 255);

		image[i] = x;

	}

	// вывод

	chrono::time_point<chrono::high_resolution_clock> finish = std::chrono::high_resolution_clock::now();

	float tim = ((float)chrono::duration_cast<chrono::microseconds>(finish - start).count()) / 1000;

#ifdef _OPENMP

	printf("Time (%i thread(s)): %g ms\n", cntThreads, tim);

#endif

	write(output_name);

	return 0;

}