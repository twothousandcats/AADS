#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>

// Типы деревев
enum TreeType {
    CHAIN, // Цепочка
    STAR, // Звезда
    RANDOM // Случайное дерево
};

void generateChain(std::ofstream &out, int N) {
    for (int i = 1; i < N; ++i) {
        out << i << " " << (i + 1) << "\n";
    }
}

void generateStar(std::ofstream &out, int N) {
    for (int i = 2; i <= N; ++i) {
        out << 1 << " " << i << "\n";
    }
}

// Генерация дерева через Прюфера
void generateRandomTree(std::ofstream &out, int N, std::mt19937 &rng) {
    if (N == 1) return;
    if (N == 2) {
        out << "1 2\n";
        return;
    }

    std::vector<int> prufer(N - 2);
    std::uniform_int_distribution<int> dist(1, N);
    for (int i = 0; i < N - 2; ++i) {
        prufer[i] = dist(rng);
    }

    // Восстанавливаем дерево
    std::vector<int> degree(N + 1, 1);
    for (int v: prufer) {
        degree[v]++;
    }

    std::vector<std::pair<int, int> > edges;
    for (int v: prufer) {
        // Находим наименьший лист (degree == 1)
        int leaf = 1;
        while (degree[leaf] != 1) ++leaf;
        edges.emplace_back(leaf, v);
        degree[leaf]--;
        degree[v]--;
    }

    // Остаются два листа
    int u = -1, w = -1;
    for (int i = 1; i <= N; ++i) {
        if (degree[i] == 1) {
            if (u == -1) u = i;
            else w = i;
        }
    }
    edges.emplace_back(u, w);

    for (auto [a, b]: edges) {
        out << a << " " << b << "\n";
    }
}

int main() {
    const int N = 10; // размерность
    const TreeType TYPE = RANDOM; // CHAIN / STAR / RANDOM

    std::ofstream fout("input.txt");
    fout << N << "\n";

    if (TYPE == CHAIN) {
        generateChain(fout, N);
    } else if (TYPE == STAR) {
        generateStar(fout, N);
    } else if (TYPE == RANDOM) {
        std::random_device rd;
        std::mt19937 rng(rd());
        generateRandomTree(fout, N, rng);
    }

    fout.close();
    std::cout << "Generated INPUT.TXT for N = " << N << ")\n" << std::endl;

    return 0;
}
