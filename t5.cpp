#include <iostream>     // библиотека вывода в консоль
#include <fstream>      // библиотека работы с файлами
#include <vector>
#include <algorithm>

class Tpair {
private:
	float x;
	float y; // координаты
	float d = 100000; // удаленность от центра графа
public:
	Tpair(float a, float b) : x(a), y(b) {}

	// геттеры и сеттеры

	float getX() const { return x; }
	float getY() const { return y; }
	float getD() const { return d; }

	void setX(float newX) { x = newX; }
	void setY(float newY) { y = newY; }
	void setD(float newD) { d = newD; }
};

Tpair ad(Tpair a, Tpair b, float i) {
	// 212-Sushchev - new mean of the set of pairs
	float x = i * a.getX() + b.getX();
	float y = i * a.getY() + b.getY();
	return Tpair(x/(i+1), y/(i+1));
}

float Sdis(Tpair a, Tpair b) {
	// 212-Sushchev - returns squared distans beetwen 2 pairs

	float x = a.getX() - b.getX();
	float y = a.getY() - b.getY();
	return x * x + y * y;
}

int main() {
	// 212-Sushchev-classes of points
	std::ifstream file("cone-points.txt"); // читаем файл с точками
	std::ofstream output("clusters.txt");


	std::vector<std::vector<Tpair>> Classeters;
	std::vector <Tpair> points;
	std::vector <Tpair> gr;
	std::vector <float> ribs;

	float x, y;
	int N = 0, M = 0;


	if ((!file.is_open()) || (!output.is_open())) {
		std::cout << "Error, file is not found" << std::endl;
		return -1;
	}


	while (file >> x >> y) {
		N++;
		points.push_back(Tpair(x, y));
	}
	file.close();

	int j, i = 1, w = 1;
	float D = 0, d = 0;

	Tpair Ai = points[0];
	Tpair Bj = points[0];
	gr.push_back(Ai);
	ribs.push_back(0.0);
	
	for (w; w < N; w++) {
		Tpair Closest = points[N-1];
		i = 1;
		for (i; i<N; i++) {
			Ai = points[i];

			j = 0;
			D = 1000000;
			for (j; j<w; j++) {
				Bj = gr[j];
				d = Sdis(Ai, Bj); // считаем удаленность от точки графа
				if (d < D) { D = d; }// считаем удаленность от графа
			}
			points[i].setD(D);
			if ((D < Closest.getD()) && D > 0.001) { Closest = points[i]; }
		}
		gr.push_back(Closest);
		ribs.push_back(Closest.getD());
	}
	// так мы получили граф, с наиболее короткими ребрами. Далее удаляяем M самых длинных ребер.
	std::cout << "Enter wanted number of clasters: ";
	std::cin >> M;
	if (M < 2) {
		output.close();
		std::cout << "Wrong value, it should be greater then 1";
		return -1;
	}
	std::sort(ribs.begin(), ribs.end(), std::greater<float>());

	D = ribs[M - 2];
	j = 0;
	i = 0;

	output << "Clusters:\n";

	for (i; i < M; i++) {
		output << i+1 << "\n";
		std::vector<Tpair> cl;

		if (j == N) {break;}
		
		cl.push_back(gr[j]);
		output << "(" << gr[j].getX() << ";  " << gr[j].getY() << ")" << "\n";
		j++;
		while ((j < N) && (gr[j].getD() < D)) {
			cl.push_back(gr[j]);			
			output << "(" << gr[j].getX() << ";  " << gr[j].getY() << ")" << "\n";
			j++;
		}
		Classeters.push_back(cl);
		output << "\n";
	}
	output.close();

	return 0;
}