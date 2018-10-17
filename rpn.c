/*************************************************************************
    > File Name: RPN计算器.c
    > Author: geeker
    > Mail: 932834897@qq.com
    > Created Time: 2017年02月06日 星期一 20时16分15秒
 ************************************************************************/
 
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>//提供函数isdigit判断char参数是否为数字
#define stacksize 20
#define maxbuffer 10//最大缓冲区
typedef double ElemType;
typedef struct
{
    ElemType *base;
    ElemType *top;
    int maxsize;
}sqStack;
 
void InitStack(sqStack *s)//初始化
{
    s->base=(ElemType *)malloc(stacksize * sizeof(ElemType));
    if(!s->base)
        exit(0);
    s->top=s->base;
    s->maxsize=stacksize;
}
 
 
void Push(sqStack *s,ElemType e)//压栈
{
    if(s->top-s->base>=s->maxsize)
        return;
    *(s->top)=e;
    s->top++;
}
 
 
void Pop(sqStack *s,ElemType *e)//出栈
{
    if(s->top==s->base)
        return;
    *e=*--(s->top);
}
 
 
int StackLen(sqStack s)//(看清s不是指针),计算当前容量
{
    return(s.top-s.base);//点是结构不是指针
}
 
int main()
{
    sqStack s;
    char c;
    double d,e;
    char str[maxbuffer];//缓冲区，作用比如scanf输入1234，计算机并不会觉得是1234，而会是字符1，字符2.
    int i=0;
 
    InitStack(&s);
 
    printf("请按逆波兰表示示输入计算数据，数据用空格隔开,以#作为结束标志\n");
    scanf("%c",&c);//怎么把c转化成double数据
   
    while(c!='#'){
        while(isdigit(c)||c=='.'){//这个函数判断char参数是否为数字（其实判断ASCII是否在48-57之间），头文件#include "ctype.h"
            str[i++]=c;
            str[i]='\0';//因为数组没有初始化，所以下一个数据给他初始化为0，此0为字符0，与数字0区别在于内存空间等
            if(i>=10){
                printf("出错，输入单个数据过大！\n");
                return -1;
            }//缓冲区有最大范围
        scanf("%c",&c);
        if(c==' '){//比如输入12 3，接受空格表示12输入结束了
           d=atof(str);//这个函数功能：将字符串转化为double型，需要头文件#include "stdlib.h",传入参数是字符串，返回double型数据
           Push(&s,d);
           i=0;//i初始化
           break;
        }
        }
        switch(c){//遇到数字就进栈，遇到加减乘除就出栈计算
            case'+': Pop(&s,&e); Pop(&s,&d); Push(&s,d+e);break;//将两个数字拿出来，再运算，再把结果放进去
            case'-': Pop(&s,&e); Pop(&s,&d); Push(&s,d-e);break;//d-e原因：比如1-2,1先进栈，所以2先出栈，所以后出栈-先出栈
            case'*': Pop(&s,&e); Pop(&s,&d); Push(&s,d*e);break;
            case'/': Pop(&s,&e); Pop(&s,&d);
                     if(e!=0){
                         Push(&s,d/e);break;//d/e与减同样的道理，最重要是除法要判断，除数不能为0
                   }
                     else{
                         printf("\n出错：除数为零！");
                         return -1;
                     }
                     break;
        }
        scanf("%c",&c);
    }
    Pop(&s,&d);//把最终结果放在任何里面都行不一定d，自己重新定义一个也可以
    printf("\n最终的计算结果为： %lf",d);
    return 0;
}
 
 
//5-(6+7)*8+9/4
//5-13*8+9/4
//5-104+2.25
//-99+2.25
//-96.75
 
//5 6 7 + 8 * - 9 4 / +
//结果 -96.750000
