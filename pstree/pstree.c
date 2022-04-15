#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
// #include "strutils.c"

#define PROC_DIR "/proc/"
#define boolean int
#define TRUE 1
#define FALSE 0
// #define itoa(x) 

int ssplit2arr(char *str, char ch, char *p1, char *p2){
    /**
     * 根据指定的字符将字符串分成两个部分，
     * 其中p1和p1需要确保能够提供足够的空间容纳拆分后的字符串
     */
    char *p;
    int ch_n = 0;
    for(p=str; *p; p++, ch_n++){
        if(*p==ch) break;
    }
    if(ch_n==strlen(str)){
        /* printf("没有找到匹配的分割字符"); */
        return -1;
    }
    int len1 = ch_n+1, len2 = strlen(str)-ch_n;
    strncpy(p1, str, len1);         p1[len1-1]='\0';
    strncpy(p2, str+ch_n+1, len2);  p2[len2-1]='\0';
    return 0;
}

int ssplit(char *str, char ch, char **p1, char **p2){
    /**
     * split the string str into two part (p1 and p2) by ch.
     * p1 and p2 is allocated through malloc and need to be free.
     * 
     * str: original string to be split
     * ch: char used to split str
     * p1: pointer to char* of the first part
     * p2: pointer to char* of the second part
     */
    char *p;
    int ch_n = 0;
    for(p=str; *p; p++, ch_n++){
        if(*p==ch) break;
    }
    if(ch_n==strlen(str)){
        /* printf("没有找到匹配的分割字符"); */
        return -1;  // return false
    }

    // memory allocate
    int len1 = ch_n+1, len2 = strlen(str)-ch_n;
    *p1 = (char *) malloc(len1 * sizeof(char));
    *p2 = (char *) malloc(len2 * sizeof(char));
    
    // copy chars
    strncpy(*p1, str, len1);         
    (*p1)[len1-1]='\0';
    strncpy(*p2, str+ch_n+1, len2);
    (*p2)[len2-1]='\0';

    return 0;  // split successfully
}

char* sstrip(char *str, char *chs){
    /**
     * strip each char in chs from the start and end of str.
     * 
     * str: string to strip
     * chs: characters to be strip
     * return: string to return after strip
     */
    char *ret;
    if(*str=='\0'){
        ret = (char*) malloc(sizeof(char) * 1);
        *ret = '\0';
        return ret;
    }
    char *pf = str;
    char *pe = str+strlen(str)-1;  // pointer to the last character except \0

    short match;
    for(; *pf; pf++){
        match = 0;
        for(char *pc=chs; *pc; pc++){
            if(*pc==*pf){
                match = 1;
                break;
            }
        }
        // 如果字符串中这个字符不与任何一个指定字符匹配，break，pf指向第一个保留字符
        if(!match) break;  
    }

    // 如果全部字符都需要被strip
    if(*pf=='\0'){
        ret = (char*) malloc(sizeof(char) * 1);
        *ret = '\0';
        return ret;
    }

    for(; pe>pf; pe--){
        match = 0;
        for(char *pc=chs; *pc; pc++){
            if(*pc==*pe){
                match = 1;
                break;
            }
        }
        // 如果字符串中这个字符不与任何一个指定字符匹配，break，pe指向最后一个保留字符
        if(!match) break;  
    }
    int sl = pe-pf+1;  // strip之后剩余的字符数量
    ret = (char*) malloc(sizeof(char) * (sl+1));
    strncpy(ret, pf, sl);
    ret[sl] = '\0';
    return ret;
}

typedef struct proc{
    char *name;  // 进程名
    int pid;  // 进程id
    int ppid;  // 父进程pid
    struct proc *parent;  // 父进程
    struct proc *next;  // 链表指针
    int child_num;  // 子进程数量
    struct proc *childs[512];  // 子进程
} ProcNode;

static ProcNode *ll_head = NULL;  // linkedlist head

ProcNode* extract_proc_info(char *file_name){
    /** 提取status文件中的进程相关信息，返回保存信息的ProcNode结构体 */
    FILE *f = fopen(file_name, "r");
    if(!f){
        // printf("%s文件打开失败\n", file_name);
        // raise(SIGTERM);
        fclose(f);
        return NULL;
    }

    char buf[128];
    char *p1, *p2, *sp1, *sp2;  // 注意这4个变量都是通过malloc获取的
    ProcNode *proc_info_ptr = (ProcNode*) malloc(sizeof(ProcNode));

    boolean flag_name=FALSE, flag_pid=FALSE, flag_ppid=FALSE;
    while(NULL!=fgets(buf, sizeof(buf), f)){
        ssplit(buf, ':', &p1, &p2);
        sp1 = sstrip(p1, " \t\n");
        sp2 = sstrip(p2, " \t\n");
        free(p1); free(p2);

        if(strcmp("Name", sp1)==0) {  // 进程名
            char* p_name = (char *) malloc((strlen(sp2)+1)*sizeof(char));
            strcpy(p_name, sp2);
            proc_info_ptr->name = p_name;
            flag_name = TRUE;
        } else if(strcmp("Pid", sp1)==0) {  // pid
            proc_info_ptr->pid = atoi(sp2);
            flag_pid = TRUE;
        } else if(strcmp("PPid", sp1)==0) {  // ppid
            proc_info_ptr->ppid = atoi(sp2);
            flag_ppid = TRUE;
        }
        free(sp1); free(sp2);
        if (flag_name && flag_pid && flag_ppid) break;
    }
    proc_info_ptr->parent = NULL;
    proc_info_ptr->next = NULL;
    proc_info_ptr->child_num = 0;
    proc_info_ptr->childs[0] = NULL;

    fclose(f);
    return proc_info_ptr;
}

void add_proc_node(ProcNode *pnode){
    /* 头插法插入进程节点到链表头部 */
    if(pnode==NULL) return;
    pnode->next = ll_head;
    ll_head = pnode;
}

ProcNode* search_proc_node_by_pid(int pid){
    for(ProcNode *pnode=ll_head; pnode!=NULL; pnode=pnode->next){
        if(pnode->pid == pid) return pnode;
    }
    return NULL;  // if no procnode match pid
}

ProcNode* build_proc_tree(){
    /** 将子进程关联到父进程的指针数组中，返回进程pid为1的根进程节点 */
    ProcNode *pnode = ll_head, *ppnode;
    while(pnode!=NULL){
        ppnode = search_proc_node_by_pid(pnode->ppid);
        if(ppnode == NULL) {
            pnode = pnode->next; 
            continue;
        }

        ppnode->childs[(ppnode->child_num)++] = pnode;
        pnode->parent = ppnode;

        pnode = pnode->next;
    }
    return search_proc_node_by_pid(1);
    /* raise(SIGTERM);  // 不可能没有pid为1的进程 */
}

void print_proc_node(ProcNode *pnode, const char *prefix, boolean print_pid){
    /** 打印进程节点信息 */
    if (print_pid) {
        printf("%s%s(%d)", prefix, pnode->name, pnode->pid);
    } else {
        printf("%s%s", prefix, pnode->name);
    }
}

char* build_indence(const char *indence, int n_info, int pid, boolean line){
    /**
     * 构建除进程相关信息、行首符号以外的缩进字符串
     * 包括：既有的indence + 3字符行首符号前缀 + 输出信息占用符 + \0
     * 
     * indence: 既有的indence
     * n_info: 当前进程输出信息占用的字符数量
     * pid: 当前进程pid
     * n_cousin: 当前进程的兄弟进程数量
     * 
     * return: 子进程的缩进字符串
     */
    int mem_calc = strlen(indence) + 3 + n_info + 1;
    char *space = (char*) malloc(mem_calc * sizeof(char));
    space[0] = '\0';

    if (pid != 1){  // pid=1的进程不需要这两项
        strcat(space, indence);
        strcat(space, line ? " │ " : "   ");  // 决定是否输出 │ 符号
    }
    for(int i=0; i<n_info; i++) strcat(space, " ");
    return space;
}

int pid_sort(const void *add1, const void *add2){
    /** 根据pid按增序排列 */
    ProcNode* pnode1 = *(ProcNode**)add1;
    ProcNode* pnode2 = *(ProcNode**)add2;
    return pnode1->pid - pnode2->pid;
}

void print_proc_tree(ProcNode *root, const char *indence, const char *prefix, boolean print_pid, boolean n_sort){
    /**
     * DFS递归打印进程树
     * root: 进程节点
     * indence: 除进程相关信息、行首符号以外的缩进字符串
     * prefix: 行首符号
     * print_pid: 是否输出进程pid，正数输出，否则不输出
     */
    int name_length = strlen(root->name);
    print_proc_node(root, prefix, print_pid);  // 打印进程节点信息
    fflush(stdout);

    if (root->child_num==0) {  // 递归结束条件
        printf("\n");
        return;
    }

    // 构建子进程的缩进字符串
    int n_info = name_length;  // n_info确定存储进程相关信息需要的内存空间
    if (print_pid) {  // 计算pid占用的字符数量
        int nc = 0;
        for(int nu=root->pid; nu!=0; nu/=10){
            nc++;
        }
        n_info  = n_info + nc + 2;
    }
    int n_cousin = 0;
    boolean print_line = 0;
    ProcNode *proot = root->parent;
    if (proot!=NULL) {
        n_cousin = proot->child_num;
        // print_line = (n_cousin>1);  // error
        // 当且仅当拥有兄弟，且该进程不是兄弟中最后一个时，才需要打印 │ 符号
        print_line = (n_cousin>1) && (root != proot->childs[n_cousin-1]);
    }
    const char *space = build_indence(indence, n_info, root->pid, print_line);

    // pid排序
    if(n_sort && root->child_num>1) qsort(root->childs, root->child_num, sizeof(ProcNode*), pid_sort);

    // 确定每一行的行首符号
    const char *cprefix;
    for (int i=0; i<root->child_num; i++){
        if (root->child_num==1){  // 有且只有一个子进程
            cprefix = "───";
        } else if (i==0) {  // 多个子进程中的第一个
            cprefix = "─┬─";
        } else if (i==root->child_num-1){  // 最后一个子进程
            printf("%s", space);  // 打印缩进字符
            cprefix = " └─";
        } else {  // 除首尾两个以外的子进程
            printf("%s", space);  // 打印缩进字符
            cprefix = " ├─";
        }

        print_proc_tree(root->childs[i], space, cprefix, print_pid, n_sort);  // recursive print

        // 释放内存
        free(root->childs[i]->name);
        free(root->childs[i]);
    }
    free((void*)space);
}

void argparse(int argc, char* argv[], boolean *show_pids, boolean *numeric_sort){
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-V")==0 || strcmp(argv[i], "--version")==0){
            char* help_info = "pstree (qizidog) 1.0\n"
                              "Copyright (C) 1996-2022 Werner Almesberger and Craig Small\n"
                              "\n"
                              "qizidog 不提供任何保证。\n"
                              "该程序为自由软件，欢迎你在 GNU 通用公共许可证 (GPL) 下重新发布。\n"
                              "详情可参阅 COPYING 文件。\n";
            fprintf(stderr, "%s", help_info);
            exit(EXIT_SUCCESS);
        }
    }
    for(int i=1; i<argc; i++) {
        assert(argv[i]);
        if(strcmp(argv[i], "-p")==0 || strcmp(argv[i], "--show-pids")==0){
            *show_pids = TRUE;
        } else if(strcmp(argv[i], "-n")==0 || strcmp(argv[i], "--numeric-sort")==0){
            *numeric_sort = TRUE;
        } else {
            fprintf(stderr, "pstree: 未识别的选项 '%s' \n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    assert(!argv[argc]);
}

int main(int argc, char* argv[]){
    // 命令行参数解析
    boolean show_pids=FALSE, numeric_sort=FALSE;
    argparse(argc, argv, &show_pids, &numeric_sort);

    DIR * proc_dir = opendir(PROC_DIR);
    if(proc_dir==NULL) raise(SIGTERM);

    // 读取/proc/{pid}/status文件中的信息，头插法构建单链表
    struct dirent *dirp;
    ProcNode *pnode;
    while ( (dirp = readdir(proc_dir)) != NULL ) {
        if(dirp==NULL) continue;
        if(atoi(dirp->d_name)==0) continue;  // 如果不是以pid作为名字的目录，跳过
        char file_name[256] = PROC_DIR;
        strcat(file_name, dirp->d_name);
	    strcat(file_name, "/status");
        pnode = extract_proc_info(file_name);  // 提取进程信息
        add_proc_node(pnode);  // 头插
    }
    closedir(proc_dir);

    ProcNode *root = build_proc_tree();  // 构建父子进程关系

    const char *indence = "", *prefix = "";
    print_proc_tree(root, indence, prefix, show_pids, numeric_sort);  // 打印进程树
    
    // 回收占用的内存
    // ProcNode *next;
    // for(pnode=ll_head; pnode!=NULL; ) {
    //     next = pnode->next;
    //     free(pnode->name);
    //     free(pnode);
    //     pnode = next;
    // }
    return 0;
}
