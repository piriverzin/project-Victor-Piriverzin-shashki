#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>

// Константы для работы с вводом
constexpr std::streamsize kMaxStreamSize = std::numeric_limits<std::streamsize>::max();

// Константы для задачи суммы
constexpr int kMinNaturalNumber = 1;
constexpr int kDivisor = 5;
constexpr int kStartValue = 5;

// Константы для задачи Тейлора
constexpr double kStartX = 0.0;
constexpr double kEndX = 1.0;
constexpr double kStepX = 0.2;
constexpr double kEpsilon = 1e-10;
constexpr int kMaxIterations = 100000;

// Константы для задачи корней
constexpr int kPartialTerms[] = {3, 5, 10};
constexpr int kPartialCount = 3;

void ClearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(kMaxStreamSize, '\n');
}

int ReadInt() {
    int value{0};
    while (true) {
        std::cin >> value;

        if (std::cin.fail()) {
            std::cout << "Ошибка. Введите целое число." << std::endl;
            ClearInputBuffer();
        } else {
            ClearInputBuffer();
            break;
        }
    }
    return value;
}

double ReadDouble() {
    double value{0.0};
    while (true) {
        std::cin >> value;

        if (std::cin.fail()) {
            std::cout << "Ошибка. Введите требуемый тип данных." << std::endl;
            ClearInputBuffer();
        } else {
            ClearInputBuffer();
            break;
        }
    }
    return value;
}

int ReadNaturalNumber() {
    int value{0};
    while (true) {
        value = ReadInt();
        if (value < kMinNaturalNumber) {
            std::cout << "Ошибка. Введите натуральное число." << std::endl;
        } else {
            break;
        }
    }
    return value;
}

// ПЕРВАЯ ЗАДАЧА (СУММА ЧИСЕЛ) 
int CalculateSum(int n, int m) {
    int total_sum = 0;

    for (int i{kStartValue}; i <= n; i += kDivisor) {
        if (i % m != 0) {
            total_sum += i;
        }
    }

    return total_sum;
}

void ExecuteSumTask() {
    int n_value{0};
    int m_value{0};

    do {
        std::cout << "Введите n(натуральное число): ";
        n_value = ReadNaturalNumber();
    } while (n_value <= 0);

    do {
        std::cout << "Введите m(натуральное число, m < n): ";
        m_value = ReadNaturalNumber();
    } while (m_value >= n_value || m_value <= 0);

    int result = CalculateSum(n_value, m_value);

    std::cout << "Результат:" << std::setw(10) << result << std::endl;
}

// ВТОРАЯ ЗАДАЧА - ИСПРАВЛЕННАЯ ВЕРСИЯ
double CalculatePiecewiseFunction(double a) {
    if (a >= 0) {
        // ∏(i²) - a для i от 2 до 8 с шагом 2
        // Вычисляем: (2² × 4² × 6² × 8²) - a
        double product = 1.0;
        for (int i = 2; i <= 8; i += 2) {
            product *= (i * i);  // i²
        }
        return product - a;  // ∏(i²) - a
    } else {
        // ∏(i-2) для i от 3 до 9 с шагом 3
        // Вычисляем: (3-2) × (6-2) × (9-2) = 1 × 4 × 7 = 28
        double product = 1.0;
        for (int i = 3; i <= 9; i += 3) {
            product *= (i - 2);
        }
        return product;
    }
}

void ExecutePiecewiseTask() {
    double a_value{0.0};
    
    std::cout << "Введите a: ";
    a_value = ReadDouble();
    
    const double result = CalculatePiecewiseFunction(a_value);
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Результат:" << std::setw(15) << result << std::endl;
}

// ТРЕТЬЯ ЗАДАЧА
double CalculateTaylorS(double x, double epsilon, int& last_n) {
    if (x == 0.0) {
        last_n = 0;
        return 0.0;
    }
    
    double sum = 0.0;
    double x_squared = x * x;
    double x_power = x * x_squared;  
    int n = 0;
    
    last_n = 0;
    
    while (n < kMaxIterations) {
        double term = x_power / (2.0 * n + 1.0);
        if (n % 2 == 1) {  
            term = -term;
        }
        
        double new_sum = sum + term;
        last_n = n;
        
        if (std::abs(new_sum - sum) < epsilon && n > 0) {
            break;
        }
        
        sum = new_sum;
        
        x_power *= x_squared;
        n++;
    }
    
    return sum;
}

double CalculateExactY(double x) {
    return x * x * std::atan(x);
}

void ExecuteTaylorTask() {
    std::cout << std::fixed << std::setprecision(6);
    
    std::cout << std::setw(10) << "x" << std::setw(15) << "Y(x)" 
              << std::setw(15) << "S(x)" << std::setw(10) << "N" << std::endl;
    
    for (double x = kStartX; x <= kEndX + 1e-9; x += kStepX) {
        int last_term_index{0};
        double y_value = CalculateExactY(x);
        double s_value = CalculateTaylorS(x, kEpsilon, last_term_index);
        
        std::cout << std::setw(10) << x << std::setw(15) << y_value 
                  << std::setw(15) << s_value << std::setw(10) << last_term_index << std::endl;
    }
}

// ЧЕТВЕРТАЯ ЗАДАЧА (КОРНИ) 
double CalculateRootSum(int n) {
    double sum = 0.0;
    
    for (int i = 0; i <= n; i++) {
        double term = std::sqrt(2.0 * i + 1.0);
        sum += term;
    }
    
    return sum;
}

void PrintLine(int length, char symbol) {
    for (int i = 0; i < length; ++i) {
        std::cout << symbol;
    }
    std::cout << std::endl;
}

void ExecuteRootTask() {
    int n_value{0};
    
    std::cout << "Введите натуральное n: ";
    n_value = ReadNaturalNumber();
    
    PrintLine(50, '-');
    std::cout << std::fixed << std::setprecision(8);
    
    std::cout << "Частичные суммы:" << std::endl;
    std::cout << std::setw(15) << "Кол-во слагаемых" << std::setw(20) << "Частичная сумма y" << std::endl;
    PrintLine(35, '-');
    
    for (int i{0}; i < kPartialCount; ++i) {
        int terms = kPartialTerms[i];
        double partial_sum = CalculateRootSum(terms);
        
        std::cout << std::setw(15) << terms << std::setw(20) << partial_sum << std::endl;
    }
    
    PrintLine(35, '-');
    
    double full_sum = CalculateRootSum(n_value);
    
    std::cout << "Финальный результат для n = " << n_value << ":" << std::endl;
    std::cout << std::setw(20) << "y:" << std::setw(20) << full_sum << std::endl;
    PrintLine(50, '=');
}

int DisplayMenu() {
    int choice{0};
    
    std::cout << std::setw(30) << "МЕНЮ ВЫБОРА" << std::endl;
    std::cout << "1. Сумма чисел, делящихся на 5, но не на m" << std::endl;
    std::cout << "2. Вычисление кусочно-заданной функции S" << std::endl;
    std::cout << "3. Вычисление функций Y(x) и S(x)" << std::endl;
    std::cout << "4. Вычисление суммы с корнями" << std::endl;
    std::cout << "5. Выйти" << std::endl;
    std::cout << "Выберите операцию (1-5): ";
    while (true) {
        choice = ReadInt();
        if (choice >= 1 && choice <= 5) {
            break;
        } else {
            std::cout << "Ошибка. Введите число от 1 до 5: ";
        }
    }
    
    return choice;
}

bool AskToContinue() {
    char response{' '};
    
    std::cout << std::endl << "Продолжить работу? (y/n): ";
    std::cin >> response;
    ClearInputBuffer();
    
    return (response == 'y' || response == 'Y');
}

int main() {
    bool continue_work{true};
    
    std::cout << std::fixed << std::setprecision(2);
    
    while (continue_work) {
        const int choice = DisplayMenu();
        switch (choice) {
            case 1:
                ExecuteSumTask();
                break;
            case 2:
                ExecutePiecewiseTask();
                break;
            case 3:
                ExecuteTaylorTask();
                break;
            case 4:
                ExecuteRootTask();
                break;
            case 5:
                std::cout << "Завершение программы..." << std::endl;
                continue_work = false;
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
                break;
        }
        
        if (continue_work && choice != 5) {
            continue_work = AskToContinue();
        }
    }
    
    return 0;
}