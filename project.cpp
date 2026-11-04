#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <cctype>
#include <limits>

using namespace std;

struct Task {
    int id;
    string name;
    string deadline;
    int priority;
    int arrival;
    string status;
};


string getTodayDate() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);

    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", local);
    return string(buffer);
}

int toInt(const string& s, int start, int length) {
    int value = 0;
    for (int i = start; i < start + length; i++)
        value = value * 10 + (s[i] - '0');
    return value;
}

bool isLeapYear(int year) {
    return (year % 400 == 0) ||
           (year % 4 == 0 && year % 100 != 0);
}

bool isValidDate(const string& date) {
    if (date.length() != 10 || date[4] != '-' || date[7] != '-')
        return false;

    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && !isdigit((unsigned char)date[i]))
            return false;
    }

    int year = toInt(date, 0, 4);
    int month = toInt(date, 5, 2);
    int day = toInt(date, 8, 2);

    if (year < 1900 || month < 1 || month > 12)
        return false;

    int days[] = {31, 28, 31, 30, 31, 30,
                  31, 31, 30, 31, 30, 31};

    if (isLeapYear(year))
        days[1] = 29;

    return day >= 1 && day <= days[month - 1];
}

class TaskHashTable {
    struct HashNode {
        int key;
        Task value;
        HashNode* next;

        HashNode(int k, const Task& t) {
            key = k;
            value = t;
            next = nullptr;
        }
    };

    static const int SIZE = 1009;
    HashNode* buckets[SIZE];

    int hashFunction(int key) const {
        int h = key % SIZE;
        if (h < 0) h += SIZE;
        return h;
    }

public:
    TaskHashTable() {
        for (int i = 0; i < SIZE; i++)
            buckets[i] = nullptr;
    }

    bool contains(int key) const {
        int index = hashFunction(key);
        HashNode* current = buckets[index];

        while (current != nullptr) {
            if (current->key == key)
                return true;
            current = current->next;
        }
        return false;
    }

    Task* get(int key) {
        int index = hashFunction(key);
        HashNode* current = buckets[index];

        while (current != nullptr) {
            if (current->key == key)
                return &current->value;
            current = current->next;
        }
        return nullptr;
    }

    const Task* get(int key) const {
        int index = hashFunction(key);
        HashNode* current = buckets[index];

        while (current != nullptr) {
            if (current->key == key)
                return &current->value;
            current = current->next;
        }
        return nullptr;
    }

    void insert(int key, const Task& value) {
        int index = hashFunction(key);
        HashNode* current = buckets[index];

        while (current != nullptr) {
            if (current->key == key) {
                current->value = value;
                return;
            }
            current = current->next;
        }

        HashNode* node = new HashNode(key, value);
        node->next = buckets[index];
        buckets[index] = node;
    }

    ~TaskHashTable() {
        for (int i = 0; i < SIZE; i++) {
            HashNode* current = buckets[i];
            while (current != nullptr) {
                HashNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }
};


class TaskQueue {
    vector<int> data;
    int frontIndex;

public:
    TaskQueue() {
        frontIndex = 0;
    }

    bool empty() const {
        return frontIndex >= (int)data.size();
    }

    void push(int value) {
        data.push_back(value);
    }

    int front() const {
        return data[frontIndex];
    }

    void pop() {
        if (!empty())
            frontIndex++;

        if (frontIndex > 50 && frontIndex * 2 > (int)data.size()) {
            vector<int> remaining;
            for (int i = frontIndex; i < (int)data.size(); i++)
                remaining.push_back(data[i]);
            data = remaining;
            frontIndex = 0;
        }
    }
};

//Stack
class IntStack {
    vector<int> data;

public:
    bool empty() const {
        return data.size() == 0;
    }

    void push(int value) {
        data.push_back(value);
    }

    int top() const {
        return data[data.size() - 1];
    }

    void pop() {
        if (!empty())
            data.pop_back();
    }

    int size() const {
        return (int)data.size();
    }

    int getFromTop(int position) const {
        return data[data.size() - 1 - position];
    }
};


class CompletedLinkedList {
    struct ListNode {
        int id;
        ListNode* next;

        ListNode(int value) {
            id = value;
            next = nullptr;
        }
    };

    ListNode* head;
    ListNode* tail;

public:
    CompletedLinkedList() {
        head = tail = nullptr;
    }

    bool empty() const {
        return head == nullptr;
    }

    void push_back(int id) {
        ListNode* node = new ListNode(id);

        if (head == nullptr)
            head = tail = node;
        else {
            tail->next = node;
            tail = node;
        }
    }

    void remove(int id) {
        ListNode* current = head;
        ListNode* previous = nullptr;

        while (current != nullptr) {
            if (current->id == id) {
                if (previous == nullptr)
                    head = current->next;
                else
                    previous->next = current->next;

                if (tail == current)
                    tail = previous;

                delete current;
                return;
            }

            previous = current;
            current = current->next;
        }
    }

    void display(const TaskHashTable& table);

    ~CompletedLinkedList() {
        ListNode* current = head;
        while (current != nullptr) {
            ListNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    ListNode* getHead() const {
        return head;
    }
};

void CompletedLinkedList::display(const TaskHashTable& table) {
    ListNode* current = head;
    bool found = false;

    while (current != nullptr) {
        const Task* task = table.get(current->id);
        if (task != nullptr && task->status == "Completed") {
            cout << "\nTask ID   : " << task->id;
            cout << "\nTask Name : " << task->name;
            cout << "\nDeadline  : " << task->deadline;
            cout << "\nPriority  : " << task->priority;
            cout << "\nStatus    : " << task->status << "\n";
            found = true;
        }
        current = current->next;
    }

    if (!found)
        cout << "No completed tasks!\n";
}

// Min heap
bool comesBefore(const Task& a, const Task& b) {
    // Earliest deadline first.
    if (a.deadline != b.deadline)
        return a.deadline < b.deadline;

    // Higher priority first (1 is highest).
    if (a.priority != b.priority)
        return a.priority < b.priority;

    // Earlier arrival first.
    return a.arrival < b.arrival;
}

class MinHeap {
    vector<Task> heap;

    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (comesBefore(heap[parent], heap[index]))
                break;

            Task temp = heap[parent];
            heap[parent] = heap[index];
            heap[index] = temp;

            index = parent;
        }
    }

    void siftDown(int index) {
        int size = (int)heap.size();

        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;

            if (left < size && comesBefore(heap[left], heap[smallest]))
                smallest = left;

            if (right < size && comesBefore(heap[right], heap[smallest]))
                smallest = right;

            if (smallest == index)
                break;

            Task temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;

            index = smallest;
        }
    }

public:
    bool empty() const {
        return heap.size() == 0;
    }

    void push(const Task& task) {
        heap.push_back(task);
        siftUp((int)heap.size() - 1);
    }

    Task top() const {
        return heap[0];
    }

    void pop() {
        if (empty())
            return;

        heap[0] = heap[heap.size() - 1];
        heap.pop_back();

        if (!empty())
            siftDown(0);
    }
};

struct Node {
    string deadline;
    vector<int> taskIDs;
    Node* left;
    Node* right;

    Node(string d, int id) {
        deadline = d;
        taskIDs.push_back(id);
        left = right = nullptr;
    }
};

Node* insertBST(Node* root, string deadline, int id) {
    if (root == nullptr)
        return new Node(deadline, id);

    if (deadline < root->deadline)
        root->left = insertBST(root->left, deadline, id);
    else if (deadline > root->deadline)
        root->right = insertBST(root->right, deadline, id);
    else
        root->taskIDs.push_back(id);

    return root;
}

Node* searchBST(Node* root, string deadline) {
    if (root == nullptr || root->deadline == deadline)
        return root;

    if (deadline < root->deadline)
        return searchBST(root->left, deadline);

    return searchBST(root->right, deadline);
}

void deleteBST(Node* root) {
    if (root == nullptr)
        return;

    deleteBST(root->left);
    deleteBST(root->right);
    delete root;
}

MinHeap minHeap;
TaskQueue incoming;
TaskHashTable taskTable;
vector<int> allTasks;
map<string, vector<int> > deadlineMap;
IntStack completedStack;
CompletedLinkedList completedList;
Node* root = nullptr;
int arrivalCounter = 0;

bool taskExists(int id) {
    return taskTable.contains(id);
}

bool isActive(int id) {
    Task* task = taskTable.get(id);
    return task != nullptr && task->status != "Deleted";
}

void printTask(const Task& t) {
    cout << "\nTask ID   : " << t.id;
    cout << "\nTask Name : " << t.name;
    cout << "\nDeadline  : " << t.deadline;
    cout << "\nPriority  : " << t.priority;
    cout << "\nStatus    : " << t.status << "\n";
}

void cleanHeap() {
    while (!minHeap.empty()) {
        int id = minHeap.top().id;
        Task* task = taskTable.get(id);

        if (task == nullptr || task->status != "Pending")
            minHeap.pop();
        else
            break;
    }
}

void displayDashboard() {
    int total = 0, pending = 0, completed = 0, overdue = 0;
    string today = getTodayDate();

    for (int i = 0; i < (int)allTasks.size(); i++) {
        int id = allTasks[i];
        Task* task = taskTable.get(id);

        if (task == nullptr || task->status == "Deleted")
            continue;

        total++;

        if (task->status == "Completed")
            completed++;
        else if (task->status == "Pending" || task->status == "Incoming") {
            pending++;
            if (task->deadline < today)
                overdue++;
        }
    }

    cout << "\n========================================\n";
    cout << "          TASK SCHEDULER DASHBOARD      \n";
    cout << "========================================\n";
    cout << "Today's Date       : " << today << "\n";
    cout << "----------------------------------------\n";
    cout << "Total Tasks        : " << total << "\n";
    cout << "Pending Tasks      : " << pending << "\n";
    cout << "Completed Tasks    : " << completed << "\n";
    cout << "Overdue Tasks      : " << overdue << "\n";
    cout << "----------------------------------------\n";
}

// -------------------- ADD TASK --------------------
void addTask() {
    Task t;

    cout << "\nEnter Task ID: ";
    cin >> t.id;

    if (taskExists(t.id)) {
        cout << "Task ID already exists!\n";
        return;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Task Name: ";
    getline(cin, t.name);

    if (t.name.length() == 0) {
        cout << "Task name cannot be empty!\n";
        return;
    }

    cout << "Enter Deadline (YYYY-MM-DD): ";
    cin >> t.deadline;

    if (!isValidDate(t.deadline)) {
        cout << "Invalid calendar date!\n";
        return;
    }

    cout << "Enter Priority (1-5, 1 = Highest): ";
    cin >> t.priority;

    if (t.priority < 1 || t.priority > 5) {
        cout << "Priority must be between 1 and 5!\n";
        return;
    }

    t.arrival = ++arrivalCounter;
    t.status = "Incoming";

    taskTable.insert(t.id, t);
    allTasks.push_back(t.id);
    incoming.push(t.id);
    deadlineMap[t.deadline].push_back(t.id);
    root = insertBST(root, t.deadline, t.id);

    cout << "\nTask added successfully!\n";
    displayDashboard();
}


void scheduleTasks() {
    while (!incoming.empty()) {
        int id = incoming.front();
        incoming.pop();

        Task* task = taskTable.get(id);

        if (task == nullptr || task->status != "Incoming")
            continue;

        task->status = "Pending";
        minHeap.push(*task);
    }
}


void markTaskCompleted() {
    int id;

    cout << "\nEnter Task ID to mark completed: ";
    cin >> id;

    Task* task = taskTable.get(id);

    if (task == nullptr) {
        cout << "Task not found!\n";
        return;
    }

    if (task->status == "Deleted") {
        cout << "This task has been deleted!\n";
        return;
    }

    if (task->status == "Completed") {
        cout << "Task is already completed!\n";
        return;
    }

    task->status = "Completed";
    completedStack.push(id);
    completedList.push_back(id);

    cout << "\nTask marked as completed successfully!\n";
    printTask(*task);
    displayDashboard();
}

void displayNextTask() {
    cleanHeap();

    if (minHeap.empty()) {
        cout << "\nNo scheduled pending tasks!\n";
        cout << "Add tasks first.\n";
        return;
    }

    cout << "\n--- NEXT RECOMMENDED TASK ---\n";
    printTask(minHeap.top());
}

void displayAllTasks() {
    bool found = false;
    cout << "\n========== ALL ACTIVE TASKS ==========\n";

    for (int i = 0; i < (int)allTasks.size(); i++) {
        int id = allTasks[i];
        Task* task = taskTable.get(id);

        if (task == nullptr || task->status == "Deleted")
            continue;

        printTask(*task);
        found = true;
    }

    if (!found)
        cout << "No active tasks available!\n";
}

void searchTask() {
    int id;
    cout << "\nEnter Task ID: ";
    cin >> id;

    Task* task = taskTable.get(id);

    if (task == nullptr || task->status == "Deleted") {
        cout << "Task not found!\n";
        return;
    }

    cout << "\nTask found successfully!\n";
    printTask(*task);
}

void displayDeadlineGroups() {
    bool found = false;
    cout << "\n========== TASKS GROUPED BY DEADLINE ==========\n";

    for (map<string, vector<int> >::iterator entry = deadlineMap.begin();
         entry != deadlineMap.end(); ++entry) {

        bool groupFound = false;

        for (int i = 0; i < (int)entry->second.size(); i++) {
            int id = entry->second[i];
            if (isActive(id)) {
                groupFound = true;
                break;
            }
        }

        if (!groupFound)
            continue;

        cout << "\nDeadline: " << entry->first << "\n";

        for (int i = 0; i < (int)entry->second.size(); i++) {
            int id = entry->second[i];
            Task* task = taskTable.get(id);

            if (task == nullptr || task->status == "Deleted")
                continue;

            cout << "ID: " << id
                 << " | Name: " << task->name
                 << " | Priority: " << task->priority
                 << " | Status: " << task->status << "\n";
            found = true;
        }
    }

    if (!found)
        cout << "No active tasks available!\n";
}

void searchDeadline() {
    string date;
    cout << "\nEnter Deadline (YYYY-MM-DD): ";
    cin >> date;

    if (!isValidDate(date)) {
        cout << "Invalid date!\n";
        return;
    }

    Node* result = searchBST(root, date);

    if (result == nullptr) {
        cout << "No tasks found for this deadline!\n";
        return;
    }

    bool found = false;
    cout << "\n--- TASKS FOR " << date << " ---\n";

    for (int i = 0; i < (int)result->taskIDs.size(); i++) {
        int id = result->taskIDs[i];
        Task* task = taskTable.get(id);

        if (task == nullptr || task->status == "Deleted")
            continue;

        printTask(*task);
        found = true;
    }

    if (!found)
        cout << "No active tasks for this deadline!\n";
}

// -------------------- UNDO SUBMISSION --------------------
void undoSubmission() {
    int id;
    cout << "\nEnter Task ID to undo submission: ";
    cin >> id;

    Task* task = taskTable.get(id);

    if (task == nullptr) {
        cout << "Task not found!\n";
        return;
    }

    if (task->status != "Completed") {
        cout << "Only completed tasks can be restored!\n";
        return;
    }

    string today = getTodayDate();

    if (today > task->deadline) {
        cout << "Cannot undo submission!\n";
        cout << "Deadline has already passed.\n";
        return;
    }

    task->status = "Pending";
    completedList.remove(id);
    minHeap.push(*task);

    cout << "\nSubmission undone successfully!\n";
    cout << "Task is Pending again.\n";
    displayDashboard();
}

void displayCompleted() {
    if (completedList.empty()) {
        cout << "\nNo completed tasks!\n";
        return;
    }

    cout << "\n========== COMPLETED TASKS ==========\n";
    completedList.display(taskTable);
}

void displayRecentCompletion() {
    if (completedStack.empty()) {
        cout << "\nNo completion history available!\n";
        return;
    }

    cout << "\n========== RECENT COMPLETION HISTORY ==========\n";
    bool found = false;

    for (int i = 0; i < completedStack.size(); i++) {
        int id = completedStack.getFromTop(i);
        Task* task = taskTable.get(id);

        // Ignore tasks that were reopened or deleted.
        if (task == nullptr || task->status != "Completed")
            continue;

        printTask(*task);
        found = true;
    }

    if (!found)
        cout << "No currently completed tasks in history!\n";
}

// -------------------- DELETE TASK --------------------
void deleteTask() {
    int id;
    cout << "\nEnter Task ID to delete: ";
    cin >> id;

    Task* task = taskTable.get(id);

    if (task == nullptr) {
        cout << "Task not found!\n";
        return;
    }

    if (task->status == "Deleted") {
        cout << "Task is already deleted!\n";
        return;
    }

    if (task->status == "Completed") {
        cout << "Completed tasks cannot be deleted!\n";
        return;
    }

    task->status = "Deleted";
    cout << "\nTask deleted successfully!\n";
    displayDashboard();
}

// -------------------- MAIN FUNCTION --------------------
int main() {
    int choice;

    cout << "\n========================================\n";
    cout << "       PRIORITY TASK SCHEDULER\n";
    cout << "========================================\n";

    displayDashboard();

    do {
        cout << "\n============== MAIN MENU ==============\n";
        cout << "1.  Add New Task\n";
        cout << "2.  Mark Task as Completed\n";
        cout << "3.  Display Next Recommended Task\n";
        cout << "4.  Display All Tasks\n";
        cout << "5.  Search Task by ID\n";
        cout << "6.  Group Tasks by Deadline\n";
        cout << "7.  Search Deadline\n";
        cout << "8.  Undo Submission by Task ID\n";
        cout << "9.  Display Completed Tasks\n";
        cout << "10. Display Recent Completion History\n";
        cout << "11. Delete Task\n";
        cout << "12. Display Dashboard\n";
        cout << "0.  Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addTask();
                scheduleTasks();
                break;
            case 2:
                markTaskCompleted();
                break;
            case 3:
                displayNextTask();
                break;
            case 4:
                displayAllTasks();
                break;
            case 5:
                searchTask();
                break;
            case 6:
                displayDeadlineGroups();
                break;
            case 7:
                searchDeadline();
                break;
            case 8:
                undoSubmission();
                break;
            case 9:
                displayCompleted();
                break;
            case 10:
                displayRecentCompletion();
                break;
            case 11:
                deleteTask();
                break;
            case 12:
                displayDashboard();
                break;
            case 0:
                cout << "\nThank you for using Task Scheduler!\n";
                break;
            default:
                cout << "\nInvalid choice! Please try again.\n";
        }
    } while (choice != 0);

    deleteBST(root);
    return 0;
}
