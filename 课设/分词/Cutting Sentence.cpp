//���ķִ�ϵͳ
//Composed by Tentol
//2018.3.5 
//����Ϊ��������B0A1
#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<time.h>
#include<stdlib.h>

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define WORDMAX	40			
#define MAX_SWORD_LEN 256				
#define	DICTLEN	100
#define	MAX_WORD 10000

typedef struct LNode
{
	char s[WORDMAX+1];
	struct  LNode *next;//������ 
}LNode,*Linklist; 

struct Dictionary{
	LNode *next;
}Dict[DICTLEN][DICTLEN];//�ʵ� 


char cutsent[MAX_WORD];//���к��Ŵ� 
int EOF_Flag;
int SEG_Flag=0;

int max(int a,int b)
{
	if(a>b)
		return a;
	return b;
}

int searchWord(unsigned char s[],int len)
{
	unsigned char start,end;
	LNode* cur,* tmp;
	start=s[0];
	end=s[1];
	start=start-0xA1;
	end=end-0xA1;
	cur=Dict[start][end].next;
	tmp=cur;
	while(tmp!=NULL)
	{
		if (strcmp((char *)s,tmp->s)==0)//�ҵ� 
		{
			return TRUE;
		}
		tmp=tmp->next;
	}
	return FALSE;
}

int Delete(unsigned char s[],int len)
{
	unsigned char start,end;
	LNode* cur,* tmp;
	start=s[0];
	end=s[1];
	start=start-0xA1;
	end=end-0xA1;
	cur=Dict[start][end].next;
	tmp=cur;
	LNode *p=cur;
	while(p->next!=NULL)
	{
		if (strcmp((char *)s,p->next->s)==0)//�ҵ� 
		{
			tmp=p->next;
			p->next=p->next->next;
			tmp->next=NULL;
			free(tmp);
		}
		p=p->next;
	}
}

char *Simplify(char s[])
{
	int i,first,prior,next;
	first=0;
	prior=0;
	next=0;
	for (i=0;s[i]!='\0';i++)
	{
		if (s[i]==' '||s[i]=='\n'||s[i]=='\t')
		{
			if (first==0)
				prior++;
		}
		else
		{
			next=i;
			first=i;
		}
	}
	for (i=prior;i<=next;i++)
	{
		s[i-prior]=s[i];//ȥ���ո��� 
	}
	s[i-prior]='\0';
	return s;
}

int addDict(char *s)//�����ʵ����� 
{
	unsigned char first,second;
	LNode* cur,* newWord ,*tmp ;
	first=s[0] ;
	second=s[1];
	if (first<0xA1||second<0xA1)//�Ǻ��� 
		  return ERROR;
	newWord=(LNode*)malloc(sizeof(LNode));
	if (newWord==NULL)
		return ERROR;
	strcpy(newWord->s,s);
	newWord->next=NULL;
	first=first-0xA1;second=second-0xA1;
	cur=Dict[first][second].next;
	if(cur==NULL)	
	{
		Dict[first][second].next=newWord;
		return 0;
	}
	tmp=cur; 
	while(tmp->next!=NULL)
	{
		tmp=tmp->next;	
	}
	tmp->next=newWord;
	return 0;
}

int loadDict(char *Filename)//���ļ��ж�ȡ 
{
	int len=0;
	int i,j;
	FILE *fp;
	unsigned char firstChar,lastChar;
	char sLine[128];
    for (i=0;i<DICTLEN;i++) 
	{
    	for (j=0;j<DICTLEN;j++) 
		{
    		Dict[i][j].next= (LNode *)NULL;
    	}
    }
	fp=fopen(Filename,"r");
	if (fp==NULL) 
	{
		 printf("Error!\n",Filename);
		 return ERROR;	
	}
	while ( !feof(fp)) {
		fgets(sLine,128,fp);
		Simplify(sLine);//ɾ�������ַ� 
		len = strlen(sLine);
		if ( len < 1 || len > WORDMAX )
			printf("%s error!\n",sLine);
		addDict(sLine);		
	}	
    fclose(fp);
    return OK;
}

void addSegWord(unsigned char *s ,int len)//�ָ��㷨 
{
	char *c=(char *)s;
	strcat(cutsent,c);
	strcat(cutsent,"|");
	SEG_Flag=SEG_Flag+len+1;
}

int segWord(unsigned char *strText,int wordlen,int If_Chi)
{
	int i,j,k,l,temp;
	i=0;j=0;k=0;l=0;
	temp=0;
	unsigned char strChar[WORDMAX+1];//�����ַ��� 
	int If_Found=FALSE;
	i=wordlen; 
	if (If_Chi==FALSE)// not Chinese
	{		
		addSegWord(strText,wordlen);
		return 0;	
	}
	while (i>1)//iΪδƥ������  
	{	
		for(j=WORDMAX;j>=2;j-=2)//�������ƥ���㷨 
		{			
			if (i<j)//�䳤С�����ʳ� 
				continue;   
			l=0;
			for(k=temp,l=0;k<temp+j;k++,l++)
				strChar[l]=strText[k];
			strChar[l]='\0';
			if (j==2) //�������� 
			{
				addSegWord(strChar,2);
				i-=2;
				temp+=j;
				break;
			} 
			If_Found = searchWord(strChar,l);//�ڴʵ���ƥ�� 
			if (If_Found==TRUE)//�ҵ� 
			{
				i-=j;
				temp+=j;
				addSegWord(strChar,l);
				break; 
			} 
		} 
	}
	return 0;
}

int isAsciiSymbol (char c)//�Ƿ�ΪӢ���ַ� 
{
	char Ascii[] ={'!','\\','*','(',')','-','_','+','{','}','[',']',':',';','\'','\"','<','>','.','?','/','|','@','#','$','%','^','&'};
	for(int i=0;i<28;i ++ ) 
		if (c==Ascii[i])
			return TRUE;
	return FALSE;
}

int segSentence (char *strText ,int If_Space ) 
{	
	unsigned char c;
	int iTextLen=strlen(strText);
	int wordlen=0;//δ���� 
	int	i=0,iNexti=0,If_Sep=FALSE,If_Chi=FALSE,If_Found=FALSE,If_Sym=FALSE;
	//If_Sep�Ƿ�Ϊ�ָ�����If_Chi�ϸ���Ч�ֵ����ͣ�����TRUE��
	//If_Found�ҵ���If_Sym�Ƿ��� 
	unsigned char s[MAX_SWORD_LEN+1],strChar[3];
	memset(s,0,sizeof(s));
	strChar[0] = '\0';
	for (i=0;i<iTextLen;i++)
	{
		c=(unsigned char)strText[i];
		if (c<128)//ΪӢ���ַ� 
		{		
			if(c=='\n'||c==' '||c=='\t'||c=='\r')
			{
				if (If_Chi==TRUE)//�ϸ���Ч�ַ�Ϊ���� 
				{		
					if (If_Space==TRUE&&(c=='\t'||c==' ')) 
						If_Sep=TRUE;			
					else 
						continue ;//���пո�������			
				} 
				else  
					If_Sep=TRUE;
			}
			else
			{ 						
				If_Sym=isAsciiSymbol(c);//���Ƿ�Ӣ�ı��	
				If_Sep=If_Sym;
			} 
			if ((If_Sep==TRUE||If_Chi==TRUE)&&wordlen>=0)//��һ�����ʽ��� 
			{		
				if (wordlen>MAX_SWORD_LEN)
					wordlen=MAX_SWORD_LEN;
				s[wordlen] = '\0';//��һ����ԶϿ��Ļ�   
				segWord(s,wordlen,If_Chi);
				wordlen=0;
				if (If_Sym==TRUE)
				{
					s[0]=c;
					s[1]='\0';
					addSegWord(s,1);
				}//�Ա����в��� 
			}
			if (If_Sep==FALSE)
			{
				s[wordlen]=c;//ͬһ��������ĸ�ϲ����	
				wordlen++;
			}
			If_Chi=FALSE;    
		}  
		else//���洦������ 
		{						
			if(If_Chi==FALSE&&If_Sep==FALSE&&wordlen>0)//Ӣ�������������������Ӣ�Ľ��� 
			{
				if(wordlen>MAX_SWORD_LEN)
					wordlen=MAX_SWORD_LEN;
				s[wordlen]='\0';
				If_Found=FALSE;
				if(If_Found==FALSE)
				{
					segWord(s,wordlen,If_Chi);
				} 
				wordlen = 0 ;
			} 
			iNexti=i+1;
			if (iNexti<iTextLen)
			{
				if(((unsigned char)strText[iNexti])<128)//���ַ�<128�������� 
					continue;					
			} 
			else 			
				break;//����128���� 
			strChar[0] = strText[i];
			strChar[1] = strText[i+1];
			strChar[2] = '\0';//�����ΪӦ��>128�����	
			If_Chi=TRUE;//������ֻ��1������			
			i++;				
			if (strChar[0]==0xA1&&strChar[1]==0xA1)//���Ŀո�0xA1,0xA1 
			{			
				if (If_Space==TRUE)
					If_Sep=TRUE;
				else
					continue ;//�������Ŀո�				
			}
			else if (strChar[0]<0xB0)//���ı�� 
			{
				If_Sep=TRUE;
				If_Sym=TRUE;
			}
			else 
				If_Sep=FALSE;
			if(TRUE==If_Sep&&wordlen>=0)
			{
				if(wordlen>MAX_SWORD_LEN )
					wordlen = MAX_SWORD_LEN;
				s[wordlen]='\0';
				segWord(s,wordlen,If_Chi);
				wordlen=0 ;
				if(If_Sym==TRUE)
				{
					addSegWord(strChar,2);
				}//�ָ������� 
			} 
			if (FALSE==If_Sep)
			{
				s[wordlen++] = strChar[0];
				s[wordlen++] = strChar[1];
			}
		}
	}
	if(wordlen>0)//������δ������ 
	{					
		if ( wordlen > MAX_SWORD_LEN )
			 wordlen = MAX_SWORD_LEN;
		s[wordlen] = '\0';
		segWord(s,wordlen,If_Chi);
		wordlen=0;	
	}
	return 0;
}

int freeSeg()
{
	FILE *fp;
	fp=fopen("output.txt","w");
	if(fp==NULL)
	{
		printf("Error!\n");
		return ERROR;
	}
	fwrite(cutsent,SEG_Flag,1,fp);//������ļ��� 
	fclose(fp);
	return 0;
}

int Save()
{
	FILE *fp;
	int i,j;
	LNode *curLst,*curTmp,*tmp;
	fp=fopen("dict.txt","w");
	for (i=0;i<DICTLEN;i++)
	{
		for (j=0;j<DICTLEN;j++)
		{
			curLst=Dict[i][j].next;
			curTmp=curLst;
			while(curTmp!=NULL)
			{
				fputs(curTmp->s,fp);
				fputc('\n',fp);//�������������ļ��� 
				curTmp=curTmp->next;
			}
	    }
	}
	fclose(fp); 
}

int ReadIn(char *word)
{ 
	FILE *fp;
	int i;
    for (i=0;i<MAX_WORD;i++)
    		word[i]='\0';
	fp=fopen("input.txt","r");
	if(fp==NULL)
	{
		printf("Error!\n");
		return ERROR;
	}
	fseek(fp,0,SEEK_END);//SEEK_SET�ļ�ͷ,SEE_CUR��ǰ,SEEK_ENDβ 
	EOF_Flag=ftell(fp);//��λ�ļ�β 
	rewind(fp);//��λ���� 
	fread(word,EOF_Flag,1,fp);//�Ӿ��׶�����β 
	fclose(fp);
}

int main()
{
	char word[MAX_WORD]; 
	ReadIn(word);
	int op=1;
	while(op)
	{
		system("cls");
		printf("            ���ķִʲ����˵�\n");
		printf("-----------------------------------------\n");
		printf("|	1.����ʵ�	2.��ѯ����	|\n");
		printf("|	3.��Ӵ���	4.ɾ������	|\n");
		printf("|	5.��ʼ�ִ�	6.����ʵ�	|\n");
		printf("|	0.�˳�ϵͳ			|\n"); 
		printf("-----------------------------------------\n");
		printf("��ѡ�������0~6����\n");
		scanf("%d",&op);
		switch(op)
		{
			case 1:
				if(loadDict("Dictionary.txt")==OK)
				{
					printf("����ʵ�ɹ���\n");
					getchar();getchar();
					break;
				}
				else
				{
					printf("����ʧ�ܣ�\n");
					getchar();getchar();
					break;
				}
			case 2:
				unsigned char u[WORDMAX];
				printf("������Ҫ��ѯ�Ĵʣ�\n"); 
				scanf("%s",u);
				if (searchWord(u,WORDMAX)==TRUE) printf("����\n");
				else printf("������\n");
				getchar();getchar();
				break;
			case 3:
				char y[100];
				printf("������Ҫ��ӵĴʣ�\n"); 
				scanf("%s",y);
				addDict(y);
				printf("��ӳɹ���\n");
				getchar();getchar();
				break;
			case 4:
				unsigned char z[100];
				printf("������Ҫɾ���Ĵʣ�\n");
				scanf("%s",z);
				if (searchWord(z,WORDMAX)==FALSE)
				{
					printf("������󣡸ôʲ����ڣ�\n");
					getchar();getchar();
					break;
				}
				else
				{
					Delete(z,32);
					printf("ɾ���ɹ���\n");
					getchar();getchar();
					break;
				}
			case 5:
				segSentence(word,FALSE);
				freeSeg();
				printf("�ִʳɹ���\n"); 
				getchar();getchar();
				break;
			case 6:
				Save();
				printf("����ɹ���\n");
				getchar();getchar();
				break;
			case 0:
				break;
		}
	}
	printf("��ӭ�´�ʹ�ñ�ϵͳ��\n"); 
	return 0 ;
}
