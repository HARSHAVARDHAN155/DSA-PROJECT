#include "UsersList.h"
#include <assert.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////
// This file contains the declaration of functions which will be used to store data 
// of the users corresponding to the UserIDs
// everytime a new User registers, we store their detail in a hash function and
// all other operations are done using the UserID only
//////////////////////////////////////////////////////////////////////////////////////

int HashFn(int UserID)
{
    return (UserID % M);
}

/////////////////////////////////////////////////////////////////////////////
// function : InitUserNode
// input    : All details of the user
// output   : Ptr to side chain node of user details
PtrToUser InitUserNode(int UserID, int Age, char *UserName, char *UserCity, char *UserOrganization)
{
    PtrToUser Q = (PtrToUser)(malloc(sizeof(UserDetails)));
    assert(Q != NULL);
    Q->UserId = UserID;
    Q->Age = Age;

    Q->Name = (char *)(malloc(sizeof(char) * 100));
    assert(Q->Name != NULL);

    Q->City = (char *)(malloc(sizeof(char) * 100));
    assert(Q->City != NULL);

    Q->Organization = (char *)(malloc(sizeof(char) * 100));
    assert(Q->Organization != NULL);

    strcpy(Q->Name, UserName);
    strcpy(Q->City, UserCity);
    strcpy(Q->Organization, UserOrganization);
    Q->Next = NULL;
    return Q;
}

/////////////////////////////////////////////////////////////////////////////
// function : RegisterNewUser
// input    : All details of the user
// result   : A new node corresponding to the new user is inserted in HT
void RegisterNewUser(Hash UsersList[], int UserID, int Age, char *UserName, char *UserCity, char *UserOrganization)
{
    int key = HashFn(UserID);
    // if side chain is empty
    if (UsersList[key] == NULL)
    {
        UsersList[key] = (PtrToUser)(malloc(sizeof(UserDetails)));
        assert(UsersList[key] != NULL);
        UsersList[key]->Next = NULL;
    }
    //traverse until end of side chain
    PtrToUser Walk = UsersList[key];
    while (Walk->Next != NULL)
    {
        Walk = Walk->Next;
    }
    Walk->Next = InitUserNode(UserID, Age, UserName, UserCity, UserOrganization);
}

/////////////////////////////////////////////////////////////////////////////
// function : SearchUser
// input    : HT and ID of the user to be searched
// output   : A node in side chain corresponding to the user with given UserID
PtrToUser SearchUser(Hash UsersList[], int UserID)
{
    int key = HashFn(UserID);
    if (UsersList[key] == NULL)
    {
        return NULL;
    }
    PtrToUser P = UsersList[key]->Next;
    while (P != NULL)
    {
        if (P->UserId == UserID)
            return P;
        P = P->Next;
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// function : UnregisterUser(HT,UserID)
// input    : HT and ID of the user to be deleted
// result   : The node corresponding to the user with given UserID is deleted from HT
void UnregisterUser(Hash UsersList[], int UserID)
{
    //find the node corresponding to the UserID to be deleted
    int key = HashFn(UserID);
    PtrToUser P = UsersList[key]->Next;
    PtrToUser Prev = UsersList[key];
    // if there is only one user with HashValue as key
    if (P == NULL)
    {
        UsersList[key] = NULL;
        free(P);
        return;
    }
    while (P != NULL)
    {
        if (P->UserId == UserID)
            break;
        Prev = P;
        P = P->Next;
    }
    Prev->Next = P->Next;
    free(P);
}
