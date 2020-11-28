#include <cmath>
#include <functional>
#include <iostream>
#include <omp.h>
#include <ctime>

using namespace std;

int amountOfThreads = 8;
double a, b; //границы интегрирования

double eps = 0.001;
double sum = 0;
double rectIntegral(
	const function<double(double)>& fun,
	const double a, const double b)
{
	if (b - a < eps)
	{
		return fun((a + b) / 2) * (b - a);
	}

	double sum1, sum2;
/*#pragma omp parallel
	{
#pragma sections
		{
		#pragma section
		sum1 = rectIntegral(fun, a, (a + b) / 2);
		#pragma section
		sum2 = rectIntegral(fun, (a + b) / 2, b);
		}
	}
	return sum1 + sum2;*/
	return rectIntegral(fun, a, (a + b) / 2) + rectIntegral(fun, (a + b) / 2, b);

	/*
		//#pragma omp task shared(sum1)
		sum1 = rectIntegral(fun, a, (a + b) / 2);
		//#pragma omp task shared(sum2)
		sum2 = rectIntegral(fun, (a + b) / 2, b);
		//#pragma omp taskwait
		result = sum1 + sum2;
	return sum1 + sum2;
	*/
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
			double integral = rectIntegral(function, a, b);

			//вывод результата
			std::cout << "Integral from " << a << " to " << b <<
				" f" + s + "(x)" << " dx " << " = " << integral << std::endl << std::endl;

			cout << "Time: " << clock() << endl;
		}
	} while (true);

	return 0;
}
