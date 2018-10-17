/*************************************************************************
    > File Name: RPN������.c
    > Author: geeker
    > Mail: 932834897@qq.com
    > Created Time: 2017��02��06�� ����һ 20ʱ16��15��
 ************************************************************************/
 
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>//�ṩ����isdigit�ж�char�����Ƿ�Ϊ����
#define stacksize 20
#define maxbuffer 10//��󻺳���
typedef double ElemType;
typedef struct
{
    ElemType *base;
    ElemType *top;
    int maxsize;
}sqStack;
 
void InitStack(sqStack *s)//��ʼ��
{
    s->base=(ElemType *)malloc(stacksize * sizeof(ElemType));
    if(!s->base)
        exit(0);
    s->top=s->base;
    s->maxsize=stacksize;
}
 
 
void Push(sqStack *s,ElemType e)//ѹջ
{
    if(s->top-s->base>=s->maxsize)
        return;
    *(s->top)=e;
    s->top++;
}
 
 
void Pop(sqStack *s,ElemType *e)//��ջ
{
    if(s->top==s->base)
        return;
    *e=*--(s->top);
}
 
 
int StackLen(sqStack s)//(����s����ָ��),���㵱ǰ����
{
    return(s.top-s.base);//���ǽṹ����ָ��
}
 
int main()
{
    sqStack s;
    char c;
    double d,e;
    char str[maxbuffer];//�����������ñ���scanf����1234������������������1234���������ַ�1���ַ�2.
    int i=0;
 
    InitStack(&s);
 
    printf("�밴�沨����ʾʾ����������ݣ������ÿո����,��#��Ϊ������־\n");
    scanf("%c",&c);//��ô��cת����double����
   
    while(c!='#'){
        while(isdigit(c)||c=='.'){//��������ж�char�����Ƿ�Ϊ���֣���ʵ�ж�ASCII�Ƿ���48-57֮�䣩��ͷ�ļ�#include "ctype.h"
            str[i++]=c;
            str[i]='\0';//��Ϊ����û�г�ʼ����������һ�����ݸ�����ʼ��Ϊ0����0Ϊ�ַ�0��������0���������ڴ�ռ��
            if(i>=10){
                printf("�������뵥�����ݹ���\n");
                return -1;
            }//�����������Χ
        scanf("%c",&c);
        if(c==' '){//��������12 3�����ܿո��ʾ12���������
           d=atof(str);//����������ܣ����ַ���ת��Ϊdouble�ͣ���Ҫͷ�ļ�#include "stdlib.h",����������ַ���������double������
           Push(&s,d);
           i=0;//i��ʼ��
           break;
        }
        }
        switch(c){//�������־ͽ�ջ�������Ӽ��˳��ͳ�ջ����
            case'+': Pop(&s,&e); Pop(&s,&d); Push(&s,d+e);break;//�����������ó����������㣬�ٰѽ���Ž�ȥ
            case'-': Pop(&s,&e); Pop(&s,&d); Push(&s,d-e);break;//d-eԭ�򣺱���1-2,1�Ƚ�ջ������2�ȳ�ջ�����Ժ��ջ-�ȳ�ջ
            case'*': Pop(&s,&e); Pop(&s,&d); Push(&s,d*e);break;
            case'/': Pop(&s,&e); Pop(&s,&d);
                     if(e!=0){
                         Push(&s,d/e);break;//d/e���ͬ���ĵ�������Ҫ�ǳ���Ҫ�жϣ���������Ϊ0
                   }
                     else{
                         printf("\n��������Ϊ�㣡");
                         return -1;
                     }
                     break;
        }
        scanf("%c",&c);
    }
    Pop(&s,&d);//�����ս�������κ����涼�в�һ��d���Լ����¶���һ��Ҳ����
    printf("\n���յļ�����Ϊ�� %lf",d);
    return 0;
}
 
 
//5-(6+7)*8+9/4
//5-13*8+9/4
//5-104+2.25
//-99+2.25
//-96.75
 
//5 6 7 + 8 * - 9 4 / +
//��� -96.750000
