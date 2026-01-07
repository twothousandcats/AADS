#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

/**
 * Список смежности - хранение дерева,
 * Массив степеней - degree[v] = количество соседей у вершины v
 * Очередь - хранение текущих листьев(degree == 1)
 *
 * условие: Время счета не должно превышать 2 сек (сложность O(N))
 */

// Загрузка дерева из файла
bool loadTree(
    const string &filename,
    int &count,
    vector<vector<int> > &adj
) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Ошибка: не удалось открыть файл '" << filename << "'\n";
        return false;
    }

    fin >> count;
    if (count < 1) {
        cerr << "Ошибка: некорректное число вершин\n";
        return false;
    }

    adj.assign(count + 1, vector<int>());
    for (int i = 0; i < count - 1; ++i) {
        int u, v;
        fin >> u >> v;
        if (u < 1 || u > count || v < 1 || v > count) {
            cerr << "Ошибка: некорректный номер компьютера\n";
            return false;
        }
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    fin.close();
    return true;
}

// Вывод дерева в виде списка смежности
void printTree(
    const vector<vector<int> > &adj,
    int count
) {
    cout << "Дерево:\n";
    for (int i = 1; i <= count; ++i) {
        cout << i << ": ";
        for (size_t j = 0; j < adj[i].size(); ++j) {
            if (j > 0) cout << ", ";
            cout << adj[i][j];
        }
        cout << '\n';
    }
}

// Поиск центров дерева
vector<int> findCenters(
    const vector<vector<int> > &adj,
    int n
) {
    if (n == 1) {
        return {1};
    }

    vector<int> degree(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        degree[i] = adj[i].size();
    }

    queue<int> q;
    for (int i = 1; i <= n; ++i) {
        if (degree[i] == 1) {
            q.push(i);
        }
    }

    int remaining = n;
    // Копируем degree, чтобы не портить оригинал
    vector<int> deg = degree;

    while (remaining > 2) {
        int leafCount = q.size();
        remaining -= leafCount;
        for (int i = 0; i < leafCount; ++i) {
            int leaf = q.front();
            q.pop();
            for (int neighbor: adj[leaf]) {
                deg[neighbor]--;
                if (deg[neighbor] == 1) {
                    q.push(neighbor);
                }
            }
        }
    }

    vector<int> centers;
    while (!q.empty()) {
        centers.push_back(q.front());
        q.pop();
    }

    sort(centers.begin(), centers.end());
    return centers;
}

// Сохранение в файл
void saveResult(
    const vector<int> &centers,
    const string &filename
) {
    ofstream fout(filename);
    fout << centers.size() << "\n";
    for (size_t i = 0; i < centers.size(); ++i) {
        if (i > 0) fout << " ";
        fout << centers[i];
    }
    fout << "\n";
    fout.close();
    cout << "Результат сохранён в файл '" << filename << "'\n";
}

int main() {
    const string INPUT_FILE = "input.txt";
    const string OUTPUT_FILE = "output.txt";

    int count;
    vector<vector<int> > adj;

    cout << "Загрузка дерева из файла '" << INPUT_FILE << "'...\n";
    if (!loadTree(INPUT_FILE, count, adj)) {
        cout << "Не удалось загрузить дерево. Завершение программы.\n";
        return 1;
    }

    cout << "Дерево успешно загружено (" << count << " компьютеров).\n";

    // Основной интерактивный цикл
    char choice;
    do {
        cout << "\nМеню:\n";
        cout << "1 — Показать дерево на экране\n";
        cout << "2 — Найти оптимальные компьютеры (центры дерева)\n";
        cout << "3 — Сохранить результат в файл\n";
        cout << "0 — Выйти\n";
        cout << "Ваш выбор: ";
        cin >> choice;

        switch (choice) {
            case '1': {
                printTree(adj, count);
                break;
            }
            case '2': {
                vector<int> centers = findCenters(adj, count);
                cout << "Количество оптимальных компьютеров: " << centers.size() << "\n";
                cout << "Номера: ";
                for (size_t i = 0; i < centers.size(); ++i) {
                    if (i > 0) cout << " ";
                    cout << centers[i];
                }
                cout << "\n";
                break;
            }
            case '3': {
                vector<int> centers = findCenters(adj, count);
                saveResult(centers, OUTPUT_FILE);
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
    } while (choice != '0');

    return 0;
}
