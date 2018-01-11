#pragma once
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1
#define GRAMMER 0
/******************lex.h************************/
//----------------predefine field-----------------//
#include <string.h>
#include <ctype.h>

#define MAX_FILE_NAME 100
#define MAX_LINE 81
#define MAX_IDEN 80
#define MAX_STR 80
#define RESERVE_WORD_NUM 14
//----------------end predefine field-----------------//

//---------------var def--------------------//
enum symbol {
	IDEN, INTCON, CHARCON, STRCON, CASETK, CHARTK, CONSTTK, DEFAULTTK,
	ELSETK, IFTK, INTTK, MAINTK, PRINTFTK, RETURNTK, SCANFTK, SWITCHTK,
	VOIDTK, WHILETK, PLUS, MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL,
	NEQ, ASSIGN, COLON, SEMICN, COMMA, QMARK, DQMARK, LPARENT, RPARENT,
	LBRACK, RBRACK, LBRACE, RBRACE
};

enum symbol rwsym[] = {
	CASETK, CHARTK, CONSTTK, DEFAULTTK, ELSETK, IFTK, INTTK,
	MAINTK, PRINTFTK, RETURNTK, SCANFTK, SWITCHTK, VOIDTK, WHILETK
};

char * rword[] = {
	"case", "char", "const", "default", "else", "if", "int",
	"main", "printf", "return", "scanf", "switch", "void", "while"
};

int cc = 0; //��ǰ�ж����ַ���
int ll = 0; //�г���
int line_num = 0;   //�����������
int kk = 0; //��ǰ���ʵĳ���
int num = 0;    //�����������
char ch = ' ';    //��ǰ������ַ�
char chcon = 0; //��������ַ�����
char strcon[MAX_STR];   //��������ַ�������
char line[MAX_LINE] = { 0 };    //��ǰ��
char iden[MAX_IDEN] = { 0 };    //������ı�ʶ��
int type;       //����������ͱ����֣�charΪ2��intΪ1��
int end_flag = 0;	//��ǰ�Ƿ��Ѿ������ļ���β������get_ch()��
enum symbol sym;

FILE *in, *out, *err, *tabout;
//---------------end var def--------------------//

//---------------func def--------------------//
void getch();
void getsym();
void error(int);
//---------------end func def--------------------//
/******************lex.h************************/

/******************GSAnlz.h************************/
//----------------predefine field-----------------//
#define MAX_SYTAB_ENTRY 200   //���ű������ֵ
#define MAX_SUB_BLOCK   100   //�ֳ�����ֵ
#define MAX_QTAB_ENTRY	1000  //��Ԫʽ������ֵ
#define MAX_LABEL_ENTRY 200	  //��ǩ��
#define MAX_TMP_STACK 20
#define MAX_STR_ENTRY 200

#define K_CONST 1
#define K_VAR   2
#define K_ARRAY 3
#define K_FUNC  4
#define K_PARA  5
#define K_TMP	6

#define T_VOID  0
#define T_INT   1
#define T_CHAR  2
#define T_STR	3

#define EX_VOID		0
#define EX_ARRAY	1
#define EX_INTI		2
#define EX_CHARI	3
//----------------end predefine field-----------------//

//----------------var def-----------------//
enum symbol	prgmabegsys[] = {
	CHARTK, CONSTTK, INTTK, VOIDTK
};

enum symbol defbegsys[] = {
	CHARTK, INTTK, VOIDTK
};

enum symbol constbegsys[] = {
	IDEN, INTCON, CHARCON, PLUS, MINU
};

enum symbol sentbegsys[] = {
	IDEN, IFTK, PRINTFTK, RETURNTK, SCANFTK, SWITCHTK,
	WHILETK, SEMICN, LBRACE
};

enum symbol sentlistbegsys[] = {
	CHARTK, CONSTTK, INTTK, VOIDTK, IDEN, IFTK, PRINTFTK, RETURNTK, SCANFTK, SWITCHTK,
	WHILETK, SEMICN, LBRACE
};

enum symbol factorbegsys[] = {
	IDEN, INTCON, CHARCON, LPARENT
};


//���ű�
typedef struct {
	char 	name[MAX_IDEN] = { 0 };	//����
	int		kind = 0;	//���ࣨ�糣�����򵥱��������飬��������������ʱ����(������Ԫʽ)��
	/*{ K_CONST 1
		K_VAR   2
		K_ARRAY 3
		K_FUNC  4
		K_PARA  5
		K_TMP	6}*/
	int 	type = 0;		//����(intΪ1��charΪ2)��Ϊ����ʱ��0����void(�޷���ֵ����)
	/*	T_VOID  0
		T_INT   1
		T_CHAR  2
		T_STR	3*/
	int  	value = 0; 	//��Ϊ�����Ļ���valueΪ��ֵ(char����ascii��ֵ)
	int  	sz = 0; 	//��Ϊ���飬�洢���С������Ϊ0
	int 	para = 0;		//��Ϊ������paraΪ������������Ϊ�������������ʾ��n������������Ϊ0
}symentry;

typedef struct {
	symentry	symtab[MAX_SYTAB_ENTRY];	//���ű�
	int 	btab[MAX_SUB_BLOCK] = {0};	//�ֳ���������,batb[0]Ҫ��ʼ��Ϊ��
}symboltab;

symboltab 	tab;//ȫ�ַ��ű�

//��Ԫʽ
enum qt_op {
	ADD, SUB, MUL, DIVV, NEG, MOV,
	JZ, JL, JLE, JG, JGE, JE,
	JNE, RET, WRITE, READ, PARAV,
	CALL, JMP, MAINF, EMAINF,
	FUNC, EFUNC, CONST, VAR
};

char *qt_op_name[] = {
	"ADD", "SUB", "MUL", "DIVV", "NEG", "MOV",
	"JZ", "JL", "JLE", "JG", "JGE", "JE",
	"JNE", "RET", "WRITE", "READ", "PARAV",
	"CALL", "JMP", "MAINF", "EMAINF",
	"FUNC", "EFUNC", "CONST", "VAR"
};

typedef struct {
	enum qt_op op;
	char var1[MAX_IDEN];
	char var2[MAX_IDEN];
	char var3[MAX_IDEN];
	int label = -1;
}Qt;


Qt qtab[MAX_QTAB_ENTRY];//��Ԫʽ��
int	qltab[MAX_QTAB_ENTRY];	//��Ԫʽ��Ӧ��label��

char ltab[MAX_LABEL_ENTRY][10];//��ǩ��

char stab[MAX_STR_ENTRY][MAX_IDEN];	//str��

int tkind;
int ttype;
int tvalue;
int tsz;
int tpara;
int tx = 0;     //���ű�ǰָ��
int btotal = 0; //0Ϊȫ��
int qtx = 0;	//��Ԫʽ��ǰָ��
int qltx = 0;	//��Ԫʽ��Ӧ��label��ָ��
int tmpx = 0;	//��ʱ����������
int lbx = 0;	//label��ǰָ��
int stx = 0;	//str��ǰָ��

int error_flag = 0;	//��������Ƿ����
int ret_flag = 0;	//�����Ƿ�����Ϸ��ķ������
//----------------end var def-----------------//

//----------------func def--------------------//
void emit(enum qt_op, char*, char *, char *);
int qtvargen(char *, int , int , int , int );
int qtmpgen(char *,int type);
int labgen();
void setlab(int, int);
void skip(enum symbol*, int, int);
int isin(enum symbol, enum symbol*, int);
int lookup(char *);
int insertab(char*);
void pushfunc(int);
void popfunc();

void program();
int defhead();
void constdef();
void vardef();
void procdef();
void funcdef();
void fparalist();
void compoundsent();
void sentlist();
void sentence();
void ifsent();
int condsn();
void whilesent();
void casesent();
void casetabel(int, int);
void writesent();
void readsent();
void returnsent();
void assignsent(int);
void rparalist(int);
int npfuncall(int);
int pfuncall(int);
int expr(int *);
int term(int *,int *, int *);
int factor(int *, int *, int *);
int lkupFunc(char *);
void labelPro();
//----------------func def-----------------//
/******************GSAnlz.h************************/

/******************tomips.h************************/
//----------------predefine field-----------------//
#define MAX_ASSIGN 200
#define MAX_TMP_VAR 100
#define	MAX_MFUNC_ENTRY	1000
#define LOOP_WEIGHT	5
#define MAX_REG	15

#define	F_VOID	0
#define	F_INT	1
#define	F_CHAR	2

#define AT_INT	1
#define AT_CHAR	2	
#define AT_STR	3
//----------------end predefine field-----------------//

//----------------var def-----------------//
typedef struct addralloc
{
	char name[MAX_IDEN];
	int type;
	/*
	1:INT
	2:CHAR
	3:STR(��ȫ�ֻ���)
	*/
	int offset;
} allocable;

allocable alloctab[MAX_ASSIGN];;

typedef struct mfunc {
	char name[MAX_IDEN];//��Ϊ������Ϊ""
	int type;
	/*
	0:VOID
	1:INT
	2:CHAR
	*/
	int para;//��Ϊ������paraΪ������������Ϊ������paraΪ������λ0-n
}mfunc;

mfunc functab[MAX_MFUNC_ENTRY];

int fx = 0;		//������ջ��ָ��
int vx = 0;      //������ַ�����ָ��
int sp = 0;        //ջָ��
int fp = 0;		//ջ֡
int gp = 0;		//ȫ�ֳ����ͱ����ĸ���
int qidx = 0;     //��Ԫʽ����ָ��
int saved = 0;		//�Ƿ񱣴���ֳ�
int fidx = 0;	//��ǰ������ָ��
int main_flag = 0;	//��ǰ�Ƿ�Ϊmain����,1�ǣ�0��
FILE *mipsOut;

char refName[MAX_ASSIGN][MAX_IDEN];
int cntList[MAX_ASSIGN];
int refx = 0;	//ref��ָ��
//----------------end var def-----------------//

//----------------func def--------------------//
void toMips();
void mipsData();
void mipsCode();
void mipsArray(char *, int, int, int);
int lkupArrayAddr(char *, int *, int*);
int lkupAddr(char *);
void mipsParav();
void mipsCall();
void mipsJmp();
void mipsRet();
void mipsComp();
void mipsNeg();
void mipsMov();
void mipsBrch();
void mipsWrite();
void mipsRead();
void refCnt(int);
bool inLoop(int);
void findName(int, char*);
void clrSreg();
int isArray(char*, char*);
int isin(char *, char[][MAX_IDEN], int, char*);
//----------------end func def--------------------//
/******************tomips.h************************/

/******************opt.h************************/
//----------------predefine field-----------------//
#define MAX_BLK_SUC 2
#define MAX_BLK_ENTRY 200
#define MAX_VAR	100
//----------------end predefine field-----------------//

//----------------var def-----------------//
enum qt_op jmpSet[] = {
	JZ, JL, JLE, JG, JGE, JE, JNE, RET, JMP
};

typedef struct blockEntry {
	int suc[MAX_BLK_SUC];	//�û�����ĺ�̣��洢��������������Qtab�е�index
	int qidx;
	int sidx=0;	//��̱��ջ��ָ��
	char use[MAX_VAR][MAX_IDEN];
	int uidx = 0;
	char def[MAX_VAR][MAX_IDEN];
	int didx = 0;
	char in[MAX_VAR][MAX_IDEN];
	int iidx = 0;
	char out[MAX_VAR][MAX_IDEN];
	int oidx = 0;
}blkEntry;

Qt optQtab[MAX_QTAB_ENTRY];	//�Ż������Ԫʽ��
//int optQltab[MAX_QTAB_ENTRY];	//�Ż������Ԫʽ��Ӧ��label��
blkEntry blocks[20][500];	//�������

int oqtx = 0;	//�Ż������Ԫʽ��ָ��
int blx = 0;	//����������ָ�룬���������

typedef struct dagNode {
	char op[MAX_IDEN];	//Ҷ�ӽ����߷��Ž��
	int lchild;
	int rchild;
}DagNode;

DagNode Dtab[MAX_QTAB_ENTRY];	//dagͼ��

int dtx = 0;	//dagͼ��ָ��

typedef struct node {
	int no;
	char name[MAX_IDEN];
}Node;

Node Ntab[MAX_QTAB_ENTRY];	//dagͼ��Ӧ�Ľ���

int ntx = 0;	//����ָ��

char varList[MAX_VAR][MAX_IDEN];
int varListFlag[MAX_VAR] = {0};
int varidx;	//��Ծ���������ı�����ָ��
//----------------end var def-----------------//

//----------------func def-----------------//
void optimize();
void holeOpt();
void mergeQt();
void procLbl(int);
void delQt(int);
void cstSp();
void dagOpt();
void blkDiv(int);
int blkProc(int, int);
int getBlkNo(int, int, int);
int isin(enum qt_op, enum qt_op*, int);
void bldDag(int, int);
void genDagNode(char *, enum qt_op, char *);
int insertDag(char *, int , int );
int lkupDag(char*, int, int);
void insertNode(char *, int );
int lkupNode(char *);
int isArrayItem(char *, char *, char *);
void dagOut();
void writeOptQtab(int);
void insertOptab(enum qt_op, char*, char *, char *);
void copyOptab();
void regDis();
void LVAnalysis(int );
void getUD(int );
int getFlag(char *, int *);
int getIO(int );
void getPoor(char[][MAX_IDEN], int, char[][MAX_IDEN], int, char[][MAX_IDEN], int *);
int getUnion(char[][MAX_IDEN], int, char[][MAX_IDEN], int, char[][MAX_IDEN], int *);
int isin(char *, char [][MAX_IDEN], int );
//----------------end func def-----------------//
/******************opt.h************************/