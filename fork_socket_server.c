#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <time.h> // 현재시간
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "book.h"
#include "customer.h"
#include "rent.h"
#define START_ID 1

void connect_socket(int argc, char *argv[]);//소켓 연결
int manager(int argc, char *argv[],int client_sockfd);
void login(int argc, char *argv[],FILE*fp,int client_sockfd,struct customer rec);
void manage_book(int argc, char *argv[],FILE*fp,int client_sockfd,struct book rec);
void manage_customer(int argc, char *argv[],FILE*fp,int client_sockfd,struct customer rec1);
void lent_book(int argc, char *argv[],FILE*fp,int client_sockfd,struct book rec,struct rent rec2);
void return_book(int argc, char *argv[],FILE*fp,int client_sockfd,struct book rec,struct rent rec2);
void print_rent(int argc, char *argv[],FILE*fp,int client_sockfd,struct rent rec);
void insert_book(int argc, char *argv[],FILE *fp,struct book rec,int client_sockfd);
void print_book(int argc, char *argv[],FILE *fp,struct book rec,int client_sockfd);
void update_book(int argc, char *argv[],FILE *fp,struct book rec,int client_sockfd);
void delete_book(int argc, char *argv[],FILE *fp,struct book rec,int client_sockfd);
void insert_customer(int argc, char *argv[],FILE *fp,struct customer rec,int client_sockfd);
void print_customer(int argc, char *argv[],FILE *fp,struct customer rec,int client_sockfd);
void update_customer(int argc, char *argv[],FILE *fp,struct customer rec,int client_sockfd);
void delete_customer(int argc, char *argv[],FILE *fp,struct customer rec,int client_sockfd);

int user_id=0;//현재 로그인 되어있는 회원 아이디

int main(int argc, char *argv[])
{
    connect_socket(argc, argv); // 소켓 연결
}

void connect_socket(int argc, char *argv[]){
    int stop=0;
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9898);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    listen(server_sockfd, 5);

    signal(SIGCHLD, SIG_IGN);

    while (1)
    {
        printf("server waiting\n");
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
    
        if (fork() == 0)
        {
            printf("클라이언트와 연결\n");
            while(stop==0) stop=manager(argc,argv,client_sockfd);
            close(client_sockfd);
            printf("종료\n");
            break;
            exit(0);
        }
        else  close(client_sockfd);
    }

}

int manager(int argc, char *argv[],int client_sockfd){
    FILE *fp;
    struct book rec;
    struct customer rec1;
    struct rent rec2;

    int choice = 0;
    if (argc != 2) {
        fprintf(stderr, "사용법: %s 파일이름\\n", argv[0]);
        exit(1);
    }
    read(client_sockfd,(int *)&choice,1);//클라이언트에서 전송한 choice받기
    printf("입력 :%d\n",choice);
    if(choice==0) login(argc,argv,fp,client_sockfd,rec1);
    else if(choice==1) manage_book(argc,argv,fp,client_sockfd,rec);
    else if(choice==2) manage_customer(argc,argv,fp,client_sockfd,rec1);
    else if(choice==3) lent_book(argc,argv,fp,client_sockfd,rec,rec2);
    else if(choice==4) return_book(argc,argv,fp,client_sockfd,rec,rec2);
    else if(choice==5) print_rent(argc,argv,fp,client_sockfd,rec2);
    else return 10;
}

void login(int argc, char *argv[],FILE*fp,int client_sockfd,struct customer rec){
    int id=0;
    char msg[100]="회원가입 해주세요.";
    print_customer(argc,argv,fp,rec,client_sockfd);//출력
    read(client_sockfd,(int *)&id,1);

    if ((fp = fopen("customer", "rb")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
    fseek(fp,(id-START_ID)*sizeof(rec),SEEK_SET);
    if((fread(&rec,sizeof(rec),1,fp)>0)&&rec.id!=0)
         write(client_sockfd, (void *)rec.name, strlen(rec.name));
    else write(client_sockfd, (void *)msg, strlen(msg));
    user_id=id;
    printf("로그인 %d\n",user_id);
    fclose(fp);
}

void manage_book(int argc, char *argv[],FILE*fp,int client_sockfd,struct book rec){
    int choice=0;
    read(client_sockfd,(int *)&choice,1);//클라이언트에서 전송한 choice받기
    printf("입력 :%d\n",choice);
    if(choice==1) print_book(argc,argv,fp,rec,client_sockfd);
    else if(choice==2) insert_book(argc,argv,fp,rec,client_sockfd);
    else if(choice==3) update_book(argc,argv,fp,rec,client_sockfd);
    else if(choice==4) delete_book(argc,argv,fp,rec,client_sockfd);
}

void manage_customer(int argc, char *argv[],FILE*fp,int client_sockfd,struct customer rec1){
    int choice=0;
    read(client_sockfd,(int *)&choice,1);//클라이언트에서 전송한 choice받기
    printf("입력 :%d\n",choice);
    if(choice==1) print_customer(argc,argv,fp,rec1,client_sockfd);
    else if(choice==2) insert_customer(argc,argv,fp,rec1,client_sockfd);
    else if(choice==3) update_customer(argc,argv,fp,rec1,client_sockfd);
    else if(choice==4) delete_customer(argc,argv,fp,rec1,client_sockfd);
}

void lent_book(int argc, char *argv[],FILE*fp,int client_sockfd,struct book rec,struct rent rec2){
    time_t now;
    int id=0,count=0;
    print_book(argc,argv,fp,rec,client_sockfd);//출력
    read(client_sockfd,(int *)&id,1);//아이디 받음

    fp=fopen("rent","ab+");//rent.h에 정보 저장
    while(fread(&rec2,sizeof(rec2),1,fp)>0){
        if(rec2.id!=0) count++;
    }
    rec2.id=count+1;
    rec2.bookid=id;
    rec2.userid=user_id;
    time(&now);
    strcpy(rec2.time, ctime(&now));
    strcpy(rec2.return_time, "대여중");
   
    fseek(fp,(rec2.id-START_ID)*sizeof(rec2),SEEK_SET);
    fwrite(&rec2,sizeof(rec2),1,fp);
    printf("%s",rec2.time);
    fclose(fp);
}

void return_book(int argc, char *argv[],FILE*fp,int client_sockfd,struct book rec,struct rent rec2){
    time_t now;
    int id=0;
    print_book(argc,argv,fp,rec,client_sockfd);//출력
    read(client_sockfd,(int *)&id,1);//아이디 받음

   if ((fp = fopen("rent", "rb+")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
    fseek(fp,(id-START_ID)*sizeof(rec2),SEEK_SET);
    if((fread(&rec2,sizeof(rec2),1,fp)>0)&&(rec2.id!=0)){
        time(&now);
        strcpy(rec2.return_time, ctime(&now));
        fseek(fp, -sizeof(rec2), SEEK_CUR);
        fwrite(&rec2, sizeof(rec2), 1, fp);
    }else printf("대여한 목록에 없습니다ㅣ. \n");
    printf("%s\n",rec2.return_time);
    fclose(fp);
}

void print_rent(int argc, char *argv[],FILE*fp,int client_sockfd,struct rent rec){
    int count=0;
    printf("대여목록 출력\n");
    if ((fp = fopen("rent", "rb")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
     while (fread(&rec, sizeof(rec), 1, fp)>0) 
        if (rec.id != 0&&rec.userid==user_id) count++; 
    write(client_sockfd, (int *)&count, 1);
    printf("count: %d\n",count);
    fseek(fp,0L,SEEK_SET);
    while (fread(&rec, sizeof(rec), 1, fp)>0){
        if(rec.id!=0&&rec.userid==user_id){
            //sleep(4);
            write(client_sockfd, (int *)&rec.id, 1);
            sleep(2);
            write(client_sockfd, (int *)&rec.bookid, 1);
            sleep(2);
            write(client_sockfd, (int *)&rec.userid, 1);
            sleep(2);
            write(client_sockfd, (void *)&rec.time, strlen(rec.time));
            sleep(2);
            write(client_sockfd, (void *)&rec.return_time, strlen(rec.return_time));
            printf("%d  %d  %d  %s  %s\n",rec.id,rec.bookid,rec.userid,rec.time,rec.return_time);
        }
        sleep(4);
    }
    fclose(fp);
}

void print_book(int argc, char *argv[],FILE *fp,struct book rec,int client_sockfd){
    int count=0;
    if ((fp = fopen("book", "rb")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
     while (fread(&rec, sizeof(rec), 1, fp)>0) 
        if (rec.id != 0) count++; 
    write(client_sockfd,(int *)&count,1);
    fseek(fp,0L,SEEK_SET);
    while (fread(&rec, sizeof(rec), 1, fp) > 0){
        if (rec.id != 0){
            int id=0,price=0;
            char name[20]={};
            id=rec.id;
            strcpy(name,rec.name);
            price=rec.price;
            write(client_sockfd, (int *)&id, 1);
            write(client_sockfd, (int *)&price, 1);
            write(client_sockfd, (void *)name, strlen(name));
            printf("%d  %s  %d\n", rec.id, rec.name, rec.price);
        }
        sleep(4);
    }
    fclose(fp);
}

void print_customer(int argc, char *argv[],FILE *fp,struct customer rec,int client_sockfd){
    int count=0;
    if ((fp = fopen("customer", "rb")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
     while (fread(&rec, sizeof(rec), 1, fp)>0) 
        if (rec.id != 0) count++; 
    write(client_sockfd,(int *)&count,1);
    fseek(fp,0L,SEEK_SET);
    while (fread(&rec, sizeof(rec), 1, fp) > 0){
        if (rec.id != 0){
            int id=0,point=0;
            char name[20]={};
            id=rec.id;
            strcpy(name,rec.name);
            point=rec.point;
            write(client_sockfd, (int *)&id, 1);
            write(client_sockfd, (int *)&point, 1);
            write(client_sockfd, (void *)name, strlen(name));
            printf("%d  %s  %d\n", rec.id, rec.name, rec.point);
        }
        sleep(4);
    }
    fclose(fp);
}

void insert_book(int argc, char *argv[],FILE *fp,struct book rec,int client_sockfd){
    fp = fopen("book", "ab+");
    printf("책 정보 입력\n");
    char name[20]={};
    int id=0, price=0;
    
    read(client_sockfd,(int *)&id,1);
    printf("%d",id);
    read(client_sockfd,&name,sizeof(name));
    printf("%s",name);
    read(client_sockfd,(int *)&price,1);
    printf("%d\n",price);
    rec.id=id;
    strcpy(rec.name,name);
    rec.price=price;
    fseek(fp, (rec.id - START_ID) * sizeof(rec), SEEK_SET);
    fwrite(&rec, sizeof(rec), 1, fp);
    fclose(fp);
}

void insert_customer(int argc, char *argv[],FILE *fp,struct customer rec,int client_sockfd){
    fp = fopen("customer", "ab+");
    printf("회원 정보 입력\n");
    char name[20]={};
    int id=0, point=0;
    
    read(client_sockfd,(int *)&id,1);
    printf("%d",id);
    read(client_sockfd,&name,sizeof(name));
    printf("%s",name);
    read(client_sockfd,(int *)&point,1);
    printf("%d\n",point);
    rec.id=id;
    strcpy(rec.name,name);
    rec.point=point;
    fseek(fp, (rec.id - START_ID) * sizeof(rec), SEEK_SET);
    fwrite(&rec, sizeof(rec), 1, fp);
    fclose(fp);
}

void update_book(int argc, char *argv[],FILE *fp,struct book rec,int client_sockfd){
    int id=0,error=0;
    char name[20]={};
     if ((fp = fopen("book", "rb+")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
    read(client_sockfd,(int *)&id,1);
    printf("id: %d\n",id);
    read(client_sockfd,&name,sizeof(name));
    printf("name: %s\n",name);
    fseek(fp,(id- START_ID)*sizeof(rec),SEEK_SET);
    if((fread(&rec,sizeof(rec),1,fp)>0)&&(rec.id!=0)){
        error=1;
        strcpy(rec.name,name);
        fseek(fp,-sizeof(rec),SEEK_CUR);
        fwrite(&rec,sizeof(rec),1,fp);
    }
    write(client_sockfd,(int *)&error,1);
    fclose(fp);
}

void update_customer(int argc, char *argv[],FILE *fp,struct customer rec,int client_sockfd){
    int id=0,error=0;
    char name[20]={};
     if ((fp = fopen("customer", "rb+")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
    read(client_sockfd,(int *)&id,1);//수정하고 싶은 항목 id 받기
    printf("id: %d\n",id);
    read(client_sockfd,&name,sizeof(name));//수정한 제목 받기
    printf("name: %s\n",name);
    fseek(fp,(id- START_ID)*sizeof(rec),SEEK_SET);
    if((fread(&rec,sizeof(rec),1,fp)>0)&&(rec.id!=0)){
        error=1;
        strcpy(rec.name,name);// 제목 고치기
        fseek(fp,-sizeof(rec),SEEK_CUR);
        fwrite(&rec,sizeof(rec),1,fp);
    }
    write(client_sockfd,(int *)&error,1);//수정 성공여부 시그널 보내기
    fclose(fp);
}

void delete_book(int argc, char *argv[],FILE *fp,struct book rec,int client_sockfd){
    char name[20]={};
    int error=0;
    if ((fp = fopen("book", "rb+")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
    read(client_sockfd,&name,sizeof(name));//삭제하고 싶은 항목 제목 받기
    printf("name: %s\n",name);
    while (fread(&rec, sizeof(rec), 1, fp) > 0){
        if(rec.id!=0&&strcmp(rec.name,name)==0){
            error=1;
            rec.id=0;
            fseek(fp,-sizeof(rec),SEEK_CUR);
            fwrite(&rec,sizeof(rec),1,fp);
        }
    }
    write(client_sockfd,(int *)&error,1);
    fclose(fp);
}

void delete_customer(int argc, char *argv[],FILE *fp,struct customer rec,int client_sockfd){
    char name[20]={};
    int error=0;
    if ((fp = fopen("customer", "rb+")) == NULL) {
		fprintf(stderr, "파일 열기 오류\\n");
		exit(2);
	}
    read(client_sockfd,&name,sizeof(name));
    printf("name: %s\n",name);
    while (fread(&rec, sizeof(rec), 1, fp) > 0){
        if(rec.id!=0&&strcmp(rec.name,name)==0){
            error=1;
            rec.id=0;
            fseek(fp,-sizeof(rec),SEEK_CUR);
            fwrite(&rec,sizeof(rec),1,fp);
        }
    }
    write(client_sockfd,(int *)&error,1);
    fclose(fp);
}