#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

void getSettingValues(
    double &count,
    double &minValue,
    double &maxValue
) {
    cout << "Введите количество чисел, минимальное и максимальное через пробел" << endl;
    cin >> count >> minValue >> maxValue;
}

void getSortType(
    int &order
) {
    cout << "Выберете порядок:" << endl;
    cout << "1 - случайный" << endl;
    cout << "2 - упорядоченный min -> max" << endl;
    cout << "3 - упорядоченный max -> min" << endl;
    cout << "Ваш выбор: " << endl;
    cin >> order;
}

void generateData(
    vector<int> &data,
    const double minVal,
    const double maxVal
) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minVal, maxVal);

    for (int &x: data) {
        x = dis(gen);
    }
}

void applyOrder(
    vector<int> &data,
    const int orderType
) {
    random_device rd;
    mt19937 gen(rd());

    switch (orderType) {
        case 1: {
            ranges::shuffle(data, gen);
            break;
        }
        case 2: {
            ranges::sort(data);
            break;
        }
        case 3: {
            ranges::sort(data, greater<int>{});
            break;
        }
        default: ;
    }
}

void writeToFile(
    const vector<int> &data,
    const string &fileName = "input.txt"
) {
    ofstream fout(fileName);
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) {
            fout << " ";
        }
        fout << data[i];
    }
    fout.close();
    cout << "Файл input.txt успешно создан!" << endl;
}

int main() {
    double count, minValue, maxValue;
    int orderType = 0;
    getSettingValues(count, minValue, maxValue);
    while (orderType != 1 && orderType != 2 && orderType != 3) {
        getSortType(orderType);
    }

    vector<int> data(count);
    generateData(data, minValue, maxValue);
    applyOrder(data, orderType);
    writeToFile(data);
    return 0;
}
