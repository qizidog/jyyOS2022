#include <string.h>
#include <stdlib.h>

// string utils function

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


