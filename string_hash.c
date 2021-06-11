//////////////////////////////////////////////////////////////////////////////////////
///////////                HASH TABLE FOR STRINGS                     ////////////////
//////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////
// This file contains string hashing functions
// Purpose: We use string hashing to store the various cities and organizations
//          registered with our system.Every time a new user with same organization
//          or city registers, we find all the users with same org,city and recommend
//////////////////////////////////////////////////////////////////////////////////////

struct string_node // Node  in separate chaining of Hash table
{
    char string_name[25]; // name of city/ organization
    int Id;               // user Id
    Node root;            // Node for AVl tree fro distinct string
    int NoOfUsers;
    struct string_node *next;
};

typedef struct string_node *string_node;

#define N 10000

typedef struct My_Hasht *Hash_table;

struct My_Hasht // struct for Hash table of separate chaining with array
{
    int iSize; // size of  hash table

    string_node *arr; // actual Hashtable (string nodes)
};

Hash_table init_hash(int size) // for  creating new hash table
{
    Hash_table H = malloc(sizeof(struct My_Hasht));
    H->iSize = size;
    H->arr = malloc(sizeof(string_node) * size);

    for (int i = 0; i < size; i++)
    {
        H->arr[i] = malloc(sizeof(struct string_node));
        H->arr[i]->string_name[0] = '\0';
        H->arr[i]->next = NULL;
        H->arr[i]->root = NULL;
        H->arr[i]->NoOfUsers = 0;
    }

    return H;
}

// function for hash value of particular string
unsigned int gethash(char *s, Hash_table H)
{
    unsigned int hash = 0;
    if (H->iSize == 0)
        return -1;
    else
    {
        while (*s != '\0')
            hash = ((hash << 5) + *s++) % H->iSize;

        return hash;
    }
}

// if Q equals  to null No city/ Organization(string)
//  if not equalto null we return pointer to  string node

string_node Find_in_string(Hash_table H, char *string)
{
    unsigned int hash = gethash(string, H);
    string_node Q = H->arr[hash]->next;
    while (Q != NULL && strcmp(Q->string_name, string) != 0) // traveres if string doesn't match
    {
        Q = Q->next;
    }
    return Q;
}

// Inserting the user in Avl Tree if it already exists
// If not Creates a New Avl Tree for String and insert the user

void Insert_string(Hash_table H, char *string, int id)
{
    string_node Q = Find_in_string(H, string);
    if (Q != NULL)
    {
        Q->root = insert(Q->root, id);
        Q->NoOfUsers++;
        //insert in AVL tree (As every city as distinct ALV tree if we find
        // same city again we insert ID in that particular AVL tree
    }
    else
    {
        unsigned int index = gethash(string, H);
        Q = H->arr[index];
        string_node Q_new = malloc(sizeof(struct string_node));
        strcpy(Q_new->string_name, string); // copying string name in new node
        Q_new->root = NULL;                 // initiating new AVL root for new city
        Q_new->next = Q->next;              // Q->root = insert(Q->root,id);
        Q->next = Q_new;
        Q_new->root = insert(Q_new->root, id);
        Q_new->NoOfUsers++;
    }
}

void Deleting_string_user(Hash_table H, int id, char *string) // take id and city/organization from user node
{
    string_node Q = Find_in_string(H, string);
    Q->root = remove_node(Q->root, id); // removing user from AVL tree of City/Organization
    Q->NoOfUsers--;
}
//call in main before first user registers to allocate a hash table to cities
Hash_table Init_city_Ht()
{
    Hash_table city = init_hash(N);
    return city;
}
//call in main before first user registers to allocate a hash table to organizations
Hash_table Init_organisation_Ht()
{
    Hash_table organization = init_hash(N);
    return organization;
}
void Insert_in_both_string_tables(int userID, char *cityName, char *orgName, Hash_table city, Hash_table organization) //call everytime when a new user registers
{
    Insert_string(city, cityName, userID);
    Insert_string(organization, orgName, userID);
}
