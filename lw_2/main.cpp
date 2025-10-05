/*
    GENERAL:
       1) вводить   исходное   дерево  из  файла  в  понятной  для
    пользователя форме, а не с клавиатуры;
       2) по требованию пользователя показывать дерево на экране;
       3) обрабатывать  дерево  в  оперативной памяти,  а не путем
    многократного обращения к файлу;
       4) обеспечить   возможность   многократных   запросов   без
    повторного запуска программы.

    1. УСЛОВИЕ ЗАДАЧИ:
    Имеется   дерево,   корень    которого    соответствует
    основателю  рода.  Сыновья  каждой  вершины  задают  сыновей и
    дочерей  соответствующего  человека.  Указываются  имена  двух
    человек  (например,  А  и  В).  Сообщить,  какая  из следующих
    ситуаций имеет место:
       1) А предок В;
       2) В предок А;
       3) А и В имеют ближайшего общего предка С (9).

    2. АВТОР:
    Потапенко Максим Владимирович

    3. СРЕДА ВЫПОЛНЕНИЯ
    Clang, MacOS

    4. ИСТОЧНИКИ
    Tree train: https://leetcode.com/problem-list/tree/
*/

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <algorithm>
#include <cctype>

using namespace std;

struct Node {
    string name;
    vector<Node *> children;
    Node *parent = nullptr;

    explicit Node(string n) : name(std::move(n)) {}
};

void deleteTree(const Node *node) {
    if (!node) return;
    for (const Node *child: node->children) {
        deleteTree(child);
    }
    delete node;
}

void printTree(const Node *node, int depth = 0) {
    if (!node) return;
    cout << string(depth * 2, '_') << node->name << endl;
    for (const Node *child: node->children) {
        printTree(child, depth + 1);
    }
}

Node *findNode(Node *root, const string &name) {
    if (!root) return nullptr;
    if (root->name == name) return root;
    for (Node *child: root->children) {
        if (Node *found = findNode(child, name)) {
            return found;
        }
    }
    return nullptr;
}

vector<string> getPathToRoot(const Node *node) {
    vector<string> path;
    while (node) {
        path.push_back(node->name);
        node = node->parent;
    }
    ranges::reverse(path);
    return path;
}

string findLCA(const Node *a, const Node *b) {
    if (!a || !b) return "";
    const auto pathA = getPathToRoot(a);
    const auto pathB = getPathToRoot(b);

    string lca;
    const size_t minLen = min(pathA.size(), pathB.size());
    for (size_t i = 0; i < minLen; ++i) {
        if (pathA[i] == pathB[i]) {
            lca = pathA[i];
        } else {
            break;
        }
    }
    return lca;
}

bool isAncestor(const Node *ancestor, const Node *descendant) {
    if (!ancestor || !descendant) return false;
    while (descendant) {
        if (descendant == ancestor) return true;
        descendant = descendant->parent;
    }
    return false;
}

pair<int, string> parseLine(const string &line) {
    if (line.empty()) return {-1, ""};

    size_t starCount = 0;
    while (starCount < line.size() && line[starCount] == '*') {
        ++starCount;
    }

    string namePart = line.substr(starCount);
    // Удаляем пробелы по краям
    auto start = namePart.find_first_not_of(" \t");
    if (start == string::npos) return {-1, ""};

    auto end = namePart.find_last_not_of(" \t");
    string name = namePart.substr(start, end - start + 1);
    return {static_cast<int>(starCount), name};
}

Node *buildTreeFromLines(const vector<pair<int, string> > &lines) {
    if (lines.empty() || lines[0].first != 0) {
        if (lines.empty()) {
            cerr << "Ошибка: нет данных для построения дерева." << endl;
        } else {
            cerr << "Ошибка: корень должен быть на уровне 0 (без '*')." << endl;
        }
        return nullptr;
    }

    stack<pair<Node *, int> > nodeStack;
    Node *root = new Node(lines[0].second);
    nodeStack.emplace(root, 0);

    for (size_t i = 1; i < lines.size(); ++i) {
        int level = lines[i].first;
        const string &name = lines[i].second;

        while (!nodeStack.empty() && nodeStack.top().second >= level) {
            nodeStack.pop();
        }

        if (nodeStack.empty()) {
            cerr << "Ошибка: некорректная структура дерева в строке " << i + 1 << "." << endl;
            deleteTree(root);
            return nullptr;
        }

        Node *parent = nodeStack.top().first;
        Node *newNode = new Node(name);
        newNode->parent = parent;
        parent->children.push_back(newNode);
        nodeStack.emplace(newNode, level);
    }

    return root;
}

Node *loadTreeFromFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл '" << filename << endl;
        return nullptr;
    }

    vector<pair<int, string> > lines;
    string line;
    while (getline(file, line)) {
        auto [level, name] = parseLine(line);
        if (level >= 0 && !name.empty()) {
            lines.emplace_back(level, name);
        }
    }
    file.close();

    return buildTreeFromLines(lines);
}

void handleRelationshipQuery(Node *root) {
    cout << "Введите имя первого человека: ";
    string nameA, nameB;
    getline(cin, nameA);
    cout << "Введите имя второго человека: ";
    getline(cin, nameB);

    Node *nodeA = findNode(root, nameA);
    Node *nodeB = findNode(root, nameB);

    if (!nodeA) {
        cout << "Человек '" << nameA << "' не найден в дереве." << endl;
        return;
    }
    if (!nodeB) {
        cout << "Человек '" << nameB << "' не найден в дереве." << endl;
        return;
    }

    if (nodeA == nodeB) {
        cout << nameA << " и "
                << nameB << " - один и тот же человек"
                << endl;
    } else if (isAncestor(nodeA, nodeB)) {
        cout << nameA << " — предок " << nameB << "." << endl;
    } else if (isAncestor(nodeB, nodeA)) {
        cout << nameB << " — предок " << nameA << "." << endl;
    } else {
        string lca = findLCA(nodeA, nodeB);
        if (!lca.empty()) {
            cout << nameA << " и " << nameB << " имеют ближайшего общего предка: " << lca << "." << endl;
        } else {
            cout << "Не удалось определить связь между " << nameA << " и " << nameB << "." << endl;
        }
    }
}

void showMenu() {
    cout << "" << endl;
    cout << "=== Меню ===" << endl;
    cout << "1 — Показать дерево" << endl;
    cout << "2 — Проверить родственные связи" << endl;
    cout << "0 — Выход" << endl;
    cout << "Выберите действие: ";
}

int main() {
    const string filename = "family.txt";
    Node *root = loadTreeFromFile(filename);
    if (!root) {
        return 1;
    }

    cout << "Дерево успешно загружено из файла '" << filename << endl;

    string command;
    while (true) {
        showMenu();
        getline(cin, command);

        if (command == "0") {
            break;
        }
        if (command == "1") {
            cout << "\nДерево:" << endl;
            printTree(root);
        } else if (command == "2") {
            handleRelationshipQuery(root);
        } else {
            cout << "Неверная команда. Попробуйте снова." << endl;
        }
    }

    deleteTree(root);
    cout << "Программа завершена." << endl;
    return 0;
}
