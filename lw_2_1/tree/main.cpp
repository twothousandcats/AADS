#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

/**
 * Список смежности - хранение дерева,
 * Массив степеней - currentDegree[v] — текущее количество активных соседей у вершины v.
   Лист <-> currentDegree[v] == 1 (при условии, что вершин больше 1).
 * Очередь - Хранит все текущие листья.
   Новые листья добавляются туда сразу, как только их степень становится 1.
 *
 * условие: Время счета не должно превышать 2 сек.
 * Для быстрого нахождения листьев используется массив степеней вершин в сочетании с очередью текущих листьев.
 * Вершина считается листом, если её степень равна 1.
 * При «удалении» листа степени его соседей уменьшаются, и новые листья немедленно добавляются в очередь
 * (линейная сложность O(N))
 */

// Загрузка дерева из файла
bool loadTree(
    const string &filename,
    int &totalVertices,
    vector<vector<int> > &adjacencyList
) {
    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл '" << filename << "'\n";
        return false;
    }

    input_file >> totalVertices;
    if (totalVertices < 1) {
        cerr << "Ошибка: некорректное число вершин\n";
        return false;
    }

    adjacencyList.assign(totalVertices + 1, vector<int>());
    for (int edge_index = 0; edge_index < totalVertices - 1; ++edge_index) {
        int first_vertex, second_vertex;
        input_file >> first_vertex >> second_vertex;
        if (first_vertex < 1 || first_vertex > totalVertices ||
            second_vertex < 1 || second_vertex > totalVertices) {
            cerr << "Ошибка: некорректный номер компьютера\n";
            return false;
        }
        adjacencyList[first_vertex].push_back(second_vertex);
        adjacencyList[second_vertex].push_back(first_vertex);
    }
    input_file.close();
    return true;
}

// Вывод дерева списком смежности
void printTree(
    const vector<vector<int> > &adjacencyList,
    int totalVertices
) {
    cout << "Дерево:\n";
    for (int vertex = 1; vertex <= totalVertices; ++vertex) {
        cout << vertex << ": ";
        for (size_t neighborIndex = 0; neighborIndex < adjacencyList[vertex].size(); ++neighborIndex) {
            if (neighborIndex > 0) {
                cout << ", ";
            }
            cout << adjacencyList[vertex][neighborIndex];
        }
        cout << '\n';
    }
}

// Поиск центров дерева
vector<int> findCenters(
    const vector<vector<int> > &adjacencyList,
    int totalVertices
) {
    if (totalVertices == 1) {
        return {1};
    }

    vector<int> vertexDegree(totalVertices + 1, 0);
    for (int vertex = 1; vertex <= totalVertices; ++vertex) {
        vertexDegree[vertex] = static_cast<int>(adjacencyList[vertex].size());
    }

    queue<int> leafQueue;
    for (int vertex = 1; vertex <= totalVertices; ++vertex) {
        if (vertexDegree[vertex] == 1) {
            leafQueue.push(vertex);
        }
    }

    int remainingVertices = totalVertices;
    vector<int> currentDegree = vertexDegree; // копия для модификации

    while (remainingVertices > 2) {
        int currentLeafCount = static_cast<int>(leafQueue.size());
        remainingVertices -= currentLeafCount;

        for (int leafIndex = 0; leafIndex < currentLeafCount; ++leafIndex) {
            int currentLeaf = leafQueue.front();
            leafQueue.pop();

            for (int neighbor: adjacencyList[currentLeaf]) {
                currentDegree[neighbor]--;
                if (currentDegree[neighbor] == 1) {
                    leafQueue.push(neighbor);
                }
            }
        }
    }

    vector<int> centers;
    while (!leafQueue.empty()) {
        centers.push_back(leafQueue.front());
        leafQueue.pop();
    }

    sort(centers.begin(), centers.end());
    return centers;
}

// Сохранение в файл
void saveResult(
    const vector<int> &centers,
    const string &outputFilename
) {
    ofstream outputFile(outputFilename);
    outputFile << centers.size() << "\n";
    for (size_t centerIndex = 0; centerIndex < centers.size(); ++centerIndex) {
        if (centerIndex > 0) {
            outputFile << " ";
        }
        outputFile << centers[centerIndex];
    }
    outputFile << "\n";
    outputFile.close();
    cout << "Результат сохранён в файл '" << outputFilename << "'\n";
}

int main() {
    const string INPUT_FILE_NAME = "input.txt";
    const string OUTPUT_FILE_NAME = "output.txt";

    int totalVertices = 0;
    vector<vector<int> > adjacencyList;

    cout << "Загрузка дерева из файла '" << INPUT_FILE_NAME << "'...\n";
    if (!loadTree(INPUT_FILE_NAME, totalVertices, adjacencyList)) {
        cout << "Не удалось загрузить дерево. Завершение программы.\n";
        return 1;
    }

    cout << "Дерево успешно загружено (" << totalVertices << " компьютеров).\n";

    // Основной интерактивный цикл
    char userChoice;
    do {
        cout << "\nМеню:\n";
        cout << "1 — Показать дерево на экране\n";
        cout << "2 — Найти оптимальные компьютеры (центры дерева)\n";
        cout << "3 — Сохранить результат в файл\n";
        cout << "0 — Выйти\n";
        cout << "Ваш выбор: ";
        cin >> userChoice;

        switch (userChoice) {
            case '1': {
                printTree(adjacencyList, totalVertices);
                break;
            }
            case '2': {
                vector<int> centers = findCenters(adjacencyList, totalVertices);
                cout << "Количество оптимальных компьютеров: " << centers.size() << "\n";
                cout << "Номера: ";
                for (size_t index = 0; index < centers.size(); ++index) {
                    if (index > 0) {
                        cout << " ";
                    }
                    cout << centers[index];
                }
                cout << "\n";
                break;
            }
            case '3': {
                vector<int> centers = findCenters(adjacencyList, totalVertices);
                saveResult(centers, OUTPUT_FILE_NAME);
                break;
            }
            case '0': {
                cout << "Завершение работы.\n";
                break;
            }
            default: {
                cout << "Некорректный выбор. Попробуйте снова.\n";
            }
        }
    } while (userChoice != '0');

    return 0;
}
