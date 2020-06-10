#include "mirr_header.h"


void Print_WdList(Wd_list MyList) {
    Wd_node *temp = MyList.head;
    printf("\n");
    while(temp) {
        printf("dirname = %s-----wd = %d\n", temp->treenode->PathName,temp->wd);
        temp = temp->next;
    }
}

void Print_Tree(struct TreeNode* treenode){
    printf("IN DIRECTORY: %s\n",treenode->PathName);
    printf("FILES:");
    List_node* temp;
    temp = treenode->Files.head;
    while (temp!=NULL){
        printf("%s----",temp->item.file->PathName);
        temp = temp->next;
    }
    printf("\n");
    printf("DIRECTORIES:");
    temp = treenode->Directories.head;
    while (temp!=NULL){
        printf("%s------",temp->item.Directory->PathName);
        temp = temp->next;
    }
    printf("\n");
    temp = treenode->Directories.head;
    while (temp!=NULL){
        Print_Tree(temp->item.Directory);
        temp = temp->next;
    }
}

void Initialize_List(List* MyList){
    MyList->head=NULL;
}

void Initialize_iNodeList(inode_list* MyList){
    MyList->head=NULL;
}

void Initialize_WdList(Wd_list* MyList){
    MyList->head=NULL;
}

void Insert_in_WdList(Tree_node* treenode, Wd_list* MyList,int wd){

    Wd_node* new_node = malloc(sizeof(Wd_node));
    new_node->treenode = treenode;
    new_node->wd = wd;
    //Inserting at the top
    new_node->next = MyList->head;
    MyList->head = new_node;
    printf("%s inserted\n", new_node->treenode->PathName);
}

void Delete_from_WdList(char* dirname, Wd_list* MyList){

    Wd_node* current = MyList->head;
    if(current==NULL)
        return;
    Wd_node* previus = current;
    while (current && strcmp(current->treenode->DirName, dirname)){
        previus = current;
        current = current->next;
    }
    if(current){
        if(current== MyList->head){
            MyList->head = current->next;
        }
        else{
            previus->next = current->next;
        }
        current->next = NULL;
        free(current);
    }
}

Tree_node* Search_in_WdList(int wd, Wd_list* MyList){
    if(MyList->head==NULL)
        return NULL;
    Wd_node* current = MyList->head;
    while(current){
        if(current->wd == wd)
            return current->treenode;
        else
            current=current->next;
    }
    return NULL;

}

Tree_node* Search_in_Tree(Tree_node* tree, char* Name){

    if(tree == NULL)
        return NULL;
    if(!strcmp(tree->DirName,Name))
        return tree;

    else{
        List_node* temp = tree->Directories.head;
        if(temp == NULL)
            return  NULL;

        while (temp){
            Tree_node* Ptr_to_return = Search_in_Tree(temp->item.Directory,Name);
            if(Ptr_to_return!=NULL)
                return Ptr_to_return;
            temp = temp->next;
        }
        return NULL;
    }
}

void Insert_in_NameList(char* pathname,Namelist* MyList){

    Namelist_node* new_node = malloc(sizeof(Namelist_node));
    new_node->Name = malloc(sizeof(char)*(strlen(pathname)+1));
    strcpy(new_node->Name,pathname);
    //Inserting at the top
    new_node->next = MyList->head;
    MyList->head = new_node;
}

void Delete_from_NameList(char* pathname,Namelist* MyList){

    Namelist_node* current = MyList->head;
    if(current==NULL)
        return;
    Namelist_node* previus = current;
    while (current && strcmp(current->Name,pathname)){
        previus = current;
        current = current->next;
    }
    if(current){
        if(current== MyList->head){
            MyList->head = current->next;
        }
        else{
            previus->next = current->next;
        }
        free(current->Name);
        current->next = NULL;
        free(current);
    }

}

void Delete_all_NameList(Namelist* MyList){

    if(MyList->head = NULL)
        return;

    Namelist_node* temp = MyList->head;

    while(temp!=NULL){
        Namelist_node* node_to_delete = temp;
        temp = temp->next;
        node_to_delete->next = NULL;
        free(node_to_delete->Name);
        free(node_to_delete);
    }
    MyList->head = NULL;
}

void Delete_from_inode_list(inode_list* MyList,ino_t inode_num,char* pathname){
    inode_list_node *current = MyList->head;
    if(current == NULL) {
        printf("Inode list is empty\n");
        return;
    }
    inode_list_node *previous = current;
    while(current && current->inode_num != inode_num) {
        previous = current;
        current = current->next;
    }
    //found it
    if(current) {
        if(current->num_of_hard_links ==1){
            // if only one file points in this inode
            if (current == MyList->head)
                MyList->head = current->next;
            else
                previous->next = current->next;
            current->ptr_to_backup = NULL;
            Delete_all_NameList(&(current->list_of_hard_links));
            free(current);
        }
        else{
            // if more than one files are pointing
            current->num_of_hard_links--;
            Delete_from_NameList(pathname,&(current->list_of_hard_links));
        }
    }
}

void Insert_in_List(char* pathname,char* FileName,List* MyList,int type,inode_list_node* inode){
    List_node* new_node = malloc(sizeof(List_node));
    switch(type){
        case 0:{
            new_node->item.file = malloc(sizeof(File));
            new_node->item.file->PathName = (char*)malloc(sizeof(char)*(strlen(pathname)+1));
            strcpy(new_node->item.file->PathName,pathname);
            new_node->item.file->FileName = malloc(sizeof(char)*(strlen(FileName)+1));
            strcpy(new_node->item.file->FileName,FileName);
            new_node->item.file->i_node=inode;
            new_node->item.file->modified = 0;
            break;
        }
        case 1:{
            new_node->item.Directory = malloc(sizeof(Tree_node));
            new_node->item.Directory->DirName = malloc(sizeof(char)*(strlen(FileName)+1));
            strcpy(new_node->item.Directory->DirName,FileName);
            new_node->item.Directory->PathName = malloc(sizeof(char)*(strlen(pathname)+1));
            strcpy(new_node->item.Directory->PathName,pathname);
            new_node->item.Directory->i_node = inode;
            new_node->item.Directory->Files.head = NULL;
            new_node->item.Directory->Directories.head = NULL;
            break;
        }
    }
    new_node->next = MyList->head;
    MyList->head = new_node;
}

void Delete_all_List(List* MyList,int type,inode_list* inode_list){

    if(MyList->head == NULL){
        return;
    }

    switch (type){
        case 0:{
            List_node* temp = MyList->head;
            while(temp!=NULL){
                List_node* node_to_delete = temp;
                temp = temp->next;
                //delete its inode
                Delete_from_inode_list(inode_list,node_to_delete->item.file->i_node->inode_num,node_to_delete->item.file->PathName);
                //free the space
                free(node_to_delete->item.file->PathName);
                free(node_to_delete->item.file->FileName);
                node_to_delete->item.file->i_node = NULL;
                node_to_delete->next = NULL;
                free(node_to_delete->item.file);
                free(node_to_delete);
            }
            MyList->head = NULL;
            break;

        }
        case 1:{
            List_node* temp = MyList->head;
            while(temp!=NULL){
                List_node* node_to_delete = temp;
                temp=temp->next;
                //delete its inode
                Delete_from_inode_list(inode_list,node_to_delete->item.file->i_node->inode_num,node_to_delete->item.file->PathName);
                //free all the space and delete the lists inside
                free(node_to_delete->item.Directory->PathName);
                free(node_to_delete->item.Directory->DirName);
                node_to_delete->item.Directory->i_node = NULL;
                Delete_all_List(&(node_to_delete->item.Directory->Files),0,inode_list);
                Delete_all_List(&(node_to_delete->item.Directory->Directories),1,inode_list);
                free(node_to_delete->item.Directory);
                node_to_delete->next = NULL;
                free(node_to_delete);
            }
            MyList->head = NULL;
            break;

        }
    }
}

void Delete_from_List(List* MyList,int type,char* Name,inode_list* inode_list) {
    List_node *current = MyList->head;
    // the list is empty
    if (current == NULL) {
        printf("The List is empty\n");
        return;
    }

    switch (type) {
        // case 0=> regular file
        case 0: {
            List_node *previus = current;
            // searching for the node
            while (current && strcmp(current->item.file->FileName, Name)) {
                previus = current;
                current = current->next;
            }
            //found the node
            if (current) {
                if (current == MyList->head)
                    MyList->head = current->next;
                else
                    previus->next = current->next;
                // delete its inode from the inode list
                Delete_from_inode_list(inode_list,current->item.file->i_node->inode_num,current->item.file->PathName);
                // free all the allocated space
                free(current->item.file->FileName);
                free(current->item.file->PathName);
                free(current->item.file);
                free(current);
            }
            break;

        }
            // case 1 => Directory
        case 1: {
            List_node *previus = current;
            // searching for the node
            while (current && strcmp(current->item.Directory->DirName, Name)) {
                previus = current;
                current = current->next;
            }
            //found the node
            if (current) {
                if (current == MyList->head)
                    MyList->head = current->next;
                else
                    previus->next = current->next;
                //delete its inode
                Delete_from_inode_list(inode_list,current->item.Directory->i_node->inode_num,current->item.Directory->PathName);
                // free all the allocated space
                current->item.Directory->i_node = NULL;
                free(current->item.Directory->DirName);
                free(current->item.Directory->PathName);
                //delete the two lists we have inside;
                Delete_all_List(&(current->item.Directory->Files),0,inode_list);
                Delete_all_List(&(current->item.Directory->Directories),1,inode_list);

            }
            break;
        }
    }
}

void Delete_all_inode_list(inode_list* MyList){

    if(MyList->head = NULL)
        return;
    inode_list_node* temp = MyList->head;
    while (temp!=NULL){
        inode_list_node* node_to_delete = temp;
        temp = temp->next;

        node_to_delete->next = NULL;
        node_to_delete->ptr_to_backup = NULL;
        Delete_all_NameList(&(node_to_delete->list_of_hard_links));
        free(node_to_delete);
    }
    MyList->head = NULL;
}

inode_list_node* UpdateList(inode_list* MyList,struct stat stat_buf,char* pathname,char* filename){
    inode_list_node* temp = MyList->head;
    while(temp!=NULL){
        // if the inode is already in the list update the fields we need to
        if(stat_buf.st_ino == temp->inode_num){
            temp->num_of_hard_links++;
            Insert_in_NameList(pathname,&(temp->list_of_hard_links));
            return temp;
        }
        else
            temp = temp->next;
    }
    // if its not we just create a new node in the list
    inode_list_node* new_node = malloc(sizeof(inode_list_node));
    new_node->num_of_hard_links=1;
    new_node->inode_num=stat_buf.st_ino;
    new_node->size=stat_buf.st_size;
    new_node->last_mod.tv_sec=stat_buf.st_mtim.tv_sec;
    new_node->ptr_to_backup=NULL;
    new_node->last_mod.tv_nsec=stat_buf.st_mtim.tv_nsec;
    //initialize the list inside
    new_node->list_of_hard_links.head=NULL;
    Insert_in_NameList(pathname,&(new_node->list_of_hard_links));
    new_node->next = MyList->head;
    MyList->head = new_node;
    return new_node;
}

File* Find_file(Tree_node *node,char *name) {
    if(node == NULL) return NULL;

    List_node *current = node->Files.head;

    // if type is 0 we search by filename
        while (current && strcmp(current->item.file->FileName, name))
            current = current->next;
    // else we search by pathname

    if(current) //found
        return current->item.file;

    //not found yet
    if(node->Directories.head) {
        File *file;
        List_node* temp = node->Directories.head;
        while(temp) {
            if((file = Find_file(temp->item.Directory,name)))
                return file;
            temp = temp->next;
        }
    }
    return NULL;
}

void Initialize_Tree(char* pathname,char* filename,Tree* MyTree,inode_list* inode_list){
    struct stat stat_buf;

    MyTree->root = malloc(sizeof(Tree_node));
    Initialize_List(&(MyTree->root->Directories));
    Initialize_List(&(MyTree->root->Files));
    if(stat(pathname,&stat_buf)==-1){
        perror("stat failed");
    }
    MyTree->root->i_node = UpdateList(inode_list,stat_buf,pathname,filename);
    MyTree->root->DirName = malloc(sizeof(char)*(strlen(filename)+1));
    strcpy(MyTree->root->DirName,filename);
    MyTree->root->PathName = malloc(sizeof(char)*(strlen(pathname)+1));
    strcpy(MyTree->root->PathName,pathname);
}

void SortFileList(List* L){
    // a bubbleshort function to order the elements alphabeticly in a fileList
    if(L->head ==NULL){
        return;
    }
    int swapped;

    List_node* current;
    List_node* previus;
    List_node* last = NULL;

    do{
        current = L->head;
        previus = L->head;
        // in each loop at least the last element will be in place
        swapped = 0;
        while (current && current->next !=last){
            swapped = 0;
            if(strcmp(current->item.file->FileName,current->next->item.file->FileName)>0){
                // special treatment for the first node
                if (current == L->head) {
                    //swapping
                    List_node *next = current->next;
                    current->next = next->next;
                    next->next = current;
                    //fixing the head an the previus pointer
                    L->head = next;
                    previus = next;
                } else {
                    //swapping
                    List_node *next = current->next;
                    current->next = next->next;
                    previus->next = next;
                    next->next = current;
                    //moving the previus pointer
                    previus = previus->next;
                }
                swapped = 1;
            }
            if(!swapped)
                //if there was no swap we need to move the pointer(if there is a swap the current pointer is moving anyway)
                if(current==L->head){
                    current = current->next;
                }else{
                    current=current->next;
                    previus=previus->next;
                }
        }
        last = current;

    }while(swapped);
}

void SortDirtList(List* L){
    // a bubbleshort function to order the elements alphabeticly in a fileList
    if(L->head ==NULL){
        return;
    }
    int swapped;

    List_node* current;
    List_node* previus;
    List_node* last = NULL;

    do{
        current = L->head;
        previus = L->head;
        // in each loop at least the last element will be in place
        swapped = 0;
        while (current && current->next !=last){
            swapped = 0;
            if(strcmp(current->item.Directory->DirName,current->next->item.Directory->DirName)>0){
                // special treatment for the first node
                if (current == L->head) {
                    //swapping
                    List_node *next = current->next;
                    current->next = next->next;
                    next->next = current;
                    //fixing the head an the previus pointer
                    L->head = next;
                    previus = next;
                } else {
                    //swapping
                    List_node *next = current->next;
                    current->next = next->next;
                    previus->next = next;
                    next->next = current;
                    //moving the previus pointer
                    previus = previus->next;
                }
                swapped = 1;
            }
            if(!swapped)
                //if there was no swap we need to move the pointer(if there is a swap the current pointer is moving anyway)
                if(current==L->head){
                    current = current->next;
                }else{
                    current=current->next;
                    previus=previus->next;
                }
        }
        last = current;

    }while(swapped);

}

void Scan_Directoy(char* pathname,Tree_node** Directory,inode_list* i_node_list){

    DIR* curr_directory;
    struct dirent* dir_ptr;
    struct stat stat_buf;
    inode_list_node *inode;

    //open the directory
    if((curr_directory=opendir(pathname))==NULL){
        perror("opendir");
        exit(-1);
    }
    //start reading the directoty
    while((dir_ptr=readdir(curr_directory))!=NULL){
        //this will make the function ignore the hidden files
        if(dir_ptr->d_name[0]!='.'){
            // making the pathname of each component of the dir
            char* new_path = malloc(sizeof(char)*((strlen(pathname)+1)+(strlen(dir_ptr->d_name)+1)));
            sprintf(new_path,"%s%s%s",pathname,"/",dir_ptr->d_name);
            // getting the stats
            if(stat(new_path,&stat_buf)==-1){
                perror("Failed to get file status");
                exit(-1);
            }
            else{
                // we have a regular file
                if((stat_buf.st_mode & S_IFMT)==S_IFREG) {
                    printf("REGULAR FILE WITH NAME %s AND INODE NUM %ld\n",new_path,stat_buf.st_ino);
                    // we keep the inode pointer because we need it for the tree
                    inode = UpdateList(i_node_list, stat_buf,new_path,dir_ptr->d_name);
                    // putting the file in the tree
                    Insert_in_List(new_path,dir_ptr->d_name, &((*Directory)->Files), 0, inode);
                }
                    //we have a directory
                else if ((stat_buf.st_mode & S_IFMT)==S_IFDIR){
                    printf("DIRECTORY WITH NAME %s AND INODE NUM %ld\n",new_path,stat_buf.st_ino);
                    inode = UpdateList(i_node_list,stat_buf,new_path,dir_ptr->d_name);
                    Insert_in_List(new_path,dir_ptr->d_name,&(*Directory)->Directories,1,inode);
                    Scan_Directoy(new_path,&((*Directory)->Directories.head->item.Directory),i_node_list);
                }
            }
            free(new_path);
        }
    }
    SortFileList(&((*Directory)->Files));
    SortDirtList(&((*Directory)->Directories));
    closedir(curr_directory);
}

List_node* Is_file_in_backup(List_node* source_node,List* Backup){
    if(Backup->head ==NULL)
        return NULL;
    List_node* backup_node = Backup->head;
    while (backup_node){
        if(!strcmp(backup_node->item.file->FileName,source_node->item.file->FileName))
            return backup_node;
        backup_node = backup_node->next;
    }
    return  NULL;
}

void CopyFile(char* source_path,char* dest_path){
    printf("SOURCE = %s, BACKUP = %s\n\n", source_path, dest_path);

    FILE* source = fopen(source_path,"r");
    FILE* backup = fopen(dest_path,"w");
    if(source==NULL || backup==NULL){
        perror("fopen failed");
        exit(-1);
    }
    int c;
    do {
        c = fgetc(source);
        if( feof(source) ) {
            break ;
        }
        printf("%c", c);
        fputc(c, backup);
    } while(1);

    fclose(source);
    fclose(backup);
}

int Create_backup_and_copy(char* source_pathname,char* backup_pathname,mode_t mode){
    int fd;
    if((fd = creat(backup_pathname,mode)==-1)){
        perror("create new file");
        exit(-1);
    }
    CopyFile(source_pathname,backup_pathname);
    return fd;
}

void File_was_not_in_backup(List_node* source_node,inode_list* Source_inode,List* Backup_file_list,inode_list* Backup_inode,char* Backup_Pathname){
    // construct the pathname of the file to be created in the backup folder
    struct stat stat_buf;
    int newfile_fd;
    char* new_file_path = malloc(sizeof(char)*((strlen(Backup_Pathname)+1)+strlen(source_node->item.file->FileName)));
    sprintf(new_file_path,"%s%s%s",Backup_Pathname,"/",source_node->item.file->FileName);
    //check if the source file was hardlinked
    if(source_node->item.file->i_node->num_of_hard_links!=1){
        // check if the inode has a copy in the backup
        if(source_node->item.file->i_node->ptr_to_backup!=NULL){
            //YES: get the name of one file that is linked to it and link the new file with it
            inode_list_node* backup_inode_ptr;
            backup_inode_ptr = source_node->item.file->i_node->ptr_to_backup;
            Namelist_node* namelist_node = backup_inode_ptr->list_of_hard_links.head;
            link(namelist_node->Name,new_file_path);
        }
        // the inode is not yet created in backup so we just create the file
        newfile_fd=Create_backup_and_copy(source_node->item.file->PathName,new_file_path,0644);
    }
    // the source file was not hard linked so we just create its copy in backup
    else
        newfile_fd=Create_backup_and_copy(source_node->item.file->PathName,new_file_path,0644);
    // after the file in backup is created physically we have to add it to our logical structs
    // get the stats of the file\

    if(stat(new_file_path,&stat_buf)==-1){
        perror("Stat failed");
        exit(-1);
    }

    // create-update the inode
    inode_list_node * new_file_inode = UpdateList(Backup_inode,stat_buf,new_file_path,source_node->item.file->FileName);
    // if there was no older file with the same inode
    if(source_node->item.file->i_node->ptr_to_backup==NULL)
        source_node->item.file->i_node->ptr_to_backup=new_file_inode;
    // insert the file in the tree
    Insert_in_List(new_file_path,source_node->item.file->FileName,Backup_file_list,0,new_file_inode);

}

void AddFile_in_backup(List* Source_file_list,inode_list* Source_inode,List* Backup_file_list,inode_list* Backup_inode,char* Backup_PathName){
    List_node* source_node = Source_file_list->head;
    List_node* backup_node ;
    struct stat stat_buf;

    while (source_node){
        // run the source and fro each file check if it is in the backup
        backup_node = Is_file_in_backup(source_node,Backup_file_list);
        if(backup_node){
            //YES: check if the are the same
            if((source_node->item.file->i_node->last_mod.tv_nsec!= backup_node->item.file->i_node->last_mod.tv_sec)
            || (source_node->item.file->i_node->last_mod.tv_nsec!=backup_node->item.file->i_node->last_mod.tv_nsec)
            || (source_node->item.file->i_node->size!=backup_node->item.file->i_node->size)){
                // they are not the same so we need to behave as the file is backup was never there
                // we unlink the file in the backup
                unlink(backup_node->item.file->PathName);
                Delete_from_List(Backup_file_list,0,backup_node->item.file->FileName,Backup_inode);
                File_was_not_in_backup(source_node,Source_inode,Backup_file_list,Backup_inode,Backup_PathName);
            }
        } else{
            // NO:create the file in the backup
            File_was_not_in_backup(source_node,Source_inode,Backup_file_list,Backup_inode,Backup_PathName);
        }
        source_node = source_node->next;
    }
}

int Is_file_in_source(List_node* backup_node,List* Source){
    List_node* source_node = Source->head;

    while (source_node){
        if(!(strcmp(source_node->item.file->FileName,backup_node->item.file->FileName)))
            return 1;
        source_node = source_node->next;
    }
    return 0;
}

void DeleteFile_from_backup(List* Source,inode_list* Source_inode,List* Backup,inode_list* Backup_inode){
    //check if backup is empty
    if(Backup->head==NULL)
        return;
    //run the backup list and check if a file is not in the source
    List_node* backup_node = Backup->head;
    while (backup_node){
        if(!Is_file_in_source(backup_node,Source)){
            //YES:
            //1.unlink the file
            if(unlink(backup_node->item.file->PathName)==-1){
                perror("Failed to unlink");
                exit(-1);
            }
            //2. delete the file from the tree and tha inode list
            //=> keep the name
            char* name = malloc(sizeof(char)*(strlen(backup_node->item.file->FileName)+1));
            strcpy(name,backup_node->item.file->FileName);
            // we must first move the pointer and then delete or else we will get segfault
            backup_node = backup_node->next;
            Delete_from_List(Backup,0,name,Backup_inode);
            free(name);
        } else
            backup_node = backup_node->next;

    }
    //NO: do nothing
}

List_node* Is_dir_in_backup(List_node* source_node,List* Backup){
    List_node* backup_node = Backup->head;
    while(backup_node){
        if(!strcmp(backup_node->item.Directory->DirName,source_node->item.Directory->DirName))
            return backup_node;
        backup_node = backup_node->next;
    }
    return NULL;
}

void AddDir_in_backup(List* Source,inode_list* Source_inode,List* Backup,inode_list* Backup_inode,char* Backup_Pathname){
    List_node* source_node = Source->head;
    List_node* backup_node;
    struct stat statbuf;
    // check if backup is empty
    if(!Backup->head){
        while (source_node){
            char* newpath = malloc(sizeof(char)*((strlen(Backup_Pathname)+1)+(strlen(source_node->item.Directory->DirName)+1)));
            sprintf(newpath,"%s%s%s",Backup_Pathname,"/",source_node->item.Directory->DirName);
            mkdir(newpath, S_IRWXU | S_IRWXG | S_IRWXO);
            //get the stats of the dir
            if(stat(newpath,&statbuf)==-1){
                perror("stat failed");
                exit(-1);
            }
            //create the inode in the inode list of backup
            inode_list_node* new_inode = UpdateList(Backup_inode,statbuf,newpath,source_node->item.Directory->DirName);
            // insert the dir in the list
            Insert_in_List(newpath,source_node->item.Directory->DirName,Backup,1,new_inode);
            source_node = source_node->next;
        }
    }
    // run the source list and check if a directory is in the backup list
    while (source_node){
        backup_node = Is_dir_in_backup(source_node,Backup);
        if(backup_node){
            //continue
            source_node = source_node->next;
        }
        else{
            // NO: create the directory in backup and the new dir in the inode list
            char* newpath = malloc(sizeof(char)*((strlen(Backup_Pathname)+1)+(strlen(source_node->item.Directory->DirName)+1)));
            sprintf(newpath,"%s%s%s",Backup_Pathname,"/",source_node->item.Directory->DirName);
            mkdir(newpath, S_IRWXU | S_IRWXG | S_IRWXO);
            if(stat(newpath,&statbuf)==-1){
                perror("stat failed");
                exit(-1);
            }
            //create the inode in the inode list of backup
            inode_list_node* new_inode = UpdateList(Backup_inode,statbuf,newpath,source_node->item.Directory->DirName);
            // insert the dir in the list
            Insert_in_List(newpath,source_node->item.Directory->DirName,Backup,1,new_inode);
            source_node = source_node->next;
        }
    }
}

void DeleteDir(char* pathname){
    DIR* current_dir;
    struct dirent* dir_ptr;
    struct stat stat_buf;

    //open the directory
    if((current_dir=opendir(pathname))==NULL){
        perror("opendir");
        exit(-1);
    }
    //start reading the dirr

    while((dir_ptr=readdir(current_dir))!=NULL){
        if(dir_ptr->d_name[0]!='.') {
            char *newpath = malloc(sizeof(char) * ((strlen(pathname) + 1) + (strlen(dir_ptr->d_name) + 1)));
            sprintf(newpath, "%s%s%s", pathname, "/", dir_ptr->d_name);
            if (stat(newpath, &stat_buf) == -1) {
                perror("stat failed");
                exit(-1);
            }
            if ((stat_buf.st_mode & S_IFMT) == S_IFDIR) {
                //its a dir
                //call the function to empty the new dir
                DeleteDir(newpath);
                // delete the current dir
                rmdir(newpath);
            } else
                // its a file so just delete it
                remove(newpath);
            free(newpath);
        }

    }
    closedir(current_dir);

}

int is_dir_in_source(List_node* backup,List* Source){
    List_node* source_node = Source->head;

    while(source_node){
        if (!(strcmp(source_node->item.Directory->DirName,backup->item.Directory->DirName)))
            return 1;
        source_node = source_node->next;
    }
    return 0;

}

void DeleteDir_from_backup(List* Source,inode_list* Source_inode,List* Backup,inode_list* Backup_inode){

    if(Backup->head == NULL)
        return;
    //run the backup list and check if a directory is not in the source list
    List_node* backup_node = Backup->head;
    while (backup_node){
        if(!is_dir_in_source(backup_node,Source)){
            //YES: delete the directory and delete the inode from the inode list
            //1. remove all the stuff from the dir
            DeleteDir(backup_node->item.Directory->PathName);
            //2. delete the stuff from the inode list and the tree
            char* dir_name = malloc(sizeof(char)*(strlen(backup_node->item.Directory->DirName)+1));
            strcpy(dir_name,backup_node->item.Directory->DirName);

            char* path_name = malloc(sizeof(char)*strlen(backup_node->item.Directory->PathName)+1);
            strcpy(path_name,backup_node->item.Directory->PathName);
            // moving the pointer before deleting
            backup_node = backup_node->next;
            Delete_from_List(Backup,1,dir_name,Backup_inode);
            // remove the actuall directory
            rmdir(path_name);
            free(dir_name);
            free(path_name);
        }
        else
            backup_node = backup_node->next;
    }
    //NO: do nothing
}

void Sync_Directory(Tree_node* Source,inode_list* Source_inode,Tree_node* Backup,inode_list* Backup_inode){
    //1.Sync the FileLists of the current Directory we are in
    AddFile_in_backup(&(Source->Files),Source_inode,&(Backup->Files),Backup_inode,Backup->PathName);
    DeleteFile_from_backup(&(Source->Files),Source_inode,&(Backup->Files),Backup_inode);
    //2.Sync the Directories of the current Directory we are in
    AddDir_in_backup(&(Source->Directories),Source_inode,&(Backup->Directories),Backup_inode,Backup->PathName);
    DeleteDir_from_backup(&(Source->Directories),Source_inode,&(Backup->Directories),Backup_inode);
    //3.For each Directory in the current Directory run Sunc_Directory
    List_node* SourceDir = Source->Directories.head;
    for(SourceDir;SourceDir!=NULL;SourceDir=SourceDir->next){
        List_node* BackupDir = Backup->Directories.head;
        for(BackupDir;BackupDir!=NULL;BackupDir=BackupDir->next){
            if(!strcmp(BackupDir->item.Directory->DirName,SourceDir->item.Directory->DirName)){
                Sync_Directory(SourceDir->item.Directory,Source_inode,BackupDir->item.Directory,Backup_inode);
            }
        }
    }

}

void FreeNameList(Namelist* MyList){
    if(MyList->head = NULL)
        return;
    Namelist_node* node = MyList->head;
    Namelist_node* temp = node;
    while(node){
        temp = temp->next;
        free(node->Name);
        free(node);
        node = temp;
    }
    MyList->head = NULL;
}

void FreeInodeList(inode_list* MyList) {
    inode_list_node *node = MyList->head;
    inode_list_node *temp = node;
    while(node) {
        FreeNameList(&(node->list_of_hard_links));
        temp = temp->next;
        free(node);
        node = temp;
    }
}

void FreeTree(Tree_node *Node) {
    List_node *node = Node->Files.head;
    List_node *temp = node;
    while(node) {
        free(node->item.file->FileName);
        free(node->item.file->PathName);
        free(node->item.file);
        temp = temp->next;
        free(node);
        node = temp;
    }
    free(Node->PathName);

    node = Node->Directories.head;
    while (node != NULL){
        FreeTree(node->item.Directory);
        node = node->next;
    }
    free(Node);
}

Tree_node* Find_father_of_dir(Tree_node* node,char* Name){

    List_node* temp;
    Tree_node* Ptr_to_return;


    if(node==NULL)
        return NULL;
    temp = node->Directories.head;
    while (temp){
        if(!strcmp(temp->item.Directory->DirName,Name)){
            return node;
        }
        Ptr_to_return=Find_father_of_dir(temp->item.Directory,Name);
        if(Ptr_to_return!=NULL)
            return Ptr_to_return;
        temp = temp->next;
    }
    return NULL;
}

void Monitor_Directories(struct TreeNode* treenode, Wd_list* WdList,int inotify_fd) {
    printf("IN DIRECTORY: %s\n",treenode->PathName);
    int wd = inotify_add_watch(inotify_fd,treenode->PathName,MASK);
    if(wd == -1){
        perror("inotify_add_watch failed");
        exit(-1);
    }
    Insert_in_WdList(treenode, WdList,wd);
    List_node* temp;
    temp = treenode->Directories.head;
    while (temp!=NULL){
        Monitor_Directories(temp->item.Directory, WdList,inotify_fd);
        temp = temp->next;
    }
}

int Is_dir_in_WdList(Tree_node* my_node, Wd_list* MyList){
    Wd_node* current = MyList->head;
    while (current){
        if(!strcmp(current->treenode->PathName,my_node->PathName))
            return 1;
        current = current->next;
    }
    return 0;
}

char* Find_from_inodeList(Tree_node* tree, ino_t inode_num){
    if(tree == NULL)
        return NULL;
   List_node* curr_file = tree->Files.head;
   // search all the files in the dir;
   while (curr_file!=NULL){
       if(curr_file->item.file->i_node->inode_num == inode_num)
           // found the file
           return curr_file->item.file->PathName;
       curr_file = curr_file->next;
   }
   //go the directories below
   List_node* curr_dir = tree->Directories.head;
   while (curr_dir!=NULL){
       char* path_to_rtrn;
       if((path_to_rtrn=Find_from_inodeList(curr_dir->item.Directory,inode_num))!=NULL){
           return path_to_rtrn;
       }
       curr_dir = curr_dir->next;
   }
    return NULL;
}

void event_IN_CREATE(struct inotify_event* event ,int inotify_fd,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode) {
    if(event->name[0] == '.')
        return;
    printf("event = %s\n", event->name);

    struct stat stat_buf;
    inode_list_node *inode;
    int wd = event->wd;
    Tree_node *treenode = Search_in_WdList(wd, Monitored_Dirs);


    // making the pathname of each component of the dir
    char* new_path = malloc(sizeof(char)*((strlen(treenode->PathName)+1)+strlen(event->name)+1));
    sprintf(new_path,"%s%s%s",treenode->PathName,"/",event->name);
    printf("newpath = %s\n\n", new_path);

    if(stat(new_path,&stat_buf)==-1){
        perror("Failed to get file status");
        exit(-1);
    } else {
        //if it is a file
        if((stat_buf.st_mode & S_IFMT)==S_IFREG) {
            printf("REGULAR FILE WITH NAME %s AND INODE NUM %ld\n",new_path,stat_buf.st_ino);

            // we keep the inode pointer because we need it for the tree
            inode_list_node* source_inode = UpdateList(Source_inode, stat_buf, new_path, event->name);
            // putting the file in the tree
            Insert_in_List(new_path, event->name, &(treenode->Files), 0, source_inode);

            Tree_node* backup_node;
            if(treenode != Source->root)
                backup_node = Search_in_Tree(Backup->root,treenode->DirName);
            else
                backup_node = Backup->root;

            AddFile_in_backup(&(treenode->Files), Source_inode,&(backup_node->Files), Backup_inode, backup_node->PathName);

        }
            //otherwise, if it is a catalog
        else if ((stat_buf.st_mode & S_IFMT)==S_IFDIR){
            printf("DIRECTORY WITH NAME %s AND INODE NUM %ld\n",new_path,stat_buf.st_ino);

            inode_list_node *source_inode;
            // we keep the inode pointer because we need it for the tree
            source_inode = UpdateList(Source_inode, stat_buf, new_path, event->name);
            // putting the file in the tree
            Insert_in_List(new_path, event->name, &(treenode->Directories), 1, source_inode);

            Tree_node* backup_node;
            if(treenode != Source->root)
                backup_node = Search_in_Tree(Backup->root, treenode->DirName);
            else
                backup_node = Backup->root;

            //create a catalog at the destination
            AddDir_in_backup(&(treenode->Directories), Source_inode,\
			&(backup_node->Directories), Backup_inode, backup_node->PathName);
            //add the new catalog in the objects under monitoring
            Monitor_Directories((treenode->Directories.head->item.Directory), Monitored_Dirs, inotify_fd);
        }
    }
    SortFileList(&(Source->root->Files));
    SortDirtList(&(Source->root->Directories));
    SortFileList(&(Backup->root->Files));
    SortDirtList(&(Backup->root->Directories));
    free(new_path);
}

void event_IN_ATTRIB(struct inotify_event* event,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode){
    if(event->name[0] == '.')
        return;
    struct stat stat_buf;
    int wd = event->wd;
    Tree_node *treenode = Search_in_WdList(wd, Monitored_Dirs);


    // making the pathname of each component of the dir
    char* new_path = malloc(sizeof(char)*((strlen(treenode->PathName)+1)+strlen(event->name)+1));
    sprintf(new_path,"%s%s%s",treenode->PathName,"/",event->name);

    if(stat(new_path,&stat_buf)==-1){
        perror("Failed to get file status");
        exit(-1);
    }
    if((stat_buf.st_mode & S_IFMT) == S_IFREG) {

        //If it is a file
        //and the last date of modification has changed
        //update the replica at destination
        File *source, *backup;
        if(!(source = Find_file(treenode,event->name))) {
            perror("Find File Source");
            exit(1);
        }
        if(!(backup = Find_file(Backup->root,event->name))) {
            perror("Find File Backup");
            exit(1);
        }

        if((source->i_node->last_mod.tv_sec!= backup->i_node->last_mod.tv_sec)
           || (source->i_node->last_mod.tv_nsec!= backup->i_node->last_mod.tv_nsec)) {
            printf("Update the replica at destination\n");
            CopyFile(source->PathName, backup->PathName);
            source->i_node->size = stat_buf.st_size;
            source->i_node->last_mod.tv_sec = stat_buf.st_mtim.tv_sec;
            source->i_node->last_mod.tv_nsec = stat_buf.st_mtim.tv_nsec;
            backup->i_node->size= source->i_node->size;
            backup->i_node->last_mod.tv_sec= source->i_node->last_mod.tv_sec;
            backup->i_node->last_mod.tv_nsec= source->i_node->last_mod.tv_nsec;
        }
    }
}

void event_IN_MODIFY(struct inotify_event* event,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode){
    if(event->name[0] == '.')
        return;
    struct stat stat_buf;
    int wd = event->wd;
    Tree_node *treenode = Search_in_WdList(wd, Monitored_Dirs);


    // making the pathname of each component of the dir
    char* new_path = malloc(sizeof(char)*((strlen(treenode->PathName)+1)+(strlen(event->name)+1)));
    sprintf(new_path,"%s%s%s",treenode->PathName,"/",event->name);

    if(stat(new_path,&stat_buf)==-1){
        perror("Failed to get file status");
        exit(-1);
    }
    //If it is a file, mark it as modified
    if((stat_buf.st_mode & S_IFMT) == S_IFREG) {
        File *file;
        if(!(file = Find_file(treenode,event->name))) {
            perror("Find File Source");
            exit(1);
        }
        file->modified = 1;
    }
}

void event_IN_CLOSE_WRITE(struct inotify_event* event,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode){
    if(event->name[0] == '.')
        return;
    struct stat stat_buf;
    int wd = event->wd;
    Tree_node *treenode = Search_in_WdList(wd, Monitored_Dirs);


    // making the pathname of each component of the dir
    char* new_path = malloc(sizeof(char)*((strlen(treenode->PathName)+1)+strlen(event->name)+1));
    sprintf(new_path,"%s%s%s",treenode->PathName,"/",event->name);

    if(stat(new_path,&stat_buf)==-1){
        perror("Failed to get file status");
        exit(-1);
    }
    if((stat_buf.st_mode & S_IFMT) == S_IFREG) {
        File *source, *backup;
        if(!(source = Find_file(Source->root,event->name)))
            exit(1);
        if(!(backup = Find_file(Source->root,event->name)))
            exit(1);

        //If it has been marked as modified, copy it
        if(source->modified) {
            source->modified = 0;
            if((source->i_node->last_mod.tv_nsec != backup->i_node->last_mod.tv_nsec)
               || (source->i_node->last_mod.tv_sec != backup->i_node->last_mod.tv_sec)
               || (source->i_node->size != backup->i_node->size)){
                CopyFile(source->PathName, backup->PathName);
                backup->i_node->size= source->i_node->size;
                backup->i_node->last_mod.tv_sec= source->i_node->last_mod.tv_sec;
                backup->i_node->last_mod.tv_nsec= source->i_node->last_mod.tv_nsec;
            }
        }
    }
}

void event_IN_DELETE(struct inotify_event* event,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode,char* source_root_name,char* backup_root_name){
    int wd = event->wd;
    struct stat stat_buf;
    Tree_node* Backup_dir;
    //get the treenode/directory in witch the event occured
    Tree_node* current_dir = Search_in_WdList(wd,Monitored_Dirs);
    if(current_dir == NULL){
        printf("Search_in_WdList Failed\n");
        exit(-1);
    }

    //check if it is a regular file
    if(event->name[0] == '.')
        return;
    // => remove it from both trees
    char* Dir_name = malloc(sizeof(char)*(strlen(current_dir->DirName)+1));
    strcpy(Dir_name,current_dir->DirName);
    Delete_from_List(&(current_dir->Files),0,event->name,Source_inode);
    if(!strcmp(Dir_name,source_root_name)){
        Backup_dir = Search_in_Tree(Backup->root,backup_root_name);
    }
    else{
        Backup_dir = Search_in_Tree(Backup->root,Dir_name);
    }

    if(Backup_dir ==NULL){
        printf("Serch_in_Tree Failed\n");
        exit(-1);
    }
    char* backup_path = malloc(sizeof(char)*((strlen(Backup_dir->PathName)+1)+(event->len)+1));
    sprintf(backup_path,"%s%s%s",Backup_dir->PathName,"/",event->name);
    Delete_from_List(&(Backup_dir->Files),0,event->name,Backup_inode);
    // => delete the actual file from backup
    unlink(backup_path);
    free(backup_path);
    free(Dir_name);

}

void event_IN_DELETE_SELF(struct inotify_event* event,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode){

    int wd = event->wd;
    Tree_node* current = Search_in_WdList(wd,Monitored_Dirs);
    // find the father dir of the dir that was deleted
    Tree_node* source_father_dir = Find_father_of_dir(Source->root,current->DirName);
    Tree_node* backup_father_dir = Find_father_of_dir(Backup->root,current->DirName);
    if(source_father_dir==NULL || backup_father_dir==NULL){
        printf("Find_father_of_dir failed\n");
        exit(-1);
    }
    // keep the name before deleting
    char* backup_pathname = malloc(sizeof(char)*(strlen(backup_father_dir->PathName)+1)+(strlen(current->DirName)+1));
    sprintf(backup_pathname,"%s%s%s",backup_father_dir->PathName,"/",current->DirName);
    // delete from source and backup tree
    Delete_from_List(&(source_father_dir->Directories),1,current->DirName,Source_inode);
    Delete_from_List(&(backup_father_dir->Directories),1,current->DirName,Backup_inode);
    //delete the actual dir from backup
    DeleteDir(backup_pathname);
    rmdir(backup_pathname);
    free(backup_pathname);
}

void Move_list_node(List* source,List* dest,char* filename){
    List_node * current = source->head;
    List_node * previus = current;
    while (current && strcmp(current->item.file->FileName,filename)){
        previus = current;
        current = current->next;
    }
    if(current){
        if(current == source->head){
            source->head = current->next;
        }
        else{
            previus->next = current->next;
        }
        current->next = NULL;
        //put it at the start of the other list
        if(dest->head==NULL){
            dest->head = current;
        } else{
            current->next = dest->head;
            dest->head = current;
        }
    }
}

void event_IN_MOVED_TO(char** old_source_pathname,char** old_source_filename,Tree_node* Moved_from,int latest_cookie,struct inotify_event* event,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode,char* source_root_name,char* backup_source_name) {
    Tree_node *Moved_to;
    Tree_node *old_Backup_node;
    Tree_node *new_Backup_node;
    //ignore the hidden files
    if (event->name[0] == '.') {
        return;
    }
    if (event->cookie == latest_cookie) {
        //movement within the hierarchy
        //just move the file inside the tree
        //get the destination
        Moved_to = Search_in_WdList(event->wd, Monitored_Dirs);
        //if(Moved_to==Moved_from){
        // the file has just been ranamed
        // find the file and change its name
        File *file_ptr = Find_file(Moved_from,*old_source_filename);
        file_ptr->FileName=(char*)realloc(file_ptr->FileName, sizeof(char)*(strlen(event->name)+1));
        strcpy(file_ptr->FileName,event->name);
        file_ptr->PathName=(char*)realloc(file_ptr->PathName,(sizeof(char)*(strlen(Moved_to->PathName)+1)+(strlen(event->name)+1)));
        sprintf(file_ptr->PathName, "%s%s%s", Moved_to->PathName, "/", event->name);
        //fix the inode namelist
        Delete_from_NameList(*old_source_pathname, &(file_ptr->i_node->list_of_hard_links));
        Insert_in_NameList(file_ptr->PathName, &(file_ptr->i_node->list_of_hard_links));
        // if the file needs to be transfered to another dir
        if (Moved_to != Moved_from){
            Move_list_node(&(Moved_from->Files),&(Moved_to->Files),file_ptr->FileName);
        }

        // do the same for the backup
        if (!strcmp(Moved_from->DirName, source_root_name))
            // if the file was in root
            old_Backup_node = Backup->root;
        else
            old_Backup_node = Search_in_Tree(Backup->root, Moved_from->DirName);

        if(!strcmp(Moved_to->DirName,source_root_name))
            new_Backup_node = Backup->root;
        else
            new_Backup_node = Search_in_Tree(Backup->root,Moved_to->DirName);


        // keep the old path name in backup
        char *old_backup_pathname = malloc(sizeof(char) * ((strlen(old_Backup_node->PathName) + 1) + (strlen(*old_source_filename) + 1)));
        sprintf(old_backup_pathname, "%s%s%s", old_Backup_node->PathName, "/", *old_source_filename);

        char* old_backup_dirname = malloc(sizeof(char)* (strlen(old_Backup_node->DirName)+1));
        strcpy(old_backup_dirname,old_Backup_node->DirName);
        // find the file in backup tree and change the name
        file_ptr = Find_file(old_Backup_node,*old_source_filename);
        file_ptr->PathName = (char*)realloc(file_ptr->PathName,sizeof(char) * ((strlen(old_backup_dirname) + 1) + (strlen(event->name) + 1)));
        file_ptr->FileName = (char*)realloc(file_ptr->FileName,sizeof(char) * (strlen(event->name) + 1));
        strcpy(file_ptr->FileName, event->name);
        sprintf(file_ptr->PathName, "%s%s%s",new_Backup_node->PathName, "/", event->name);
        // also change the name in the inode name list
        Delete_from_NameList(old_backup_pathname, &(file_ptr->i_node->list_of_hard_links));
        Insert_in_NameList(file_ptr->PathName, &(file_ptr->i_node->list_of_hard_links));
        if(Moved_to!=Moved_from){
            Move_list_node(&(old_Backup_node->Files),&(new_Backup_node->Files),*old_source_filename);
        }
        rename(old_backup_pathname, file_ptr->PathName);
        free(old_backup_pathname);
        free(old_backup_dirname);

    }
    else{
        //call event_IN_CREATE
        event_IN_CREATE(event,0,Monitored_Dirs,Source,Source_inode,Backup,Backup_inode);
    }
}

Tree_node* event_IN_MOVED_FROM(char** pathname,char** filename,int *latest_cookie,int is_next_moved_to,struct inotify_event* event,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode,char* source_root_name,char* backup_root_name){
    //if it is a hidden file we dont to anything
    if(event->name[0]=='.')
        return NULL;
    //keep the cookie
    *latest_cookie = event->cookie;
    //check if the next event is moved_to
    if(is_next_moved_to){
        Tree_node* ptr_to_return = Search_in_WdList(event->wd,Monitored_Dirs);
        File* temp = Find_file(ptr_to_return,event->name);
        // keep the old filename and pathname
        *filename = malloc(sizeof(char)*(strlen(temp->FileName)+1));
        strcpy(*filename,temp->FileName);
        *pathname  = malloc(sizeof(char)*(strlen(temp->PathName)+1));
        strcpy(*pathname,temp->PathName);
        return ptr_to_return;
    }

    else{
        // delete the file from the trees
        Tree_node* Backup_dir;
        Tree_node* current_dir = Search_in_WdList(event->wd,Monitored_Dirs);
        if(current_dir==NULL){
            printf("Search_in_WDList failed\n");
            exit(-1);
        }
        char* Dir_name = malloc(sizeof(char)*(strlen(current_dir->DirName)+1));
        strcpy(Dir_name,current_dir->DirName);
        Delete_from_List(&(current_dir->Files),0,event->name,Source_inode);
        if(!strcmp(Dir_name,source_root_name)){
            Backup_dir = Search_in_Tree(Backup->root,backup_root_name);
        }
        else{
            Backup_dir = Search_in_Tree(Backup->root,Dir_name);
        }

        if(Backup_dir ==NULL){
            printf("Serch_in_Tree Failed\n");
            exit(-1);
        }
        char* backup_path = malloc(sizeof(char)*((strlen(Backup_dir->PathName)+1)+(event->len)+1));
        sprintf(backup_path,"%s%s%s",Backup_dir->PathName,"/",event->name);
        Delete_from_List(&(Backup_dir->Files),0,event->name,Backup_inode);
        // => delete the actual file from backup
        unlink(backup_path);
        free(backup_path);
        free(Dir_name);
        return NULL;
    }
}

void Event_listener(int inotify_fd,Wd_list* Monitored_Dirs,Tree* Source,inode_list* Source_inode,Tree* Backup,inode_list* Backup_inode,char* source_root_name,char* backup_root_name){

    ssize_t num_read;
    struct inotify_event* event;
    char buffer[BUF_LEN];

    for(;;){
        printf("Listening for events\n");
        num_read = read(inotify_fd,buffer,BUF_LEN);
        if(num_read== 0 || num_read == -1){
            perror("read failed");
            exit(-1);
        }
        // prosses the events in the buffer
        char* p ;
        char* pathname=NULL,*filename=NULL;
        Tree_node* Moved_from = NULL;
        int latest_cookie;
        for(p = buffer;p<buffer+num_read;){
            event = (struct inotify_event*)p;
            if(event->mask&IN_CREATE){
                printf("IN_CREATE EVENT CAUGHT!\n");
                event_IN_CREATE(event, inotify_fd, Monitored_Dirs, Source, Source_inode, Backup, Backup_inode);

            }
            else if(event->mask&IN_ATTRIB){
                printf("IN_ATTRIB EVENT CAUGHT!\n");
                event_IN_ATTRIB(event, Monitored_Dirs, Source, Source_inode, Backup, Backup_inode);


            }
            else if(event->mask&IN_MODIFY){
                printf("IN_MODIFY EVENT CAUGHT!\n");
                event_IN_MODIFY(event, Monitored_Dirs, Source, Source_inode, Backup, Backup_inode);

            }
            else if(event->mask&IN_CLOSE_WRITE){
                printf("IN_CLOSE_WRITE EVENT CAUGHT!\n");
                event_IN_CLOSE_WRITE(event, Monitored_Dirs, Source, Source_inode, Backup, Backup_inode);

            }
            else if(event->mask&IN_DELETE){
                printf("IN_DELETE EVENT CAUGHT!\n");
                event_IN_DELETE(event,Monitored_Dirs,Source,Source_inode,Backup,Backup_inode,source_root_name,backup_root_name);

            }
            else if(event->mask&IN_DELETE_SELF){
                printf("IN_DELETE_SELF EVENT CAUGHT!\n");
                event_IN_DELETE_SELF(event,Monitored_Dirs,Source,Source_inode,Backup,Backup_inode);
            }
            else if(event->mask&IN_MOVED_FROM){
                printf("IN_MOVED_FROM EVENT CAUGHT!\n");
                //ckeck the next event
                int is_next_moved_to=0;
                char* temp = p;
                temp += sizeof(struct inotify_event)+event->len;
                struct inotify_event* next_event = (struct inotify_event*)temp;
                //we need to know if it is moved_to
                if(next_event->mask&IN_MOVED_TO)
                    is_next_moved_to=1;
                Moved_from =event_IN_MOVED_FROM(&pathname,&filename,&latest_cookie,is_next_moved_to,event,Monitored_Dirs,Source,Source_inode,Backup,Backup_inode,source_root_name,backup_root_name);
                if(is_next_moved_to==0){
                    free(pathname);
                    free(filename);
                    pathname = NULL;
                    filename = NULL;
                }
            }
            else if(event->mask&IN_MOVED_TO){
                printf("IN_MOVED_TO EVENT CAUGHT!\n");
                event_IN_MOVED_TO(&pathname,&filename,Moved_from,latest_cookie,event,Monitored_Dirs,Source,Source_inode,Backup,Backup_inode,source_root_name,backup_root_name);
                if(pathname!=NULL && filename!=NULL){
                    free(pathname);
                    free(filename);
                }
            }
            p += sizeof(struct inotify_event)+event->len;
        }
    }

}
