// связный ациклический граф
// цель: минимизация максимального пути
// общая O(n)

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>

constexpr auto INPUT_FILE_NAME = "input.txt";
constexpr auto OUTPUT_FILE_NAME = "output.txt";

int main() {
    std::ifstream fin(INPUT_FILE_NAME);
    std::ofstream fout(OUTPUT_FILE_NAME);

    int N;

    // одно устройство
    fin >> N;
    if (N == 1) {
        fout << "1\n1\n";
        return 0;
    }

    std::vector<std::vector<int> > graph(N + 1);
    std::vector<int> degree(N + 1, 0);

    // читаем ребра дерева
    for (int i = 0; i < N - 1; ++i) {
        int u, v;
        fin >> u >> v;
        graph[u].push_back(v);
        graph[v].push_back(u);
        degree[u]++;
        degree[v]++;
    }

    // Q текущих листьев (степень 1)
    std::queue<int> leaves;
    for (int i = 1; i <= N; ++i) {
        if (degree[i] == 1) {
            leaves.push(i);
        }
    }

    int remainingNodes = N;

    // Удаляем листья слой за слоем, пока не останется 1 или 2 вершины
    while (remainingNodes > 2) {
        int currentLeavesCount = leaves.size();
        remainingNodes -= currentLeavesCount;

        // Удаляем все листья текущего уровня
        for (int i = 0; i < currentLeavesCount; ++i) {
            int leaf = leaves.front();
            leaves.pop();

            // Уменьшаем степень соседей
            for (int neighbor: graph[leaf]) {
                degree[neighbor]--;
                // Если сосед стал листом —> добавляем в очередь
                if (degree[neighbor] == 1) {
                    leaves.push(neighbor);
                }
            }
        }
    }

    // Оставшиеся вершины в очереди — центры дерева
    std::vector<int> centers;
    while (!leaves.empty()) {
        centers.push_back(leaves.front());
        leaves.pop();
    }

    std::sort(centers.begin(), centers.end());

    fout << centers.size() << '\n';
    for (size_t i = 0; i < centers.size(); ++i) {
        if (i > 0) fout << ' ';
        fout << centers[i];
    }

    fout << '\n';

    return 0;
}
