#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define k 500.600
#define SIZE 200

//comment
/**
 * comment
 */
/*comment*/
int main(int argc,char *argv[]){//start
    for(int i=0;i<argc;i++){
        printf("%s",argv[i]);
    }

    printf("hi\n");
    

    int a=100,b=200;
    float c=300.400;
    //0100-64,0x10-16
    printf("Val=%d\n",a);
    //0.01=1E-2=10^-2
    const char e='1';
    printf("%f\n",k);
    //++,--,*=,/=,%=,^=(xor)
    int num1=1;
    int num2=2;
    printf("%d %d %d\n",num1++,++num2,!!(num1^num2));
    //==,>=,<=,!=
    //&&(oloi),||(1 toul)
    printf("%d\n",(num1!=num2)&&(num1>num2));
    printf("%c\r\t\n",e);//\a:sound,\\,\",\b,%%
    char str[]="string";
    printf("%u %s %E %p %x %o \n ",a,str,b,c,a,b);
    //scanf("%type",&input):%lf/d(prec);printf("%d",input);
    char c1;
    putchar('1');putchar('\n');
    //c1=getchar();
    //putchar(c1);
    printf("%.6f %.2s %+015.5f\n",c,str,c);//def:-.6f
    printf("same0000000000000000000000000000000 \ line\n");
    printf("%f %c %d\n",(float)a,(char)c);
    int day;
    scanf("%d",&day);
    if(day==1){}
    else if(day==2){}
    else {}
    //return exp1? iftrue exp2:iffalse exp3;
    //switch(){case i:break(exit)/continue(next it);default:;(last resort)}
    //case 1:case 2:;common
    //while(){},do{}while();
    for(int i=0,j=0;i<100;i++){printf("%0d-%0i\n",i,j);}
    //for(;i>0;)=while(i>0),for(;;)=while(1)
    int a[5]=0;//a[0]=1,a[1]=2
    //sum++=a[i]
    int arr[]={1,2,3};//sizeof(arr)=SIZE*sizeof(dt)
    //define SIZE ((int)(sizeof(arr)/sizeoff(arr[0])))
    const char chars[]={'1','2'};
    //T arr[ROWS][COLS]
    //Matrix[N][N];for(row<N){for(col<N)}

    //ΘεσηΜνημης A[i][j]=(i*COLS)+j+1

    int m[][5]={{1,2,3},
               {4,5,6}};
    //1*3d[2][3][4]->2*2d[3][4]
    //int x[]={0,1,0};->
    int x[]={[1]=1};//size=2
    double diag[][2]={[0][0]=1,[1][1]=1};

    int num,num1=1;
    int *ptr=NULL;//T* const ptr=&var;
    ptr=&num;//ptr->&num=1,pointer(addr)
    //*ptr=2; NOT POSSIBLE
    //j=*&i; i->&mem=var=j
    printf("%5i %5p %5p",*ptr,&ptr,ptr);//1,0xFF->0χCC=num,0xCC=num
    //T *q,*p;q=p == &q->[varQ]=&p->[varP]=>q[&varP],*q=*p=>q[varP] 
    //*p+i == p+(i*sizeof(T))
    //arr[i]=>*p+j->arr[i+j]
    //*q=*p+3=>p+3*sizeof(T)
    //p->a[i],q->a[j]=>p-q=i-j(in between!)
    //ptr1>ptr2=>BIGGER ADDR,ptr1+ptr2 NOT ALLOWED
    //T *p;for(p=&a[0](or p=a);p<&a[N](or a+N);p++){sum+=*p}
    //p->a[i]=>a[i++]=*(p++)=j,++(*p)
    char* p1;int *p2;p2=(int*)p1;
    void *p3;int k1=1;p3=&k1;*(int*)p3+=1;//2
    //T *ptr=&arr[0];arr[4]==*(ptr+4)
    //T a[]={};T *b=a;b[0]++; == *b+=1;
    
    //T *ptr,arr[]={};ptr=arr;"%p %d",&ptr[i],ptr[i]
    //arr[]={10,20,30,40,50};*ptr+2=12,*(ptr+2)=30

    //T* p;for(p=&a[0][0];p<=&a[ROWS-1][COLS-1];p++)
    //&arr[i][j]=arrBaseAddr+[(i*COLS+j)*sizeof(T)]
    //arr[a][b]<-*(ptr+(a*COLS+b))
    //p=&arr[i][0] == p=arr[i]

    //T (*ptr)[][];ptr->T[][]
    
    int **ipp;
    int n1=1,n2=2,n3=3;
    int *ip1=&n1,*ip2=&n2;//->n1,->n2
    ipp=&ip1;//->ip1->n1
    *ipp=ip2;//->ip1->n2
    *ipp=&n3;//->ip1->n3

    //T *a[]==T **a;

    //return_type fun_name(par){..return T};
    int a2=1;
   printf("%d %d\n",max(1,2),incr(&a2));
  // int largest=find_max();
  //int,float,double=0,char='\0',*p=NULL,char[]=""
    f();//a=1
    f();//a=2
    int i3,j3,*p10;
    decompose(3.14,&i3,&j3);//i3->3,j3->.14
    get1024B(&p10);//
    test(3,"txt",1,2.2);
    //if(!p==NULL)==if(p)
    strAppend("ad","bd");
    int nrows,ncols=10;
    int **array=(int **)malloc(nrows*sizeof(int *));
    for(int i=0;i<nrows;i++){
        array[i]=(int*)malloc(ncols*sizeof(int));
    }//nrows->ncols

    //memcpy(str1,str2,sizeof(str2)):copy-paste
    //memcmp(arr1,arr2,sizeof(arr1)):compare

    //stack,queue-linked,trees,graphs!!!

    char *pointer;
    pointer="abc";//p->a
    //scanf("%31s",str):31 chars(max)+'\0'
    //c=scanf("%[123]",str),123abc->c=1,str[]="123"
    //%[^]0-9-]:ALL CHARS EXCEPT ],0-9,-
    //scanf("%[^\n]",str):NO WHITE CHARS,NEW LINE

    char myStr[]="ece";
    puts(myStr);//PRINT+\n
    //strings[]=pointers
    //strlen("abc")->3
    //char *strncpy(char *dest,char *src,size_t count)

    //strncpy(dest,src,sizeof(dest)-1);dest[sizeof(dest)-1]='\0';
    //if(strcmp(str1,str2)<=0) == str1<=str2
    //strcat(str1,"abc"),strcat(str2,"def")=>strcat(str1,str2):str1="abcdef",str2="def"

    //sprintf()
    char *token,string[]="string,tokens\0,after,null";
    token=strtok(string,",");
    do{
        printf("token= %s\n",token);
    }while(token=strtok(NULL,","));

    //strto*()->αριθμητικη τιμη
    char *Str="123";char *StopStr;
    for(int i=0;i<=3;i++){
        long l=strtol(Str,&StopStr,i);
        printf("%ld (base %i)",l,i);
    }

    //char *str=malloc(sizeof(char)*N);const char *p;
    //p=str;while(*str){str++;}printf("%zu",str-p);


    char *table[]={"q","r"};//0->q\0,1->r\0
    //access structs
    var1.a=1;
    part1.x=2.2;
  
    strcpy(student1.name.first,"Geo");
    struct student students[]={"Geo","ECE",1};
    int h;
    printf("%10d",students[h].id);
    

    struct date *dp,d[10];
    d[0].day=1;
    d[0].month=2;
    d[0].year=2026;
    d[1]=d[0];
    dp=d;//dp->d[0]
    dp->year++;//d[0].year=2027
    (*dp).month=12;//dp->date,d[0].month=12
    dp++;//dp->d[1]
    dp->year++;//d[1].year=2027 == (*dp).year++;


    v.a='1';v.b=0x6152;

    Number numbers[100];numbers[0].i=1;
    N n;n.type=0;n.u.i=1;
    //struct gifts c;c.item.book.title="C FOR DUMMIES"
  
    enum boolean{a=false,b=true};
    if(!b){}
    enum boolean B;

    typedef enum{Mon=1,Sun=7}Days;
    Days d1,d2;

    typedef struct{
        enum{int_t,double_t}type;
        union{int i;double d;}u;
    }number;
    //print_num(number x){if(x.type==int_t)}

    //FILE *fp=fopen("in.txt","r");w(rite),a(ppend)
    //r+(start),w+(delele υπαρχων),a+(end)
    //rb+,a+b,wb δυαδικα
    
    //path \\,//:file,\:terminal
    
    //FILE *f=fopen(argv[1],"w")->αρχειο απο cli
    //fprintf(f,"%s",str);->εγγγραφη txt,while(strcmp(str,"Q"));
    //fflush(f);ffclose(f);while(!feof(f)){}

    //fwrite((void*)&i,sizeof(int),1,f);->εγγραφη σε δυαδικο


    //perror("err");

        /* εισαγωγή στον πίνακα b SIZE τιμών με τύπο δεδομένων double */
// FILE *fp = fopen("test.bin","rb");
// const size_t ret_code = fread(b, sizeof b[0], SIZE, fp);

// if (ret_code == SIZE) {
// printf("Array at %p read successfully, contents:\n", (void *)&a);
// for (int n = 0; n != SIZE; ++n)
// printf("%f ", b[n]);
// putchar('\n');
// }
// else { /* χειρισμός λαθών */
// if (feof(fp))
// printf("Error reading test.bin: unexpected end of file\n");
// else if (ferror(fp))
// perror("Error reading test.bin");
// }

    /* Συνάρτηση χειρισμού λαθών */
// void check(_Bool condition, const char* func, int line) {
// if (condition)
// return;
// perror(func);
// fprintf(stderr, "%s failed in file %s at line # %d\n",

// func, __FILE__, line - 1);

// exit(EXIT_FAILURE);
// }


//FILE *fp=fopen(fname,"rb");
//check(fp != NULL, "fopen()", __LINE__);


// FILE *fp = fopen("file.txt", "w");
// for (i=0; i < 1000000; ++i)
// fputc('A', fp);
// fclose(f);

/*fflush(fp);  γράφει σε μόνιμο χώρο την προσωρινή
αποθήκευση που συσχετίζεται με τη ροή fp

fflush(NULL); γράφονται όλες οι ροές εξόδου σε
μόνιμο χώρο αποθήκευσης */

//T (*fun)(T's *);

//int (*g)(int a);g->fun(int)=>int

//int f4(int(*f5)(int));f4(f5->fun(int)=>int)=>int


//T func(T (*f),T a);y=(*f)(x); == y=f(x)

// int callerFunc((int (*f)(int, int)), int a, int b);
// ισοδύναμα, αλλά πιο εύκολα κατανοητή, γίνεται ως εξής:
// typedef int (*funcPtr)(int, int);
// int callerFunc(funcPtr f, int a, int b);


// void (*fileOps[])(void) = { new,
// open,
// close,
// save,
// save_as,
// printd,
// }; (*fileOps[n])(); /* ή fileOps[n](); */




    return 0;//exit(EXIT_SUCCESS);
}
struct entry {
char used; /* 1 υπό χρήση, 0 ελεύθερο */
char name[64]; /* όνομα (συμβολοσειρά) */
char phone[64]; /* τηλέφωνο (συμβολοσειρά) */
};
/* Δήλωση βάσης δεδομένων επαφών ως πίνακα δομών */
struct entry entries[200];
int compare_contacts_v3(const void *p, const void *q) {

return strcmp(((struct entry *) p)->name,
((struct entry *) q)->name);
}
int compare_contacts_v2(const void *p, const void *q) {

if (strcmp(((struct entry *) p)->name,
((struct entry *) q)->name) > 0)
return 1;
else if (strcmp(((struct entry *) p)->name,
((struct entry *) q)->name) == 0)
return 0;
else
return -1;
}

int compare_contacts(const void *p, const void *q) {
const struct entry *p1 = p;
const struct entry *q1 = q;

if (strcmp(p1->name, q1->name) > 0)
return 1;
else if (strcmp(p1->name, q1->name) == 0)
return 0;
else
return -1;
}

struct gifts{
    int type;
    double price;
    union{
        struct{
            char title[100];
            char author[100];
        }book;
        struct{
            char design[100];
        }mug;
        struct{
            char design[100];
            int colors,sizes;
        }shirt;
    }item;
};
typedef struct{
    int type;//0:int,1:double
    union{
        int i;
        double d;
    }u;
}N;
typedef union{
    int i;
    double d;
}Number;
union ab{
    char a;
    short int b;
}v;
struct date{
    int day,month,year;
};
struct compact_date{
    unsigned int day:5;//2^5
};
int nameStruct(struct name n1,struct name n2){
    if(n1.a<n2.a){;}
    else if(n1.a>n2.a){;}
    else return 0;
}
struct person_name{
    char first[100];
    char last[100];
};
struct student{
    struct person_name name;
    char class[100];
    int id;
}student1;
 struct name{
        int a;
        double b;
    }var1,var2;
    struct name var3,var4;

    typedef struct{
        float x;
        char z[];
    }Part;
    Part part1,part2;
    
char *strtok(char *s1,const char *s2);//FRAGM
char *strcat(char *s1,const char *s2);//s1+s2-concat
void *memcpy(void *dest,const void *src,size_t count);
int strcmp(const char *s1,const char *s2){//compare,UPPER<LOWER,DIG<ALPHAN,WHITE CHARS<CHARS
    while(*s1 && *s2 && *s1==*s2){
        s1++;s2++;
    }
    return *s1-*s2;
}
int readline(char str[],int n){
    int i=0;
    int ch=getchar();
    while(ch!='\n'){
        if(i<n){
            str[i]=ch;
            i++;
        }
        ch=getchar();
    }
    str[i]='\0';
    printf("%s\n",str);
    return i;
}
void FreeSpace(int **arr,int rows){
    for(int i=0;i<rows;i++){
        free(arr[i]);
    }free(arr);
}
//int *f(){return &i}WRONG-NOT PTR->LOCAL
char *strAppend(const char *s1,const char *s2){
    int len1,len2,i;char *s3;
    for(int len1;s1[len1]!='\0';len1++);
    for(int len2;s2[len2]!='\0';len2++);
    char *s3=(char *)malloc((len1+len2+1)*sizeof(char));
    for(int i=0;i<len1;i++){s3[i]=s1[i];}
    for(int j=0;j<len2;j++){s3[j]=s2[j];}
    s3[i]='\0';return(s3);
}
void test(int num,char *str, ...);
int get1024B(int **p){
    *p=malloc(1024);
    return (*p==0)?-1:0;
}
void decompose(double x,long *int_part,double *frac_part){
    *int_part=(long)x;
    *frac_part=x-*int_part;
}
void f(){
    static int a=0;
    a++;
    printf("%d",a);
}

//T func(T n,T a[n]){}VLA
 int max(int x,int y){
        return x>y?x:y;
}
int incr(int *x){
    return ++(*x);
}
int findMax(const int a[],int n){//int *a
    int max=a[0];
    for(int i;i<n;i++){
        //return a[i]>max?max=a[i]:a[i++];
        if(a[i]>max)max=a[i];return max;
    }
}