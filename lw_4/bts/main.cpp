#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

struct Node {
    int key;
    Node *left{};
    Node *right{};

    explicit Node(const int k) : key(k) {
    }
};

void insert(
    Node *&root,
    const int key
) {
    if (!root) {
        root = new Node(key);
        return;
    }

    Node *current = root;
    while (true) {
        if (key < current->key) {
            if (!current->left) {
                current->left = new Node(key);
                break;
            }
            current = current->left;
        } else {
            if (!current->right) {
                current->right = new Node(key);
                break;
            }
            current = current->right;
        }
    }
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

/* на рекурсии
void insert(
    Node *&root,
    const int key
) {
    if (!root) {
        root = new Node(key);
        return;
    }

    if (key < root->key) {
        insert(root->left, key);
    } else {
        insert(root->right, key);
    }
}

void writeTree(
    Node *root,
    vector<int> &data
) {
    if (!root) {
        return;
    }

    writeTree(root->left, data);
    data.push_back(root->key);
    writeTree(root->right, data);
}

void clearTree(
    Node *root
) {
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
    ifstream fin(fileName);
    vector<int> data;
    int x;
    while (fin >> x) {
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
    const vector<int> inputData = readFile();
    Node *root = nullptr;
    for (const int x: inputData) {
        insert(root, x);
    }

    vector<int> sortedData;
    writeTree(root, sortedData);

    const auto end = chrono::high_resolution_clock::now();
    const auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    writeFile(sortedData);

    cout << "BTS сортировка: ";
    cout << duration.count();
    cout << " мкс" << endl;
    clearTree(root);

    return 0;
}
