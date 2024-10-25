#include <iostream>
#include <string>
#include <windows.h>
#include <unistd.h>

using namespace std;

template<typename T>
unsigned int simpleHash(const T& key)
{
    unsigned int hash = 0;
    for (char ch : key)
    {
        hash = hash * 31 + ch;
    }
    return hash;
}

template<typename T>
struct QueueNode
{
    T activity;
    QueueNode* next;

    QueueNode(const T& activity) : activity(activity), next(nullptr) {}
};

template<typename T>
class Queue
{
private:
    QueueNode<T>* front;
    QueueNode<T>* rear;
    int size;
    int maxSize;

public:
    Queue(int maxSize) : front(nullptr), rear(nullptr), size(0), maxSize(maxSize) {}

    ~Queue()
    {
        while (front != nullptr)
        {
            QueueNode<T>* temp = front;
            front = front->next;
            delete temp;
        }
    }

    void enqueue(const T& activity)
    {
        QueueNode<T>* newNode = new QueueNode<T>(activity);
        if (size >= maxSize)
        {
            dequeue();
        }

        if (rear == nullptr)
        {
            front = rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }

    void dequeue()
    {
        if (front == nullptr) return;

        QueueNode<T>* temp = front;
        front = front->next;
        delete temp;
        size--;

        if (front == nullptr) rear = nullptr;
    }

    void printActivities()
    {
        QueueNode<T>* current = front;
        while (current != nullptr)
        {
            cout << current->activity << ", ";
            current = current->next;
        }
        cout << endl;
    }

    T getRecentActivity()
    {
        if (rear != nullptr)
        {
            return rear->activity;
        }
        return T();
    }
    QueueNode<T>* getFront()
    {
        return front;
    }
};

template<typename T>
struct AVLNode
{
    T key;
    unsigned int hashedKey;
    Queue<T>* activities;
    AVLNode* left;
    AVLNode* right;
    int height;
    int likesCount;
    int commentsCount;
    int sharesCount;
    int postsCount;

    AVLNode(const T& k) : key(k), hashedKey(simpleHash(k)), activities(new Queue<T>(5)), left(nullptr), right(nullptr), height(1), likesCount(0), commentsCount(0), sharesCount(0), postsCount(0) {}
};

template<typename T>
class AVLTree
{
private:
    AVLNode<T>* root;

    int height(AVLNode<T>* node)
    {
        if (node == nullptr) return 0;
        return node->height;
    }

    int getBalance(AVLNode<T>* node)
    {
        if (node == nullptr) return 0;
        return height(node->left) - height(node->right);
    }

    AVLNode<T>* rotateRight(AVLNode<T>* y)
    {
        AVLNode<T>* x = y->left;
        AVLNode<T>* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        return x;
    }

    AVLNode<T>* rotateLeft(AVLNode<T>* x)
    {
        AVLNode<T>* y = x->right;
        AVLNode<T>* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
    }

    AVLNode<T>* minValueNode(AVLNode<T>* node)
    {
        if (node->left != nullptr)
        {
            node->left = minValueNode(node->left);
        }
        return node;
    }

    AVLNode<T>* insertNode(AVLNode<T>* node, const T& key)
    {
        if (node == nullptr)
        {
            return new AVLNode<T>(key);
        }

        unsigned int hashedKey = simpleHash(key);

        if (hashedKey < node->hashedKey)
        {
            node->left = insertNode(node->left, key);
        }
        else if (hashedKey > node->hashedKey)
        {
            node->right = insertNode(node->right, key);
        }
        else
        {
            return node;
        }

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && hashedKey < node->left->hashedKey)
        {
            return rotateRight(node);
        }

        if (balance > 1 && hashedKey > node->left->hashedKey)
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && hashedKey > node->right->hashedKey)
        {
            return rotateLeft(node);
        }

        if (balance < -1 && hashedKey < node->right->hashedKey)
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void inOrderTraversal(AVLNode<T>* node)
    {
        if (node != nullptr)
        {
            inOrderTraversal(node->left);
            cout << "->User: " << node->key << endl << "-Activities: ";
            node->activities->printActivities();
            inOrderTraversal(node->right);
        }
    }

    void deleteTree(AVLNode<T>* node)
    {
        if (node != nullptr)
        {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree()
    {
        deleteTree(root);
    }

    void insert(const T& key)
    {
        root = insertNode(root, key);
    }

    AVLNode<T>* searchNode(AVLNode<T>* node, const T& key)
    {
        if (node == nullptr || node->key == key)
        {
            return node;
        }

        unsigned int hashedKey = simpleHash(key);

        if (hashedKey < node->hashedKey)
        {
            return searchNode(node->left, key);
        }
        else
        {
            return searchNode(node->right, key);
        }
    }

    void addActivity(const T& username, const T& activity)
    {
        AVLNode<T>* node = searchNode(root, username);
        if (node != nullptr)
        {
            node->activities->enqueue(activity);
            updateActivityCount(node, activity);
        }
        else
        {
            cout << "User doesn't exist." << endl;
        }
    }

    void updateActivityCount(AVLNode<T>* node, const T& activity)
    {
        if (node != nullptr)
        {
            if (activity == "Like")
            {
                node->likesCount++;
            }
            else if (activity == "Comment")
            {
                node->commentsCount++;
            }
            else if (activity == "Share")
            {
                node->sharesCount++;
            }
            else if (activity == "Post")
            {
                node->postsCount++;
            }
        }
    }

    void displayActivityCount(AVLNode<T>* node)
    {
        if (node != nullptr)
        {
            displayActivityCount(node->left);
            cout << "User: " << node->key << " - Likes: " << node->likesCount
                 << ", Comments: " << node->commentsCount
                 << ", Shares: " << node->sharesCount
                 << ", Posts: " << node->postsCount << endl;
            displayActivityCount(node->right);
        }
    }

    void removeActivity(const T& key)
    {
        AVLNode<T>* node = searchNode(root, key);
        if (node != nullptr)
        {
            node->activities->dequeue();
        }
        else
        {
            cout << "User doesn't exist." << endl;
        }
    }

    void printUserActivities(const T& key)
    {
        AVLNode<T>* node = searchNode(root, key);
        if (node != nullptr)
        {
            cout << "User: " << key << "'s activities: ";
            node->activities->printActivities();
        }
        else
        {
            cout << "User doesn't exist." << endl;
        }
    }

    void printRecentActivity(const T& key)
    {
        AVLNode<T>* node = searchNode(root, key);
        if (node != nullptr)
        {
            cout << "The last activity is '" << node->activities->getRecentActivity() << endl;
        }
        else
        {
            cout << "User doesn't exist." << endl;
        }
    }

    void displayAllUsersWithActivities()
    {
        inOrderTraversal(root);
    }

    AVLNode<T>* getRoot() const
    {
        return root;
    }

    AVLNode<T>* search(const T& key)
    {
        return searchNode(root, key);
    }

    bool searchUser(const T& username)
    {
        AVLNode<T>* node = searchNode(root, username);
        return (node != nullptr);
    }
};

template<typename T>
class ActivityManager
{
private:
    AVLTree<T> avlTree;


    void inOrderForTotal(AVLNode<T>* node, int& maxActivities, T& userWithMaxActivities)
    {
        if (node != nullptr)
        {
            inOrderForTotal(node->left, maxActivities, userWithMaxActivities);

            int totalActivities = node->likesCount + node->commentsCount + node->sharesCount + node->postsCount;

            if (totalActivities > maxActivities)
            {
                maxActivities = totalActivities;
                userWithMaxActivities = node->key;
            }

            inOrderForTotal(node->right, maxActivities, userWithMaxActivities);
        }
    }

    T findMostFrequentActivity(const T& username)
    {
        AVLNode<T>* node = avlTree.searchNode(avlTree.getRoot(), username);

        if (node != nullptr)
        {
            Queue<T>* activitiesQueue = node->activities;


            T mostFrequentActivity;
            int maxCount = 0;


            QueueNode<T>* current = activitiesQueue->getFront();
            while (current != nullptr)
            {
                T currentActivity = current->activity;


                int count = 1;
                QueueNode<T>* runner = current->next;
                while (runner != nullptr)
                {
                    if (runner->activity == currentActivity)
                        count++;
                    runner = runner->next;
                }


                if (count > maxCount)
                {
                    mostFrequentActivity = currentActivity;
                    maxCount = count;
                }

                current = current->next;
            }

            return mostFrequentActivity;
        }
        else
        {
            return T();
        }
    }

public:
    void addUser(const T& username)
    {
        avlTree.insert(username);
    }

    bool searchUser(const T& username)
    {
        return avlTree.searchUser(username);
    }

    void addActivity(const T& username, const T& activity)
    {
        avlTree.addActivity(username, activity);
    }

    T getHighestActivitiesU()
    {
        int maxActivities = 0;
        T userWithMaxActivities;

        inOrderForTotal(avlTree.getRoot(), maxActivities, userWithMaxActivities);

        if (maxActivities > 0)
        {
            cout << "User with the highest total activities->  " << userWithMaxActivities << endl<<"- Total Activities: " << maxActivities << endl<<endl<<endl;
        }
        else
        {
            cout << "No users found with activities." << endl<<endl<<endl;
        }

        return userWithMaxActivities;
    }

    void removeActivity(const T& username)
    {
        avlTree.removeActivity(username);
    }

    void printUserActivities(const T& username)
    {
        avlTree.printUserActivities(username);
    }

    void displayAllUsersWithActivities()
    {
        avlTree.displayAllUsersWithActivities();
    }

    void printRecentActivity(const T& key)
    {
        avlTree.printRecentActivity(key);
    }

    void displayActivityCount(const T& username)
    {
        AVLNode<T>* node = avlTree.search(username);
        if (node != nullptr)
        {
            cout << "User-> " << username << "'s Activity Counts:\n";
            cout << "Likes: " << node->likesCount << endl;
            cout << "Comments: " << node->commentsCount << endl;
            cout << "Shares: " << node->sharesCount << endl;
            cout << "Posts: " << node->postsCount << endl;
        }
        else
        {
            cout << "User not found." << endl;
        }
    }

    T getMostFrequentActivity(const T& username)
    {
        return findMostFrequentActivity(username);
    }
};

int main()
{
    ActivityManager<string> activityManager;

    int choice;
    string username, activity;

    do
    {
        cout << "\t\t\t\t\t --------------------------" << endl;
        cout << "\t\t\t\t\t ------- Activities -------" << endl;
        cout << "\t\t\t\t\t --------------------------" << endl;

        cout << "\nMenu:\n";
        cout << "1. Add User\n";
        cout << "2. Add Activity\n";
        cout << "3. Remove Activity\n";
        cout << "4. Print User Activities\n";
        cout << "5. Display All Users with Activities\n";
        cout << "6. Get Most Frequent Activity\n";
        cout << "7. Get The Highest User Activiy\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";



        while (true)
        {
            cin >> choice;
            if (cin.fail())
            {
                cout << "Invalid input. Please enter a number." << endl;
                cin.clear();
                cin.ignore();
            }
            else
            {
                break;
            }
        }


        system("cls");

        switch (choice)
        {
        case 1:
            cout << "Enter username to add: ";
            cin.ignore();
            getline(cin, username);
            // Check if the user exists
            if (activityManager.searchUser(username))
            {
                int counter = 1;
                string modifiedUsername = username + " " + to_string(counter);


                while (activityManager.searchUser(modifiedUsername))
                {
                    counter++;
                    modifiedUsername = username + " " + to_string(counter);
                }


                cout << "The user exists. Added with a modified username: " << modifiedUsername << endl;
                activityManager.addUser(modifiedUsername);
            }
            else
            {

                activityManager.addUser(username);
                cout << "User '" << username << "' has been added\n";
            }

            system("pause");
            system("cls");
            break;

        case 2:
            cout << "Enter username to add activity: ";
            cin.ignore();
            getline(cin, username);

            if (activityManager.searchUser(username))
            {
                cout << "User found. You can add activity." << endl;

                int activityChoice;
                do
                {
                    activityManager.displayActivityCount(username);
                    activityManager.printRecentActivity(username);
                    cout << "Which Activity do you want? \n";
                    cout << "1. Like\n";
                    cout << "2. Comment\n";
                    cout << "3. Share\n";
                    cout << "4. Post\n";
                    cout << "5. Exit\n";
                    cout << "Enter your choice: ";

                    while (true)
                    {
                        cin >> activityChoice;
                        if (cin.fail())
                        {
                            cout << "Invalid input. Please enter a number." << endl;
                            cin.clear();
                            cin.ignore();
                        }
                        else
                        {
                            break;
                        }
                    }
                    string activity;
                    switch (activityChoice)
                    {
                    case 1:
                        activity = "Like";
                        break;
                    case 2:
                        activity = "Comment";
                        break;
                    case 3:
                        activity = "Share";
                        break;
                    case 4:
                        activity = "Post";
                        break;
                    case 5:
                        cout << "Exiting activity addition...\n";
                        break;
                    default:
                        cout << "Invalid activity choice! Please try again.\n";
                        break;
                    }

                    if (activityChoice >= 1 && activityChoice <= 4)
                    {
                        activityManager.addActivity(username, activity);
                        cout << "Activity '" << activity << "' added for user '" << username << "'.\n";
                        sleep(1);
                        system("cls");
                    }

                }
                while (activityChoice != 5);
            }
            else
            {
                cout << "User not found. Please add a valid user first." << endl;
            }
            system("pause");
            system("cls");
            break;

        case 3:
            cout << "Enter username to remove activity: ";
            cin.ignore();
            getline(cin, username);
            activityManager.removeActivity(username);
            system("pause");
            system("cls");
            break;
        case 4:
            cout << "Enter username to print activities: ";
            cin.ignore();
            getline(cin, username);
            activityManager.printUserActivities(username);
            system("pause");
            system("cls");
            break;
        case 5:
            cout << "All Users with Activities:\n";
            activityManager.displayAllUsersWithActivities();
            system("pause");
            system("cls");
            break;
        case 6:
            cout << "Enter username to get the most frequent activity: ";
            cin.ignore();
            getline(cin, username);
            cout << "Most frequent activity: " << activityManager.getMostFrequentActivity(username) << endl;
            system("pause");
            system("cls");
            break;
        case 7:
            activityManager.getHighestActivitiesU();
            system("pause");
            system("cls");
            break;

        case 8:
            cout << "Exiting program...\n";

            break;

        default:
            cout << "Invalid choice! Please try again.\n";
            system("pause");
            system("cls");
            break;
        }
    }
    while (choice != 8);

    return 0;
}
