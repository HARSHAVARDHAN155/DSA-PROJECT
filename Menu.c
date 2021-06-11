#ifndef __MENU_C
#define __MENU_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GUI.c"
#include "NetworkGraph.c"
#include "string_hash.c"
#include "UsersList.c"
#include "mybfs.c"
#include "GUI.c"
#include "Menu.c"

struct System //Controls the entire program
{
    int NoOfUsers;        //stores total users registered
    int NextID;           //stores what should be the ID if a new user joins
    Node UnregisteredIDs; //BST to store the ID of the unregistered users
    int NoOfUnregisteredUsers;
    Node RegisteredIDs;
    Hash *Users; //hashmap to store the user and their ids
};
struct System *Control;

Hash_table City = NULL;
Hash_table Organisation = NULL;

void WelcomePage();
int MenuPage();
void AddNewUser();
void DisplayUserDetails();
void MakeFriends();
void DisplayUserDetailsInBuilt(int UserId);
void DisplayFriendsOfUser();
void ClickEnter();
void SetupBorders(char title[]);
void UserNotFoundError();
void RecommendationsForRegisteredUsersFunc();
bool IsUserRegistered(int userID);
void Delete_in_both_string_tables(PtrToUser User, Hash_table city, Hash_table organization);
void cleanBuffer();
int readNum();
void CheckFriendshipStatus();

void cleanBuffer()
{
    char TempBufferCleaner;
    scanf("%c", &TempBufferCleaner);
}

int commons[10];
int handles[10];

void NewUserRecommendations(int userID)
{
    system("clear");
    SetupBorders("Friends Recommendations");

    PtrToUser NewUserID = SearchUser(Control->Users, userID);
    // gives the string node corresponding to the city and organisation of the user
    string_node Org; //org
    string_node Cit; //city
    Cit = Find_in_string(City, NewUserID->City);
    Org = Find_in_string(Organisation, NewUserID->Organization);
    // will store the handles and number of common parameters for the recommendations
    for (int i = 0; i < 10; i++)
    {
        commons[i] = 0;
        handles[i] = -1;
    }
    // loop will execute twice: once for org and second time for city
    for (int times = 0; times < 2; times++)
    {
        if (Org == NULL && Cit == NULL)
            break;
        // stores max same parameters
        int max_same;
        // check if there is atleast one user with same city
        if (Cit != NULL)
            max_same = Cit->NoOfUsers;
        // check if there is atleast one user with same org
        if (times == 0 && Org != NULL)
        {
            GetSortedNodesAVL(Org->root, Org->NoOfUsers);
            max_same = Org->NoOfUsers;
        }
        else if (Cit != NULL)
            GetSortedNodesAVL(Cit->root, Cit->NoOfUsers);
        // in the above few lines, we got the Sorted list of nodes present in
        // the AVL corresponding to the users stored in that string_node
        // (string nodes store the different cities and organisations)

        // loop to check all the users found in that AVL
        for (int i = 0; i < max_same; i++)
        {
            PtrToUser Recom = SearchUser(Control->Users, storingNodes[i]);
            int common = 1; //since they are found in avl atleast one param is common
            if (Recom == NULL)
                continue;
            if (strcmp(Recom->City, NewUserID->City) == 0 && strcmp(Recom->Organization, NewUserID->Organization) == 0)
                common++; // if the other param also matches,we increase common_params_counter
            // we iterate thru all the 10 recommendations currently present to see
            // if the new recommendation is good enough to be inserted
            for (int j = 9; j >= 0; j--)
            {
                //case 1: the common_params are equal
                if (commons[j] == common)
                {
                    //if this user has already been recommended, break
                    if (handles[j] == Recom->UserId)
                    {
                        break;
                    }
                }
                else if (commons[j] < common)
                {
                    // if we find a recommendation with fewer common params,
                    // we replace that recommendation with new recom
                    // swap out all elements with less common params
                    for (int k = 0; k < j; k++)
                    {
                        commons[k] = commons[k + 1];
                        handles[k] = handles[k + 1];
                    }
                    commons[j] = common;
                    handles[j] = Recom->UserId;
                    break;
                }
            }
        }
    }
    Insert_in_both_string_tables(userID, NewUserID->City, NewUserID->Organization, City, Organisation);
}

/////////////////////////////////////////////////////////////////////////////
// function: readNum()
// purpose : returs a integer read using the function
//         : useful when we want to read integers only and avoid
//         : unnecessary strings given as input by mistake
int readNum()
{
    char ch = 'k';
    int num = -1;
    scanf("%c", &ch);
    if (ch == 10)
        return -1;
    while (ch != 10)
    {
        if (ch >= '0' && ch <= '9')
        {
            if (num == -1)
                num = ch - '0';
            else if (num != -2)
                num = num * 10 + ch - '0';
        }
        else
        {
            num = -2;
        }
        scanf("%c", &ch);
    }
    return num;
}

/////////////////////////////////////////////////////////////////////////////
// function: RecommendationsForRegisteredUsersFunc()
// purpose : shows friends recommendations for already registered users
void RecommendationsForRegisteredUsersFunc()
{
    // setting up the UI
    int UserID, K;
    system("clear");
    SetupBorders("Friends Recommendations");
    gotoxy(17, 9);
    printWhite("Enter the UserID of user for whom");
    gotoxy(17, 10);
    printWhite("recommendations must be found: ");
    int num = -1;
    // getting inputs
    while (num == -1 || num == -2)
    {
        gotoxy(48, 10);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(48, 10);
        num = readNum();
    }
    UserID = num;
    if (!IsUserRegistered(UserID))
    {
        UserNotFoundError();
        cleanBuffer();
        ClickEnter();
        return;
    }
    gotoxy(17, 11);
    printWhite("Number of recommendations to be found: ");
    num = -1;
    while (num == -1 || num == -2)
    {
        gotoxy(56, 11);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(56, 11);
        num = readNum();
    }
    K = num;
    struct User *CurrUser = GetUser(UserID);
    // corner case: when the user doesnt have any friends
    if (CurrUser->NoOfFriends == 0)
    {
        gotoxy(20, 15);
        printRed("Sorry!The user doesnt have any friends.");
        gotoxy(26, 16);
        printRed("Can't recommend friends");
        ClickEnter();
        return;
    }
    // calling the supplementary BFS functions
    FrndsRecomm_alredy_RegisUsers(UserID, K);
    // displaying the UserIDs of the recommendations
    gotoxy(17, 12);
    printf("We found %d recommendations!", RecommendationsFound);
    gotoxy(17, 13);
    printWhite("{ ");
    int yMovement = 0;
    int movement = 0;
    for (int i = 0; i < RecommendationsFound; i++)
    {
        int temp = RecommendationsForRegisteredUser[i];
        while (temp)
        {
            temp /= 10;
            movement++;
        }
        movement += 2;
        if (movement >= 45)
        {
            movement = 0;
            yMovement++;
            gotoxy(19, yMovement + 13);
        }
        printf("%d, ", RecommendationsForRegisteredUser[i]);
    }
    printWhite("}");
    // ClickEnter();
    char BufferCleaner;
    scanf("%c", &BufferCleaner);
    if (RecommendationsFound > 0)
    {
        gotoxy(18, 17);
        printWhite("Click [ENTER] to continue or 'n'+[ENTER]");
        gotoxy(24, 18);
        printWhite("to view details of recommendations");
    }
    else
    {
        gotoxy(30, 18);
        printWhite("Click [ENTER] to continue");
    }
    char Option = 'a';
    int RecommendationNo = 0;
    while (Option != 10)
    {
        printf("\e[38;2;0;0;0m");
        gotoxy(48, 18);
        scanf("%c", &Option);
        if (Option != 10 && Option != 'n')
            continue;
        if (Option != 10)
            scanf("%c", &BufferCleaner);
        if (Option == 'n')
        {
            SetupBorders("Details of Recommendations");
            DisplayUserDetailsInBuilt(RecommendationsForRegisteredUser[RecommendationNo]);
            RecommendationNo++;
            if (RecommendationNo == RecommendationsFound)
            {
                RecommendationNo = 0;
            }
            gotoxy(18, 15);
            printWhite("Do you wish to follow this user?");
            gotoxy(18, 16);
            printWhite("Your answer [y/n]: ");
            char addFriendChoice = 'a';
            while (addFriendChoice != 'y' && addFriendChoice != 'n')
            {
                scanf("%c", &addFriendChoice);
            }
            if (addFriendChoice == 'y')
            {
                AddFriend(UserID, RecommendationsForRegisteredUser[RecommendationNo]);
            }
            scanf("%c", &BufferCleaner);
            gotoxy(18, 17);
            printWhite("Click [ENTER] to continue or 'n'+[ENTER]");
            gotoxy(24, 18);
            printWhite("to view details of friends");
        }
    }
    system("clear");
}

void UnregisterUserFromSystem()
{
    SetupBorders("Unregister user from system");
    gotoxy(17, 9);
    printf("Enter the UserID of the person to be");
    gotoxy(17, 10);
    printf("unregistered: ");
    int userID;
    int num = -1;
    while (num == -1 || num == -2)
    {
        gotoxy(32, 10);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(32, 10);
        num = readNum();
    }
    userID = num;
    if (!IsUserRegistered(userID))
    {
        UserNotFoundError();
        cleanBuffer();
        ClickEnter();
        return;
    }
    struct User *CurrUser = GetUser(userID);
    GetSortedNodesAVL(CurrUser->Friend, CurrUser->NoOfFriends);
    for (int i = 0; i < CurrUser->NoOfFriends; i++)
    {
        struct User *temp = GetUser(storingNodes[i]);
        temp->Follower = remove_node(temp->Follower, userID);
        temp->NoOfFollowers--;
    }
    GetSortedNodesAVL(CurrUser->Follower, CurrUser->NoOfFollowers);
    for (int i = 0; i < CurrUser->NoOfFollowers; i++)
    {
        struct User *temp = GetUser(storingNodes[i]);
        temp->Friend = remove_node(temp->Friend, userID);
        temp->NoOfFriends--;
    }
    Delete_in_both_string_tables(SearchUser(Control->Users, userID), City, Organisation);
    UnregisterUser(Control->Users, userID);
    Control->NoOfUnregisteredUsers += 1;
    Control->UnregisteredIDs = insert(Control->UnregisteredIDs, userID);
    Control->RegisteredIDs = remove_node(Control->RegisteredIDs, userID);
    Control->NoOfUnregisteredUsers++;
    Control->NoOfUsers--;
    DeleteUser(userID);
    gotoxy(24, 15);
    printGreen("User unregistered sucessfully!!");
    char clearBuffer;
    scanf("%c", &clearBuffer);
    ClickEnter();
}

/////////////////////////////////////////////////////////////////////////////
// function: main
// purpose : controls the entire working of the project
int main(void)
{
    Control = (struct System *)malloc(sizeof(struct System));
    Control->NoOfUsers = 0;
    Control->NextID = 1;
    Control->NoOfUnregisteredUsers = 0;
    Control->UnregisteredIDs = NULL;
    Control->RegisteredIDs = NULL;
    Control->Users = (Hash *)malloc(100 * sizeof(Hash));
    City = Init_city_Ht();
    Organisation = Init_organisation_Ht();
    InitGraphHT();
    WelcomePage();
    int Option = -1;
    while (Option != 10)
    {
        while (Option > 10 || Option < 1)
        {
            system("clear");
            Option = MenuPage();
        }
        if (Option == 1)
            AddNewUser();
        else if (Option == 2)
            DisplayUserDetails();
        else if (Option == 3)
            MakeFriends();
        else if (Option == 4)
        {
            DisplayFriendsOfUser();
        }
        else if (Option == 5)
        {
            RecommendationsForRegisteredUsersFunc();
        }
        else if (Option == 6)
        {
            UnregisterUserFromSystem();
        }
        else if (Option == 7)
        {
            CheckFriendshipStatus();
        }
        else if (Option == 8)
        {
            break;
        }

        Option = -1;
    }
    system("clear");
    SetupBorders("Program Terminated");
    gotoxy(24, 15);
    printGreen("Thank you! Do visit us again!");
    ClickEnter();
    system("clear");
}

/////////////////////////////////////////////////////////////////////////////
// function: MakeFriends()
// purpose : Make user1 follow user2
void MakeFriends()
{
    int User1, User2;
    system("clear");
    border_with_delay();
    gotoxy(23, 4);
    blueBG("IIIT-H Friends Recommendation System");
    MidBox();
    gotoxy(30, 7);
    printYellow("Make User1 follow User2");
    gotoxy(17, 9);
    printWhite("Enter the UserID of User1 : ");
    int num = -1;
    while (num == -1 || num == -2)
    {
        gotoxy(45, 9);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(45, 9);
        num = readNum();
    }
    User1 = num;
    if (!IsUserRegistered(User1))
    {
        UserNotFoundError();
        cleanBuffer();
        ClickEnter();
        return;
    }
    gotoxy(17, 10);
    printWhite("Enter the UserID of User2 : ");
    num = -1;
    while (num == -1 || num == -2)
    {
        gotoxy(45, 10);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(45, 10);
        num = readNum();
    }
    User2 = num;
    if (!IsUserRegistered(User2))
    {
        UserNotFoundError();
        cleanBuffer();
        ClickEnter();
        return;
    }
    AddFriend(User1, User2);
}

/////////////////////////////////////////////////////////////////////////////
// function: WelcomePage()
// purpose : Displays the welcome page
void WelcomePage()
{
    char Option = 'a';
    system("clear");
    border_with_delay();
    gotoxy(12, 13);
    printWhite("Team 55:");
    gotoxy(22, 13);
    printWhite("Nandini Reddy (2020101038)");
    gotoxy(22, 14);
    printWhite("Rohit Lingala (2020102035)");
    gotoxy(22, 15);
    printWhite("Harshavardhan (2020101106)");
    gotoxy(22, 16);
    printWhite("Surya Teja    (2020101042)");
    gotoxy(22, 17);
    printWhite("Keyur Ganesh  (2020101100)");
    gotoxy(22, 20);
    printWhite("Click [ENTER] to continue....");
    gotoxy(17, 4);
    blueBG("Welcome to IIIT-H Friends Recommendation System");
    gotoxy(52, 20);
    while (Option != 10)
        scanf("%c", &Option);
    //exiting the program with cmd line down
}

/////////////////////////////////////////////////////////////////////////////
// function: MenuPage()
// purpose : Displays the initial menu
// output  : Option selected by the user
int MenuPage()
{
    system("clear");
    border_with_delay();
    gotoxy(23, 4);
    blueBG("IIIT-H Friends Recommendation System");
    MidBox();
    gotoxy(38, 7);
    printYellow("MENU");
    gotoxy(17, 9);
    printWhite("1. Add New User");
    gotoxy(17, 10);
    printWhite("2. Get Details of a User from UserID");
    gotoxy(17, 11);
    printWhite("3. Make Friends");
    gotoxy(17, 12);
    printWhite("4. Show Friends of a User");
    gotoxy(17, 13);
    printWhite("5. Show Friends Recommendation for a User");
    gotoxy(17, 14);
    printWhite("6. Unregister a user");
    gotoxy(17, 15);
    printWhite("7. Check Friendship Status");
    gotoxy(17, 16);
    printWhite("8. Exit");
    gotoxy(17, 18);
    printWhite("Your Option: ");

    int Option = -1;
    while (Option == -1 || Option == -2)
    {
        gotoxy(30, 18);
        for (int clear = 0; clear < 17; clear++)
            printf("  ");
        gotoxy(30, 18);
        Option = readNum();
    }
    return Option;
}

/////////////////////////////////////////////////////////////////////////////
// function: AddNewUser()
// purpose : provides the GUI for registering a new user
// outcome : a new user is added the hash table defined in UserList.c file
void AddNewUser()
{
    system("clear");
    border_with_delay();
    gotoxy(23, 4);
    blueBG("IIIT-H Friends Recommendation System");
    MidBox();
    gotoxy(38, 7);
    printYellow("Add a new user");
    gotoxy(17, 9);
    printWhite("1. User's Name         : ");
    gotoxy(17, 10);
    printWhite("2. User's Age          : ");
    gotoxy(17, 11);
    printWhite("3. User's Organisation : ");
    gotoxy(17, 12);
    printWhite("4. User's City         : ");
    int NewUserID = 0, Age = 0;
    char Name[32], UCity[20], UOrganization[30];
    gotoxy(42, 9);
    scanf("%s", Name);
    gotoxy(42, 10);
    Age = -1;
    while (Age == -1 || Age == -2)
    {
        gotoxy(42, 10);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(42, 10);
        Age = readNum();
    }
    gotoxy(42, 11);
    scanf("%s", UOrganization);
    gotoxy(42, 12);
    scanf("%s", UCity);

    if (Control->NoOfUnregisteredUsers != 0)
    {
        NewUserID = findMin(Control->UnregisteredIDs);
        Control->NoOfUnregisteredUsers--;
        Control->UnregisteredIDs = remove_node(Control->UnregisteredIDs, NewUserID);
    }
    else
    {
        NewUserID = Control->NextID;
    }
    Control->RegisteredIDs = insert(Control->RegisteredIDs, NewUserID);
    RegisterNewUser(Control->Users, NewUserID, Age, Name, UCity, UOrganization);
    Control->NoOfUsers++;
    Control->NextID++;
    InsertInGraphHT(NewUserID);
    gotoxy(24, 15);
    printGreen("User registered successfully!");
    gotoxy(27, 16);
    printGreen("Your UserID is: ");
    printf("%d", NewUserID);
    char BufferCleaner;
    scanf("%c", &BufferCleaner);
    ClickEnter();
    system("clear");
    if (Control->NoOfUsers > 1)
    {
        int finalRecommendations[10];
        GetSortedNodesAVL(Control->RegisteredIDs, Control->NoOfUsers);
        int allUsers[Control->NoOfUsers];
        for (int i = 0; i < Control->NoOfUsers; i++)
        {
            allUsers[i] = storingNodes[i];
        }
        NewUserRecommendations(NewUserID);
        for (int i = 0; i < 10 && i < Control->NoOfUsers; i++)
        {
            if (handles[i] != -1)
                break;
            bool isPresent = false;
            if (allUsers[i] != NewUserID)
            {
                for (int j = 9; j > i; j--)
                {
                    if (handles[j] == allUsers[i])
                    {
                        isPresent = true;
                        break;
                    }
                }
                if (!isPresent)
                    handles[i] = allUsers[i];
            }
        }
        int total_recoms = 0;
        for (int i = 9; i >= 0; i--)
        {
            if (handles[i] != -1)
            {
                finalRecommendations[total_recoms] = handles[i];
                total_recoms++;
            }
        }
        SetupBorders("Friends Recommendations");
        gotoxy(17, 10);
        printf("We found %d friends recommendations for you!", total_recoms);
        gotoxy(17, 11);
        printWhite("{");
        int yMovement = 0;
        int movement = 0;
        for (int i = 0; i < total_recoms; i++)
        {
            int temp = finalRecommendations[i];
            while (temp)
            {
                temp /= 10;
                movement++;
            }
            movement += 2;
            if (movement >= 45)
            {
                movement = 0;
                yMovement++;
                gotoxy(19, yMovement + 11);
            }
            printf("%d, ", finalRecommendations[i]);
        }
        printWhite("}");
        gotoxy(18, 17);
        printWhite("Click [ENTER] to continue or 'n'+[ENTER]");
        gotoxy(24, 18);
        printWhite("to view details of recommendations");

        char Option = 'a';
        int RecommendationNo = 0;
        while (Option != 10)
        {
            printf("\e[38;2;0;0;0m");
            gotoxy(59, 18);
            scanf("%c", &Option);
            if (Option != 10 && Option != 'n')
                continue;
            if (Option != 10)
                scanf("%c", &BufferCleaner);
            if (Option == 'n')
            {
                SetupBorders("Details of Recommendations");
                DisplayUserDetailsInBuilt(finalRecommendations[RecommendationNo]);
                gotoxy(18, 15);
                printWhite("Do you wish to follow this user?");
                gotoxy(18, 16);
                printWhite("Your answer [y/n]: ");
                char addFriendChoice = 'a';
                while (addFriendChoice != 'y' && addFriendChoice != 'n')
                {
                    scanf("%c", &addFriendChoice);
                }
                if (addFriendChoice == 'y')
                {
                    AddFriend(NewUserID, finalRecommendations[RecommendationNo]);
                }
                scanf("%c", &BufferCleaner);
                gotoxy(18, 17);
                printWhite("Click [ENTER] to continue or 'n'+[ENTER]");
                gotoxy(24, 18);
                printWhite("to view details of friends");
                RecommendationNo++;
                if (RecommendationNo == total_recoms)
                {
                    RecommendationNo = 0;
                }
            }
        }
    }
    else
    {
        Insert_in_both_string_tables(NewUserID, UCity, UOrganization, City, Organisation);
    }
    system("clear");
}
/////////////////////////////////////////////////////////////////////////////
// function: DisplayUserDetails()
// purpose : Displays the details of a User
void DisplayUserDetails()
{
    system("clear");
    border_with_delay();
    gotoxy(23, 4);
    blueBG("IIIT-H Friends Recommendation System");
    MidBox();
    gotoxy(30, 7);
    printYellow("View Details of an user");
    gotoxy(17, 9);
    printWhite("Enter the User ID of the user: ");
    int UserId;
    int num = -1;
    while (num == -1 || num == -2)
    {
        gotoxy(48, 9);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(48, 9);
        num = readNum();
    }
    UserId = num;
    DisplayUserDetailsInBuilt(UserId);

    gotoxy(30, 18);
    printWhite("Click [ENTER] to continue");
    char Option = 'a';
    while (Option != 10)
    {
        printf("\e[38;2;0;0;0m");
        gotoxy(30, 17);
        scanf("%c", &Option);
    }
    system("clear");
}

/////////////////////////////////////////////////////////////////////////////
// function: DisplayUserDetailsInBuilt(int UserID)
// purpose : Displays the details of a User where UserID is given as argument
void DisplayUserDetailsInBuilt(int UserId)
{
    PtrToUser CurrUser = NULL;
    CurrUser = SearchUser(Control->Users, UserId);
    if (CurrUser == NULL)
    {
        gotoxy(17, 10);
        printRed("WARNING!!!");
        gotoxy(17, 11);
        printRed("No user found in database with given UserID");
    }
    else
    {
        gotoxy(17, 10);
        printWhite("1. User's Name         : ");
        printWhite(CurrUser->Name);
        gotoxy(17, 11);
        printWhite("2. User's Age          : ");
        printf("%d", CurrUser->Age);
        gotoxy(17, 12);
        printWhite("3. User's Organisation : ");
        printWhite(CurrUser->Organization);
        gotoxy(17, 13);
        printWhite("4. User's City         : ");
        printWhite(CurrUser->City);
    }
}

/////////////////////////////////////////////////////////////////////////////
// function: ClickEnter()
// purpose : waits for user to click [ENTER] key before continuing
void ClickEnter()
{
    gotoxy(27, 18);
    printWhite("Click [ENTER] to continue");
    char Option = 'a';
    while (Option != 10)
    {
        printf("\e[38;2;0;0;0m");
        gotoxy(30, 17);
        scanf("%c", &Option);
    }
    system("clear");
    return;
}
/////////////////////////////////////////////////////////////////////////////
// function: SetupBorders(char title[])
// purpose : sets up the outer and inner border.
//         : gives the project title
//         : gives the page title
void SetupBorders(char title[])
{
    system("clear");
    border_with_delay();
    gotoxy(23, 4);
    blueBG("IIIT-H Friends Recommendation System");
    MidBox();
    gotoxy(30, 7);
    printYellow(title);
    gotoxy(17, 9);
}
/////////////////////////////////////////////////////////////////////////////
// function: DisplayFriendsOfUser()
// purpose : Display all the friends of a user
//         : Displays a set of UserIDs which are friends of User
//         : Can also view the details of the friends
void DisplayFriendsOfUser()
{
    int UserID;
    system("clear");
    border_with_delay();
    gotoxy(23, 4);
    blueBG("IIIT-H Friends Recommendation System");
    MidBox();
    gotoxy(30, 7);
    printYellow("Display Friends of a User");
    gotoxy(17, 9);
    printWhite("Enter the UserID of the User : ");
    scanf("%d", &UserID);
    if (!IsUserRegistered(UserID))
    {
        UserNotFoundError();
        cleanBuffer();
        ClickEnter();
        return;
    }
    GetFriends(UserID);
    gotoxy(17, 10);
    printf("The friends of User with ID%d are: ", UserID);
    gotoxy(17, 11);
    printWhite("{ ");
    int yMovement = 0;
    int movement = 0;
    for (int i = 1; i <= FriendsOfUser[0]; i++)
    {
        int temp = FriendsOfUser[i];
        while (temp)
        {
            temp /= 10;
            movement++;
        }
        movement += 2;
        if (movement >= 45)
        {
            movement = 0;
            yMovement++;
            gotoxy(19, yMovement + 11);
        }
        printf("%d, ", FriendsOfUser[i]);
    }
    printWhite("}");
    // ClickEnter();
    char BufferCleaner;
    scanf("%c", &BufferCleaner);
    if (FriendsOfUser[0] > 0)
    {
        gotoxy(18, 17);
        printWhite("Click [ENTER] to continue or 'n'+[ENTER]");
        gotoxy(24, 18);
        printWhite("to view details of friends");
    }
    else
    {
        gotoxy(30, 18);
        printWhite("Click [ENTER] to continue");
    }
    char Option = 'a';
    int FriendNo = 1;
    while (Option != 10)
    {
        printf("\e[38;2;0;0;0m");
        gotoxy(30, 16);
        scanf("%c", &Option);
        if (Option != 10)
            scanf("%c", &BufferCleaner);
        if (Option == 'n')
        {
            SetupBorders("Details of Friends");
            DisplayUserDetailsInBuilt(FriendsOfUser[FriendNo]);
            FriendNo++;
            if (FriendNo == FriendsOfUser[0] + 1)
            {
                FriendNo = 1;
            }
            gotoxy(18, 17);
            printWhite("Click [ENTER] to continue or 'n'+[ENTER]");
            gotoxy(24, 18);
            printWhite("to view details of friends");
        }
    }
    system("clear");
}

/////////////////////////////////////////////////////////////////////////////
// function: UserNotFoundError()
// purpose : pops an  error message if the users enter an invalid userid
void UserNotFoundError()
{
    gotoxy(35, 14);
    printRed("WARNING!!!");
    gotoxy(19, 15);
    printRed("No user found in database with given UserID");
}

/////////////////////////////////////////////////////////////////////////////
// function : IsUserRegistered
// input    : UserID of the user to be verified
// output   : true if the user is registered. false otherwise
bool IsUserRegistered(int UserID)
{
    PtrToUser Checker = SearchUser(Control->Users, UserID);
    if (Checker == NULL)
    {
        return false;
    }
    return true;
}
/////////////////////////////////////////////////////////////////////////////
// function: CheckFriendshipStatus()
// purpose : Checks whether User1 follows User2
void CheckFriendshipStatus()
{
    system("clear");
    SetupBorders("Check Friendship");
    gotoxy(17, 9);
    printWhite("Enter the UserID of User1 : ");
    int User1, User2;
    int num = -1;
    while (num == -1 || num == -2)
    {
        gotoxy(45, 9);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(45, 9);
        num = readNum();
    }
    User1 = num;
    if (!IsUserRegistered(User1))
    {
        UserNotFoundError();
        cleanBuffer();
        ClickEnter();
        return;
    }
    gotoxy(17, 10);
    printWhite("Enter the UserID of User2 : ");
    num = -1;
    while (num == -1 || num == -2)
    {
        gotoxy(45, 10);
        for (int clear = 0; clear < 9; clear++)
            printf(" ");
        gotoxy(45, 10);
        num = readNum();
    }
    User2 = num;
    if (!IsUserRegistered(User2))
    {
        UserNotFoundError();
        cleanBuffer();
        ClickEnter();
        return;
    }
    struct User *User = GetUser(User1);
    if (search(User->Friend, User2))
    {
        gotoxy(24, 15);
        printGreen("Yes! User1 follows User2!");
    }
    else
    {
        gotoxy(24, 15);
        printRed("No! User1 does not follow User2.");
    }
    cleanBuffer();
    ClickEnter();
}

void Delete_in_both_string_tables(PtrToUser User, Hash_table city, Hash_table organization) //call everytime when a user unregisters
{
    Deleting_string_user(city, User->UserId, User->City);
    Deleting_string_user(organization, User->UserId, User->Organization);
}
#endif