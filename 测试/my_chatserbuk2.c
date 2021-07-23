#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<string.h>
#include<sys/epoll.h>
#include<signal.h>
#include<fcntl.h>
#include"./cJSON.h"
#include<pthread.h>
#include<mysql/mysql.h>
#include<time.h>
#include<semaphore.h>
#include<sys/sendfile.h>
#include<sys/stat.h>

int  globe_listen;

#define  LISTENQ 1024
#define MAXEVENTS 64
#define buffersize   1024



int efd; 
struct epoll_event event;
struct epoll_event *events=NULL;
//用来控制数据库查询
sem_t sem;


pthread_mutex_t  acount_mutex;
pthread_cond_t acount_cond;


struct sign_up
{
    pthread_t pth;
    int connfd;
    char buf[buffersize];
}sign;
pthread_mutex_t sign_mutex ;
pthread_cond_t sign_cond;
int sign_up_flag = 0;

struct find_back
{
    pthread_t pth;
    int connfd;
    char buf[buffersize];
}fin;
pthread_mutex_t find_mutex;
pthread_cond_t find_cond;
int find_back_flag  = 0;

struct sign_in
{
    pthread_t pth;
    int connfd;
    char buf[buffersize];
}signin;
pthread_mutex_t in_mutex;
pthread_cond_t in_cond;
int sign_in_flag = 0;

struct add_friend
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}add;
pthread_mutex_t add_mutex;
pthread_cond_t add_cond;
int add_friend_flag  = 0;

struct ex
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}ex;
pthread_mutex_t exit_mutex;
pthread_cond_t exit_cond;
int exit_flag = 0;


struct agree
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}agr;

pthread_cond_t agr_cond;
pthread_mutex_t agr_mutex;
int agr_flag = 0;

struct de_fri
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}del_fri;

pthread_cond_t del_cond;
pthread_mutex_t del_mutex;
int del_flag = 0;

struct scan_friend
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}scan_fri;

pthread_cond_t scan_fri_cond;
pthread_mutex_t scan_fri_mutex;
int scan_fri_flag = 0;


struct private_chat
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}pri_chat;

pthread_cond_t pri_chat_cond;
pthread_mutex_t pri_chat_mutex;
int pri_chat_flag = 0;

struct scan_pri_message
{
    pthread_t pth; 
    int connfd;
    char buf[1024];
}scan_pri_msg;

pthread_cond_t scan_pri_cond;
pthread_mutex_t scan_pri_mutex;
int scan_pri_msg_flag = 0;

struct shield_fri
{
    pthread_t pth;
    char buf[1024];
    int connfd;
}shi_fri;

pthread_mutex_t shi_fri_mutex;
pthread_cond_t  shi_fri_cond;
int shi_fri_flag = 0;

struct recover_friend
{
    pthread_t pth;
    char buf[1024];
    int connfd;
}rec_fri;

pthread_mutex_t rec_fri_mutex;
pthread_cond_t rec_fri_cond;
int rec_fri_flag = 0;

struct create_group
{
    pthread_t pth;
    char buf[1024];
    int connfd;
}cre_gro;

pthread_mutex_t cre_gro_mutex;
pthread_cond_t cre_gro_cond;
int cre_gro_flag = 0;

struct delete_group
{
    pthread_t pth;
    char buf[1024];
    int connfd;
}del_gro;

pthread_mutex_t del_gro_mutex;
pthread_cond_t del_gro_cond;
int del_gro_flag = 0;

struct join_group
{
    pthread_t pth;
    char buf[1024];
    int connfd;
}join_gro;


pthread_mutex_t  join_gro_mutex;
pthread_cond_t  join_gro_cond;
int join_gro_flag = 0;

struct agree_join_group
{
    pthread_t pth;
    char buf[1024];
    int connfd;
}agr_joi_gro;

pthread_mutex_t agr_joi_gro_mutex;
pthread_cond_t agr_joi_gro_cond;
int agr_joi_gro_flag = 0;

struct scan_group
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}sca_gro;

pthread_mutex_t sca_gro_mutex;
pthread_cond_t sca_gro_cond;
int sca_gro_flag = 0;


struct scan_group_people
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}sca_gro_peo;

pthread_mutex_t sca_gro_peo_mutex;
pthread_cond_t sca_gro_peo_cond;
int sca_gro_peo_flag = 0;

struct set_admin
{
    pthread_t pth;
    int connfd;
    char buf[1024];

}set_adm;

pthread_mutex_t set_adm_mutex;
pthread_cond_t set_adm_cond;
int set_adm_flag = 0;

struct kick
{
    pthread_t pth;
    int connfd;
    char buf[1024];
}kic;

pthread_mutex_t kic_mutex;
pthread_cond_t kic_cond;
int kic_flag = 0;
sem_t kick_sem;

struct quit_group
{
    char buf[1024];
    int connfd;
    pthread_t pth;
}qui_gro;

pthread_mutex_t  qui_gro_mutex;
pthread_cond_t qui_gro_cond;
int qui_gro_flag = 0;

struct group_chat
{
    char buf[1024];
    int connfd;
    pthread_t pth;
}gro_cha;

pthread_mutex_t gro_cha_mutex;
pthread_cond_t gro_cha_cond;
int gro_cha_flag = 0;
//看群聊记录
struct scan_group_chat_message
{
    char buf[1024];
    int connfd;
    pthread_t pth;
}sca_gro_cha_mes;

pthread_mutex_t sca_gro_cha_mes_mutex;
pthread_cond_t sca_gro_cha_mes_cond;
int sca_gro_cha_mse_flag  =0;

//传输文件
struct deliver_file
{
    char buf[1024];
    int connfd;
    pthread_t pth;
}del_fil;

pthread_mutex_t del_fil_mutex;
pthread_cond_t del_fil_cond;
int del_fil_flag=0;

struct receive_file
{
    int connfd;
    char buf[1024];
    pthread_t pth;
}rec_fil;

pthread_mutex_t rec_fil_mutex;
pthread_cond_t rec_fil_cond;
int rec_fil_flag = 0;

//连接数据库
MYSQL mysql;                                                              

//接受文件
void*
receive_file(void *arg);

//写服务器日志的函数
void 
write_log(char *str);

//错误处理函数
void
my_err(char *string,int line);

int 
open_listenfd(int port);


//将文件描述符设置为非阻塞状态
int
setnonblocking(int fd);

int 
my_read(int connfd,char *buf1,char *buf2);

//解析输入
int 
find_function(int connfd,char *buf);

//注册用的函数
void*
sign_up(void*arg);
//找回密码

void*
find_back(void*arg);

void*
sign_in(void*arg);

void *
add_friend(void *arg);

void *
my_exit(void *arg);

void*
agree(void *arg);

void *
delete_fri(void *arg);

void*
scan_friend(void*arg);

void*
private_chat(void*arg);

void*
scan_pri_message(void * arg);

void *
shield_friend(void*);

//恢复好友
void *
recover_friend(void*);

//建群
void*
create_group(void*);

//解散群
void*
delete_group(void*);

void*
join_group(void*);

//同意加群
void*
agree_join_group(void*);

//看自己加的群
void *
scan_group(void*);

//看群里面的好友
void*
scan_group_people(void*);

//设置管理员
void*
set_admin(void*);

//踢人功能
void*
kick(void*);

//退群
void*
quit_group(void*);

//群聊
void*
group_chat(void*);

//查看群聊记录
void*
scan_group_chat_message(void*);

//传输文件
void*
deliver_file (void*);

int 
main(int argc,char*argv[])
{
    
    //初始化信号量
    sem_init(&sem,0,1);
    //这个信号量用于更新群的人数
    sem_init(&kick_sem,0,1);
    signal(SIGPIPE,SIG_IGN);
    if(mysql_init(&mysql)==NULL)
         my_err("mysql_init",__LINE__);
     if(mysql_library_init(0,NULL,NULL)!=0)
         my_err("mysql_library_init",__LINE__);
     if(NULL==mysql_real_connect(&mysql,"127.0.0.1","root","123456","my_chat",0,NULL,0))
         my_err("mysql_real_connect",__LINE__);
     if(mysql_set_character_set(&mysql,"utf8")<0)
         my_err("mysql_set_character_set",__LINE__);
    write_log("数据库连接成功");

    pthread_mutex_init(&rec_fil_mutex,NULL);
    pthread_cond_init(&rec_fil_cond,NULL);

    pthread_mutex_init(&sign_mutex,NULL);
    pthread_cond_init(&sign_cond,NULL);
    
    pthread_mutex_init(&find_mutex,NULL);
    pthread_cond_init(&find_cond,NULL);

    pthread_mutex_init(&acount_mutex,NULL);
    pthread_cond_init(&acount_cond,NULL);

    pthread_mutex_init(&in_mutex,NULL);
    pthread_cond_init(&in_cond,NULL);
    
    pthread_mutex_init(&add_mutex,NULL);
    pthread_cond_init(&add_cond,NULL);

    pthread_mutex_init(&exit_mutex,NULL);
    pthread_cond_init(&exit_cond,NULL);
    
    pthread_mutex_init(&agr_mutex,NULL);
    pthread_cond_init(&agr_cond,NULL);
    
    pthread_mutex_init(&del_mutex,NULL);
    pthread_cond_init(&del_cond,NULL);

    pthread_mutex_init(&scan_fri_mutex,NULL);
    pthread_cond_init(&scan_fri_cond,NULL);

    pthread_mutex_init(&pri_chat_mutex,NULL);
    pthread_cond_init(&pri_chat_cond,NULL);
    
    pthread_mutex_init(&scan_pri_mutex,NULL);
    pthread_cond_init(&scan_pri_cond,NULL);

    pthread_mutex_init(&shi_fri_mutex,NULL);
    pthread_cond_init(&shi_fri_cond,NULL); 

    pthread_mutex_init(&rec_fri_mutex,NULL);
    pthread_cond_init(&rec_fri_cond,NULL);
    
    pthread_mutex_init(&cre_gro_mutex,NULL);
    pthread_cond_init(&cre_gro_cond,NULL);
        
    pthread_mutex_init(&del_gro_mutex,NULL);
    pthread_cond_init(&del_gro_cond,NULL);
    
    pthread_mutex_init(&join_gro_mutex,NULL);
    pthread_cond_init(&join_gro_cond,NULL);
    
    pthread_mutex_init(&agr_joi_gro_mutex,NULL);
    pthread_cond_init(&agr_joi_gro_cond,NULL);

    pthread_mutex_init(&sca_gro_mutex,NULL);
    pthread_cond_init(&sca_gro_cond,NULL);
    
    pthread_mutex_init(&sca_gro_peo_mutex,NULL);
    pthread_cond_init(&sca_gro_peo_cond,NULL);
    
    pthread_mutex_init(&set_adm_mutex,NULL);
    pthread_cond_init(&set_adm_cond,NULL);
    
    pthread_mutex_init(&kic_mutex,NULL);
    pthread_cond_init(&kic_cond,NULL);
    
    pthread_mutex_init(&qui_gro_mutex,NULL);
    pthread_cond_init(&qui_gro_cond,NULL);
    
    pthread_mutex_init(&gro_cha_mutex,NULL);
    pthread_cond_init(&gro_cha_cond,NULL);

    pthread_mutex_init(&sca_gro_cha_mes_mutex,NULL);
    pthread_cond_init(&sca_gro_cha_mes_cond,NULL);

    pthread_mutex_init(&del_fil_mutex,NULL);
    pthread_cond_init(&del_fil_cond,NULL);


    char buf1[buffersize];
    char buf2[buffersize];
    struct sockaddr_in addr;
    socklen_t len=sizeof(addr);
    int port = atoi(argv[1]);
    int listenfd = open_listenfd(port);
    globe_listen = listenfd;
    if((efd=epoll_create(1))==-1)
        my_err("epoll_create",__LINE__);
    event.data.fd = listenfd;
    event.events = EPOLLIN;   
    if(epoll_ctl(efd,EPOLL_CTL_ADD,listenfd,&event)==-1)
    my_err("epoll_ctl",__LINE__);
    events = (struct epoll_event*)calloc(MAXEVENTS,sizeof(struct epoll_event));
    if(events==NULL)
        my_err("calloc",__LINE__);
    int connfd;
    while(1)
    {
        printf("进入epoll循环\n");
        int n,i;
        n = epoll_wait(efd,events,MAXEVENTS,-1);
        for(i = 0;i<n;i++)
        {
             if(events[i].data.fd == listenfd)
            {
              connfd = accept(listenfd,(struct sockaddr*)&addr,&len);
              if(connfd<0)
                    my_err("accept",__LINE__);
              event.data.fd = connfd;
              event.events = EPOLLIN|EPOLLET;
              setnonblocking(connfd);
              epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&event);
              sprintf(buf1,"监听到心的文件描述符:%d",connfd);
              write_log(buf1);
            }
            else if (events[i].events&EPOLLIN)
            {
                memset(buf2,0,strlen(buf2));
                recv(events[i].data.fd,buf2,buffersize,0);
                write_log("服务器接受到消息");
                printf("%s\n",buf2);
                find_function(events[i].data.fd,buf2);
            }
            
        }
    }
    close(efd);
    return 0;
}


void *
find_back(void *arg)
{

    struct find_back a=fin;
    while(find_back_flag ==0)
    pthread_cond_broadcast(&find_cond);
    find_back_flag = 0;
    pthread_detach(pthread_self());
    char cmd[1024];
    char question[256];
    char answer[1024];
    char password[256];
    char buf[1024];
    cJSON *root =NULL;
    cJSON *item = NULL;
    root = cJSON_Parse(a.buf);
    item = cJSON_GetObjectItem(root,"find_back");
    sprintf(cmd,"select *from user_data where account = \"%s\"",item->valuestring);
    printf("%s\n",cmd);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    MYSQL_RES *res = mysql_store_result(&mysql);
    sem_post(&sem);
    MYSQL_ROW row = mysql_fetch_row(res);
    printf("row = %d\n",row);
    if(row == NULL)
    {
        sprintf(buf,"{\"find_back\":{\"question\":\"null\",\"answer\":\"null\",\"password\":\"null\"}}");
        send(a.connfd,buf,1024,0);
        return 0;
    }
    sprintf(question,"%s",row[3]);
    sprintf(answer,"%s",row[4]);
    sprintf(password,"%s",row[2]);
    sprintf(buf,"{\"find_back\":{\"question\":\"%s\",\"answer\":\"%s\",\"password\":\"%s\"}}",question,answer,password);
    printf("%s\n",buf);
    send(a.connfd,buf,1024,0);
    return NULL;
}


void *
sign_in(void *arg)
{
    
    struct sign_in in  = signin;
    while(sign_in_flag == 0)
    pthread_cond_signal(&in_cond);
    sign_in_flag = 0;
    pthread_detach(pthread_self());
    char id[30];
    char password1[30];
    char password2[30];
    char cmd[1024];
    char buf[1024];
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    root = cJSON_Parse(in.buf);
    item = cJSON_GetObjectItem(root,"sign_in");
    next = cJSON_GetObjectItem(item,"id");
    sprintf(id,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"passwd");
    sprintf(password1,"%s",next->valuestring);
    sprintf(cmd,"select *from user_data where account = \"%s\"",id);
    sem_wait(&sem);
    int n=mysql_query(&mysql,cmd);
    if(n!=0)
    my_err("mysql_query",__LINE__);
    MYSQL_RES *res = mysql_store_result(&mysql);
    mysql_num_rows(res);
    MYSQL_ROW row = mysql_fetch_row(res);
    sem_post(&sem);
    if(!row)
    {
        strcpy(buf,"{\"sign_find\":\"0\"}");
        send(in.connfd,buf,1024,0);
        return 0;
    }
        sprintf(password2,"%s",row[2]);
        if(0==strcmp(password1,password2))
        {


                sem_wait(& sem);
                strcpy(buf,"{\"sign_in\":\"1\"}");
                send(in.connfd,buf,1024,0);
                sprintf(cmd,"update user_data set socket = %d where account = %s",in.connfd,id);
                mysql_query(&mysql,cmd);
                sprintf(cmd,"update user_data set state = 1 where account = %s",id);
                mysql_query(&mysql,cmd);
                sem_post(&sem);
                sprintf(buf,"{\"name\":{\"name\":\"%s\",\"account\":\"%s\"}}",row[1],row[0]);
                send(in.connfd,buf,1024,0);
                sprintf(buf,"用户%s登陆成功",id);
                write_log(buf);
        }
        else 
        {
                strcpy(buf,"{\"sign_in\":\"0\"}");
                send(in.connfd,buf,1024,0);
                sprintf(buf,"用户%s登陆失败",id);
                write_log(buf);
        }
        return 0;

}

void*
sign_up(void *arg)
{
    
    struct sign_up si = *(struct sign_up*)arg;
    while(sign_up_flag==0)
    pthread_cond_signal(&sign_cond);
    sign_up_flag = 0;
    pthread_detach(pthread_self());
    pthread_detach(si.pth);
    char buf[1024];
    char name[30];
    char password[30];
    char question[256];
    char answer[256];
    char acount_number[20];
    cJSON * root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL;
    root = cJSON_Parse(si.buf);
    item = cJSON_GetObjectItem(root,"sign_up");
    next = cJSON_GetObjectItem(item,"name");
    sprintf(name,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"password");
    sprintf(password,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"question");
    sprintf(question,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"answer");
    sprintf(answer,"%s",next->valuestring);
    
    char cmd[256];
    sprintf(cmd,"select *from account_number");
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    int number =atoi(row[0]);
    printf("account_account = %d\n",number);
    sprintf(buf,"{\"sign_up\":\"%d\"}",number);
    send(si.connfd,buf,1024,0);
    number++;
    sprintf(cmd,"update account_number set account_number = %d",number);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
    {
        printf("hello world\n");
        sleep(1);
    }
    printf("account_number  = %d\n",number);
    sem_post(&sem);
    sem_wait(&sem);
    sprintf(cmd,"insert user_data values(\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",0,-1)",number-1,name,password,question,answer);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);
    return NULL;
}


void*
agree(void *)
{
        struct agree  tp = agr;
        while(agr_flag == 0)
        pthread_cond_signal(&agr_cond);
        agr_flag = 1;
        pthread_detach(pthread_self());
        char buf[1024];
        char cmd[1024];
        char fir_account[20];
        char use_account[20];
        cJSON * root =NULL;
        cJSON * item = NULL;
        cJSON * next = NULL;
        root = cJSON_Parse(tp.buf);
        item = cJSON_GetObjectItem(root,"agree");
        next  = cJSON_GetObjectItem(item,"account1");
        sprintf(use_account,"%s",next->valuestring);
        next = cJSON_GetObjectItem(item,"account2");
        sprintf(fir_account,"%s",next->valuestring);
        sprintf(cmd,"select *from add_friend where usr_id = \"%s\" and friend_id = \"%s\"",fir_account,use_account);
        sem_wait(&sem);
        mysql_query(&mysql,cmd);
        MYSQL_RES *res = mysql_store_result(&mysql);
        MYSQL_ROW row  = mysql_fetch_row(res);
        sem_post(&sem);
        if(row==NULL)
        {
            strcpy(buf,"{\"agr\":\"对方没有发送添加好友消息\"}");
            send(agr.connfd,buf,1024,0);
            sprintf(buf,"添加好友，对方没有发送添加好友的消息");
            write_log(buf);
            return 0;
        }
        else 
        {
            sprintf(cmd,"insert into friends values(\"%s\",\"%s\",\"0\")",fir_account,use_account);
            sem_wait(&sem);
            mysql_query(&mysql,cmd);
            sem_post(&sem);
            sprintf(cmd,"insert into friends values(\"%s\",\"%s\",\"0\")",use_account,fir_account);
            sem_wait(&sem);
            mysql_query(&mysql,cmd);
            sem_post(&sem);
            strcpy(buf,"{\"agr\":\"添加好友成功\"}");
            send(agr.connfd,buf,1024,0);
            sprintf(cmd,"delete from add_friend where usr_id = \"%s\" and friend_id = \"%s\"",fir_account,use_account);
            sem_wait(&sem);
            if(mysql_query(&mysql,cmd)!=0)
                my_err("mysql_query",__LINE__);
            sem_post(&sem);
            sprintf(cmd,"添加好友成功");
            write_log(cmd);
            return 0;
        }

}


int
find_function(int connfd,char *str)
{
    char buf[30];
    int j = 0;
    char *find = strchr(str,':');
    if(find == NULL)   //没有输入正确直接退出
        return 0;
    for(char * i = str+2;i!=find;i++)
    {
       if(*i!='\"')
       {    
           buf[j]=*i;
           j++;
        }
    }
    buf[j]='\0';
    if(strcmp(buf,"sign_up")==0)
    {
        
        sign.connfd=connfd;
        strcpy(sign.buf,str);
        pthread_create(&sign.pth,NULL,sign_up,(void*)&sign);
        pthread_mutex_lock(&sign_mutex);
        pthread_cond_wait(&sign_cond,&sign_mutex);
        sign_up_flag = 1;
        pthread_mutex_unlock(&sign_mutex);
    }
    else if(strcmp(buf,"find_back")==0)
    {
        fin.connfd = connfd;
        strcpy(fin.buf,str);
        pthread_create(&fin.pth,NULL,find_back,NULL);
        pthread_mutex_lock(&find_mutex);
        pthread_cond_wait(&find_cond,&find_mutex);
        find_back_flag = 1;
        pthread_mutex_unlock(&find_mutex);
    }
    else if(strcmp(buf,"sign_in")==0)
    {
        signin.connfd = connfd;
        strcpy(signin.buf,str);
        pthread_create(&signin.pth,NULL,sign_in,NULL);
        pthread_mutex_lock(&in_mutex);
        pthread_cond_wait(&in_cond,&in_mutex);
        sign_in_flag = 1;
        pthread_mutex_unlock(&in_mutex);
        
    }
    else if(strcmp(buf,"quit")==0)
    {
        
        epoll_ctl(efd,EPOLL_CTL_DEL,connfd,&event);
        close(connfd);
        return 0;
    }
    else if(strcmp(buf,"add_friend")==0)
    {
        add.connfd = connfd;
        strcpy(add.buf,str);
        pthread_create(&add.pth,NULL,add_friend,NULL);
        pthread_mutex_lock(&add_mutex);
        pthread_cond_wait(&add_cond,&add_mutex);
        add_friend_flag = 1;
        pthread_mutex_unlock(&add_mutex);
        
    }
    else if(strcmp(buf,"my_exit")==0)
    {
        ex.connfd = connfd;
        strcpy(ex.buf,str);
        pthread_create(&ex.pth,NULL,my_exit,NULL);
        pthread_mutex_lock(&exit_mutex);
        pthread_cond_wait(&exit_cond,&exit_mutex);
        exit_flag = 1;
        pthread_mutex_unlock(&exit_mutex);

    }
    else if(strcmp(buf,"agree")==0)
    {

        agr.connfd = connfd;
        strcpy(agr.buf,str);
        pthread_create(&agr.pth,NULL,agree,NULL);
        pthread_mutex_lock(&agr_mutex);
        pthread_cond_wait(&agr_cond,&agr_mutex);
        agr_flag = 1;
        pthread_mutex_unlock(&agr_mutex);

    }
    else if(strcmp(buf,"delete_fri")==0)
    {
        del_fri.connfd = connfd;
        strcpy(del_fri.buf,str);
        pthread_create(&del_fri.pth,NULL,delete_fri,NULL);
        pthread_mutex_lock(&del_mutex);
        pthread_cond_wait(&del_cond,&del_mutex);
        del_flag = 1;
        pthread_mutex_unlock (&del_mutex);
    }
    else if(strcmp(buf,"scan_fri")==0)
    {
        scan_fri.connfd = connfd;
        strcpy(scan_fri.buf,str);
        pthread_create(&scan_fri.pth,NULL,scan_friend,NULL);
        pthread_mutex_lock(&scan_fri_mutex);
        pthread_cond_wait(&scan_fri_cond,&scan_fri_mutex);
        scan_fri_flag = 1;
        pthread_mutex_unlock(&scan_fri_mutex);

    }
    else if(strcmp(buf,"pri_chat")==0)
    {
        pri_chat.connfd = connfd;
        strcpy(pri_chat.buf,str);
        pthread_create(&pri_chat.pth,NULL,private_chat,NULL);
        pthread_mutex_lock(&pri_chat_mutex);
        pthread_cond_wait(&pri_chat_cond,&pri_chat_mutex);
        pri_chat_flag = 1;
        pthread_mutex_unlock(&pri_chat_mutex);

    }
    else if(strcmp(buf,"scan_msg")==0)
    {
        scan_pri_msg.connfd = connfd;
        strcpy(scan_pri_msg.buf,str);
        pthread_create(&scan_pri_msg.pth,NULL,scan_pri_message,NULL);
        pthread_mutex_lock(&scan_pri_mutex);
        pthread_cond_wait(&scan_pri_cond,&scan_pri_mutex);
        scan_pri_msg_flag = 1;
        pthread_mutex_unlock(&scan_pri_mutex);
        
    }
    else if(strcmp(buf,"shi_fri")==0)
    {
        shi_fri.connfd = connfd;
        strcpy(shi_fri.buf,str);
        pthread_create(&shi_fri.pth,NULL,shield_friend,NULL);
        pthread_mutex_lock(&shi_fri_mutex);
        pthread_cond_wait(&shi_fri_cond,&shi_fri_mutex);
        shi_fri_flag = 1;
        pthread_mutex_unlock(&shi_fri_mutex);
    }
    else if(strcmp(buf,"recover_fri")==0)
    {
        rec_fri.connfd = connfd;
        strcpy(rec_fri.buf,str);
        pthread_create(&rec_fri.pth,NULL,recover_friend,NULL);
        pthread_mutex_lock(&rec_fri_mutex);
        pthread_cond_wait(&rec_fri_cond,&rec_fri_mutex);
        rec_fri_flag = 1;
        pthread_mutex_unlock(&rec_fri_mutex);
    }
    else if(strcmp(buf,"create_group")==0)
    {
        cre_gro.connfd = connfd;
        strcpy(cre_gro.buf,str);
        pthread_create(&cre_gro.pth,NULL,create_group,NULL);
        pthread_mutex_lock(&cre_gro_mutex);
        pthread_cond_wait(&cre_gro_cond,&cre_gro_mutex);
        cre_gro_flag = 1;
        pthread_mutex_unlock(&cre_gro_mutex);

    }
    else if(strcmp(buf,"delete_group")==0)
    {
        del_gro.connfd = connfd;
        strcpy(del_gro.buf,str);
        pthread_create(&del_gro.pth,NULL,delete_group,NULL);
        pthread_mutex_lock(&del_gro_mutex);
        pthread_cond_wait(&del_gro_cond,&del_gro_mutex);
        del_gro_flag = 1;
        pthread_mutex_unlock(&del_gro_mutex);
    }
    else if(strcmp(buf,"join_group")==0)
    {
        join_gro.connfd = connfd;
        strcpy(join_gro.buf,str);
        pthread_create(&join_gro.pth,NULL,join_group,NULL);
        pthread_mutex_lock(&join_gro_mutex);
        pthread_cond_wait(&join_gro_cond,&join_gro_mutex);
        join_gro_flag = 1;
        pthread_mutex_unlock(&join_gro_mutex);

    }
    else if(strcmp(buf,"agree_join_group")==0)
    {
        agr_joi_gro.connfd = connfd;
        strcpy(agr_joi_gro.buf,str);
        pthread_create(&agr_joi_gro.pth,NULL,agree_join_group,NULL);
        pthread_mutex_lock(&agr_joi_gro_mutex);
        pthread_cond_wait(&agr_joi_gro_cond, &agr_joi_gro_mutex);
        agr_joi_gro_flag = 1;
        pthread_mutex_unlock(&agr_joi_gro_mutex);
    }
    else if(strcmp(buf,"scan_group")==0)
    {
        sca_gro.connfd = connfd;
        strcpy(sca_gro.buf,str);
        pthread_create(&sca_gro.pth,NULL,scan_group,NULL);
        pthread_mutex_lock(&sca_gro_mutex);
        printf("主线程等待\n");
        pthread_cond_wait(&sca_gro_cond,&sca_gro_mutex);
        sca_gro_flag = 1;
        pthread_mutex_unlock(&sca_gro_mutex);
        printf("主线程被唤醒\n");
    }
    else if(strcmp(buf,"scan_group_people")==0)
    {
        sca_gro_peo.connfd = connfd;
        strcpy(sca_gro_peo.buf,str);
        pthread_create(&sca_gro_peo.pth,NULL,scan_group_people,NULL);
        pthread_mutex_lock(&sca_gro_peo_mutex);
        pthread_cond_wait(&sca_gro_peo_cond,&sca_gro_peo_mutex);
        sca_gro_peo_flag = 1;
        pthread_mutex_unlock(&sca_gro_peo_mutex);
    }
    else if(strcmp(buf,"set_admin")==0)
    {
        set_adm.connfd = connfd;
        strcpy(set_adm.buf,str);
        pthread_create(&set_adm.pth,NULL,set_admin,NULL);
        pthread_mutex_lock(&set_adm_mutex);
        pthread_cond_wait(&set_adm_cond,&set_adm_mutex);
        set_adm_flag = 1;
        pthread_mutex_unlock(&set_adm_mutex);
    }
    else if(strcmp(buf,"kick")==0)
    {
        kic.connfd = connfd;
        strcpy(kic.buf,str);
        pthread_create(&kic.pth,NULL,kick,NULL);
        pthread_mutex_lock(&kic_mutex);
        pthread_cond_wait(&kic_cond,&kic_mutex);
        kic_flag = 1;
        pthread_mutex_unlock(&kic_mutex);
        printf("主线程被唤醒\n");

    }
    else if(strcmp(buf,"quit_group")==0)
    {
        qui_gro.connfd = connfd;
        strcpy(qui_gro.buf,str);
        pthread_create(&qui_gro.pth,NULL,quit_group,NULL);
        pthread_mutex_lock(&qui_gro_mutex);
        pthread_cond_wait(&qui_gro_cond,&qui_gro_mutex);
        qui_gro_flag =1;
        pthread_mutex_unlock(&qui_gro_mutex);
    }
    else if(strcmp(buf,"group_chat")==0)
    {
        gro_cha.connfd = connfd;
        strcpy(gro_cha.buf,str);
        pthread_create(&gro_cha.pth,NULL,group_chat,NULL);
        pthread_mutex_lock(&gro_cha_mutex);
        pthread_cond_wait(&gro_cha_cond,&gro_cha_mutex);
        gro_cha_flag = 1;
        pthread_mutex_unlock(&gro_cha_mutex);
        printf("线程被唤醒\n");
    }
    else if(strcmp(buf,"scan_group_chat_message")==0)
    {
        sca_gro_cha_mes.connfd = connfd;
        strcpy(sca_gro_cha_mes.buf,str);
        pthread_create(&sca_gro_cha_mes.pth,NULL,scan_group_chat_message,NULL);
        pthread_mutex_lock(&sca_gro_cha_mes_mutex);
        pthread_cond_wait(&sca_gro_cha_mes_cond,&sca_gro_cha_mes_mutex);
        sca_gro_cha_mse_flag = 1;
        pthread_mutex_unlock(&sca_gro_cha_mes_mutex);
        printf("线程被唤醒\n");
    }
    else if(strcmp(buf,"deliver_file")==0)
    {
        //存文件
        char path[50];
        char path1[100]; // 存储的路径
        char buf[1024];
        cJSON *root = NULL;
        cJSON *item = NULL;
        cJSON *next = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"deliver_file");
        next = cJSON_GetObjectItem(item,"path");
        strcpy(path,next->valuestring);
        sprintf(path1,"./文件/%s",path);
        FILE *fp = fopen(path1,"a");
        if(fp == NULL)
        {
            sprintf(buf,"打开文件失败");
            write_log("打卡文件失败");
            return 0;
        }
        while(recv(connfd,buf,1024,0)>0)
        {
            if (strcmp(buf,"null")==0)
            {
               fclose(fp);
               break;
            }
            fwrite(buf,1,1024,fp);
        }

        del_fil.connfd = connfd;
        strcpy(del_fil.buf,str);
        pthread_create(&del_fil.pth,NULL,deliver_file,NULL);
        pthread_mutex_lock(&del_fil_mutex);
        pthread_cond_wait(&del_fil_cond,&del_fil_mutex);
        del_fil_flag = 1;
        pthread_mutex_unlock(&del_fil_mutex);

    }
    else if(strcmp(buf,"receive_file")==0)
    {
        rec_fil.connfd = connfd;
        strcpy(rec_fil.buf,str);
        pthread_create(&rec_fil.pth,NULL,receive_file,NULL);
        pthread_mutex_lock(&rec_fil_mutex);
        pthread_cond_wait(&rec_fil_cond,&rec_fil_mutex);
        rec_fil_flag = 1;
        pthread_mutex_unlock(&rec_fil_mutex);
    }

    return 0;
}

void*
receive_file(void *arg)
{
    struct receive_file temp = rec_fil;
    while(rec_fil_flag == 0)
        pthread_cond_signal(&rec_fil_cond);
    rec_fil_flag = 0;
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    char account[30];
    char way[30];
    char name[50];
    char path[50];
    char buf[1024];
    MYSQL_RES *res =  NULL;
    MYSQL_ROW row = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"receive_file");
    next = cJSON_GetObjectItem(item,"account");
    strcpy(account,next->valuestring);
    next = cJSON_GetObjectItem(item,"way");
    strcpy(way,next->valuestring);
    next = cJSON_GetObjectItem(item,"name");
    strcpy(name,next->valuestring);

    sprintf(path,"./文件/%s",name);
    if(strcmp(way,"no")==0)
    {
        sprintf(buf,"拒绝接受文件,删除文件");
        write_log(buf);
        remove(path);
        return 0;
    }
    FILE *fp = fopen(path,"r");
    if(fp == NULL)
    {
        sprintf(buf,"接受文件，但是接受的文件不存在");
        write_log(buf);
        return 0;    
    }
    int fd = open(path,O_RDONLY);
    printf("path %s\n",path);
    struct stat stat_buf;
    int n= fstat(fd,&stat_buf);

    //查询account 的socket
    sprintf(buf,"select *from user_data where account = \"%s\"",account);
    sem_wait(&sem);
    while(mysql_query(&mysql,buf))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    int socket = atoi(row[6]);
    sprintf(buf,"{\"receive_file\":{\"file_name\":\"%s\",\"way\":\"begin\"}}",name);
    //发送文件
    send(socket,buf,1024,0);
    int i = sendfile(socket,fd,NULL,stat_buf.st_size);
    printf("i  ==== %d\n",i);
    sprintf(buf,"{\"receive_file\":{\"file_name\":\"%s\",\"way\":\"end\"}}",name);
    send(socket,buf,1024,0);
    sprintf(buf,"接受文件成功");
    write_log(buf);

}
void*
deliver_file(void *arg)
{

    struct deliver_file temp = del_fil;
    while(del_fil_flag ==0)
        pthread_cond_signal(&del_fil_cond);
    del_fil_flag = 0;
    pthread_detach(pthread_self());
    printf("temp = %s\n",temp.buf);
    char path[100];
    char path1[512];
    char recv_account[30];
    char deliver_account[30];
    char buf[1024];
    cJSON *root  = NULL;
    cJSON *item =NULL;
    cJSON *next = NULL;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL;
    root =  cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"deliver_file");
    next = cJSON_GetObjectItem(item,"path");
    strcpy(path,next->valuestring);
    next = cJSON_GetObjectItem(item,"recv_account");
    strcpy(recv_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"deliver_account");
    strcpy(deliver_account,next->valuestring);
    sprintf(path1,"./文件/%s",path);
    //查询user_account的socket
    sprintf(buf,"select *from user_data where account = \"%s\"",recv_account);
    printf("%s\n",buf);
    sem_wait(&sem);
    while(mysql_query(&mysql,buf))
        printf("hello worlddasdasd\n");
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)   //也就是说用户不存在
    {
        sprintf(buf,"{\"deliver_file\":\"用户不存在\"}");
        remove(path1);
        send(temp.connfd,buf,1024,0);
        sprintf(buf,"传送文件，用户不存在，删除文件");
        write_log(buf);
        return 0;
    }
    int socket = atoi(row[6]);
    //不能自己跟自己发送文件
    if(strcmp(recv_account,deliver_account)==0)
    {
        sprintf(buf,"{\"deliver_file\":\"不能自己给自己传文件\"}");
        send(temp.connfd,buf,1024,0);
        sprintf(buf,"发送文件，但是自己给自己发文件");
        write_log(buf);
        return 0;
    }
    if(strcmp(row[6],"-1"))
    {
        sprintf(buf,"{\"deliver_file\":\"用户%s给你发来文件:%s\"}",deliver_account,path);
        send(socket,buf,1024,0);
        sprintf(buf,"发送文件成功");
        write_log(buf);
        return 0;
    }
    sprintf(buf,"select *from user_data where account = \"%s\"",recv_account);
    while(1)
    {
        sleep(1);
        sem_wait(&sem);
        while(mysql_query(&mysql,buf))
            ;
        res = mysql_store_result(&mysql);
        sem_post(&sem);
        row = mysql_fetch_row(res);
        if(strcmp(row[6],"-1"))
        {
            socket = atoi(row[6]);
            sprintf(buf,"{\"deliver_file\":\"用户%s给你发来文件:%s\"}",deliver_account,path);
            send(socket,buf,1024,0);
            sprintf(buf,"发送文件成功");
            write_log(buf);
            return 0;
        }

    }
}

void*
scan_group_chat_message(void *arg)
{
    
    struct scan_group_chat_message temp = sca_gro_cha_mes;
    while(sca_gro_cha_mse_flag==0)
        pthread_cond_signal(&sca_gro_cha_mes_cond);
    sca_gro_cha_mse_flag = 0;
    pthread_detach(pthread_self());
    char group_account[30];
    char user_account[30];
    char cmd[1024];
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    MYSQL_RES *res =NULL;
    MYSQL_ROW row = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"scan_group_chat_message");
    next = cJSON_GetObjectItem(item,"group_account");
    strcpy(group_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"user_account");
    strcpy(user_account,next->valuestring);
    //看群是不是存在
    printf("%s %s\n",group_account,user_account);

    sprintf(cmd,"select *from gros where group_account =\"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row =mysql_fetch_row(res);
    printf("hello world\n");
    if(row ==NULL)   //如果群不存在
    {
        sprintf(cmd,"{\"scan_group_chat_message\":\"群不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"查看群聊消息，群不存在");
        write_log(cmd);
        return 0;
    }
    printf("123456789\n");
    //看是不是群成员
    sprintf(cmd,"select *from group_members where group_account =\"%s\" and group_member_account = \"%s\"",group_account,user_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)
    {
        sprintf(cmd,"{\"scan_group_chat_message\":\"你不在这个群里面\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"查看群聊消息，但是用户不再这个群里面");
        write_log(cmd);

        return 0;
    }


    //查询聊天数据库
    sprintf(cmd,"select *from group_chat_message where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    int n =0;
    while(row=mysql_fetch_row(res))
    {
        n++;
        sprintf(cmd,"{\"scan_group_chat_message\":\"发送消息的账户:%s 消息:%s\"}",row[0],row[2]);
        send(temp.connfd,cmd,1024,0);
    }
    if(n==0)
    {
        sprintf(cmd,"{\"scan_group_chat_message\":\"没有聊天记录\"}");
        send(temp.connfd,cmd,1024,0);
    }
    sprintf(cmd,"查看聊天记录成功");
    write_log(cmd);
}

void*
group_chat(void*arg)
{
    
    struct group_chat temp = gro_cha;
    while(gro_cha_flag==0)
    pthread_cond_signal(&gro_cha_cond);
    gro_cha_flag = 0;
    pthread_detach(pthread_self());
    char cmd[1024];
    char group_account[30];   //这个是组的帐号
    char message[1024];   //这个是要发送的消息
    char user_account[30];     //这个是发送的人的帐号
    char user_name[50];       // 这个是发送的人的名字
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    MYSQL_RES *res =NULL;
    MYSQL_ROW row = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"group_chat");
    next = cJSON_GetObjectItem(item,"group_account");
    strcpy(group_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"message");
    strcpy(message,next->valuestring);
    next = cJSON_GetObjectItem(item,"user_account");
    strcpy(user_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"user_name");
    strcpy(user_name,next->valuestring);
    //查询群是否存在
    sprintf(cmd,"select *from gros where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row =mysql_fetch_row(res);
    if(row ==NULL)   //就是这个群不存在
    {
        sprintf(cmd,"{\"group_chat\":\"群不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"想要群聊的群不存在");
        write_log(cmd);

        return 0;
    }
    int number = atoi(row[2]);   //这个是群员的数量
    if(number == 1)
    {
        sprintf(cmd,"{\"group_chat\":\"群里面只有你一个人\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"群聊天，但是群里面只有一个人");
        write_log(cmd);
        return 0;
    }
    char other_account[number-1][30];
    int flag[number-1];
    for(int i=0;i<number-1;i++)
        flag[i]=0;
    int i = 0;
    int all_flag = 1;   //没有给所有的人发消息
    //获取这个群里面除了发消息的人自己所有的帐号
    sprintf(cmd,"select *from group_members where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    while(row=mysql_fetch_row(res))
    {
        if(strcmp(row[2],user_account))
        {
            strcpy(other_account[i],row[2]);
            printf("%s\n",other_account[i]);
            i++;
        }
    }
    
    //更新群聊信息数据库
    sprintf(cmd,"insert into group_chat_message values(\"%s\",\"%s\",\"%s\")",user_account,group_account,message);
    while(mysql_query(&mysql,cmd)) 
        ;
    //给发送这条消息的人发消息
    sprintf(cmd,"{\"group_chat\":\"消息发送成功\"}");
    send(temp.connfd,cmd,1024,0);

    //给这些帐号发消息
    while(all_flag)    //如果all_flag 是1 的话，那么就要一直发消息
    {
        sleep(1);
        all_flag = 0;
        for(int j = 0; j<number-1; j++)
        {
            if(flag[j] == 0)   //如果有一个人没有发消息，那么就要继续发消息
                all_flag = 1;
        }
        for(int j = 0; j<number-1; j++)
        {
            if(flag[j] == 0)    //如果flag[j]  没有发消息给他
            {
                //我们发消息给他
                //检查是否在线
                sprintf(cmd,"select *from user_data where account = \"%s\"",other_account[j]);
                printf("cmd:%s\n",cmd);
                sem_wait(&sem);
                while(mysql_query(&mysql,cmd))
                    ;
                res = mysql_store_result(&mysql);
                sem_post(&sem);
                row = mysql_fetch_row(res);
                //如果在线，那么就发消息过去
                if(strcmp("1",row[5]) == 0 && strcmp("-1",row[6]))
                {
                    //我们发消息过去
                    int socket  =  atoi(row[6]);
                    sprintf(cmd,"{\"group_chat\":\"帐号为:%s 的群,帐号为:%s,名字为:%s的用户发来消息:%s\"}",group_account,user_account,user_name,message);
                    send(socket,cmd,1024,0);
                    flag[j] = 1;
                }
            }
        }
        
    }
    sprintf(cmd,"群聊，消息全部发送");
    write_log(cmd);

}

void*
quit_group(void *arg)
{
    struct quit_group temp = qui_gro;
    while(qui_gro_flag ==0)
    pthread_cond_signal(&qui_gro_cond);
    qui_gro_flag =0;
    pthread_detach(pthread_self());
    char cmd[1024];
    char group_account[30];
    char user_account[30];
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"quit_group");
    next = cJSON_GetObjectItem(item,"group_account");
    strcpy(group_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"user_account");
    strcpy(user_account,next->valuestring);
    //查询这个群存不存在
    sprintf(cmd,"select *from gros where group_account = \"%s\"",group_account);
    printf("%s\n",cmd);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res  = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)   //这个群不存在
    {
        sprintf(cmd,"{\"quit\":\"想要推出的群不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"退群，但是想要退的群不存在");
        write_log(cmd);

         return 0;
    }
    //群存在
    //查看自己是否在群里面
    sprintf(cmd,"select*from group_members where group_account = \"%s\" and group_member_account = \"%s\"",group_account,user_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)   //群里面没有自己
    {
        sprintf(cmd,"{\"quit_group\":\"你没在群里面\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"退群，但是却没有在群里");
        write_log(cmd);
    }
    //群主不能退群
    if(strcmp(row[4],"2")==0)
    {
        sprintf(cmd,"{\"quit_group\":\"群主只能解散群\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"群主退群");
        write_log(cmd);
        return 0;
    }
    //如果你在群里面，那么退群
    sprintf(cmd,"delete from group_members where group_account =\"%s\" and group_member_account = \"%s\"",group_account,user_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    sprintf(cmd,"{\"quit_group\":\"退群成功\"}");
    send(temp.connfd,cmd,1024,0);
    //更新群内成员的数量
    sprintf(cmd,"select *from gros where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    int num = atoi(row[2]);
    num--;
    sprintf(cmd,"update  gros set group_number = %d where group_account =\"%s\"",num,group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);
    sprintf(cmd,"退群成功");
    write_log(cmd);
}

void*
kick(void *arg)
{
    struct kick temp =kic;
    while(kic_flag == 0)
    pthread_cond_signal(&kic_cond);
    kic_flag = 0;
    pthread_detach(pthread_self());
    char group_account[30];   //这个是群号
    char kick_account[30];     //这个是被踢的人的帐号
    char user_account[30];      //这个是踢人的人的帐号
    char cmd[1024];
    cJSON *root = NULL;
    cJSON *item =NULL;
    cJSON *next = NULL;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL;
    root  = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"kick");
    next = cJSON_GetObjectItem(item,"group_account");
    strcpy(group_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"kick_account");
    strcpy(kick_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"account");
    strcpy(user_account,next->valuestring);
    sprintf(cmd,"select *from gros where group_account =\"%s\"",group_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)
    {
        sprintf(cmd,"{\"kick\":\"群不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"踢人，但是这个群不存在");
        write_log(cmd);
        return 0;
    }
    //查看 user_account 是不是群主或者是管理员
    sprintf(cmd,"select *from group_members where group_account = \"%s\" and group_member_account = \"%s\"",group_account,user_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)
    {
       sprintf(cmd,"{\"kick\":\"你不在这个群里面\"}");
       send(temp.connfd,cmd,1024,0);
       sprintf(cmd,"踢人，但是踢人的人不在群里面");
       write_log(cmd);
       return 0;
    }
    if(strcmp("1",row[4])!=0 && strcmp("2",row[4])!=0)   //既不是管理员也不是群主
    {   
        sprintf(cmd,"{\"kick\":\"你不是群主也不是管理员\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"踢人，但是又不是群主，也不是管理员");
        return 0;
    }
    int diwei = atoi(row[4]);
    //查询被踢的人是不是在群里面
    sprintf(cmd,"select *from group_members where group_account = \"%s\" and group_member_account = \"%s\"",group_account,kick_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)   //被踢的人不在群里面
    {
        sprintf(cmd,"{\"kick\":\"被踢的人不在群里面\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"被踢的人不再群里面");
        write_log(cmd);
        return 0;
    }
    if(strcmp(kick_account,user_account)==0)
    {
        sprintf(cmd,"{\"kick\":\"不能给自己踢自己\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"自己踢自己");
        write_log(cmd);
        return 0;
    }
    //看被踢的人是不是群主
    sprintf(cmd,"select *from group_members where group_account = \"%s\" and group_member_account = \"%s\"",group_account,kick_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(strcmp(row[4],"2")==0)
    {
        sprintf(cmd,"{\"kick\":\"不能踢群主\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"踢群主");
        write_log(cmd);
        return 0;
        
    }
    int diwei1= atoi(row[4]);
    if(diwei == diwei1)
    {
        sprintf(cmd,"{\"kick\":\"不能管理员踢管理员\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"管理员踢管理员");
        write_log(cmd);
        return 0;
    }
    //被踢的人在群里面
    sprintf(cmd,"delete from group_members where group_account =\"%s\" and group_member_account = \"%s\"",group_account,kick_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    sem_post(&sem);
    sprintf(cmd,"{\"kick\":\"成功踢人\"}");
    send(temp.connfd,cmd,1024,0);
    //更新群里面的人数
    sprintf(cmd,"select *from gros where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    int number = atoi(row[2]);
    number--;
    sem_wait(&sem);

    sprintf(cmd,"update gros set group_number = %d where  group_account = \"%s\"",number,group_account);
    printf("%s\n",cmd);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);
    sprintf(cmd,"踢人成功");
    write_log(cmd);

}

void*
set_admin(void*arg)
{
    struct set_admin temp = set_adm;
    while(set_adm_flag == 0)
    pthread_cond_signal(&set_adm_cond);
    set_adm_flag = 0;
    pthread_detach(pthread_self());
    char group_account[30];    //这个是群号
    char admin_account[30];     // 这个是将要设置的管理员的帐号
    char user_account[30];     //这个是设置管理员的帐号
    char cmd[1024];
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"set_admin");
    next = cJSON_GetObjectItem(item,"group_account");
    strcpy(group_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"admin_account");
    strcpy(admin_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"user_account");
    strcpy(user_account,next->valuestring);
    //查询这个群存不存在
    sprintf(cmd,"select * from gros where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)
    {
        sprintf(cmd,"{\"set_admin\":\"该群不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"设置管理员，但是该群不存在");
        write_log(cmd);
        return NULL;
    }
    //先要查询user_account是不是群主
    sprintf(cmd,"select * from group_members where group_account =\"%s\" and group_member_account = \"%s\"",group_account,user_account);
    sem_wait(&sem);
    int n =mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row==NULL)
    {
        sprintf(cmd,"{\"set_admin\":\"你不是群主\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"设置管理员,但是不是群主");
        write_log(cmd);
        return NULL;
    }

    if(strcmp(row[4],"2"))
    {
        sprintf(cmd,"{\"set_admin\":\"你不是群主\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"设置管理员，但是不是群主");
        write_log(cmd);
        return NULL;
    }
   if(strcmp(admin_account,user_account)==0)   //群主设置自己为管理员禁止
   {
       sprintf(cmd,"{\"set_admin\":\"不能群主设置自己为管理员\"}");
       send(temp.connfd,cmd,1024,0);
       sprintf(cmd,"群主设置自己为管理员");
       write_log(cmd);
       return NULL;
   }
    //查询admin_account 在不在群里面
    sprintf(cmd,"select *from group_members where group_account = \"%s\" and group_member_account = \"%s\"",group_account,admin_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)
    {
        sprintf(cmd,"{\"set_admin\":\"这个人不在群里面\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"设置管理员，但是这个人不在群里面");
        write_log(cmd);
        return NULL;
    }
    //更新admin_account 的地位
    sprintf(cmd,"update group_members set group_state = 1 where group_account = \"%s\" and group_member_account = \"%s\"",group_account,admin_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    sem_post(&sem);
    sprintf(cmd,"{\"set_admin\":\"设置管理员成功\"}");
    send(temp.connfd,cmd,1024,0);
    sprintf(cmd,"设置管理员成功");
    write_log(cmd);

}


void*
scan_group_people(void *arg)
{
    struct scan_group_people temp = sca_gro_peo;
    while(sca_gro_peo_flag == 0)
    pthread_cond_signal(&sca_gro_peo_cond);
    sca_gro_peo_flag = 0;
    pthread_detach(pthread_self());
    char group_account[30];
    char buf[1024];
    char cmd[1024];
    cJSON *root = NULL;
    cJSON *item =NULL;
    MYSQL_RES * res = NULL;
    MYSQL_ROW row = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"scan_group_people");
    strcpy(group_account,item->valuestring);
    sprintf(cmd,"select *from group_members where  group_account= \"%s\"",group_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    int n=0;
    while(row = mysql_fetch_row(res))
    {
        printf("%s\n",row[4]);
        n++;
        if(strcmp(row[4],"2")==0)
        {
            sprintf(buf,"{\"scan_group_people\":\"帐号:%s 名字:%s 地位:群主\"}",row[2],row[3]);
            send(temp.connfd,buf,1024,0);
        }
        else if(strcmp(row[4],"0")==0)
        {
            sprintf(buf,"{\"scan_group_people\":\"帐号:%s 名字:%s 地位:普通\"}",row[2],row[3]);
            send(temp.connfd,buf,1024,0);
        }
        else if(strcmp(row[4],"1")==0)
        {
            sprintf(buf,"{\"scan_group_people\":\"帐号:%s 名字:%s 地位:管理员\"}",row[2],row[3]);
            send(temp.connfd,buf,1024,0);
        }
    }
    if(n==0)
    {
        sprintf(buf,"{\"scan_group_people\":\"群不存在\"}");
        send(temp.connfd,buf,1024,0);
        sprintf(buf,"查看群人员，但是这个群不存在");
        write_log(buf);
    }
    else 
        sprintf(cmd,"成功查看群成员");
        write_log(cmd);
}



void *
scan_group(void *arg)
{
    struct scan_group temp = sca_gro;
    while(sca_gro_flag==0)
    pthread_cond_signal(&sca_gro_cond);
    sca_gro_flag = 0;
    pthread_detach(pthread_self());
    char account[30];  //这个是要查询的帐号
    char group_account[30];
    char cmd[1024];
    char buf[1024];
    cJSON *root = NULL;
    cJSON *item = NULL;
    MYSQL_RES *res1 =NULL;
    MYSQL_RES *res2 =NULL;
    MYSQL_ROW row1 = NULL;
    MYSQL_ROW row2 = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"scan_group");
    strcpy(account,item->valuestring);
    sprintf(cmd,"select * from group_members where group_member_account = \"%s\"",account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res1 = mysql_store_result(&mysql);
    sem_post(&sem);

    int n = 0;
    while(row1=mysql_fetch_row(res1))
    {
        strcpy(group_account,row1[0]);
        sprintf(cmd,"select *from gros where group_account = \"%s\"",group_account);
        sem_wait(&sem);
        mysql_query(&mysql,cmd);
        res2 = mysql_store_result(&mysql);
        sem_post(&sem);
        row2 = mysql_fetch_row(res2);
        sprintf(buf,"{\"scan_group\":\"群号:%s 群名:%s 群人数:%s\"}",row2[0],row2[1],row2[2]);
        send(temp.connfd,buf,1024,0);
        n++;
    }
    if(n==0)
    {
        sprintf(buf,"{\"scan_group\":\"该用户没有加群\"}");
        send(temp.connfd,buf,1024,0);
        sprintf(buf,"查看群，但是该用户没有加群");
        write_log(buf);
    }
    sprintf(buf,"成功查看群信息");
    write_log(buf);
    


}



void *
agree_join_group(void*)
{

    struct agree_join_group temp = agr_joi_gro;
    while(agr_joi_gro_flag == 0)
    pthread_cond_signal(&agr_joi_gro_cond);
    agr_joi_gro_flag = 0;
    pthread_detach(pthread_self());
    char group_account[30];   // 群号
    char group_owner_account[30];  // 群主帐号
    char account[30];       // 加群的人的帐号 
    char group_name[100];    // 群的名字
    char user_name[30];     //加群的人的名字
    char cmd[1024];
    MYSQL_RES * res  = NULL;
    MYSQL_ROW row = NULL;
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"agree_join_group");
    next = cJSON_GetObjectItem(item,"group_account");
    strcpy(group_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"group_owner_account");
    strcpy(group_owner_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"account");
    strcpy(account,next->valuestring);
    sprintf(cmd,"select *from group_join where user_account = \"%s\" and group_account = \"%s\" and group_owner_account = \"%s\"",account,group_account,group_owner_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    if(row == NULL)   //如果没有查找到添加群的消息
    {   
        sprintf(cmd,"{\"agree_join_group\":\"对方没有发送添加群的消息\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"同意加群，但是对方没有发加群的消息");
        write_log(cmd);
        return 0;
    }
    printf(" %s %s %s \n",account,group_account,group_owner_account);
    //查询群的名字和用户的名字
    sprintf(cmd,"select *from user_data where account = \"%s\"",account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    strcpy(user_name,row[1]);
    sprintf(cmd,"select * from gros where group_account = \"%s\" ",group_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    strcpy(group_name,row[1]);
    //接下來添加用户到数据库
    sprintf(cmd,"insert into group_members values(\"%s\",\"%s\",\"%s\",\"%s\",0)",group_account,group_name,account,user_name);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    sem_post(&sem);
    sprintf(cmd,"select *from gros where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    int num = atoi(row[2]);
    num++;
    sprintf(cmd,"update gros set group_number = %d where group_account = \"%s\"",num,group_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    sem_post(&sem);
    //删除添加群的消息
    sprintf(cmd,"delete from group_join where user_account = \"%s\" and group_account = \"%s\" and group_owner_account = \"%s\"",account,group_account,group_owner_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    sem_post(&sem);
    sprintf(cmd,"{\"agree_join_group\":\"同意用户进群成功\"}");
    send(temp.connfd,cmd,1024,0);
    sprintf(cmd,"同意进群成功");
    write_log(cmd);
    return 0;

}

//加群
void*
join_group(void*arg)
{
    struct join_group temp = join_gro;
    while(join_gro_flag == 0)
    pthread_cond_signal(&join_gro_cond);
    join_gro_flag = 0;
    pthread_detach(pthread_self());
    char name[30];     // 申请加群的人的名称
    char account[30];   //申请加群的刃的帐号
    char group_account[30];   // 这个是要加的群号
    char group_name[100];       // 这个是要加群的名称
    char group_owner_account[30];    //群主帐号
    char cmd[1024];
    int socket = -1;
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL;   
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"join_group");
    next = cJSON_GetObjectItem (item,"name");
    strcpy(name,next->valuestring);
    next = cJSON_GetObjectItem(item,"user_account");
    strcpy(account,next->valuestring);
    next = cJSON_GetObjectItem(item,"group_account");
    strcpy(group_account,next->valuestring);
    //接下来查询这个群的名称  方便查数据库
    sprintf(cmd,"select *from gros where group_account =\"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    if(row == NULL)    //如果这个群不存在
    {
        sprintf(cmd,"{\"join_group\":\"要加入的群不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"加群，但是想加的群不存在");
        write_log(cmd);
        return NULL;
    }
    strcpy(group_name,row[1]);
    //查询加群的人是不是已经在这个群里面了
    sprintf(cmd,"select *from group_members where group_account = \"%s\" and group_member_account = \"%s\"",group_account,account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    //如果不在群里面，就不用在发家群的申请了
    row = mysql_fetch_row(res);
    if(row != NULL)
    {
        sprintf(cmd,"{\"join_group\":\"你已经在群里面了，不用在申请了\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"申请加群，但是已经在群里面了");
        write_log(cmd);
        return 0;
    }
    //接下来查找群主
    sprintf(cmd,"select * from group_members where group_account = \"%s\" and group_state = 2",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    strcpy(group_owner_account,row[2]);
    
    sprintf(cmd,"{\"join_group\":\"发送消息成功\"}");
    printf("找到要发送的群\n");
    send(temp.connfd,cmd,1024,0);

    //接下来查询群主的状态
    sprintf(cmd,"select *from user_data where account = \"%s\"",group_owner_account);
    while(1)
    {
        sleep(1);
        sem_wait(&sem);
        while(mysql_query(&mysql,cmd))
            ;
        res = mysql_store_result(&mysql);
        row = mysql_fetch_row(res);
        sem_post(&sem);
        if(!strcmp(row[5],"1") && strcmp(row[6],"-1"))
         {
            socket = atoi(row[6]);
            sprintf(cmd,"{\"join_apply_for\":{\"account\":\"%s\",\"name\":\"%s\",\"group_name\":\"%s\",\"group_account\":\"%s\"}}",account,name,group_name,group_account);
            send(socket,cmd,1024,0);
            sprintf(cmd,"insert into group_join values(\"%s\",\"%s\",\"%s\")",account,group_account,group_owner_account);
            sem_wait(&sem);
            while(mysql_query(&mysql,cmd))
                ;
            sem_post(&sem);
            return NULL;
            
        }

    }
    sprintf(cmd,"成功发送加群信息");
    write_log(cmd);
}



void *
delete_group(void*arg)
{
    struct delete_group temp = del_gro;
    while(del_gro_flag == 0)
    pthread_cond_signal(&del_gro_cond);
    del_gro_flag =0;
    pthread_detach(pthread_self());
    char  group_account[30];    //群号
    char user_account[30];      //这个是删这个群的人的帐号 
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL; 
    char cmd[1024];
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"delete_group");
    next = cJSON_GetObjectItem(item,"group_account");
    strcpy(group_account,next->valuestring);
    next =cJSON_GetObjectItem(item,"user_account");
    strcpy(user_account,next->valuestring);
    printf("group_account = \"%s\" user_account = \"%s\"",group_account,user_account);
    sprintf(cmd,"select *from gros where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    printf("cmd = %s\n",cmd);
    if(NULL == row)
    {
        sprintf(cmd,"{\"delete_group\":\"删除的群不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"删群，但是删除的群不存在");
        write_log(cmd);
        return 0;
    }
    //看删除群的人是不是群主
    int i,j;
    sprintf(cmd,"select *from group_members where group_account = \"%s\" and group_member_account = \"%s\"",group_account,user_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);

    if(row == NULL)
    {
        sprintf(cmd,"{\"delete_group\":\"你不在群里面\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"删群，但是不在群里面");
        write_log(cmd);
        return 0;
    }
    if(strcmp("2",row[4]))
    {
        sprintf(cmd,"{\"delete_group\":\"你不是群主\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"删除群，但是不是群主删群");
        write_log(cmd);
        return 0;
    }
    //删除群聊天记录
    sprintf(cmd,"delete from group_chat_message where  group_account = \"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);
    //删除群
    sprintf(cmd,"delete from group_members where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);
    sprintf(cmd,"{\"delete_fri\":\"删除成功\"}");
    send(temp.connfd,cmd,1024,0);

    sprintf(cmd,"delete from gros where group_account = \"%s\"",group_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);

    sprintf(cmd,"删除群成功");
    write_log(cmd);
}

void *
create_group(void *arg)
{
    struct  create_group temp = cre_gro;
    while(cre_gro_flag == 0)
    pthread_cond_signal(&cre_gro_cond);
    cre_gro_flag = 0;
    pthread_detach(pthread_self());
    char group_name[100];    //群的名字
    char manager_account[30];   //群主的帐号
    char manager_name[30];       //群主的名字
    char group_account[30];    // 群号
    char cmd[1024];
    int i,j;
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"create_group");
    next = cJSON_GetObjectItem(item,"group_name");
    strcpy(group_name,next->valuestring);
    next = cJSON_GetObjectItem(item ,"manager_account");
    strcpy(manager_account,next->valuestring);
    next = cJSON_GetObjectItem(item,"manager_name");
    strcpy(manager_name,next->valuestring);
    //查询数据库获得帐号
    sprintf(cmd,"select *from group_account");
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    int number = atoi(row[0]);
    printf("%d\n",number);
    number++;
    sprintf(cmd,"update group_account set account_account = %d",number);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);

    sprintf(group_account,"%s",row[0]);
    sprintf(cmd,"insert into gros values(\"%s\",\"%s\",1)",group_account,group_name);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);
    sprintf(cmd,"insert into group_members values(\"%s\",\"%s\",\"%s\",\"%s\",2)",group_account,group_name,manager_account,manager_name);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    sem_post(&sem);
    sprintf(cmd,"{\"create_group\":\"你的帐号是:%s\"}",group_account);
    send(temp.connfd,cmd,1024,0);
    sprintf(cmd,"创建群成功");
    write_log(cmd);
    return NULL;
}

void *
recover_friend(void*arg)
{
    struct recover_friend temp = rec_fri;
    while(rec_fri_flag == 0)
    pthread_cond_signal(&rec_fri_cond);
    rec_fri_flag == 0;
    pthread_detach(pthread_self());
    char cmd[1024];
    char user[30];
    char fri[30];
    MYSQL_RES  *res =NULL;
    MYSQL_ROW row = NULL;
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    root = cJSON_Parse(temp.buf);
    item  = cJSON_GetObjectItem(root,"recover_fri");
    next  =  cJSON_GetObjectItem(item,"user");
    strcpy(user,next->valuestring);
    next  = cJSON_GetObjectItem (item,"fri");
    strcpy(fri,next->valuestring);
    sprintf(cmd,"select *from user_data where account = \"%s\"",fri);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    if(row == NULL)
    {
        sprintf(cmd,"{\"recover\":\"该帐号不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"恢复好友，但是帐号不存在");
        write_log(cmd);
        return NULL;
    }
    sprintf(cmd,"select *from friends where usr_id = \"%s\" and friend_id = \"%s\"",user,fri);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res =mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    if(row == NULL)
    {
        sprintf(cmd,"{\"recover\":\"你们不是好友\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"恢复好友，但是你们不是好友");
        write_log(cmd);
        return NULL;
    }
    if(strcmp(row[2],"0")==0)
    {
        sprintf(cmd,"{\"recover\":\"不用恢复状态\"}");
        send(temp.connfd,cmd,1024,0);
        return NULL;
    }
    sprintf(cmd,"update friends set state = \"0\" where usr_id = \"%s\" and friend_id =\"%s\"",user,fri);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    sem_post(&sem);
    sprintf(cmd,"{\"recover\":\"恢复成功\"}");
    send(temp.connfd,cmd,1024,0);
    sprintf(cmd,"恢复好友成功");
    write_log(cmd);
    return NULL;

    
}


void*
shield_friend(void *arg)
{
    struct shield_fri temp = shi_fri;
    while(shi_fri_flag==0)
    pthread_cond_signal(&shi_fri_cond);
    shi_fri_flag = 0;
    pthread_detach(pthread_self());
    char account[30];
    char fri_account[30];
    char buf[1024];
    char cmd[1024];
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root, "shi_fri");
    next = cJSON_GetObjectItem(item,"account1");
    sprintf(account,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"account2");
    sprintf(fri_account,"%s",next->valuestring);
    sprintf(cmd,"select *from friends where usr_id = \"%s\" and friend_id = \"%s\"",account,fri_account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    MYSQL_RES *res = mysql_store_result(&mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    sem_post(&sem);
    if(row == NULL)
    {
        sprintf(cmd,"{\"shield_fri\":\"你们不是好友\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"拉黑好友，但是你们不是好友");
        write_log(cmd);
        return NULL;
    }
    else 
    {
        sprintf(cmd,"{\"shield_fri\":\"拉黑成功\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"update friends set state = \"1\" where usr_id = \"%s\" and friend_id = \"%s\" ",account,fri_account);
        sem_wait(&sem);
        mysql_query(&mysql,cmd);
        sem_post(&sem);
        sprintf(cmd,"拉黑成功");
        write_log(cmd);
        return NULL;
    }
     

}
void *
scan_pri_message(void*)
{
    struct scan_pri_message  temp = scan_pri_msg;
    while(scan_pri_msg_flag == 0)
    pthread_cond_signal(&scan_pri_cond);
    scan_pri_msg_flag = 0;
    pthread_detach(pthread_self());
    char buf[1024];
    char buf1[512];
    char cmd[1024];
    char account[30];    //这个是用户的帐号
    char fri_account[30];   //这个是好友的帐号
    MYSQL_RES *res = NULL;
    MYSQL_ROW row ;
    cJSON *root  = NULL;
    cJSON *item  = NULL;
    cJSON * next = NULL;
    int n ;
    root = cJSON_Parse(temp.buf);
    item  = cJSON_GetObjectItem(root,"scan_msg");
    next = cJSON_GetObjectItem(item,"account");
    sprintf(account,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"fri_account");
    sprintf(fri_account,next->valuestring);
    //如果用户帐号和好友帐号相同
    //直接推出
    if(strcmp(account,fri_account)==0)
    {
        memset(cmd,0,1024);
        sprintf(cmd,"{\"chat_message\":\"不能自己看自己的聊天记录\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"查看好友聊天记录，自己看自己跟自己的记录");
        write_log(cmd);
    }
    //查看好友是否存在
    sprintf(cmd,"select *from user_data where account = \"%s\"",fri_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    row = mysql_fetch_row(res);
    if(row == NULL)
    {
        sprintf(cmd,"{\"chat_message\":\"该用户不存在\"}");
        send(temp.connfd,cmd,1024,0);
        sprintf(cmd,"查询聊天记录，但是好友不存在");
        write_log(cmd);
        return 0;
    }
    sprintf(cmd, "select *from chat_messages where send_user = \"%s\" and recv_user = \"%s\"",fri_account,account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    n = mysql_num_fields(res);
    
    sem_post(&sem);
    while(row = mysql_fetch_row(res))
    {
        memset(buf,0,1024);
        memset(buf1,0,512);
        for(int i=0;i < n;i++)
        {
            if(i==0)
            sprintf(buf1,"%s--->",row[i]);
            else if(i==1)
            sprintf(buf1,"%s:",row[i]);
            else 
            sprintf(buf1,"%s ",row[i]);
            strcat(buf,buf1);
        }
        sprintf(cmd,"{\"chat_message\":\"%s\"}",buf);
        send(temp.connfd,cmd,1024,0);
    }
    sprintf(cmd,"select *from chat_messages where send_user = \"%s\" and recv_user = \"%s\"",account,fri_account);
    sem_wait(&sem);
    while(mysql_query(&mysql,cmd))
        ;
    res = mysql_store_result(&mysql);
    sem_post(&sem);
    
    while(row = mysql_fetch_row(res))
    {
        memset(buf,0,1024);
        memset(buf1,0,512);
        for(int i = 0; i < n; i++)
        {
           if(i == 0)
               sprintf(buf1,"%s--->",row[i]);
           else if(i==1)
               sprintf(buf1,"%s:",row[i]);
           else
               sprintf(buf1,"%s ",row[i]);
           strcat(buf,buf1);
        }
        sprintf(cmd,"{\"chat_message\":\"%s\"}",buf);
        send(temp.connfd,cmd,1024,0);
    }
    sprintf(cmd,"查看聊天记录");
    write_log(cmd);

}


void*
private_chat(void*arg)
{
    struct private_chat temp = pri_chat;
    while(pri_chat_flag == 0)
    pthread_cond_signal(&pri_chat_cond);
    pri_chat_flag = 0;
    pthread_detach(pthread_self());
    char buf[1024];
    char cmd[1024];
    char account[30];    //接受者
    char account1[30];    // 发送者
    char name[30];      //发送者的名字
    cJSON * root  = NULL;
    cJSON * item  = NULL;
    cJSON * next = NULL;
    root  = cJSON_Parse(temp.buf);
    item  = cJSON_GetObjectItem(root,"pri_chat");
    next  = cJSON_GetObjectItem(item,"buf");
    sprintf(buf,"%s",next->valuestring);
    next  = cJSON_GetObjectItem(item,"account");
    sprintf(account,"%s",next->valuestring);
    next  = cJSON_GetObjectItem(item,"name");
    sprintf(name,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"account1");
    sprintf(account1,"%s",next->valuestring);
    sprintf(cmd,"select *from user_data where account =  \"%s\"",account);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    MYSQL_RES *res = mysql_store_result(&mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    sem_post(&sem);
    if(row == NULL)
    {
        strcpy(buf,"{\"pri_chat_find\":\"该帐号不存在\"}");
        send(temp.connfd,buf,1024,0);
        sprintf(buf,"私聊，但是该帐号不存在");
        write_log(buf);
        return 0;
    }
    else 
    {
        char buf1[1024];
        strcpy(buf1,"{\"pri_chat_find\":\"找到该帐号\"}");
        send(temp.connfd,buf1,1024,0);
        sprintf(cmd,"select *from friends where usr_id =\"%s\" and friend_id =\"%s\"",account,account1);
        sem_wait(&sem);
        mysql_query(&mysql,cmd);
        res = mysql_store_result(&mysql);
        row = mysql_fetch_row(res);
        sem_post(&sem);
        if(row == NULL)
        {
            sprintf(cmd,"{\"pri_chat_find\":\"你们不是好友\"}");
            send(temp.connfd,cmd,1024,0);
            sprintf(cmd,"私聊,但是不是好友");
            write_log(cmd);
            return 0;
        }
        if(strcmp(row[2],"1")==0)   //被拉黑了
        {
            sprintf(cmd,"{\"pri_chat_find\":\"你已经被对方拉黑了\"}");
            send(temp.connfd,buf,1024,0);
            sprintf(cmd,"私聊，但是已经被拉黑");
            write_log(cmd);
            return 0;
        }
        sprintf(cmd,"select *from user_data where account = \"%s\"",account);
        while(1)
        {
            sleep(1);        
            sem_wait(&sem);
            mysql_query(&mysql,cmd);
            MYSQL_RES *res = mysql_store_result(&mysql);
            sem_post(&sem);
            MYSQL_ROW row = mysql_fetch_row(res);
            if(!strcmp(row[5],"1")&&strcmp(row[6],"-1"))
            {
                int fd = atoi(row[6]);
                sprintf(buf1,"{\"pri_chat_ser\":{\"account\":\"%s\",\"message\":\"%s\",\"name\":\"%s\"}}",account1,buf,name);
                send(fd,buf1,1024,0);
                printf("%s\n",buf1);
                sprintf(cmd,"insert into chat_messages values(\"%s\",\"%s\",\"%s\")",account1,account,buf);
                sem_wait(&sem);
                mysql_query(&mysql,cmd);
                sem_post(&sem);
                sprintf(cmd,"私聊消息成功发送");
                write_log(cmd);
                return 0;
            }
        }
    }

}

void *
scan_friend(void  *arg)
{
    struct scan_friend temp = scan_fri;
    while(scan_fri_flag==0)
    pthread_cond_signal(&scan_fri_cond);
    //分离线程
    scan_fri_flag = 0;
    pthread_detach(pthread_self());
    char account[20];
    cJSON *root = NULL;
    cJSON *item = NULL;

    char cmd[1024];
    char buf[50];
    char buf1[1024];

    int num ;
    int num_data;
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"scan_fri");
    sprintf(account,"%s",item->valuestring);
    sprintf(cmd,"select *from friends where usr_id = \"%s\"",account);
    MYSQL_ROW row;
    MYSQL_ROW row_user;
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    sem_post(&sem);
    memset(cmd,0,1024);
    memset(buf,0,1024);
    MYSQL_RES * res = mysql_store_result(&mysql);
    MYSQL_RES * res_data  = NULL;
    num = mysql_num_fields(res);
    int n = 0;
    while(row = mysql_fetch_row(res))  
    {
        
        sprintf(cmd,"select * from user_data where account = \"%s\"",row[1]);
        sem_wait(&sem);
        mysql_query(&mysql,cmd);
        res_data = mysql_store_result(&mysql);
        row_user = mysql_fetch_row(res_data);
        sem_post(&sem);
        if(strcmp("0",row_user[5])==0)
        sprintf(buf1,"{\"scan_fri\":\"name:%s account:%s state:不在线\"}",row_user[1],row_user[0]);
        else if(strcmp("1",row_user[5])==0)
        sprintf(buf1,"{\"scan_fri\":\"name:%s account:%s state:在线\"}",row_user[1],row_user[0]);
        send(temp.connfd,buf1,1024,0);
        n++;
    }

        if(n == 0 )
        {
            strcpy(buf1,"{\"scan_fri\":\"没有好友\"}");
            send(temp.connfd,buf1,1024,0);
        }
        sprintf(buf,"好友查询结束");
        write_log(buf);
    //sem_post(&sem);
}


void *
delete_fri(void *arg)
{
    struct de_fri temp = del_fri;
    while(del_flag == 0)
    pthread_cond_broadcast(&del_cond);
    del_flag = 0;
    //分离线程
    pthread_detach(pthread_self());
    char user[20];
    char fri[20];
    char cmd[1024];
    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *next = NULL;
    MYSQL_RES *res =NULL;
    MYSQL_ROW row = NULL;

    //MYSQL_RES *res = mysql_store_result(&mysql);
    //MYSQL_ROW row = mysql_fetch_row(res);
    root = cJSON_Parse(temp.buf);
    item = cJSON_GetObjectItem(root,"delete_fri");
    next = cJSON_GetObjectItem(item,"user");
    sprintf(user,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"fri");
    sprintf(fri,"%s",next->valuestring);
    sprintf(cmd,"select *from user_data where account = \"%s\"",fri);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    sem_post(&sem);
    if(row == NULL)
    {
        strcpy(cmd,"{\"del_fri\":\"删除的好友不存在\"}");
        send(del_fri.connfd,cmd,1024,0);
        write_log("删除的好友不存在");
        return 0;
    }
    sprintf(cmd,"select * from friends where usr_id = \"%s\" and friend_id = \"%s\"",user,fri);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    if(NULL==row)
    {
        strcpy(cmd,"{\"del_fri\":\"你们不是好友\"}");
        send(del_fri.connfd,cmd,1024,0);
        write_log("删除好友，但是还不是好友");
        return 0;
    }
    sem_post(&sem);
    sem_wait(&sem);
    sprintf(cmd,"delete from friends where usr_id =\"%s\" and friend_id =\"%s\"",user,fri);
    if(mysql_query(&mysql,cmd)!=0)
        my_err("mysql_query",__LINE__);
    sem_post(&sem);

    sem_wait(&sem);
    sprintf(cmd,"delete from friends where usr_id = \"%s\" and friend_id =\"%s\"",fri,user);
    if(mysql_query(&mysql,cmd)!=0)
        my_err("mysql_query",__LINE__);
    sem_post(&sem);
    strcpy(cmd,"{\"del_fri\":\"成功删除好友\"}");
    send(del_fri.connfd,cmd,1024,0);
    write_log("成功删除好友");
}


void *
my_exit(void*arg)
{
    struct ex tp = ex;
    char cmd[256];
    while(exit_flag == 0)
    pthread_cond_signal(&exit_cond);
    exit_flag = 0;
    pthread_detach(pthread_self());
    char account[20];
    cJSON *root = NULL;
    cJSON *item = NULL;
    root  = cJSON_Parse(tp.buf);
    item = cJSON_GetObjectItem(root,"my_exit");
    sprintf(account,"%s",item->valuestring);
    sprintf(cmd,"update user_data set state = 0 where  account = \"%s\"",account);
    
    sem_wait(&sem);
    if(mysql_query(&mysql,cmd)!=0)
        my_err("mysql_query",__LINE__);
    sprintf(cmd,"update user_data set socket = -1 where account = \"%s\"",account);
    if(mysql_query(&mysql,cmd)!=0)
        my_err("mysql_query",__LINE__);
    sem_post(&sem);
    write_log("退出登陆");
}



void *
add_friend(void*arg)
{
    struct add_friend fri = add;
    while(add_friend_flag==0)
    pthread_cond_signal(&add_cond);
    add_friend_flag = 0;
    pthread_detach(pthread_self());
    char account1[20];
    char account2[20];
    char name[30];
    char cmd[1024];
    cJSON *root = NULL;
    cJSON *item =NULL;
    cJSON *next = NULL;
    root = cJSON_Parse(fri.buf);
    item = cJSON_GetObjectItem(root,"add_friend");
    next = cJSON_GetObjectItem(item,"account1");
    sprintf(account1,"%s",next->valuestring);
    next = cJSON_GetObjectItem(item,"account2");
    sprintf(account2,"%s",next->valuestring);    //这个是要添加的帐号的id
    next = cJSON_GetObjectItem(item,"name");
    sprintf(name,"%s",next->valuestring);
    sprintf(cmd,"select *from user_data where account = \"%s\"",account2);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    MYSQL_RES *res = mysql_store_result(&mysql);
    MYSQL_ROW row   = mysql_fetch_row(res);
    sem_post(&sem);

    if(row == NULL)
    {
        strcpy(cmd,"{\"add_friend\":\"该帐号不存在\"}");
        send(fri.connfd,cmd,1024,0);
        write_log("要加好的好友不存在");
        return 0;
    }
    else
    {
        if(strcmp(account1,account2)==0)
        {
            sprintf(cmd,"{\"add_friend\":\"不能添加自己为好友\"}");
            send(fri.connfd,cmd,1024,0);
            sprintf(cmd,"添加自己为好友");
            write_log(cmd);
            return 0;
        }
        char buf[1024];
        strcpy(buf,"{\"add_friend\":\"找到该帐号\"}");
        send(fri.connfd,buf,1024,0);
        while(1)
        {
            sleep(1);
            sem_wait(&sem);
            mysql_query(&mysql,cmd);

            MYSQL_RES *res = mysql_store_result(&mysql);
            MYSQL_ROW row   = mysql_fetch_row(res);

            sem_post(&sem);
            if(!strcmp(row[5],"1")&&strcmp(row[6],"-1"))    //如果在线
            {

                sem_wait(&sem);
                sleep(2);
                int fd = atoi(row[6]);
                sprintf(buf,"{\"agree\":{\"account\":\"%s\",\"id\":\"%s\"}}",account1,name);
                send(fd,buf,1024,0);
                sprintf(cmd,"insert into add_friend values(\"%s\",\"%s\")",account1,account2);
                if(mysql_query(&mysql,cmd)!=0)
                    my_err("mysql_query",__LINE__);
                sem_post(&sem);
                return 0;
                write_log("添加好友消息发送成功");
            }
        }
    }

}



void
my_err(char *string,int line)
{
    printf("%d %s error\n",line,string);
    printf("%s\n",strerror(errno));
    exit(1);
}

int
open_listenfd(int port)
{
     int listenfd,optval=1;
     struct sockaddr_in serveraddr;
     if((listenfd = socket(AF_INET,SOCK_STREAM,0))<0)
     my_err("socket",__LINE__);

     if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int))<0)
     my_err("setsockoptk",__LINE__);
        bzero((char *) &serveraddr, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serveraddr.sin_port = htons((unsigned short)port);
       if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
       my_err("bind",__LINE__);
        if (listen(listenfd, LISTENQ) < 0)
            my_err("listenfd",__LINE__);
        return listenfd;

 }

int 
setnonblocking(int fd)
{
    int old_option  = fcntl(fd,F_GETFL);
    int new_option = old_option|SOCK_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

void 
write_log(char *str)
{
    time_t t;
    struct tm*tmp;
    char buf[100];
    char cmd[4096];
    time(&t);
    tmp = localtime(&t);
    strftime(buf,100,"time and date: %r,%a %b %d,%Y",tmp);
    sprintf(cmd,"insert into server_log values(\"%s\",\"%s\")",buf,str);
    printf("%s\n",cmd);
    sem_wait(&sem);
    mysql_query(&mysql,cmd);
    sem_post(&sem);
}
