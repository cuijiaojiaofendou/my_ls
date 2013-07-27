/*
 * =====================================================================================
 *
 *       Filename:  my_a.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年07月20日 12时00分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  cuijiaojiao, cuijiaojiaofendou@gmail.com
 *        Company:  Class 1204 of Software
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>


int file_count;
	
	// // 对文件的处理函数
void process_file(char *filename, char *type)
{
    char *pname = filename;
    int name_length = strlen(filename);
    char curr_dir[PATH_MAX];
	//
	// // 指针移动到文件名尾部
    while (*pname != '\0') {
	 pname++;
    }
	//
	// // 文件名移动到文件名中最后一个 . 处
    while (*pname != '.' && name_length--) {
	 pname--;
    }
	//
    if (strcmp(type, pname+1) == 0) {
       if (getcwd(curr_dir, PATH_MAX) == NULL) {
	 perror("getcwd Error");
	 return;
	 }
	 file_count++;
       printf("%s%s\n", curr_dir, filename);
   }
	
}
	//
	// // 递归遍历一个目录
void find_directory(char *dir_name, void (*process_file)(char *, char *), char *type)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
	
    if ((dir = opendir(dir_name)) == NULL) {
	 perror("Open Directory Error");
	 return;
    }
	
	// 改变当前工作目录
    chdir(dir_name);
	
    while ((entry = readdir(dir)) != NULL) 
    {

        if (lstat(entry->d_name, &statbuf) == -1) {
	 perror(entry->d_name);
	 return;
	 }
	// // 如果目标是一个目录
	 if (S_ISDIR(statbuf.st_mode)) {
	// // 注意忽略掉特殊目录 . 和 ..
	 if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
	 continue;
	 }
	// // 对子目录递归查找
	 find_directory(entry->d_name, process_file, type);
	 } else { 
	process_file(entry->d_name, type);
	}
    }

    // 返回上一层目录
    chdir("..");

    // 释放资源
    closedir(dir);
 }

 // 打印使用方法
void print_useage(char *filename)
{
   printf("Useage：\n");
   printf("\t%s path -t types\n", filename);
}

void my_err(const char *err_string, int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}

//获取并打印文件的属性
void display_info(struct stat buf, char *name)
{
    char buf_time[32];
    struct passwd *psd;
    struct group *grp;

    //获取并打印文件类型
    if(S_ISLNK(buf.st_mode)){
       printf("l");
    }else if(S_ISREG(buf.st_mode)){
       printf("-");
    }else if(S_ISDIR(buf.st_mode)){
       printf("d");
    }else if(S_ISCHR(buf.st_mode)){
       printf("c");
    }else if(S_ISBLK(buf.st_mode)){
       printf("b");
    }else if(S_ISFIFO(buf.st_mode)){
       printf("f");
    }else if(S_ISSOCK(buf.st_mode)){
       printf("s");
    }

    //获取并打印文件所有者的权限

    if(buf.st_mode & S_IRUSR){
        printf("r");
    }else{
        printf("-");
    }

    if(buf.st_mode & S_IWUSR){
        printf("w");
    }else{
        printf("-");
    }

    if(buf.st_mode & S_IXUSR){
        printf("x");
    }else{
        printf("-");
    }

//获取同组用户权限
    if(buf.st_mode & S_IRGRP){
        printf("r");
    }else{
        printf("-");
    }

    if(buf.st_mode & S_IWGRP){
        printf("w");
    }else{
        printf("-");
    }

    if(buf.st_mode & S_IXGRP){
        printf("x");
    }else{
        printf("-");
    }

    //获取其他用户权限
    if(buf.st_mode & S_IROTH){
        printf("r");
    }else{
        printf("-");
    }

    if(buf.st_mode & S_IWOTH){
        printf("w");
    }else{
        printf("-");
    }

    if(buf.st_mode & S_IXOTH){
        printf("x");
    }else{
        printf("-");
    }

    printf("    ");

    psd = getpwuid(buf.st_uid);
    grp = getgrgid(buf.st_gid);
    

   
    printf("%-4d",buf.st_nlink);
    printf("%-8s",psd->pw_name);
    printf("   %-8s",grp->gr_name);

    printf("%6d",buf.st_size);
    strcpy(buf_time,ctime(&buf.st_mtime));
    buf_time[strlen(buf_time) - 1] = '\0';   //去掉换行符
    printf(" %s",buf_time);
}


void display_l(const char *path)
{
    DIR *dir;
    struct stat buf;
    struct dirent *ptr;
    char name[254][NAME_MAX - 1];
    int i = 0, j = 0;


    if((dir = opendir(path)) == NULL)
    {
        perror("opendir");
	exit(0);
    }

    while((ptr = readdir(dir)) != NULL )
    { 
	strcpy(name[i], path);
        strcat(name[i], ptr->d_name);
	
    	if(lstat(name[i], &buf) == -1)
    	{
        	my_err("stat",__LINE__);
    	}
        
	if(ptr->d_name[0] != '.')
	{ display_info(buf,name[i]);
	  printf("   %s\n",ptr->d_name);
	  j++;
        }
	i++;
    }

    printf("总量:%d\n",j);
}

void display_a(const char *path)
{
    DIR *dir;
    struct dirent *ptr;
 
    if((dir = opendir(path)) == NULL)
    {
        perror("opendir");
	exit(0);
    }
    while((ptr = readdir(dir)) != NULL)
    {
       printf("%s\n",ptr->d_name);

    }
    closedir(dir);
    
}

int display(const char *path)
{
    DIR *dir;
    struct dirent *ptr;
  

    if((dir = opendir(path)) == NULL)
    {
        perror("opendir");
	return -1;
    }

    while((ptr = readdir(dir)) != NULL)
    {
        if(ptr->d_name[0] != '.'){
	   printf("%s\n",ptr->d_name);
        }
    }
    
    closedir(dir);

    return 0;
}

int main(int argc, char *argv[])
{
	char path[256];
	strcpy(path, "/home/cuijiaojiao/暑假/");
	
	
	if(argc == 1){

           display(path);

	} else if(argc == 2 && strcmp(argv[1], path) != 0 && strcmp(argv[1],"-a") != 0)
	{
           display(argv[1]);
	   
	}else if(argc == 3 && strcmp(argv[1], "-a") == 0)
	{
	    display_a(argv[2]);
	
	}else if(argc == 3 && strcmp(argv[1], "-l") == 0)
	{
	    display_l(argv[2]);
	}
	else if(argc == 4)
	{
	    if(strcmp(argv[2] ,"-t") == 0)
	    {
	        find_directory(argv[1], process_file, argv[3]);
		printf("总量:%d\n",file_count);
	    }else 
	    {
	        print_useage(argv[0]);
	    }
	}

	return EXIT_SUCCESS;
}

