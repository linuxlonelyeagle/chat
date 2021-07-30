#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<errno.h>
#include<signal.h>
#include"./cJSON.h"
#include<pthread.h>
#include<semaphore.h>
#include<sys/epoll.h>
#include<semaphore.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#define MAXEVENTS 64


//用来保存文件
FILE *fp = NULL;
pthread_t pth;

pthread_mutex_t sign_in_mutex;
pthread_cond_t  sign_in_cond;

pthread_mutex_t my_chat_mutex;
pthread_cond_t my_chat_cond;

//专门用来找回密码的锁
pthread_mutex_t find_back_muetx;
pthread_cond_t find_back_cond;
int find_back_flag = 0;
char buffer[1024];

//信号量，用来控制线程间接受消息的同步
sem_t sem;

struct person
{
    char account[20];
    char id[30];
}ps;

int have_sign_in = 0;

int efd;
struct epoll_event event;
struct epoll_event *events = NULL;


//设置文件描述符非阻塞
int 
setnoblocking(int fd);

void 
find_function(int connfd,char *str);

void 
sign_in(int connfd);
void
find_back(int connfd);

void 
my_err(char *string,int line);
//连接
//
int
open_clientfd(char *hostname,int port);

void
start();

int
sign_up(int fd);

void 
have_in();

void 
quit(int connfd);

int 
add_friend(int connfd);


//查看自身的信息
void 
look_self_data(void);

void 
my_exit(int connfd);

void *
pthread(void*arg);

//同意好友添加
int 
agree(int connfd);

//删除好友
void 
delete_friend(int connfd);

//查看好友列表
void 
scan_friend(int connfd);

//私聊功能
void 
private_chat(int connfd);

//查看聊天消息
void 
scan_msg(int connfd);

//拉黑好友
void 
shield_friend(int connfd);


//恢复好友
void 
recover_friend(int connnfd);

//建群
void 
create_group(int connfd);

//删除群
void
delete_group(int connnfd);

//
void 
join_group(int connfd);

//同意加群申请
void
agree_join_group(int connfd);

//查看已加群
void 
scan_group(int connfd);

//查看群成员
void 
scan_group_people(int connfd);

//设置管理员
void 
set_administrator(int connfd);

//踢人
void 
kick(int connnfd);


//退群
void
quit_group(int connfd);

//群聊
void 
group_chat(int connnfd);

//查看群聊聊天记录
void 
scan_group_chat_message(int connfd);

//文件传输
void 
deliver_file(int connfd);

void
receive_file(int connfd);

int
main(int argc,char *argv[])
{
 
    //信号处理
    signal(SIGPIPE,SIG_IGN);
    
    char op;
    char cmd[1024];
    int port = atoi(argv[2]);
    int connfd = open_clientfd(argv[1],port);
    //创建一个专门用来接收消息的线程
    pthread_create(&pth,NULL,pthread,&connfd);

    //设置文件描述符非阻塞
    //setnoblocking(connfd);
    pthread_mutex_init(&sign_in_mutex,NULL);
    pthread_cond_init(&sign_in_cond,NULL);
    pthread_mutex_init(&find_back_muetx,NULL);
    pthread_cond_init(&find_back_cond,NULL);
    pthread_mutex_init(&my_chat_mutex,NULL);
    pthread_cond_init(&my_chat_cond,NULL);

    while(1)
    {
        while(have_sign_in == 0)
        {
        start();
        printf("my_chat:");
        fflush(stdout);
        scanf("%s",cmd);
        while(getchar()!='\n')
            ;
        if(strlen(cmd) != 1)
        {
            printf("输入错误,请重新输入。\n");
            break;
        }
        else if(strlen(cmd) == 1)
        op = cmd[0];
        memset(cmd,0,1024);
        switch(op)
        {
            case 'a':
                pthread_mutex_lock(&sign_in_mutex);
                sign_in(connfd);
                pthread_cond_wait(&sign_in_cond,&sign_in_mutex);
                pthread_mutex_unlock(&sign_in_mutex);
                break;
            case 'b':
                pthread_mutex_lock(&sign_in_mutex);
                sign_up(connfd);
                pthread_cond_wait(&sign_in_cond,&sign_in_mutex);
                pthread_mutex_unlock(&sign_in_mutex);
                break;
            case 'c':   //找回密码
                pthread_mutex_lock(&find_back_muetx);
                find_back(connfd);
                pthread_cond_wait(&find_back_cond,&find_back_muetx);
                find_back_flag = 1;
                pthread_mutex_unlock(&find_back_muetx);
                break;
            case 'd':
                quit(connfd);
                return 0;
                break;
            default:
                 printf("输入错误，请重新输入.\n");
                 break;
        }
        }
        while(have_sign_in ==1 )
        {
            have_in();
            fflush(stdout);
            printf("my_chat:");
            fflush(stdout);
            scanf("%s",cmd);
            while(getchar()!='\n')
                ;
            if(strlen(cmd) != 1)
            {
                printf("输入错误，请重新输入.\n");
                break;
            }
            else if(strlen(cmd) == 1)
            op = cmd[0];
            memset(cmd,0,1024);
            printf("%c\n",op);
            
            switch(op)
            {
                case 'a':
            private_chat(connfd);
            pthread_mutex_lock(&my_chat_mutex);
            pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
            pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'b':
                    add_friend(connfd);
            pthread_mutex_lock(&my_chat_mutex);
            pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
            pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'c':
                    agree(connfd);                
            pthread_mutex_lock(&my_chat_mutex);
            pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
            pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'd':
                    delete_friend(connfd);
            pthread_mutex_lock(&my_chat_mutex);
            pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
            pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'e':
                    scan_friend(connfd);
            pthread_mutex_lock(&my_chat_mutex);
            pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
            pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'f':
                    scan_msg(connfd);
            pthread_mutex_lock(&my_chat_mutex);
            pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
            pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'g':
                    shield_friend(connfd);
            pthread_mutex_lock(&my_chat_mutex);
            pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
            pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'x':
                    have_sign_in -- ;
                    my_exit(connfd);
                    break;
                case 'i':    //查看自己帐号的一些信息
                    look_self_data();
                    break;
                case 'j':  //群聊
                    group_chat(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'k': // 建群
                    create_group(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'l':   //解散
                    delete_group(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'm':   //加群
                    join_group(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'n':    //同意加入群
                    agree_join_group(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'o':  //退群
                    quit_group(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'p':   //查看已加群
                    scan_group(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'r':   //查看群成员
                    scan_group_people(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 's':   //设置群管理员
                    set_administrator(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 't':   //踢人
                    kick(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'u':    //查看聊天记录
                    scan_group_chat_message(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'h':    //恢复好友
                    recover_friend(connfd);
                    pthread_mutex_lock(&my_chat_mutex);
                    pthread_cond_wait(&my_chat_cond,&my_chat_mutex);
                    pthread_mutex_unlock(&my_chat_mutex);
                    break;
                case 'v':   //传输文件
                    deliver_file(connfd);
                    break;
                case 'y' :  //接受文件
                    receive_file(connfd);
                    break;
                default:
                    printf("输入错误,请重新输入.\n");
                    break;
            }


        }
        
    }
}

void 
deliver_file(int connfd)
{
    char path[100];
    char buf[1024];
    char read_buf[512];
    char recv_account[30];
    printf("请输入你要发送的用户的帐号:");
    fflush(stdout);
    scanf("%s",recv_account);
    while(getchar()!='\n')
        ;
    printf("请输入你传入的文件名字:");
    scanf("%s",path);
    while(getchar()!='\n')
        ;

    int fd = open(path,O_RDONLY);
    if(fd < 0)
    {
        printf("打开文件失败\n");
        return ;
    }
    struct stat stat_buf;
    fstat(fd,&stat_buf);
    sprintf(buf,"{\"deliver_file\":{\"path\":\"%s\",\"recv_account\":\"%s\",\"deliver_account\":\"%s\"}}",path,recv_account,ps.account);
    send(connfd,buf,1024,0);
    sendfile(connfd,fd,NULL,stat_buf.st_size);
    sprintf(buf,"null");
    send(connfd,buf,1024,0);
    printf("传输文件成功");
}

void 
scan_group_chat_message(int connfd)
{
    char group_account[30];
    char buf[1024];
    printf("请输入你要查看聊天记录的群");
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"scan_group_chat_message\":{\"group_account\":\"%s\",\"user_account\":\"%s\"}}",group_account,ps.account);
    send(connfd,buf,1024,0);
}

void 
group_chat(int connfd)
{
    char buf[1024];
    char group_account[30];
    char message[1024];
    char ch;
    int i = 0;
    printf("请输入你要发送消息的群:");
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
        ;
    printf("请输入你要发送的消息:");
    fflush(stdout);
    while ((ch = getchar()) != '\n')
        message[i++] = ch ;
    message[i]='\0';
    sprintf(buf,"{\"group_chat\":{\"group_account\":\"%s\",\"message\":\"%s\",\"user_account\":\"%s\",\"user_name\":\"%s\"}}",group_account,message,ps.account,ps.id);
    send(connfd,buf,1024,0);
}

void
quit_group(int connfd)
{
    char group_account[30];
    char buf[1024];
    printf("请输入你想要退出群的帐号:\n");
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"quit_group\":{\"group_account\":\"%s\",\"user_account\":\"%s\"}}",group_account,ps.account);
    send(connfd,buf,1024,0);
}

void 
kick(int connfd)
{
    char kick_account[30];
    char group_account[30];
    char buf[1024];
    printf("输入想要踢人的群:");
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
        ;
    printf("输入你要踢的人的帐号:");
    fflush(stdout);
    scanf("%s",kick_account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"kick\":{\"group_account\":\"%s\",\"kick_account\":\"%s\",\"account\":\"%s\"}}",group_account,kick_account,ps.account);
    send(connfd,buf,1024,0);
}

void 
set_administrator(int connfd)
{
    char group_account[30];
    char admin_account[30];
    char buf[1024];
    printf("输入你要设置管理员的群的群号:");
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
        ;
    printf("输入你要设置管理员的帐号:");
    fflush(stdout);
    scanf("%s",admin_account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"set_admin\":{\"group_account\":\"%s\",\"admin_account\":\"%s\",\"user_account\":\"%s\"}}",group_account,admin_account,ps.account);
    send(connfd,buf,1024,0);
}

void
scan_group_people(int connfd)
{
    char buf[1024];
    char group_account[30];
    printf("输入你想要查看的群:");
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"scan_group_people\":\"%s\"}",group_account);
    send(connfd,buf,1024,0);
}

void 
scan_group(int connfd)
{
    char buf[1024];
    sprintf(buf,"{\"scan_group\":\"%s\"}",ps.account);
    send(connfd,buf,1024,0);
}

void 
agree_join_group(int connfd)
{
    char group_account[30];
    char group_owner_account[30];
    char account[30];   //这个是要添加这个群的人的帐号
    char buf[1024];
    printf("请输入你要同意添加的群:") ;
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
        ;
    strcpy(group_owner_account,ps.account);
    
    printf("请输入你要同意添加这个群的用户的帐号:");
    fflush(stdout);
    scanf("%s",account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"agree_join_group\":{\"group_account\":\"%s\",\"group_owner_account\":\"%s\",\"account\":\"%s\"}}",group_account,group_owner_account,account);
    send(connfd,buf,1024,0);
}


void 
join_group(int connfd)
{
    char buf[1024];
    char group_account[30];
    printf("请输入你想要加入你的群:");
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"join_group\":{\"name\":\"%s\",\"user_account\":\"%s\",\"group_account\":\"%s\"}}",ps.id,ps.account,group_account);  
    send(connfd,buf,1024,0);
}


void 
delete_group(int connfd)
{
    char buf[1024];
    char group_account[30];
    printf("请输入要删除的群:");
    fflush(stdout);
    scanf("%s",group_account);
    while(getchar()!='\n')
            ;
    sprintf(buf,"{\"delete_group\":{\"group_account\":\"%s\",\"user_account\":\"%s\"}}",group_account,ps.account);
    send(connfd,buf,1024,0);
}


void 
create_group(int connfd)
{
    char buf[1024];
    char name[100];
    printf("请输入你建的群的名称:");
    fflush(stdout);
    scanf("%s",name);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"create_group\":{\"group_name\":\"%s\",\"manager_account\":\"%s\",\"manager_name\":\"%s\"}}",name,ps.account,ps.id);
    send(connfd,buf,1024,0);
}


void 
recover_friend(int connfd)
{
    char account[30];
    char buf[1024];
    printf("输入你要恢复的好友帐号:");
    fflush(stdout);
    scanf("%s",account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"recover_fri\":{\"user\":\"%s\",\"fri\":\"%s\"}}",ps.account,account);
    send(connfd,buf,1024,0);
}


void 
shield_friend(int connfd)
{
    char buf[1024];
    char account[30];
    printf("输入你要拉黑的帐号:");
    fflush(stdout);
    scanf("%s",account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"shi_fri\":{\"account1\":\"%s\",\"account2\":\"%s\"}}",ps.account,account);
    send(connfd,buf,1024,0);
}


//查看聊天记录
void 
scan_msg(int connfd)
{
    char buf[1024];
    memset(buf,0,1024);
    char fri_account[30];
    printf("输入你要查看的记录的好友的帐号:");
    scanf("%s",fri_account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"scan_msg\":{\"account\":\"%s\",\"fri_account\":\"%s\"}}",ps.account,fri_account);
    send(connfd,buf,1024,0);
}


//私聊功能
void 
private_chat(int connfd)
{
    char account[30];
    char buf[1024];
    char buf1[1024];
    printf("输入你要发送的帐号:");
    fflush(stdout);
    scanf("%s",account);
    while(getchar()!='\n')
        ;
    printf("输入你要发送的消息:");
    fflush(stdout);
    int  ch,i=0;
    while((ch = getchar())!='\n')
        buf[i++]=ch;
    buf[i] = '\0';

    sprintf(buf1,"{\"pri_chat\":{\"account\":\"%s\",\"buf\":\"%s\",\"account1\":\"%s\",\"name\":\"%s\"}}",account,buf,ps.account,ps.id);
    send(connfd,buf1,1024,0);   
}


int
setnoblocking(int fd)
{

    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option|SOCK_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

int 
agree(int connfd)
{
    char account[20];
    char buf[1024];
    printf("请输入你要同意的帐号:");
    scanf("%s",account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"agree\":{\"account1\":\"%s\",\"account2\":\"%s\"}}",ps.account,account);
    send(connfd,buf,1024,0);
    return 0;

}

void *
pthread(void *arg)
{
    pthread_detach(pthread_self());
    char buf[1024];
    int fd = *(int*)arg;

    if((efd = epoll_create(1))==-1)
        my_err("epoll_create",__LINE__);
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(epoll_ctl(efd,EPOLL_CTL_ADD,fd,&event)==-1)
        my_err("epoll_ctl",__LINE__);
    events = (struct epoll_event*)calloc(MAXEVENTS,sizeof(struct epoll_event));
    if(events == NULL)
        my_err("calloc",__LINE__);
    

    while(1)
    {
        int n,i;
        n = epoll_wait(efd,events,MAXEVENTS,-1);
        for(i = 0;i<n;i++)
        {

           if(events[i].events&EPOLLIN)
           {
                recv(fd,buf,1024,0);
                find_function(fd,buf);
                pthread_cond_signal(&my_chat_cond);
           }

        }
    }

}

void receive_file(int connfd)
{
    char name[50];
    char way[30];
    char buf[1024];
    printf("输入你想要接受或拒绝的文件");
    scanf("%s",name);
    while(getchar()!='\n');
    ;
    while(1)
    {
        printf("输入yes或者no接收或者拒绝文件:");
        scanf("%s",way);
        while(getchar()!='\n')
            ;
        if(!strcmp(way,"yes"))
            break;
        else if(!strcmp(way,"no"))
            break;
        printf("输入错误，请重新输入");
    }
    sprintf(buf,"{\"receive_file\":{\"account\":\"%s\",\"way\":\"%s\",\"name\":\"%s\"}}",ps.account,way,name);
    send(connfd,buf,1024,0);
    printf("发送成功\n");
}


//查看好友列表和状态
void 
scan_friend(int connfd)
{
    char buf[1024];
    sprintf(buf,"{\"scan_fri\":\"%s\"}",ps.account);
    if(send(connfd,buf,1024,0)!=1024)
        my_err("send",__LINE__);
}

//删除好友
void 
delete_friend(int connfd)
{
    char fri_account[20];
    printf("输入你要删除的好友的帐号:");
    fflush(stdout);
    scanf("%s",fri_account);
    while(getchar()!='\n')
        ;
    char buf[1024];
    sprintf(buf,"{\"delete_fri\":{\"user\":\"%s\",\"fri\":\"%s\"}}",ps.account,fri_account);
    send(connfd,buf,1024,0);
}


void 
find_function(int fd,char *str)
{
    char buf[30];
    int j = 0;
    char *find = strchr(str,':');
    if(find != NULL)   //没有输入正确直接推出
   {
    for(char *i = str+2;i!=find;i++)
    {
        if(*i != '\"')
        {
            buf[j] = *i;
            j++;
        }
    }
    buf[j] = '\0';
    if(strcmp(buf,"agree")==0)
    {
        char account[20];
        char name[20];
        cJSON *root = NULL;
        cJSON *item = NULL;
        cJSON *next = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"agree");
        next = cJSON_GetObjectItem(item,"account");
        sprintf(account,"%s",next->valuestring);
        next = cJSON_GetObjectItem(item,"id");
        sprintf(name,"%s",next->valuestring);
        printf("帐号为:%s id为:%s 请求添加好友。",account,name);
        fflush(stdout);
    }
    else if(0==strcmp(buf,"sign_find"))
    {
        printf("该帐号不存在\n");
        pthread_cond_signal(&sign_in_cond);
    }
    else if(strcmp(buf,"sign_in")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"sign_in");
        char buf[5];
        sprintf(buf,"%s",item->valuestring);
        if(strcmp(buf,"1")==0)
        {
            printf("帐号登陆成功\n");
            have_sign_in = 1;
        }
        else if(strcmp(buf,"0")==0) 
        {
            printf("帐号登陆失败\n");
        }
        pthread_cond_broadcast(&sign_in_cond);
    }
    else if(strcmp(buf,"sign_up")==0)
    {
        char account[20];
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"sign_up");
        printf("你的帐号是:%s\n",item->valuestring);
        pthread_cond_broadcast(&sign_in_cond);
        printf("注册结束、、\n");
    }
    else if(strcmp(buf,"name")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        cJSON *next = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"name");
        next = cJSON_GetObjectItem(item,"name");
        strcpy(ps.id,next->valuestring);
        next = cJSON_GetObjectItem(item,"account");
        strcpy(ps.account,next->valuestring);
    }
    else if(strcmp(buf,"add_friend")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"add_friend");
        printf("%s\n",item->valuestring);
    
    }
    else if(strcmp(buf,"agree")==0)
    {
        char account[20];
        char name[20];
        cJSON *root = NULL;
        cJSON *item = NULL;
        cJSON *next = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"argee");
        next = cJSON_GetObjectItem(item,"account");
        strcpy(account,next->valuestring);
        next  = cJSON_GetObjectItem(item,"id");
        strcpy(name,next->valuestring);
        printf("帐号为:%s 名称为：%s 请求添加好友.",account,name);

    }
    else if(strcmp(buf,"agr")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"agr");
        printf("%s\n",item->valuestring);

    }
    else if(strcmp(buf,"del_fri")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"del_fri");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"scan_fri")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root  = cJSON_Parse(str);
        item =  cJSON_GetObjectItem(root,"scan_fri");
        printf("%s\n",item->valuestring);
    
    }
    else if(strcmp(buf,"pri_chat_find")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"pri_chat_find");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"pri_chat_ser")==0)
    {
        char account[30];
        char name[30];
        char message[1024];
        cJSON *root = NULL;
        cJSON *item = NULL;
        cJSON *next = NULL;
        root  = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"pri_chat_ser");
        next = cJSON_GetObjectItem(item,"account");
        strcpy(account,next->valuestring);
        next = cJSON_GetObjectItem(item,"name");
        strcpy(name,next->valuestring);
        next = cJSON_GetObjectItem(item,"message");
        strcpy(message,next->valuestring);
        printf("帐号:%s 名字:%s 给你发送消息:%s\n",account,name,message);
    }
    else if (strcmp(buf,"chat_message")==0)
    {
        cJSON *root  = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"chat_message");
        printf("%s\n",item->valuestring);
    
    }
    else if(strcmp(buf,"shield_fri")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"shield_fri");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"recover")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root  = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"recover");
        printf("%s",item->valuestring);
    }
    else if(strcmp(buf,"create_group")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"create_group");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"join_group")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"join_group");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"join_apply_for")==0)
    {
        char account[30];
        char name[30];
        char group_name[100];
        char group_account[30];
        cJSON *root = NULL;
        cJSON *item =NULL;
        cJSON *next = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"join_apply_for");
        next = cJSON_GetObjectItem(item,"account");
        strcpy(account,next->valuestring);
        next = cJSON_GetObjectItem(item,"name");
        strcpy(name,next->valuestring);
        next = cJSON_GetObjectItem(item,"group_name");
        strcpy(group_name,next->valuestring);
        next = cJSON_GetObjectItem(item,"group_account");
        strcpy(group_account,next->valuestring);
        printf("帐号为%s 名字:%s 请求加入群%s，该群的群号是:%s\n",account,name,group_name,group_account); 
    }
    else if(strcmp(buf,"agree_join_group")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item =cJSON_GetObjectItem(root,"agree_join_group");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"scan_group")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"scan_group");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"scan_group_people")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"scan_group_people");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"set_admin")==0)
    {
        cJSON*root =NULL;
        cJSON*item =NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"set_admin");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"kick")==0)
    {
        cJSON *root =NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"kick");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"delete_group")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"delete_group");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"quit_group")==0)
    {
        cJSON *root = NULL;
        cJSON *item =NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"quit_group");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"group_chat")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"group_chat");
        printf("%s\n",item->valuestring);

    }
    else if(strcmp(buf,"scan_group_chat_message")==0)
    {
        cJSON *root = NULL;
        cJSON *item = NULL;
        root  = cJSON_Parse(str);
        item  = cJSON_GetObjectItem(root,"scan_group_chat_message");
        printf("%s\n",item->valuestring);
    }
    else if(strcmp(buf,"deliver_file")==0)
    {
        cJSON *root =NULL;
        cJSON *item =NULL;
        root = cJSON_Parse(str);
        item = cJSON_GetObjectItem(root,"deliver_file");
        printf("%s\n",item->valuestring);

    }
        else if(strcmp("find_back",buf)==0)
        {
            
            char question[512];
            char answer1[512];
            char answer[512];
            char passwd[512];
            cJSON *root = NULL;
            cJSON *item = NULL;
            cJSON *next = NULL;
            root = cJSON_Parse(str);
            item = cJSON_GetObjectItem(root,"find_back");
            next = cJSON_GetObjectItem(item,"question");
            strcpy(question,next->valuestring);
            next = cJSON_GetObjectItem(item,"answer");
            strcpy(answer,next->valuestring);
            next = cJSON_GetObjectItem(item,"password");
            strcpy(passwd,next->valuestring);
            if(strcmp(question,"null")==0)
            {
                printf("该用户不存在\n");
                while(find_back_flag == 0)
                    pthread_cond_signal(&find_back_cond);
                find_back_flag = 0;
                return ;
            }
            printf("你的密保问题:%s\n",question);
            printf("请输入你的答案:");
            fflush(stdout);
            scanf("%s",answer1);
            while(getchar()!='\n')
                ;
            if(strcmp(answer,answer1)==0)
            {
                printf("你的密码是:%s\n",passwd);

            }
            else 
            {
                printf("输入的答案错误\n");
                fflush(stdout);
            }
            while(find_back_flag == 0)
                pthread_cond_signal(&find_back_cond);
            find_back_flag = 0;
        }
        else if( strcmp(buf,"receive_file")==0 )
        {
            cJSON *root = NULL;
            cJSON *item = NULL;
            cJSON *next = NULL;
            char file_name[50];
            root = cJSON_Parse(str);
            item = cJSON_GetObjectItem(root,"receive_file");
            next = cJSON_GetObjectItem(item,"file_name");
            strcpy(file_name,next->valuestring);
            next = cJSON_GetObjectItem(item,"way");
            char path[50];
            sprintf(path,"%s.rec",file_name);
            if(strcmp("begin",next->valuestring)==0)
            {
                printf("开始接受文件\n");
                fp =  fopen(path,"a");
            }
            else if(strcmp("end",next->valuestring)==0)
            {
                printf("接受文件完成\n");
            }
        }
        else 
        {
            fwrite(str,1,1024,fp);
            printf("%s\n",str);
        }
    }
    else
    {
        fwrite(str,1,1024,fp);
    }
}


void
my_exit(int connfd)
{
    printf("退出系统\n");
    char buf[1024];
    sprintf(buf,"{\"my_exit\":\"%s\"}",ps.account);
    send(connfd,buf,1024,0);
    memset(ps.account,0,20);
    memset(ps.id,0,30);
}

void 
look_self_data(void)
{
    printf("你的帐号:");
    printf("%s\n",ps.account);
    printf("你的名称:");
    printf("%s\n",ps.id);
}

int 
add_friend(int connfd)
{
    char account[20];
    char buf[1024];
    printf("输入你要添加好友的帐号:");
    scanf("%s",account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"add_friend\":{\"account1\":\"%s\",\"name\":\"%s\",\"account2\":\"%s\"}}",ps.account,ps.id,account);
    send(connfd,buf,1024,0);
    return 0;
}



void 
quit(int connfd)
{
    char buf[1024];
    sprintf(buf,"{\"quit\":\"quit\"}");
    send(connfd,buf,1024,0);
    close(connfd);
    printf("say good bye!\n");
}

void 
have_in()
{
    printf("*************************\n");
    printf("A.私聊、\n");
    printf("B.好友添加、\n");
    printf("C.同意好友添加、\n");
    printf("D.好友删除、\n");
    printf("E.查看好友列表和状态、\n");
    printf("F.查看好友聊天记录、\n");
    printf("G.拉黑好友、\n");
    printf("H.恢复好友\n");
    printf("I.查看自己的信息、\n");
    printf("J.群聊、\n");
    printf("K.建群、\n");
    printf("L.解散、\n");
    printf("M.加群、\n");
    printf("N.同意加入群、\n");
    printf("O.退群、\n");
    printf("P.查看已加群、\n");
    printf("R.查看群成员、\n");
    printf("S.设置群管理员、\n");
    printf("T.踢人、\n");
    printf("U.查看聊天记录、\n");
    printf("V.传输文件、\n");
    printf("Y.接受或者拒绝文件、\n");
    printf("X.退出帐号、\n");
    printf("*************************\n");
}

int
sign_up(int fd)
{
    char buf[1024];
    char name[30];
    
   printf("请输入你的用户名:");
   scanf("%s",name);
   while(getchar()!='\n');
   char passwd1[20];
   char passwd2[20];
   char question[256];
   char answer[256];
   while(1)
   {
       system("stty -echo");
        printf("请输入你的密码:\n");
        scanf("%s",passwd1);
        while(getchar()!='\n')
            ;
        printf("请再次输入你的密码:\n");
        scanf("%s",passwd2);
        system("stty echo");
        while(getchar()!='\n')
            ;

        if(strcmp(passwd1,passwd2)==0)
        {
            printf("请输入你的密保问题:");
            scanf("%s",question);
            printf("请输入你的密保答案:");
            scanf("%s",answer);
            sprintf(buf,"{\"sign_up\":{\"name\":\"%s\",\"password\":\"%s\",\"question\":\"%s\",\"answer\":\"%s\"}}",name,passwd1,question,answer);
            if(send(fd,buf,156,0)<0)
            my_err("send",__LINE__);
            printf("你的密码是:%s\n",passwd1);
            while(getchar()!='\n');
            return 0;
        }
        else 
        {
            printf("两次输入的密码不一样，请重新输入!\n");
        }
   }
}


void
start()
{
    printf("*****************\n");
    printf("A.登陆、\n");
    printf("B.注册、\n");
    printf("C.找回密码、\n");
    printf("D.退出、\n");
    printf("*****************\n");
    fflush(stdout);
}

int
open_clientfd(char *hostname,int port)
{
    int clientfd;
    struct hostent *hp =NULL;
    struct sockaddr_in serveraddr;
    if((clientfd = socket(AF_INET,SOCK_STREAM,0))<0)
        my_err("socket",__LINE__);
    if((hp = gethostbyname(hostname))==NULL)
        my_err("gethostbyname",__LINE__);
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char*)hp->h_addr_list[0],(char*)&serveraddr.sin_addr,hp->h_length);
    serveraddr.sin_port = htons(port);
    
    //connect
    if(connect(clientfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0)
        my_err("connect",__LINE__);
    return clientfd;

}


void 
my_err(char *string,int line)
{
    printf("%d %s error!\n",line,string);
    printf("%s\n",strerror(errno));
    exit(1);
}

void 
sign_in(int connfd)
{
    char buf[1024];
    char id[30];
    char passwd[30];
    printf("输入你的帐号:");
    scanf("%s",id);
    printf("输入你的密码:\n");
    system("stty -echo");
    scanf("%s",passwd);
    system("stty echo");
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"sign_in\":{\"id\":\"%s\",\"passwd\":\"%s\"}}",id,passwd);
    send(connfd,buf,1024,0);
    
}


//找回密码

void 
find_back(int connfd)
{
    char buf[1024];
    char account[30];
    printf("请输入你的帐号:");
    scanf("%s",account);
    while(getchar()!='\n')
        ;
    sprintf(buf,"{\"find_back\":\"%s\"}",account);
    send(connfd,buf,1024,0);
}

