// 212-Sushchev построение графа, волновой алгоритм, DBSCAN 

#include <iostream>     // библиотека вывода в консоль
#include <fstream>      // библиотека работы с файлами
#include <random>		// генератор сл. чисел
#include <vector>		// вектор
#include <algorithm>
#include <cmath>

class Interface {
	// 212-Sushchev - чтение файлов, введение параметров	
protected:

	std::ifstream V_file { "variables.txt" }; // читаем файл с переменными
	std::vector <float> V; // вектор из всех параметров задачи  
	float w = 0; //переменная пробегающая все значения файла

	float Dens = 0; //число точек в 1 конусе
	float K = 0; //число конусов

	std::vector <float> X_vec; // координаты центров и радиусы
	std::vector <float> Y_vec;
	std::vector <float> R_vec;

	float Eps;

public:
	Interface() {
		if (!V_file.is_open()) {
			std::cout << "Error, file is not found" << std::endl;
			return;
		}
		while (V_file >> w) {
			V.push_back(w); // заполняем файл данными из листа
		}
		Dens = V[0];
		K = V[1];
		for (w = 0; w < K; w++) {
			X_vec.push_back( V[2 + w] );
			Y_vec.push_back( V[2 + w + K] );
			R_vec.push_back( V[2 + w + 2*K] );
		}
		Eps = V[2 + 3 * K];
	}
};





class Tpair {
// 212-Sushchev-класс работы с точкой, как парой координат со свойствами 
private:
	float X = 0;
	float Y = 0;
	int cell_X;
	int cell_Y;
	int Cl = 0;

public: 
	Tpair() = default;
	Tpair(float x, float y) : X(x), Y(y) {}

	// сеттеры и геттеры
	  void setX(float x) { X = x; }
	  void setY(float y) { Y = y; }
	  void set_cell_X(float x) { cell_X = x; }
	  void set_cell_Y(float y) { cell_Y = y; }
	  void setCl(float c) { Cl = c; }

	  float getX() { return X; }
	  float getY() { return Y; }
	  int get_cell_X() { return cell_X; }
	  int get_cell_Y() { return cell_Y; }
	  int getCl() { return Cl; }
};



class Cone: public Interface {
// 212-Sushchev-класс работы с конусом 
private: 

	int Cone_Num; // порядковый номер конуса
	float coneX;
	float coneY;
	float coneR;
	int i = 0;

	std::vector <Tpair> inside;

public:
	Cone(int N) {
		Cone_Num = N; // присваеваем параметры в соответствии с номером
		coneX = X_vec[N];
		coneY = Y_vec[N];
		coneR = R_vec[N];

		for (i = 0; i < Dens; i++) {
			static std::random_device rd;
			static std::mt19937 seed(rd()); // создаем генератор с нормальным распределением
			std::normal_distribution<double> generated_X(coneX, coneR/3); 	
			std::normal_distribution<double> generated_Y(coneY, coneR/3); 	

			inside.push_back( Tpair( generated_X(seed), generated_Y(seed) )); // заполняем маконус точками
		}
	}
	// геттеры
	float getCX() { return coneX; }
	float getCY() { return coneY; }
	float getCR() { return coneR; }
	std::vector<Tpair> points() { return inside;  }
};




class Pole : public Interface {
	//212-Sushchev класс самой плоскости на которой находятся точки
protected:
	std::vector<Tpair> Points; // множество точек
	float netR = V[3 + 3 * K]; // размер сетки (радиус) 
	int netA = V[4 + 3 * K]; // длина радиуса в клетках
	int i = 0;
	Tpair A = Tpair(0,0);
public: 
	Pole() {
		for (i = 0; i < K; i++) { // перебираем точки из конусов, вставляем их в список всех точек
			for (Tpair A : Cone(i).points()) {

				A.set_cell_X(1 + int(A.getX() / netA));
				A.set_cell_Y(1 + int(A.getY() / netA));

				Points.push_back(A);

			}
		}
	}
	std::vector<Tpair>& getPoints() { return Points; }
	
};






class Distance : public Pole {
	// 212-Sushchev - класс работы с расстояниями между точками

public: Distance() {}
	  
	  float between(Tpair A, Tpair B) { // функция вычисления дистанции между 2 точками
		  return std::sqrt( (A.getX() - B.getX())* (A.getX() - B.getX()) + 
			  (A.getY() - B.getY()) * (A.getY() - B.getY()));
	  }

	  Tpair closest(Tpair A) {
		  float D = 100000; 
		  Tpair C = Tpair(0, 0);
		  for (Tpair B : Points) {
			  if ( (between(A, B) < D) && (between(A, B) > Eps) ){
				  D = between(A, B);
				  C = B;
			  }
		  }
		  return C;
	  }
};


class Graph : public Distance {
	// 212-Sushchev - графф и вункции работы с ним
protected:
	float Max_edge = V[5 + 3*K];
	int Edge_count = 0;
public: 

	int Edge_Exist(Tpair A, Tpair B) { // проверка существования ребра
		if (between(A, B) < Eps) { return 0; }
		if (between(A, B) < Max_edge) { return 1; }
		return { 0 };
	}

	int Neigh(Tpair A) { // счет соседей у точки
		int D = 0;
		for (Tpair B : Points) {
			D += Edge_Exist(A, B);
		}
		return D;
	}

	int Dust(Tpair A) { // проверка, является ли точка аномалией (у нее меньше 3 соседей)
		if (Neigh(A) < 3) { return 0; }
		return 1;
	}

	Graph() {
		Edge_count = 0;
		for (Tpair A : Points) {
			Edge_count += Neigh(A);
		}
		Edge_count = int(Edge_count / 2);
	}
};





class cluster {
	// 212-Sushchev-класс работы с кластером
private:

	std::vector<Tpair> points; // точки кластера
	int points_num = 1;
	int cl_Num = 0; // нромер кластера
	Tpair Initial; // точка, вокруг которой он наращивается

public:

	void add(Tpair& A) { // добавление точки в кластер

		A.setCl(cl_Num);
		points.push_back(A); 
		points_num += 1;
	} 


	std::vector<Tpair> get_points() { return points; } // получение точек кластера

	void set_cl_Num(int a) { cl_Num = a; }
	int get_cl_Num() { return cl_Num; }

	
	cluster(Tpair& A, int N, Graph& graph) : Initial(A), cl_Num(N) {
		// запуск алгоритма волны на 1 точке
		A.setCl(cl_Num);
		points.push_back(A);
		Tpair B;
		for (int i = 0; i < points_num; i++) {
			B = points[i];
			if (!(graph.Dust(B))) { continue; }
			for (Tpair& C : graph.getPoints()) {
				if ((graph.Edge_Exist(B, C)) && (C.getCl() == 0)) {
					add(C); // если у B и C есть ребро, добавляем С к кластеру
				}
			}
		}

	}
};






class Wave : public Graph {
	// 212-Sushchev волновой алгоритм
private:
	std::vector<cluster> Clusters; // вектор всех найденных кластеров
	int cluster_count = 0; // счетчик количества кластеров

public:
	Wave() {
		cluster_count = 0;

		for (Tpair& A : Points) {
			// если точка еще не относится к кластеру и не является шумом (аномалией)
			if ((A.getCl() == 0) && (Dust(A))) {
				cluster_count += 1;

				// запускаем волну от точки A, передавая текущий объект *this как Graph&
				cluster New_Cl(A, cluster_count+1, *this);
				Clusters.push_back(New_Cl);
			}
		}
	}

	// геттеры
	std::vector<cluster> get_Clusters() { return Clusters; }
	int get_cluster_count() { return cluster_count; }
};



class Plot : public Wave{
	// 212-Sushchev - класс вывода результатов в файл для GNU plot
private:
	std::ofstream output{ "clusters.txt" }; // открываем файл для записи

public:
	Plot() {
		if (!output.is_open()) {
			std::cout << "Error, cannot open output file" << std::endl;
			return;
		}

		// перебираем все точки из вектора Points по ссылке
		for (Tpair& A : getPoints()) {
			// выводим X, Y и номер кластера через пробел (у шума Cl будет равен 0)
			output << A.getX() << " " << A.getY() << " " << A.getCl() << "\n";
		}
	}
};



int main() {
	//212-Sushchev - итоговый мэйн
	Plot();
	return 0;
}