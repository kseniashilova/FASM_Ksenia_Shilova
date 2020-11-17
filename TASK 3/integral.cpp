
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cmath>
#include <string>

double a;
double b;
double sum = 0;
double delta;
std::mutex g_lock;

const int threadNumber = 8;

void calculateIntegral(double (*func)(double), double lower, double higher, int numbThread) {
	g_lock.lock();
	for (double x = lower; x <= higher - delta / 2; x += delta) {
		double h = func(x + delta / 2);//высота прямоугольника
		//double h = (func(x) + func(x+delta))/2;//высота прямоугольника
		sum += h * delta;
	}
	std::cout << "thread " + std::to_string(numbThread) + " " + std::to_string(sum) + '\n';
	g_lock.unlock();
}

//ФУНКЦИИ
//константная функция
double f1(double x) {
	return 1;
}
//линейная
double f2(double x) {
	return x;
}
//степенная
double f3(double x) {
	return pow(x, 3);
}
//sin
double f4(double x) {
	return sin(x);
}
//показательная
double f5(double x) {
	return exp(x);
}
//корень
double f6(double x) {
	return sqrt(x);
}

void displayFunctions() {
	std::cout << "____________________________________________________________________" << std::endl;
	std::cout << "Type number from 1 to 6 to choose the function: " << std::endl;
	std::cout << "f1(x) = 1" << std::endl << "f2(x) = x" << std::endl << "f3(x) = pow(x,3)"
		<< std::endl << "f4(x) = sin(x)" << std::endl << "f5(x) = exp(x)" << std::endl
		<< "f6(x) = sqrt(x)" << std::endl;

	std::cout << "Type \"exit\" to exit the program." << std::endl;

	std::cout << "____________________________________________________________________" << std::endl;
}

//Метод считывает и проверяет на корректность интервал интегрирования
int readInterval() {
	std::cout << "Type integration interval [a, b] (real numbers): " << std::endl;
	std::cout << "a = ";
	std::cin >> a;
	std::cout << std::endl << "b = ";
	std::cin >> b;
	std::cout << std::endl;
	if (!std::cin) {
		std::cout << "Incorrect number. " << std::endl;
		return -1;
	}
	if (b < a) {
		std::cout <<
			"Incorrect numbers. The second number must be greater than or equal to the first." << std::endl;
		return -1;
	}

	delta = (b - a) / 1000.0;
	return 0;
}


int main() {

	//ВВОД ИНТЕРВАЛА ИНТЕГРИРОВАНИЯ И ЕГО ПРОВЕРКА НА КОРРЕКТНОСТЬ
	int read = readInterval();
	if (read == -1) return 0; //что-то пошло не так, завершение программы


	//ЦИКЛ ПОВТОРА ВЫБОРА ТЕСТИРУЕМОЙ ФУНКЦИИ (ИЗ 6-ТИ ПРЕДЛОЖЕННЫХ)
	do {
		sum = 0;
		bool again = false;

		displayFunctions(); //отображаем возможные для выбора функции
		
		//СЧИТЫВАНИЕ НОМЕРА ФУНКЦИИ ИЛИ ДРУГОЙ ИНФОРМАЦИИ
		std::string s;
		double (*function)(double) = nullptr;
		std::cin >> s;
		//если это номер, то берем нужную функцию
		if (s == "1") function = f1;
		else if (s == "2") function = f2;
		else if (s == "3") function = f3;
		else if (s == "4") function = f4;
		else if (s == "5") function = f5;
		else if (s == "6") { //нельзя, если границы отрицательные
			if (a < 0) {
				std::cout << "Function f6 is not defined in negative numbers. " << std::endl;
				again = true;
			}
			function = f6;
		}
		//если выход
		else if (s == "exit") break;
		//иначе - некорректная строка
		else {
			std::cout << "Incorrect number of function. Type number (1, ..., 6)  or \"exit\"" << std::endl;
			again = true;
		}
		//-----------------------------------------------------
		if (a == b) {
			//вывод результата
			std::cout << "Integral from " << a << " to " << b <<
				" f" + s + "(x)" << " dx " << " = " << 0 << std::endl << std::endl;
			again = true;
		}
		//если не нужно возвращаться к началу цикла
		if (!again) {
			std::thread* thr[threadNumber];
			//задаем область для подсчета интеграла одним потоком
			double oneInterval = (b - a) / threadNumber;
			for (int i = 0; i < threadNumber; i++) {
				//считаем интегралы по частям
				thr[i] = new std::thread{ calculateIntegral, function, a + i * (oneInterval), a + (i + 1) * (oneInterval), i };
			}
			//удаляем
			for (int i = 0; i < threadNumber; i++) {
				thr[i]->join();
				delete thr[i];
			}
			//вывод результата
			std::cout << "Integral from " << a << " to " << b <<
				" f" + s + "(x)" << " dx " << " = " << sum << std::endl << std::endl;
		}
	} while (true);

	return 0;
}