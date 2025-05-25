/**
 * @file main.cpp
 * @brief задача написать программу, которая будет высчитывать значение интеграла через метод трапеций
 * Особенность этой задачи: использовать потоки(threads) для ускорения вычислений (нужно разделить задачу для вычисления части интеграла на каждый поток)
 *
 * на вход программе при запуске(это в argv[]) подается 3 числа: a, b, n, tn где [a, b] - интервал (целочисленный, неотрицательный и не больше 50 для задачи), n - количество разбиений, tn - threads number - количество потоков для подсчёта
 * примечание: n - количество разбиений в тестах будет явно нацело делиться на tn - количество потоков.
 *
 * на выход программе выведите значение интеграла через метод трапеций (вывод в стандартный поток std::cout, в конце выведите '\n'), вывод в поток с точностью до 4 знака
 * в функции trapezoidalIntegral есть переменная const std::function<double (double)> &f, при вызове подсчётов передайте ссылку на функцию из задания
 * реализовать подсчёт интеграла S(a, b) = (1+e^x)^0.5 dx
 * 
 *
 * литература:
 * https://ru.wikipedia.org/wiki/Метод_трапеций
 * https://habr.com/ru/articles/420867/
 */

#include <cmath>
#include <functional>
#include <numeric>
#include <vector>
#include <thread>
#include <iostream>
#include <algorithm>
#include <iomanip>


class Integral {
private:
    int a, b, n, tn;

public:
    Integral(int& argc, char** argv) {
        if (argc != 5) {
            throw std::invalid_argument("Wrong number of arguments");
        }
        a  = std::stoi(argv[1]);
        b  = std::stoi(argv[2]);
        n  = std::stoi(argv[3]);
        tn = std::stoi(argv[4]);
    }


    static double integralFunction(double x) {
        // тут нужно реализовать функцию интеграла S(a, b) = (1+e^x)^0.5 dx
        return std::sqrt(1.0 + std::exp(x));
    }

    // Метод трапеций на подынтервале [a_i, b_i] с n_i шагами
    static double trapezoidalWorker(double a_i, double b_i, int n_i) {
        double h = (b_i - a_i) / n_i;
        double sum = 0.5 * (integralFunction(a_i) + integralFunction(b_i));

        for (int i = 1; i < n_i; ++i) {
            sum += integralFunction(a_i + i * h);
        }

        return sum * h;
    }


    double calculateIntegral() {
        // в зависимости от количество потоков (tn) реализуйте подсчёт интеграла
        std::vector<double> results(tn);  
        std::vector<std::thread> threads;
        double h_total = (double)(b - a) / n;
        int n_per_thread = n / tn;

        for (int t = 0; t < tn; ++t) {
            double start_x = a + t * n_per_thread * h_total;
            double end_x = start_x + n_per_thread * h_total;
            threads.emplace_back([&, t, start_x, end_x]() {
                results[t] = trapezoidalWorker(start_x, end_x, n_per_thread);
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        return std::accumulate(results.begin(), results.end(), 0.0);
    }

};



int main(int argc, char** argv)
{
    auto i = Integral(argc, argv);
    std::cout << std::fixed << std::setprecision (4);
    std::cout << i.calculateIntegral() << std::endl;
    return 0;
}