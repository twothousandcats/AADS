#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Spy {
    string name;
    int observationDuration; // врем с биноклем
    int maxQueueDuration; // максимальное время в очереди
    int currentQueueDuration; // текущее время в очереди
    int startedAt; // начало наблюдения (-1 - ждет)
    Spy *next;
};

void addSpy(Spy*& head, Spy*& tail, const string& name, int obsTime, int maxTime) {
    Spy* newSpy = new Spy;
    newSpy->name = name;
    newSpy->observationDuration = obsTime;
    newSpy->maxQueueDuration = maxTime;
    newSpy->currentQueueDuration = 0;
    newSpy->startedAt = -1;
    newSpy->next = nullptr;

    if (tail == nullptr) {
        head = tail = newSpy;
    } else {
        tail->next = newSpy;
        tail = newSpy;
    }
}

void readFileData(Spy*& head, Spy*& tail, const string& filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла: " << filename << endl;

        exit(1);
    }

    string name;
    int observationTime, maxTime;

    while (file >> name >> observationTime >> maxTime) {
        addSpy(head, tail, name, observationTime, maxTime);
    }

    file.close();
}

void freeMemory(Spy*& head) {
    while (head != nullptr) {
        Spy* temp = head;
        head = head->next;
        delete temp;
    }
}

void removeSpy(Spy*& head, Spy*& tail, Spy* prev, Spy* current) {
    if (prev == nullptr) {
        head = current->next;
        if (head == nullptr) {
            tail = nullptr;
        }
    } else {
        prev->next = current->next;
        if (current == tail) {
            tail = prev;
        }
    }

    delete current;
}

// основа
void simulation(Spy* head, Spy* tail) {
    int currentTime = 0;
    Spy* curObs = nullptr;
    int remainingTime = 0;

    while (head != nullptr) {
        currentTime++;

        Spy* current = head;
        Spy* prev = nullptr;

        while (current != nullptr) {
            current->currentQueueDuration++;

            // проверка истечения времени ожидания
            if (current->currentQueueDuration > current->maxQueueDuration) {
                cout << "Момент времени " << currentTime << ": " << current->name << " уходит к резиденту (время в очереди закончилось)" << endl;

                if (current == curObs) {
                    cout << "Момент времени " << currentTime << ": " << current->name << " прервал наблюдение" << endl;
                    curObs = nullptr;
                    remainingTime = 0;
                }

                // удаляем шпиона из очереди
                Spy* spyToRemove = current;
                current = current->next;
                removeSpy(head, tail, prev, spyToRemove);
                continue;
            }

            prev = current;
            current = current->next;
        }
        // Если нет наблюдателя, берем следующего
        if (curObs ==  nullptr && head != nullptr) {
            curObs = head;
            remainingTime = curObs->observationDuration;
            curObs->startedAt = currentTime;
            cout << "Момент времени " << currentTime << ": " << curObs->name << " начал наблюдение" << endl;
        }
        // если есть наблюдатель, уменьшаем время наблюдения
        if (curObs != nullptr) {
            remainingTime--;

            if (remainingTime == 0) {
                cout << "Момент времени " << currentTime << ": " << curObs->name << " окончил наблюдение" << endl;

                // ставим наблюдателя в конец
                if (head == curObs) {
                    head = head->next;
                    if (head == nullptr) {
                        tail = nullptr;
                    }
                } else {
                    Spy* prev = head;
                    while (prev->next != curObs) {
                        prev = prev->next;
                    }
                    prev->next = curObs->next;
                }

                if (tail != nullptr) {
                    tail->next = curObs;
                    tail = curObs;
                } else {
                    head = tail = curObs;
                }

                curObs->next = nullptr;
                curObs->currentQueueDuration = 0;
                curObs->startedAt = -1;
                curObs = nullptr;
            }
        }

        if (head == nullptr) {
            cout << "Момент времени " << currentTime << ": все шпионы ушли к резиденту" << endl;
            break;
        }
    }
}

int main() {
    Spy* head = nullptr;
    Spy* tail = nullptr;

    cout << "Лабораторная работа: Моделирование наблюдения шпионов." << endl;

    string filename;
    cout << "Введи название имя файла с данными на латинице: ";
    cin >> filename;

    readFileData(head, tail, filename);

    if (head == nullptr) {
        cout << "Шпионы не найдены!" << endl;
        return 0;
    }

    cout << "--НАЧАЛО СИМУЛЯЦИИ--" << endl;
    simulation(head, tail);
    cout << "--КОНЕЦ СИМУЛЯЦИИ--" << endl;

    freeMemory(head);

    return 0;
}
