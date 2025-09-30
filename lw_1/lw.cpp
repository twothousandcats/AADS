/*
    1. УСЛОВИЕ ЗАДАЧИ:
        У множества шпионов, собравшихся вместе для наблюдения секретного объекта,
        имеется единственный бинокль. Сформировалась очередь на этот бинокль.
        Для каждого шпиона задан период наблюдения в минутах и предельное время нахождения в очереди.
        После наблюдения каждый шпион становится снова в конец очереди.
        Как только для какого-либо шпиона истекает предельное время нахождения в очереди,
        он покидает ее (даже если в этот момент владеет биноклем) и отправляется к резиденту.
        Вывести протокол наблюдения шпионов за объектом(9).

    2. АВТОР:
        Потапенко Максим Владимирович

    3. СРЕДА ВЫПОЛНЕНИЯ
        Clang, MacOS

    4. ИСТОЧНИКИ
        Queue train: https://leetcode.com/problem-list/queue/
*/
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Spy {
    string name;
    int observationTime{};
    int maxQueueTime{};
    int totalTimeSpent{};
    int currentObservationTime{};
    bool isObserving{};
    Spy* next{};
};

struct Queue {
    Spy* front;
    Spy* rear;
};

void initQueue(Queue& q) {
    q.front = nullptr;
    q.rear = nullptr;
}

bool isEmpty(Queue& q) {
    return q.front == nullptr;
}

void enqueue(Queue& q, Spy* spy) {
    spy->next = nullptr;
    if (isEmpty(q)) {
        q.front = spy;
        q.rear = spy;
    } else {
        q.rear->next = spy;
        q.rear = spy;
    }
}

Spy* dequeue(Queue& q) {
    if (isEmpty(q)) {
        return nullptr;
    }
    Spy* spy = q.front;
    q.front = q.front->next;
    if (q.front == nullptr) {
        q.rear = nullptr;
    }

    spy->next = nullptr;
    return spy;
}

Spy* readSpiesFromFile(const string& filename, int& spyCount) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файлы: " << filename << endl;
        return nullptr;
    }

    spyCount = 0;
    string name;
    int obsTime, maxTime;
    Spy* head = nullptr;
    Spy* current = nullptr;

    while (file >> name >> obsTime >> maxTime) {
        Spy* newSpy = new Spy;
        newSpy->name = name;
        newSpy->observationTime = obsTime;
        newSpy->maxQueueTime = maxTime;
        newSpy->totalTimeSpent = 0;
        newSpy->currentObservationTime = 0;
        newSpy->isObserving = false;
        newSpy->next = nullptr;

        if (head == nullptr) {
            head = newSpy;
            current = head;
        } else {
            current->next = newSpy;
            current = newSpy;
        }
        spyCount++;
    }

    file.close();
    return head;
}

void simulateObservation(Queue& q, int spyCount) {
    int currentTime = 0;
    Spy* currentSpy = nullptr;
    int remainingSpies = spyCount;

    while (remainingSpies > 0) {
        bool isChanged = false;

        // Проверяем шпионов в очереди на превышение времени ожидания
        Spy* temp = q.front;
        Spy* prev = nullptr;
        while (temp != nullptr) {
            Spy* next = temp->next;

            if (temp->totalTimeSpent >= temp->maxQueueTime && temp->maxQueueTime != -1) {
                cout << "Момент времени " << currentTime << ": " << temp->name << " отправился к резиденту (превышено время ожидания)." << endl;

                // Удаляем из очереди
                if (prev == nullptr) {
                    q.front = next;
                } else {
                    prev->next = next;
                }

                if (temp == q.rear) {
                    q.rear = prev;
                }

                delete temp;
                remainingSpies--;
                isChanged = true;
            } else {
                prev = temp;
            }

            temp = next;
        }

        // Обрабатываем текущего наблюдающего
        if (currentSpy != nullptr) {
            currentSpy->currentObservationTime++;

            // Проверяем, завершил ли шпион наблюдение
            if (currentSpy->currentObservationTime >= currentSpy->observationTime) {
                if (remainingSpies > 1) {
                    cout << "Момент времени " << currentTime << ": " << currentSpy->name << " окончил наблюдение." << endl;
                }

                // Проверяем, может ли шпион продолжать
                if (currentSpy->totalTimeSpent < currentSpy->maxQueueTime || currentSpy->maxQueueTime == -1) {
                    // Может продолжать -> идет в конец очереди
                    currentSpy->currentObservationTime = 0;
                    currentSpy->isObserving = false;
                    enqueue(q, currentSpy);
                } else {
                    // Превысил время -> уходит
                    cout << "Момент времени " << currentTime << ": " << currentSpy->name << " отправился к резиденту (закончил наблюдение)." << endl;
                    delete currentSpy;
                    remainingSpies--;
                }

                currentSpy = nullptr;
                isChanged = true;
            }

            // Проверяем, не превысил ли текущий наблюдающий шпион время ожидания во время наблюдения
            if (currentSpy != nullptr && currentSpy->totalTimeSpent >= currentSpy->maxQueueTime && currentSpy->maxQueueTime != -1) {
                cout << "Момент времени " << currentTime << ": " << currentSpy->name << " отправился к резиденту (превышено время ожидания во время наблюдения)." << endl;
                delete currentSpy;
                currentSpy = nullptr;
                remainingSpies--;
                isChanged = true;
            }
        }

        // Если бинокль свободен - берем следующего из очереди
        if (currentSpy == nullptr && !isEmpty(q)) {
            currentSpy = dequeue(q);
            currentSpy->currentObservationTime = 0;
            currentSpy->isObserving = true;
            if (remainingSpies > 1) {
                cout << "Момент времени " << currentTime << ": " << currentSpy->name << " приступил к наблюдению." << endl;
            }
            isChanged = true;
        }

        // Вывод состояния
        if (isChanged && remainingSpies > 1) {
            cout << "Текущий наблюдатель: " << (currentSpy ? currentSpy->name : "нет") << endl;
            cout << "Очередь: ";
            temp = q.front;
            while (temp != nullptr) {
                cout << temp->name << "(" << temp->totalTimeSpent << "/" << temp->maxQueueTime << ") ";
                temp = temp->next;
            }
            cout << endl;
            cout << "Осталось шпионов: " << remainingSpies << endl;
            cout << "" << endl;
        }

        if (currentSpy != nullptr) { // счетчик для выбранного
            currentSpy->totalTimeSpent++;
        }

        temp = q.front;
        while (temp != nullptr) { // для остальных
            temp->totalTimeSpent++;
            temp = temp->next;
        }
        currentTime++;
    }

    cout << "\n=== Все шпионы завершили работу ===" << endl;
}

void clearQueue(Queue& q) {
    while (!isEmpty(q)) {
        Spy* spy = dequeue(q);
        delete spy;
    }
}

int main() {
    string filename;
    cout << "Введите имя файла: ";
    cin >> filename;

    int spyCount = 0;
    Spy* spies = readSpiesFromFile(filename, spyCount);
    if (spies == nullptr || spyCount == 0) {
        cout << "Нет данных о шпионах или ошибка чтения файла." << endl;
        return 1;
    }

    Queue q;
    initQueue(q);

    // Добавляем всех шпионов в очередь
    Spy* current = spies;
    while (current != nullptr) {
        Spy* next = current->next;
        enqueue(q, current);
        current = next;
    }

    simulateObservation(q, spyCount);
    clearQueue(q);

    return 0;
}