#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/inotify.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#define  BUF_LEN (10* (sizeof(struct inotify_event)+NAME_MAX+1))
#define  MASK (IN_CREATE|IN_ATTRIB|IN_MODIFY|IN_CLOSE_WRITE|IN_DELETE|IN_DELETE_SELF|IN_MOVED_FROM|IN_MOVED_TO)

//list that contains the name of files that are hardlinked to a file (member of each node in an inode_list)
typedef struct NameListNode{
    char* Name;
    struct NameListNode* next;
}Namelist_node;


typedef struct NameList{
    Namelist_node* head;
}Namelist;

//The inode list that contains info for each inode
typedef struct iNode_ListNode{
    ino_t inode_num;
    struct timespec last_mod;
    off_t size;
    int num_of_hard_links;
    struct iNode_ListNode* next;
    struct iNode_ListNode* ptr_to_backup;
    Namelist list_of_hard_links;

}inode_list_node;

typedef struct iNodeList{
    inode_list_node* head;
}inode_list;

//a file struct that represents a file in a tree
typedef struct File{
    char* FileName;
    char* PathName;
    int modified;
    inode_list_node* i_node;

}File;

typedef union Item{
    File* file;
    struct TreeNode*  Directory;
}List_item;

// each node in the list can contain ether a file* or a TreeNode* (another directoty ==> a new level in our tree)
typedef struct ListNode{
    // the union is created to prevent us making two diferent list structures
    List_item item;
    struct ListNode* next;

}List_node;

typedef struct List{
    List_node* head;
}List;

//the number of files and directories in each directory is not predefined so we use list to keep them
typedef struct TreeNode{
    List Files;
    List Directories;
    inode_list_node* i_node;
    char* PathName;
    char* DirName;
}Tree_node;

typedef struct Tree{
    Tree_node* root;
}Tree;

typedef struct Wd_node{
    Tree_node* treenode;
    struct Wd_node* next;
    int wd;
}Wd_node;

typedef struct Wd_List{
    Wd_node* head;
}Wd_list;

void Print_WdList(Wd_list);

void Print_Tree(struct TreeNode*);

void Initialize_List(List*);

void Initialize_iNodeList(inode_list*);

void Initialize_WdList(Wd_list*);

void Insert_in_WdList(Tree_node*,Wd_list*,int);

void Delete_from_WdList(char*,Wd_list*);

Tree_node* Search_in_WdList(int,Wd_list*);

Tree_node* Search_in_Tree(Tree_node*,char*);

void Insert_in_NameList(char*,Namelist*);

void Delete_from_NameList(char* ,Namelist*);

void Delete_all_NameList(Namelist*);

void Delete_from_inode_list(inode_list*,ino_t,char*);

void Insert_in_List(char*,char*,List*,int,inode_list_node*);

void Delete_all_List(List*,int,inode_list*);

void Delete_from_List(List*,int,char*,inode_list*);

void Delete_all_inode_list(inode_list*);

inode_list_node* UpdateList(inode_list*,struct stat,char*,char*);

File* Find_file(Tree_node *,char *);

void Initialize_Tree(char*,char*,Tree*,inode_list*);

void SortFileList(List*);

void SortDirtList(List*);

void Scan_Directoy(char*,Tree_node**,inode_list*);

List_node* Is_file_in_backup(List_node*,List*);

void CopyFile(char*,char*);

int Create_backup_and_copy(char*,char*,mode_t );

void File_was_not_in_backup(List_node*,inode_list*,List*,inode_list*,char*);

void AddFile_in_backup(List*,inode_list*,List*,inode_list*,char*);

int Is_file_in_source(List_node*,List*);

void DeleteFile_from_backup(List*,inode_list*,List*,inode_list*);

List_node* Is_dir_in_backup(List_node*,List*);

void AddDir_in_backup(List*,inode_list*,List*,inode_list*,char*);

void DeleteDir(char*);

int is_dir_in_source(List_node*,List*);

void DeleteDir_from_backup(List*,inode_list*,List*,inode_list*);

void Sync_Directory(Tree_node*,inode_list*,Tree_node*,inode_list*);

void FreeNameList(Namelist*);

void FreeInodeList(inode_list*);

void FreeTree(Tree_node *);

Tree_node* Find_father_of_dir(Tree_node*,char*);

void Monitor_Directories(struct TreeNode*,Wd_list*,int);

int Is_dir_in_WdList(Tree_node*,Wd_list*);

char* Find_from_inodeList(Tree_node*,ino_t);

void event_IN_CREATE(struct inotify_event*,int,Wd_list*,Tree*,inode_list*,Tree*,inode_list*);

void event_IN_ATTRIB(struct inotify_event*,Wd_list*,Tree*,inode_list*,Tree*,inode_list*);

void event_IN_MODIFY(struct inotify_event*,Wd_list*,Tree*,inode_list*,Tree*,inode_list*);

void event_IN_CLOSE_WRITE(struct inotify_event*,Wd_list*,Tree*,inode_list*,Tree*,inode_list*);

void event_IN_DELETE(struct inotify_event*,Wd_list*,Tree*,inode_list*,Tree*,inode_list*,char*,char*);

void event_IN_DELETE_SELF(struct inotify_event*,Wd_list*,Tree*,inode_list*,Tree*,inode_list*);

void Move_list_node(List*,List*,char*);

void event_IN_MOVED_TO(char**,char**,Tree_node*,int,struct inotify_event*,Wd_list*,Tree*,inode_list*,Tree*,inode_list*,char*,char*);

Tree_node* event_IN_MOVED_FROM(char**,char**,int*,int,struct inotify_event*,Wd_list*,Tree*,inode_list*,Tree*,inode_list*,char*,char*);

void Event_listener(int,Wd_list*,Tree*,inode_list*,Tree*,inode_list*,char*,char*);





















