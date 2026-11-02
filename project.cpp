
#include <bits/stdc++.h>
using namespace std;

// Defining the task structure
struct Task {
    int id;
    string name;
    string deadline;
    int priority;
    int arrival;
    string status;
};

// Date function
string getTodayDate() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);

    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", local);

    return string(buffer);
}

bool isLeapYear(int year) {
    return (year % 400 == 0) ||
           (year % 4 == 0 && year % 100 != 0);
}

bool isValidDate(string date) {

    if (date.length() != 10 ||
        date[4] != '-' || date[7] != '-') {
        return false;
    }

    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && !isdigit(date[i]))
            return false;
    }

    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    if (year < 1900 || month < 1 || month > 12)
        return false;

    int days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (isLeapYear(year))
        days[1] = 29;

    return day >= 1 && day <= days[month - 1];
}

// 3. Min Heap - Priority Queue
struct Compare {
    bool operator()(const Task& a, const Task& b) const {

        // Earliest deadline first
        if (a.deadline != b.deadline)
            return a.deadline > b.deadline;

        // Higher priority first
        if (a.priority != b.priority)
            return a.priority < b.priority;

        // Earlier arrival first
        return a.arrival > b.arrival;
    }
};

// Binary Search Tree
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

// Priority Queue / Min Heap
priority_queue<Task, vector<Task>, Compare> minHeap;

// Queue: Incoming tasks
queue<int> incoming;

// Hash Table: Task ID lookup
unordered_map<int, Task> taskTable;

// Vector: All task IDs
vector<int> allTasks;

// Map: Deadline-wise grouping
map<string, vector<int>> deadlineMap;

// Stack: Completion history
stack<int> completedStack;

// Linked List: Completed tasks
list<int> completedList;

// BST root
Node* root = nullptr;

int arrivalCounter = 0;

// Defining Helper functions
void printTask(const Task& t) {

    cout << "\nTask ID   : " << t.id;
    cout << "\nTask Name : " << t.name;
    cout << "\nDeadline  : " << t.deadline;
    cout << "\nPriority  : " << t.priority;
    cout << "\nStatus    : " << t.status << "\n";
}

bool taskExists(int id) {
    return taskTable.find(id) != taskTable.end();
}

bool isActive(int id) {
    return taskExists(id) &&
           taskTable.at(id).status != "Deleted";
}

// Remove outdated entries from heap
void cleanHeap() {

    while (!minHeap.empty()) {

        int id = minHeap.top().id;

        if (!taskExists(id) ||
            taskTable.at(id).status != "Pending") {
            minHeap.pop();
        }
        else {
            break;
        }
    }
}

// Function to display dashboard
void displayDashboard() {

    int total = 0;
    int pending = 0;
    int completed = 0;
    int overdue = 0;

    string today = getTodayDate();

    for (int id : allTasks) {

        if (!isActive(id))
            continue;

        Task t = taskTable.at(id);

        total++;

        if (t.status == "Completed") {
            completed++;
        }
        else if (t.status == "Pending" ||
                 t.status == "Incoming") {

            pending++;

            if (t.deadline < today)
                overdue++;
        }
    }

    cout << "\n";
    cout << "========================================\n";
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

// Function to add task
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

    if (t.name.empty()) {
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

    // Hash Table
    taskTable[t.id] = t;

    // Vector
    allTasks.push_back(t.id);

    // Queue
    incoming.push(t.id);

    // Map
    deadlineMap[t.deadline].push_back(t.id);

    // BST
    root = insertBST(root, t.deadline, t.id);

    cout << "\nTask added successfully!\n";
    cout << "Task is waiting in incoming queue.\n";

    displayDashboard();
}

// Scheduling incoming tasks
void scheduleTasks() {

    if (incoming.empty()) {
        cout << "\nNo incoming tasks to schedule!\n";
        return;
    }

    int count = 0;

    cout << "\n--- SCHEDULING TASKS ---\n";

    while (!incoming.empty()) {

        int id = incoming.front();
        incoming.pop();

        if (!isActive(id))
            continue;

        if (taskTable[id].status != "Incoming")
            continue;

        // Change status
        taskTable[id].status = "Pending";

        // Insert into Min Heap
        minHeap.push(taskTable[id]);

        cout << "Scheduled: " << taskTable[id].name
             << " | Deadline: " << taskTable[id].deadline
             << "\n";

        count++;
    }

    cout << "\n" << count
         << " task(s) scheduled successfully!\n";

    displayDashboard();
}

// Function to mark tasks as completed
void markTaskCompleted() {

    int id;

    cout << "\nEnter Task ID to mark completed: ";
    cin >> id;

    if (!taskExists(id)) {
        cout << "Task not found!\n";
        return;
    }

    Task& t = taskTable[id];

    if (t.status == "Deleted") {
        cout << "This task has been deleted!\n";
        return;
    }

    if (t.status == "Completed") {
        cout << "Task is already completed!\n";
        return;
    }

    // Change status
    t.status = "Completed";

    // Stack
    completedStack.push(id);

    // Linked List
    completedList.push_back(id);

    // Old Queue / Heap entries are skipped by status checks

    cout << "\nTask marked as completed successfully!\n";
    printTask(t);

    displayDashboard();
}

// Function to display next recommended task
void displayNextTask() {

    cleanHeap();

    if (minHeap.empty()) {
        cout << "\nNo scheduled pending tasks!\n";
        cout << "Add tasks and select option 2 first.\n";
        return;
    }

    Task t = minHeap.top();

    cout << "\n--- NEXT RECOMMENDED TASK ---\n";

    printTask(t);
}

// Function to display all tasks
void displayAllTasks() {

    bool found = false;

    cout << "\n========== ALL ACTIVE TASKS ==========\n";

    for (int id : allTasks) {

        if (!isActive(id))
            continue;

        printTask(taskTable.at(id));
        found = true;
    }

    if (!found)
        cout << "No active tasks available!\n";
}

// Search task by ID
void searchTask() {

    int id;

    cout << "\nEnter Task ID: ";
    cin >> id;

    if (!isActive(id)) {
        cout << "Task not found!\n";
        return;
    }

    cout << "\nTask found successfully!\n";

    printTask(taskTable.at(id));
}

// Group tasks by deadline
void displayDeadlineGroups() {

    bool found = false;

    cout << "\n========== TASKS GROUPED BY DEADLINE ==========\n";

    for (auto& entry : deadlineMap) {

        bool groupFound = false;

        for (int id : entry.second) {

            if (isActive(id)) {
                groupFound = true;
                break;
            }
        }

        if (!groupFound)
            continue;

        cout << "\nDeadline: " << entry.first << "\n";

        for (int id : entry.second) {

            if (!isActive(id))
                continue;

            Task t = taskTable.at(id);

            cout << "ID: " << id
                 << " | Name: " << t.name
                 << " | Priority: " << t.priority
                 << " | Status: " << t.status << "\n";

            found = true;
        }
    }

    if (!found)
        cout << "No active tasks available!\n";
}

// Search deadline using BST
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

    for (int id : result->taskIDs) {

        if (!isActive(id))
            continue;

        printTask(taskTable.at(id));
        found = true;
    }

    if (!found)
        cout << "No active tasks for this deadline!\n";
}

// Function to undo submission
void undoSubmission() {

    int id;

    cout << "\nEnter Task ID to undo submission: ";
    cin >> id;

    if (!taskExists(id)) {
        cout << "Task not found!\n";
        return;
    }

    Task& t = taskTable[id];

    if (t.status != "Completed") {
        cout << "Only completed tasks can be restored!\n";
        return;
    }

    string today = getTodayDate();

    if (today > t.deadline) {
        cout << "Cannot undo submission!\n";
        cout << "Deadline has already passed.\n";
        return;
    }

    // Change status
    t.status = "Pending";

    // Remove from completed Linked List
    completedList.remove(id);

    // Add back to Min Heap
    minHeap.push(t);

    cout << "\nSubmission undone successfully!\n";
    cout << "Task is Pending again.\n";

    displayDashboard();
}

// Function to display completed tasks
void displayCompleted() {

    if (completedList.empty()) {
        cout << "\nNo completed tasks!\n";
        return;
    }

    cout << "\n========== COMPLETED TASKS ==========\n";

    for (int id : completedList) {

        if (taskExists(id) &&
            taskTable.at(id).status == "Completed") {

            printTask(taskTable.at(id));
        }
    }
}

// Recent completion history
void displayRecentCompletion() {

    if (completedStack.empty()) {
        cout << "\nNo completion history available!\n";
        return;
    }

    stack<int> temp = completedStack;

    cout << "\n========== RECENT COMPLETION HISTORY ==========\n";

    bool found = false;

    while (!temp.empty()) {

        int id = temp.top();
        temp.pop();

        // Ignore tasks that were reopened or deleted
        if (!taskExists(id) ||
            taskTable.at(id).status != "Completed") {
            continue;
        }

        printTask(taskTable.at(id));
        found = true;
    }

    if (!found)
        cout << "No currently completed tasks in history!\n";
}

// Function to delete task
void deleteTask() {

    int id;

    cout << "\nEnter Task ID to delete: ";
    cin >> id;

    if (!taskExists(id)) {
        cout << "Task not found!\n";
        return;
    }

    Task& t = taskTable[id];

    if (t.status == "Deleted") {
        cout << "Task is already deleted!\n";
        return;
    }

    if (t.status == "Completed") {
        cout << "Completed tasks cannot be deleted!\n";
        return;
    }

    t.status = "Deleted";

    cout << "\nTask deleted successfully!\n";

    displayDashboard();
}

int main() {

    int choice;

    cout << "\n========================================\n";
    cout << "       DEADLINE-BASED TASK SCHEDULER    \n";
    cout << "========================================\n";

    displayDashboard();

    do {

        cout << "\n============== MAIN MENU ==============\n";

        cout << "1.  Add New Task\n";
        cout << "2.  Schedule Incoming Tasks\n";
        cout << "3.  Mark Task as Completed\n";
        cout << "4.  Display Next Recommended Task\n";
        cout << "5.  Display All Tasks\n";
        cout << "6.  Search Task by ID\n";
        cout << "7.  Group Tasks by Deadline\n";
        cout << "8.  Search Deadline using BST\n";
        cout << "9.  Undo Submission by Task ID\n";
        cout << "10. Display Completed Tasks\n";
        cout << "11. Display Recent Completion History\n";
        cout << "12. Delete Task\n";
        cout << "13. Display Dashboard\n";
        cout << "0.  Exit\n";

        cout << "========================================\n";

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {

            case 1:
                addTask();
                break;

            case 2:
                scheduleTasks();
                break;

            case 3:
                markTaskCompleted();
                break;

            case 4:
                displayNextTask();
                break;

            case 5:
                displayAllTasks();
                break;

            case 6:
                searchTask();
                break;

            case 7:
                displayDeadlineGroups();
                break;

            case 8:
                searchDeadline();
                break;

            case 9:
                undoSubmission();
                break;

            case 10:
                displayCompleted();
                break;

            case 11:
                displayRecentCompletion();
                break;

            case 12:
                deleteTask();
                break;

            case 13:
                displayDashboard();
                break;

            case 0:
                cout << "\nThank you for using Task Scheduler!\n";
                break;

            default:
                cout << "\nInvalid choice! Please try again.\n";
        }

    } while (choice != 0);

    // Free BST memory
    deleteBST(root);

    return 0;
}
