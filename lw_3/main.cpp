/*
   GENERAL:
      1) вводить  граф из файла в понятной для пользователя форме
      (не в виде матрицы смежности, без дублирования информации и т.п.);
      2) обеспечить   возможность   многократных   запросов   без
      повторного запуска программы.

    1. УСЛОВИЕ ЗАДАЧИ:
       17. Имеется сеть автомобильных  дорог с двусторонним движением.
       Для каждой дороги известна максимальная возможная масса провозимого
       по ней груза. С помощью алгоритма Дейкстры определить максимальный
       груз, который можно доставить  между  двумя  указанными  городами,
       заезжая по пути в заданный промежуточный город (9).

    2. АВТОР:
    Потапенко Максим Владимирович

    3. СРЕДА ВЫПОЛНЕНИЯ
    Clang, MacOS

    4. ИСТОЧНИКИ
    Graph: https://leetcode.com/problem-list/graph/
*/

#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <set>
#include <string>
#include <climits>
#include <algorithm>

/// Тип графа: отображение города в список соседей с весами дорог
using RoadNetwork = std::map<std::string, std::vector<std::pair<std::string, int> > >;

/**
 * @brief Находит максимальную массу груза, которую можно провезти от start до finish.
 * Используется модифицированный алгоритм Дейкстры (максимизация минимального веса на пути).
 */
int findMaxCargoWeight(
    const RoadNetwork &network,
    const std::string &start,
    const std::string &finish
) {
    std::map<std::string, int> maxMinWeight;
    std::priority_queue<std::pair<int, std::string> > pq; // значение узкого места, имя

    for (const auto &cityNode: network) {
        maxMinWeight[cityNode.first] = 0;
    }

    maxMinWeight[start] = INT_MAX;
    pq.emplace(INT_MAX, start);

    while (!pq.empty()) {
        // берем город с наибольшей пропускной способностью
        // по дефолту в priority_queue он будет определен сверху
        int currentBottleneck = pq.top().first;
        std::string currentCity = pq.top().second;
        pq.pop();

        // старое пропустим
        if (currentBottleneck != maxMinWeight[currentCity]) {
            continue;
        }

        // обходить соседей
        for (const auto &edge: network.at(currentCity)) {
            const std::string &neighbor = edge.first;
            int roadCapacity = edge.second;
            // взять минимальное среди текущего и нового значения
            int newBottleneck = std::min(currentBottleneck, roadCapacity);

            // обновить путь, если новое меньше
            if (newBottleneck > maxMinWeight[neighbor]) {
                maxMinWeight[neighbor] = newBottleneck;
                pq.emplace(newBottleneck, neighbor);
            }
        }
    }

    return maxMinWeight[finish];
}

/**
 * @brief Загружает граф дорог из текстового файла.
 */
RoadNetwork loadRoadNetworkFromFile(
    const std::string &filename,
    std::set<std::string> &allCities
) {
    RoadNetwork network;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл '" << filename << "'" << std::endl;
        return network;
    }

    int numberOfCities, numberOfRoads;
    file >> numberOfCities >> numberOfRoads;

    for (int i = 0; i < numberOfRoads; ++i) {
        std::string cityA, cityB;
        int maxWeight;
        file >> cityA >> cityB >> maxWeight;

        network[cityA].emplace_back(cityB, maxWeight);
        network[cityB].emplace_back(cityA, maxWeight);

        allCities.insert(cityA);
        allCities.insert(cityB);
    }

    file.close();
    return network;
}

/**
 * @brief Выводит список всех городов.
 */
void displayCities(
    const std::set<std::string> &allCities
) {
    std::cout << "Список городов:" << std::endl;
    for (const auto &city: allCities) {
        std::cout << "  " << city << std::endl;
    }
}

/**
 * @brief Обрабатывает запрос пользователя на расчёт максимального груза через промежуточный город.
 */
void handleRouteQuery(
    const RoadNetwork &roadNetwork,
    const std::set<std::string> &allCities
) {
    std::string origin, intermediate, destination;
    std::cout << "Введите три города: откуда, через какой, куда (через пробел): ";
    std::cin >> origin >> intermediate >> destination;

    if (allCities.find(origin) == allCities.end()) {
        std::cout << "Ошибка: город '" << origin << "' отсутствует в графе." << std::endl;
        return;
    }
    if (allCities.find(intermediate) == allCities.end()) {
        std::cout << "Ошибка: промежуточный город '" << intermediate << "' отсутствует в графе." << std::endl;
        return;
    }
    if (allCities.find(destination) == allCities.end()) {
        std::cout << "Ошибка: город '" << destination << "' отсутствует в графе." << std::endl;
        return;
    }

    const int weightPart1 = findMaxCargoWeight(roadNetwork, origin, intermediate);
    const int weightPart2 = findMaxCargoWeight(roadNetwork, intermediate, destination);

    if (weightPart1 == 0 || weightPart2 == 0) {
        std::cout << "Невозможно проложить маршрут через '" << intermediate << "'." << std::endl;
    } else {
        const int maxCargo = std::min(weightPart1, weightPart2);
        std::cout << "Максимальный груз, который можно доставить из '"
                << origin << "' в '" << destination
                << "' через '" << intermediate << "': "
                << maxCargo << std::endl;
    }
}

/**
 * @brief Формирует главное мени и обрабатывает выбор пользователя
 */
void runMenu(
    const RoadNetwork &roadNetwork,
    const std::set<std::string> &allCities
) {
    int userChoice;
    do {
        std::cout << "Меню:" << std::endl;
        std::cout << "1 - вывод городов" << std::endl;
        std::cout << "2 - ввод городов (откуда, через, куда)" << std::endl;
        std::cout << "3 - выход" << std::endl;
        std::cout << "Выберите действие: ";
        std::cin >> userChoice;

        switch (userChoice) {
            case 1:
                displayCities(allCities);
                break;
            case 2:
                handleRouteQuery(roadNetwork, allCities);
                break;
            case 3:
                std::cout << "Выход из программы." << std::endl;
                break;
            default:
                std::cout << "Неверный выбор. Введите 1, 2 или 3." << std::endl;
                break;
        }
    } while (userChoice != 3);
}

int main() {
    std::set<std::string> allCities;
    const RoadNetwork roadNetwork = loadRoadNetworkFromFile("graph.txt", allCities);

    if (roadNetwork.empty()) {
        std::cout << "Граф не загружен. Убедитесь, что файл 'graph.txt' существует и имеет правильный формат." <<
                std::endl;
        return 1;
    }

    runMenu(roadNetwork, allCities);
    return 0;
}
