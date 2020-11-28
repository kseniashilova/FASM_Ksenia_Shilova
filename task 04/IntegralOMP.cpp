
#include <cmath>
#include <functional>
#include <iostream>
#include <omp.h>
#include <ctime>

using namespace std;

int amountOfThreads = 8;
double a, b; //границы интегрирования


double rectIntegral(
	const function<double(double)>& fun,
	const double a, const double b, const int n)
{
	//разбиваем на 8 отрезков, подобны методу дихотомии
	double oneInterval = (b - a) / amountOfThreads;
	double sum = 0;
	
#pragma omp parallel num_threads(8) reduction (+: sum)
	{
		//проходимся по всем потокам
		for (int counter = 0; counter < amountOfThreads; counter++) {

			double delta = oneInterval / n;
#pragma omp for nowait
			//внутри одной из восьми частей производим сложение площадей прямоугольников
			for (int i = 0; i < n; i++) {
				//counter = 0 =>   fun(a)...fun(a + oneInterval)
				//counter = 1 =>   fun(a + oneInterval)...fun(a + 2*oneInterval)
				//counter = 7 =>   fun(a + 7*oneInterval)...fun(b)
				sum += fun(a + counter*oneInterval + i*delta) * delta;
			}
		}
#pragma omp critical
		{
			std::cout << "Thread number " << omp_get_thread_num() << ": integral = " << sum << endl;
		}
	}  // pragma omp parallel
	return sum;
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

	return 0;
}


int main() {

	//ВВОД ИНТЕРВАЛА ИНТЕГРИРОВАНИЯ И ЕГО ПРОВЕРКА НА КОРРЕКТНОСТЬ
	int read = readInterval();
	if (read == -1) return 0; //что-то пошло не так, завершение программы


	//ЦИКЛ ПОВТОРА ВЫБОРА ТЕСТИРУЕМОЙ ФУНКЦИИ (ИЗ 6-ТИ ПРЕДЛОЖЕННЫХ)
	do {
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
		if (!again && a == b) {
			//вывод результата
			std::cout << "Integral from " << a << " to " << b <<
				" f" + s + "(x)" << " dx " << " = " << 0 << std::endl << std::endl;
			again = true;
		}
		//если не нужно возвращаться к началу цикла
		if (!again) {
			double integral = rectIntegral(function, a, b, 10000);
		
			//вывод результата
			std::cout << "Integral from " << a << " to " << b <<
				" f" + s + "(x)" << " dx " << " = " << integral << std::endl << std::endl;

			cout << "Time: " << clock() << endl;
		}
	} while (true);

	return 0;
}
