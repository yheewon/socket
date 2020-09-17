#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<signal.h>
#include<sys/types.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>


void connect_socket();
void event_handler();//alarm 종료 함수
int manager();
void login();
void manage_book();
void manage_customer();
void lent_book();
void return_book();
void print_rent();
int print_book();
void insert_book();
void update_book();
void delete_book();
int print_customer();
void insert_customer();
void update_customer();
void delete_customer();


int sid;


void main()
{
    connect_socket();
}

void connect_socket(){
    int stop=0;
    char s[10]={},s1[10]={};
    struct sockaddr_in ssock,csock;
    sid=socket(AF_INET,SOCK_STREAM,0);
    ssock.sin_family=AF_INET;
    ssock.sin_addr.s_addr=inet_addr("127.0.0.1");
    ssock.sin_port=htons(9898);
    connect(sid,(struct sockaddr *)&ssock,sizeof(ssock));
    while(stop==0) stop=manager();
    close(sid);
}

void event_handler(){
    printf("\n입력이 없어 종료합니다.");
    exit(0);
}

int manager(){
    signal(SIGALRM,event_handler);
    int choice=0;
    alarm(60);
    printf("1분안에 입력하세요.\n0.로그인\n1.책관리\n2.회원관리\n3.책 빌리기\n4.책 반납\n5.책 대여기록 보기\n6.종료\n번호 입력 : ");
    scanf("%d",&choice);
    alarm(0);
    write(sid,(int *)&choice,1);//choice 서버에 전송
    if(choice==0) login();
    else if(choice==1) manage_book();//책관리
    else if(choice==2) manage_customer();
    else if(choice==3) lent_book();
    else if(choice==4) return_book();
    else if(choice==5) print_rent();
    else return 10;
}

void login(){
    int id=0;
    char msg[100]={};
    print_customer();//회원 출력
    printf("로그인 하고싶은 회원 아이디 입력 : ");
    scanf("%d",&id);
    write(sid,(int *)&id,1);
    read(sid, &msg, sizeof(msg));
    if(strcmp(msg,"회원가입 해주세요.")==0) printf("%s\n",msg);
    else printf("%s님 환엽합니다.\n",msg);
}

void manage_book(){
    signal(SIGALRM,event_handler);
    int choice=0;

    alarm(60);
    printf("\n1분안에 입력하세요.\n1.전체 책검색\n2.입력\n3.수정\n4.삭제\n번호 입력 :");
    scanf("%d",&choice);
    alarm(0);
    write(sid,(int *)&choice,1);//choice 서버에 전송
    if (choice == 1) print_book();
    else if (choice == 2) insert_book();
    else if(choice==3) update_book();
    else if (choice==4) delete_book();
}

void manage_customer(){
    signal(SIGALRM,event_handler);
    int choice=0;

    alarm(60);
    printf("\n1분안에 입력하세요.\n1.전체 회원검색\n2.입력\n3.수정\n4.삭제\n번호 입력 :");
    scanf("%d",&choice);
    alarm(0);
    write(sid,(int *)&choice,1);//choice 서버에 전송
    if (choice == 1) print_customer();
    else if (choice == 2) insert_customer();
    else if(choice==3) update_customer();
    else if (choice==4) delete_customer();
}

void lent_book(){
    int id=0;
    print_book();//출력
    printf("대여하고 싶은 책 id입력 : ");
    scanf("%d",&id);
    write(sid,(int *)&id,1);

}

void return_book(){
    int id=0;
    print_book();
    printf("반납하고 싶은 책 id 입력 : ");
    scanf("%d",&id);
    write(sid,(int *)&id,1);
}

void print_rent(){
    int count=0,stop=1;
    read(sid,(int *)&count,1);
    printf("count: %d\n",count);
    while(stop<=count){
        int id=0,bookid=0,userid=0;
        char time[100]={},return_time[100]={};
        read(sid, (int *)&id, 1);
        printf("id : %d\n",id);
        read(sid, (int *)&bookid, 1);
        printf("bookid : %d\n",bookid);
        read(sid, (int *)&userid, 1);
        printf("userid : %d\n",userid);
        read(sid, &time, sizeof(time));
        printf("대여 시간 : %s\n",time);
        read(sid, &return_time, sizeof(return_time));
        printf("반납 시간 : %s\n",return_time);
        printf("-------------------------------------\n");
        stop ++;
    }
}

int print_book(){
    int count=0,stop=1;
    read(sid,(int *)&count,1);
    printf("count:%d\n",count);
    printf("id  제목    가격\n");
    while(stop<=count){
        int id = 0, price = 0;
        char name[20] = {};
        read(sid, (int *)&id, 1);
        read(sid, (int *)&price, 1);
        read(sid, &name, sizeof(name));
        printf("%d  %s  %d\n", id, name, price);
        stop++;
    }
    return 0;
}

int print_customer(){
    int count=0,stop=1;
    read(sid,(int *)&count,1);
    printf("count:%d\n",count);
    printf("id  이름    포인트\n");
    while(stop<=count){
        int id = 0, point = 0;
        char name[20] = {};
        read(sid, (int *)&id, 1);
        read(sid, (int *)&point, 1);
        read(sid, &name, sizeof(name));
        printf("%d  %s  %d\n", id, name, point);
        stop++;
    }
    return 0;
}

void insert_book(){
    int id=0,price=0;
    char name[20]={};
     
    printf("id 제목 가격 입력 :");
    scanf("%d ",&id);
    write(sid, (int *)&id, 1);
    scanf("%s",name); 
    write(sid,(void*)name,strlen(name));
    scanf("%d",&price);
    write(sid, (int *)&price, 1); 
}
void insert_customer(){
    int id=0,point=0;
    char name[20]={};
     
    printf("id 이름 포인트 입력 :");
    scanf("%d ",&id);
    write(sid, (int *)&id, 1);
    scanf("%s",name); 
    write(sid,(void*)name,strlen(name));
    scanf("%d",&point);
    write(sid, (int *)&point, 1); 
}

void update_book(){
    int id=0,error=0;
    char name[20]={};
    printf("수정하고 싶은 항목의 아이디 입력:");
    scanf("%d",&id);
    write(sid, (int *)&id, 1);
    printf("변경하고 싶은 제목: ");
    scanf("%s",name);
    write(sid,(void*)name,strlen(name));
    read(sid, (int *)&error, 1);
    if(error==1) printf("변경되었습니다.\n");
    else printf("없는 id입니다.\n");
}

void update_customer(){
    int id=0,error=0;
    char name[20]={};
    printf("수정하고 싶은 항목의 아이디 입력:");
    scanf("%d",&id);
    write(sid, (int *)&id, 1);
    printf("변경하고 싶은 이름: ");
    scanf("%s",name);
    write(sid,(void*)name,strlen(name));
    read(sid, (int *)&error, 1);
    if(error==1) printf("변경되었습니다.\n");
    else printf("없는 id입니다.\n");
}

void delete_book(){
    char name[20]={};
    int error=0;
    printf("삭제하고 싶은 항목의 제목 입력: ");
    scanf("%s",name);
    write(sid,(void*)name,strlen(name));
    read(sid, (int *)&error, 1);
    if(error==1) printf("삭제되었습니다.\n");
    else printf("없는 항목입니다.\n");
}

void delete_customer(){
    char name[20]={};
    int error=0;
    printf("삭제하고 싶은 항목의 이름 입력: ");
    scanf("%s",name);
    write(sid,(void*)name,strlen(name));
    read(sid, (int *)&error, 1);
    if(error==1) printf("삭제되었습니다.\n");
    else printf("없는 항목입니다.\n");
}

