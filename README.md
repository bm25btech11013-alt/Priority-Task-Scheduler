The project uses different Data Structures and Algorithms (DSA) concepts to handle task scheduling, searching, task management and completion tracking.

Features:
Add tasks with unique IDs, names, deadlines and priorities.
Recommend the next task based on deadline and priority.
Search tasks using Task ID or deadline.
Group tasks according to their deadlines.
Mark tasks as completed and maintain completion history.
Undo completed tasks before their deadlines.
Delete pending tasks.
View a dashboard showing total, pending, completed and overdue tasks.

1.Min Heap — Task Scheduling
Concept: A Min Heap is a tree-based data structure that gives quick access to the element with the highest precedence according to its ordering.
Our implementation: We use a Min Heap to select the next task based on three conditions:
Earliest deadline comes first.
If deadlines match, the task with higher priority comes first (1 = highest).
If both match, the task added earlier is selected.
We implemented this using C++ priority_queue and a custom comparator.
Time complexity: Insertion – O(log n) | Getting the next task – O(1) | Removal – O(log n)

2. Binary Search Tree — Deadline Search
Concept: A BST stores elements in an ordered tree, allowing us to search by comparing the required value with each node.
Our implementation: Each BST node represents a deadline and contains the IDs of tasks assigned to that date. When searching for a deadline, we compare it with the current node and move either left or right.
This is handled by insertBST() and searchBST().
Time complexity: Average – O(log d) | Worst case – O(d)

3. Hash Table — Task ID Management
Concept: A Hash Table stores information in key-value pairs and provides fast access using a key.
Our implementation: We use unordered_map to connect each unique Task ID with its complete task details. Whenever we need to find or update a task, we use its ID instead of searching through all the tasks.
Time complexity: Average insertion and search – O(1) | Worst case – O(n)

4. Queue — Incoming Tasks
Concept: A Queue follows FIFO (First In, First Out), meaning the task that enters first leaves first.
Our implementation: Newly added tasks are placed in the incoming queue. The scheduleTasks() function takes them one by one and transfers them to the Min Heap. This maintains the arrival order during the initial scheduling process.
Time complexity: Enqueue – O(1) | Dequeue – O(1)

5.Stack — Completion History
Concept: A Stack follows LIFO (Last In, First Out), so the most recently inserted element is accessed first.
Our implementation: Whenever a task is completed, its ID is pushed onto the stack. This allows us to display completion history starting with the latest completed task.
Time complexity: Push – O(1) | Pop – O(1) | Top – O(1)

6. Linked List — Completed Tasks
Concept: A Linked List stores elements in connected nodes rather than in consecutive memory locations.
Our implementation: We maintain a separate list of completed task IDs. When a task is completed, its ID is added to the list. If its submission is undone, the ID is removed.
Time complexity: Insertion at end – O(1) | Removal by ID – O(c)

7. Vector — Storing All Tasks
Concept: A Vector is a dynamic array that grows automatically when new elements are added.
Our implementation: The allTasks vector stores every Task ID added to the program. We use it to traverse tasks when displaying all tasks and calculating dashboard statistics.
Time complexity: Insertion at end – O(1) amortized | Traversal – O(n)

8. Map — Deadline-wise Grouping
Concept: A Map stores key-value pairs in sorted key order.
Our implementation: We use the deadline as the key and a vector of Task IDs as the value. This helps us group tasks with the same deadline and display the groups in chronological order.
Time complexity: Insertion and search – O(log d)
