#include "mirr_header.h"

int main(int argc,char** argv){

    if(argc < 5) {
        printf("Correct Syntax is %s SourcePath SourceFileName BackupPath BackupFileName\n", argv[0]);
        exit(1);
    }
    char* source_path,*backup_path,*source_file_name,*backup_file_name;
    // get the source path
    source_path = malloc(sizeof(char)*(strlen(argv[1])+1));
    strcpy(source_path,argv[1]);
    // get tha backup path
    backup_path = malloc(sizeof(char)*(strlen(argv[3])+1));
    strcpy(backup_path,argv[3]);
    //get source root filename
    source_file_name = malloc(sizeof(char)*(strlen(argv[2])+1));
    strcpy(source_file_name,argv[2]);
    //get backup root filename
    backup_file_name = malloc(sizeof(char)*(strlen(argv[4])+1));
    strcpy(backup_file_name,argv[4]);

    //Initializing the i-node list
    inode_list Source_inode,Backup_inode;
    Initialize_iNodeList(&Source_inode);
    Initialize_iNodeList(&Backup_inode);

    //Initialize the two trees
    Tree Source,Backup;
    Initialize_Tree(source_path,source_file_name,&Source,&Source_inode);
    Initialize_Tree(backup_path,backup_file_name,&Backup,&Backup_inode);

    Scan_Directoy(source_path,&(Source.root),&Source_inode);
    Scan_Directoy(backup_path,&(Backup.root),&Backup_inode);
    Print_Tree(Source.root);
    Print_Tree(Backup.root);



    Sync_Directory(Source.root,&Source_inode,Backup.root,&Backup_inode);

    // create the inotify instance
    int inotify_fd = inotify_init();
    if(inotify_fd ==-1){
        perror("inotify_init failed");
        exit(-1);
    }
    // create and initalize tha list of the watch discriptors
    Wd_list Monitored_Dirs;
    Initialize_WdList(&Monitored_Dirs);
    //get the directories to monitor
    Monitor_Directories(Source.root,&Monitored_Dirs,inotify_fd);
    Print_WdList(Monitored_Dirs);
    // start listening for events
    Event_listener(inotify_fd,&Monitored_Dirs,&Source,&Source_inode,&Backup,&Backup_inode,source_file_name,backup_file_name);

    Print_Tree(Source.root);
    Print_Tree(Backup.root);

    FreeTree(Source.root);
    FreeTree(Backup.root);
    FreeInodeList(&Source_inode);
    FreeInodeList(&Source_inode);
    free(source_path);
    free(backup_path);
}
