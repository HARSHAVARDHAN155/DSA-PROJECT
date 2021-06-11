
#include <stdlib.h>
#include <stdio.h>
// #include "DSA_Project.h"
// #include <assert.h>
// #include "NetworkGraph.c"

/////////////////////////////////////////////////////////////////////////////
// structure for a queue which will be used in BFS for getting the
// recommendations for already registered users
struct Q_Node
{
    int data;
    struct Q_Node *Next;
};
typedef struct Q_Node *QNode;
struct Q_
{
    struct Q_Node *Front;
    struct Q_Node *Rear;
    int size;
};
typedef struct Q_ *Queue;
Queue bfsQ = NULL;

// Queue structure ends  here
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Function Prototypes
void AVL_BFS();
QNode InitQNode(int val);
Queue InitQ();
Queue Enque(Queue q, int value);
Queue Deque(Queue q);
int visitedHash(int userID);
void GetSortedNodes(Node Root);
void GetSortedNodesAVL(Node Root, int NumberOfNodes);
void DFS(Node Root);
void GetFriends(int UserID);
void FrndsRecomm_alredy_RegisUsers(int Id, int K);
// Function Prototypes end here
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// This functions intialises the QNodes
QNode InitQNode(int val)
{
    QNode newNode = (QNode)malloc(sizeof(struct Q_Node));
    newNode->data = val;
    newNode->Next = NULL;
}
Queue InitQ()
{
    Queue newQ = (Queue)malloc(sizeof(struct Q_));
    newQ->Front = NULL;
    newQ->Rear = NULL;
    newQ->size = 0;
    return newQ;
}

/////////////////////////////////////////////////////////////////////////////
// Adds a new element to the rear of the Queue
Queue Enque(Queue q, int value)
{
    QNode newNode = InitQNode(value);
    if (q->size == 0)
    {
        q->Rear = newNode;
        q->Front = newNode;
        q->size = 1;
        return q;
    }
    q->Rear->Next = newNode;
    q->Rear = q->Rear->Next;
    q->size++; //increasing count of number of elems in Queue
    return q;
}

/////////////////////////////////////////////////////////////////////////////
// Pops the element present at the front of the Queue
Queue Deque(Queue q)
{
    if (q->size == 0)
        return q;
    QNode temp = q->Front;
    q->Front = q->Front->Next;
    free(temp);
    if (q->size == 1)
    {
        q->Rear = NULL;
    }
    q->size--;
    return q;
}

/////////////////////////////////////////////////////////////////////////////
// Structure to check if a friend with a particular UserID has been visited
// during the BFS
struct checkVisited
{
    int num;
    int status;
    struct checkVisited *Next;
};
typedef struct checkVisited *Visit;

/////////////////////////////////////////////////////////////////////////////
// Visited array to store users who have already been visited
Visit visited[10003];
int visitedHash(int userID)
{
    return userID % 10003;
}
bool isVisited(int UserID);

/////////////////////////////////////////////////////////////////////////////
// whenever a AVLQ will be used, these will act as the front and rear
AVLq AVLqFront = NULL;
AVLq AVLqRear = NULL;
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Supplementary array for storing recommendations
int *RecommendationsForRegisteredUser;
int RecommendationsRequired;
int RecommendationsFound;
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Initialises the BFS process for a given user to get recommendations
// does supplementary work to store recommendations
void FrndsRecomm_alredy_RegisUsers(int Id, int K)
{
    // marking visited Hash Table with NULL
    for (int i = 0; i < 10003; i++)
    {
        if (visited[i] != NULL)
        {
            free(visited[i]);
            visited[i] = NULL;
        }
    }
    // Marking the User for whom recommendations are to be found as visited
    visited[visitedHash(Id)] = (Visit)malloc(sizeof(struct checkVisited));
    visited[visitedHash(Id)]->num = Id;
    visited[visitedHash(Id)]->status = 1;
    visited[visitedHash(Id)]->Next = NULL;

    // getting the Node in Graph corresponding to the user
    struct User *CurrUser = GetUser(Id);
    // initialising the BFSQ which stores IDs of Friends and their Friends
    bfsQ = InitQ();
    // Initialising the AVLQ for BFS on Friends AVL tree of each user
    AVLqFront = (AVLq)malloc(sizeof(struct AVLNodeQ_));
    AVLqRear = (AVLq)malloc(sizeof(struct AVLNodeQ_));
    AVLqFront->data = CurrUser->Friend;
    AVLqRear = AVLqFront;
    // Enquing the first friend of the user
    bfsQ = Enque(bfsQ, AVLqFront->data->data);
    RecommendationsRequired = 10000;
    // storing all other friends of the user into the Queue
    AVL_BFS();
    // counter will inform us if all friends of the users have been dequed
    int Counter = bfsQ->size;
    // giving size & initialising the array which will store the recommendations
    RecommendationsForRegisteredUser = (int *)malloc(K * (sizeof(int)));
    for (int i = 0; i < K; i++)
    {
        RecommendationsForRegisteredUser[i] = -1;
    }
    // k+size because friends of the user shouldnt be recommended
    RecommendationsRequired = K + bfsQ->size;
    RecommendationsFound = 0;
    int RecommendationsInserted = 0;
    // We add friends of friends and enque recommendations here
    while (bfsQ->size > 0)
    {
        AVLqFront = (AVLq)malloc(sizeof(struct AVLNodeQ_));
        AVLqRear = (AVLq)malloc(sizeof(struct AVLNodeQ_));
        AVLqFront->data = GetUser(bfsQ->Front->data)->Friend;
        AVLqRear = AVLqFront;
        if (AVLqFront->data != NULL)
            if (!isVisited(AVLqFront->data->data))
            {
                bfsQ = Enque(bfsQ, AVLqFront->data->data);
            }
        AVL_BFS();
        if (Counter > 0)
            Counter--;
        else
        {
            RecommendationsForRegisteredUser[RecommendationsInserted] = bfsQ->Front->data;
            RecommendationsInserted++;
        }
        bfsQ = Deque(bfsQ);
        // exit when k recommendations are found
        if (RecommendationsInserted == K)
        {
            RecommendationsFound = RecommendationsInserted;
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// Checks if user with given ID has been visited before in the BFS
bool isVisited(int ID)
{
    Visit walk = NULL;
    walk = visited[visitedHash(ID)];
    if (walk == NULL)
        return false;
    while (walk != NULL)
    {
        if (walk->num == ID)
            return true;
        walk = walk->Next;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// BFS on the AVL tree
void AVL_BFS()
{
    if (AVLqFront == NULL || AVLqFront->data == NULL)
    {
        AVLqRear = NULL;
        return;
    }
    if (RecommendationsRequired == RecommendationsFound)
    {
        return;
    }
    Node Root = AVLqFront->data;
    bool has_been_visited = false;
    Visit walk = NULL;
    walk = visited[visitedHash(Root->data)];
    if (walk == NULL)
    {
        visited[visitedHash(Root->data)] = (Visit)malloc(sizeof(struct checkVisited));
        walk = visited[visitedHash(Root->data)];
        walk->num = Root->data;
        walk->status = 1;
        walk->Next = NULL;
        RecommendationsFound++;
    }
    else
    {
        if (walk->num == Root->data)
            has_been_visited = true;
        while (walk->Next != NULL)
        {
            if (walk->num == Root->data)
            {
                has_been_visited = true;
                break;
            }
        }
        if (!has_been_visited)
        {
            walk->Next = (Visit)malloc(sizeof(struct checkVisited));
            walk = walk->Next;
            walk->num = Root->data;
            walk->status = 1;
            walk->Next = NULL;
            RecommendationsFound++;
        }
    }

    if (Root->left != NULL)
    {

        AVLq newnode = (AVLq)malloc(sizeof(struct AVLNodeQ_));
        newnode->data = Root->left;
        newnode->Next = NULL;
        AVLqRear->Next = newnode;
        AVLqRear = AVLqRear->Next;
        if (!isVisited(Root->left->data))
            bfsQ = Enque(bfsQ, Root->left->data);
    }
    if (Root->right != NULL)
    {
        AVLq newnode = (AVLq)malloc(sizeof(struct AVLNodeQ_));
        newnode->data = Root->right;
        newnode->Next = NULL;
        AVLqRear->Next = newnode;
        AVLqRear = AVLqRear->Next;
        if (!isVisited(Root->right->data))
            bfsQ = Enque(bfsQ, Root->right->data);
    }
    AVLqFront = AVLqFront->Next;
    AVL_BFS();
}

/////////////////////////////////////////////////////////////////////////////
// getting friends of a user using DFS ==> sorted order of UserIDs
int *FriendsOfUser;
int FriendsEntered;
void GetFriends(int UserID)
{
    struct User *CurrUser = GetUser(UserID);
    FriendsOfUser = (int *)malloc((CurrUser->NoOfFriends + 1) * sizeof(int));
    FriendsOfUser[0] = CurrUser->NoOfFriends;
    FriendsEntered = 1;
    DFS(CurrUser->Friend);
    return;
}

/////////////////////////////////////////////////////////////////////////////
// DFS on AVL trees
void DFS(Node Root)
{
    if (Root == NULL)
    {
        return;
    }
    DFS(Root->left);
    FriendsOfUser[FriendsEntered] = Root->data;
    FriendsEntered++;
    DFS(Root->right);
    return;
}

/////////////////////////////////////////////////////////////////////////////
// gets the sorted order of data present in an AVL tree
int *storingNodes;
int NodesStored;
void GetSortedNodesAVL(Node Root, int NumberOfNodes)
{
    free(storingNodes);
    storingNodes = (int *)malloc(NumberOfNodes * sizeof(int));
    NodesStored = 0;
    GetSortedNodes(Root);
}
/////////////////////////////////////////////////////////////////////////////
// Pseudo DFS to get sorted order
void GetSortedNodes(Node Root)
{
    if (Root == NULL)
    {
        return;
    }
    GetSortedNodes(Root->left);
    storingNodes[NodesStored] = Root->data;
    NodesStored++;
    GetSortedNodes(Root->right);
    return;
}