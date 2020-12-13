#include <iostream>
#include <fstream>
#include <thread>
#include <stack>
#include <mutex>
#include "condition_variable"
#include <string>
#include <ctime>
#include <random>


std::mutex shootMutex;
double projectilePrice; //стоимость снаряда для минометов
int startTime; 
std::ofstream fout;

//Класс государства
class Country {

private:
    double totalPrice; //цена всего имущества
    double damage; //нанесенный урон
    double hitProbability; //вероятность попадания в цель этой страны
    double minBound, maxBound; //границы, в которых может находиться стоимость нанесенного ущерба 
    double totalProjectilePrice; //сумма, потраченная на снаряды
    std::string name; //название государства

public:
    //конструктор
    Country(std::string _name, double _totalPrice, double _hitProbability) {
        name = _name;
        totalPrice = _totalPrice;
        hitProbability = _hitProbability;
        damage = 0;
        //Пусть противник может поразить за один раз от 10% до 20%
        minBound = 0.1 * totalPrice;
        maxBound = 0.2 * totalPrice;
        totalProjectilePrice = 0;
    }

    //Метод стрельбы по противнику
    void shoot(Country* rival) {
        std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<> distribution1(0, 1), distribution2(rival->getMinBound(), rival->getMaxBound());

        //Выполняем стрельбу, пока либо не уничтожены все цели,
        //либо стоимость потраченных снарядов не превысит стоимость всего имущества противника
        while (rival->getCurrentPrice() > 0 &&
            totalProjectilePrice <= rival->getTotalPrice()) {
           
            //Критическая секция
            shootMutex.lock();
            //С вероятностью hitProbability государство попадает в какую-либо цель
            if (distribution1(gen) <= hitProbability) { //попадает
                //Генерация пораженной стоимости
                double singleDamage = distribution2(gen);
                if (singleDamage > rival->getCurrentPrice())
                    singleDamage = rival->getCurrentPrice();
                //Прибавление к урону противника
                rival->causeDamage(singleDamage);
                //Вывод информации
                fout<< name << " caused damage to the rival in the amount of " << singleDamage << std::endl;
                fout << "\t Now total price of rival is " << rival->getCurrentPrice() << std::endl;
            }
            else { //не попадает
                //Вывод информации
                fout << name << " missed" << std::endl;
            }
            shootMutex.unlock();

            //Увеличение суммарной стоимости потраченных снарядов в любом случае
            totalProjectilePrice += projectilePrice;
            //Солдатам надо отдохнуть
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        //Критическая секция
        shootMutex.lock();
        //Вывод информации о завершении цикла
        if (rival->getCurrentPrice() <= 0)
            fout << "ATTENTION!!!  " << name << " destroyed all rival targets" << std::endl;
        else
            fout << "ATTENTION!!!  " << name << " has spent too much projectiles: "<< totalProjectilePrice
            <<" (more than total price of the rival)" << std::endl;
        shootMutex.unlock();
    }

    
    //Метод нанесения ущерба (прибавление к общему урону)
    void causeDamage(double singleDamage) {
        damage += singleDamage;
    }

    //Метод возвращает текущую стоимость не уничтоженного имущества
    double getCurrentPrice() {
        return totalPrice - damage;
    }

    //Метод возвращает общую стоимость имущества государства
    double getTotalPrice() {
        return totalPrice;
    }

    double getMinBound() {
        return minBound;
    }

    double getMaxBound() {
        return maxBound;
    }
};


//argv: argv[1] вероятность попадания Анчурии, argv[2] стоимость имущества Анчурии, 
// argv[3] вероятность попадания Тарантерии, argv[4] стоимость имущества Тарантерии 
//argv[5] выходной файл
void main(int argc, char* argv[]) {

    
    startTime = clock();
    //Аргументы командной строки
    double hitProb1, totalPr1, hitProb2, totalPr2;
    //Проверка входных аргументов
    if (argc != 6) {
        std::cout << "Incorrect number of arguments: " << argc << std::endl;
        std::cout <<
            "There must be 5 arguments: hit probability and total price for Anchuria; hit probability and total price for Taranteria; name of output file"
            << std::endl;
        return;
    }
    try
    {
        hitProb1 = std::stod(argv[1]);
        totalPr1 = std::stod(argv[2]);
        hitProb2 = std::stod(argv[3]);
        totalPr2 = std::stod(argv[4]);
    }
    catch (std::exception& e) //если не получается преобразовать число в real number (double)
    {
        std::cout << "Incorrect format of arguments. It must be real number" << std::endl;
        return; //закрываемся
    }

    if (hitProb1 < 0 || hitProb1 > 1 || hitProb2 < 0 || hitProb2 > 1) {
        std::cout << "Incorrect format of probability argument. Probability must be real number between 0 and 1" << std::endl;
        return;
    }

    if (totalPr1 < 0 || totalPr2 < 0) {
        std::cout << "Incorrect format of total price argument. Total price of country must be a real number greater than 0" << std::endl;
        return;
    }


    //Генерируем стоимость одного снаряда
    projectilePrice = std::max(totalPr1, totalPr2)/30.0;

    Country* Anchuaria = new Country("Anchuaria", totalPr1, hitProb1);
    Country* Taranteria = new Country("Taranteria", totalPr2, hitProb2);

    fout.open(argv[5]); //открываем поток для записи
    
    std::thread AnchuariaThread(&Country::shoot, Anchuaria, Taranteria);
    std::thread TaranteriaThread(&Country::shoot, Taranteria, Anchuaria);

    AnchuariaThread.join();
    TaranteriaThread.join();
    std::cout << std::endl << "The war is over!" << std::endl;
    fout.close();
    delete Anchuaria;
    delete Taranteria;
    return;
}


