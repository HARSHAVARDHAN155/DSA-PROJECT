#ifndef _FRIENDS
#define _FRIENDS

// #include "DSA_Project.h"

#define M 100003

typedef struct UserDetails UserDetails;
typedef UserDetails* PtrToUser;
typedef UserDetails* Hash;

struct UserDetails             //stores the details of the Users registered on our system
{
    int UserId;
    int Age;
    char* Name;
    char* City;
    char* Organization;
    PtrToUser Next;
    //int* Table;
};


int HashFn(int X);
PtrToUser InitUserNode(int UserID , int Age , char* UserName , char* UserCity , char* UserOrganization);
void RegisterNewUser(Hash UsersList[] , int UserID , int Age , char* UserName , char* UserCity , char* UserOrganization);
PtrToUser SearchUser(Hash UsersList[] , int UserID);
void UnregisterUser(Hash UsersList[] , int UserID);


#endif