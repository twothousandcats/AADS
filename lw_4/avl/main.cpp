#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;

struct Node {
    int key, height;
    Node *left{};
    Node *right{};

    explicit Node(const int k) : key(k), height(1) {
    };
};

int getHeight(const Node *node) {
    return node ? node->height : 0;
}

// -1, 0, 1
int getBalanceFactor(const Node *node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// Пересчитывает высоту узла на основе высот его детей
void fixHeight(Node *node) {
    if (node) {
        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
    }
}

Node *rotateRight(Node *y) {
    Node *x = y->left;
    y->left = x->right;
    x->right = y;

    fixHeight(y);
    fixHeight(x);
    return x;
}

Node *rotateLeft(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    y->left = x;
    fixHeight(x);
    fixHeight(y);
    return y;
}

Node *balance(Node *node) {
    if (!node) {
        return node;
    }
    fixHeight(node);

    if (getBalanceFactor(node) == 2) {
        if (getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left); // LR-случай
        }

        return rotateRight(node); // LL-случай
    }

    if (getBalanceFactor(node) == -2) {
        if (getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right); // RL-случай
        }

        return rotateLeft(node); // RR-случай
    }

    return node;
}

// с использованием стека в куче
Node *insert(
    Node *root,
    const int key
) {
    if (!root) {
        return new Node(key);
    }

    vector<Node **> path; // указатель на указатель
    Node **current = &root;

    // спускаемся в место вставки запоминая путь
    while (*current) {
        path.push_back(current);
        if (key < (*current)->key) {
            current = &((*current)->left);
        } else {
            current = &((*current)->right);
        }
    }

    // создаем
    *current = new Node(key);
    path.push_back(current); // добавляем узел как последний

    for (int i = static_cast<int>(path.size()) - 2; i >= 0; --i) {
        Node *parent = *path[i];
        fixHeight(parent);
        Node *balancedNode = balance(parent);
        // если корень поддерева обновился, обновляем родителя
        if (balancedNode != parent) {
            if (i == 0) {
                root = balancedNode;
            } else {
                *(path[i - 1]) = balancedNode;
            }
        }
    }

    return root;
}

void writeTree(
    Node *root,
    vector<int> &data
) {
    if (!root) {
        return;
    }

    stack<Node *> stack;
    Node *current = root;

    while (current || !stack.empty()) {
        while (current) {
            stack.push(current);
            current = current->left;
        }
        current = stack.top();
        stack.pop();
        data.push_back(current->key);
        current = current->right;
    }
}

void clearTree(
    Node *root
) {
    if (!root) {
        return;
    }

    queue<Node *> q;
    q.push(root);
    while (!q.empty()) {
        Node *node = q.front();
        q.pop();
        if (node->left) {
            q.push(node->left);
        }
        if (node->right) {
            q.push(node->right);
        }

        delete node;
    }
}

/* Node *insert(
    Node *root,
    const int key
) {
    if (!root) {
        return new Node(key);
    }

    if (key < root->key) {
        root->left = insert(root->left, key);
    } else {
        root->right = insert(root->right, key);
    }

    return balance(root);
}

void writeTree(
    const Node *root,
    vector<int> &data
) {
    if (!root) {
        return;
    }

    writeTree(root->left, data);
    data.push_back(root->key);
    writeTree(root->right, data);
}

void clearTree(Node *root) {
    if (!root) {
        return;
    }

    clearTree(root->left);
    clearTree(root->right);
    delete root;
} */

vector<int> readFile(
    const string &fileName = "input.txt"
) {
    vector<int> data;
    ifstream fin(fileName);
    int x;
    while (!fin.eof() && fin >> x) {
        data.push_back(x);
    }

    fin.close();
    return data;
}

void writeFile(
    const vector<int> &data,
    const string &fileName = "output.txt"
) {
    ofstream fout(fileName);
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) {
            fout << " ";
        }
        fout << data[i];
    }
    fout.close();
}

int main() {
    const auto start = chrono::high_resolution_clock::now();

    // implementation
    vector<int> data = readFile();
    Node *root = nullptr;
    for (const auto x: data) {
        root = insert(root, x);
    }

    vector<int> sortedData;
    writeTree(root, sortedData);

    const auto end = chrono::high_resolution_clock::now();
    const auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    writeFile(sortedData);

    cout << "AVL-tree сортировка: ";
    cout << duration.count();
    cout << " мкс" << endl;

    clearTree(root);

    return 0;
}
