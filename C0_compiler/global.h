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

int cc = 0; //当前行读入字符数
int ll = 0; //行长度
int line_num = 0;   //读入的总行数
int kk = 0; //当前单词的长度
int num = 0;    //最后读入的整数
char ch = ' ';    //当前读入的字符
char chcon = 0; //最后读入的字符常量
char strcon[MAX_STR];   //最后读入的字符串常量
char line[MAX_LINE] = { 0 };    //当前行
char iden[MAX_IDEN] = { 0 };    //最后读入的标识符
int type;       //最后读入的类型保留字（char为2，int为1）
int end_flag = 0;	//当前是否已经到达文件结尾，用在get_ch()中
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
#define MAX_SYTAB_ENTRY 200   //符号表表项阈值
#define MAX_SUB_BLOCK   100   //分程序阈值
#define MAX_QTAB_ENTRY	1000  //四元式表项阈值
#define MAX_LABEL_ENTRY 200	  //标签表
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


//符号表
typedef struct {
	char 	name[MAX_IDEN] = { 0 };	//名字
	int		kind = 0;	//种类（如常量，简单变量，数组，函数，参数，临时变量(用于四元式)）
	/*{ K_CONST 1
		K_VAR   2
		K_ARRAY 3
		K_FUNC  4
		K_PARA  5
		K_TMP	6}*/
	int 	type = 0;		//类型(int为1，char为2)若为函数时，0代表void(无返回值函数)
	/*	T_VOID  0
		T_INT   1
		T_CHAR  2
		T_STR	3*/
	int  	value = 0; 	//若为常量的话，value为其值(char存其ascii码值)
	int  	sz = 0; 	//若为数组，存储其大小，否则为0
	int 	para = 0;		//若为函数，para为参数个数，若为函数参数，则表示第n个参数，否则为0
}symentry;

typedef struct {
	symentry	symtab[MAX_SYTAB_ENTRY];	//符号表
	int 	btab[MAX_SUB_BLOCK] = {0};	//分程序索引表,batb[0]要初始化为零
}symboltab;

symboltab 	tab;//全局符号表

//四元式
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


Qt qtab[MAX_QTAB_ENTRY];//四元式表
int	qltab[MAX_QTAB_ENTRY];	//四元式对应的label表

char ltab[MAX_LABEL_ENTRY][10];//标签表

char stab[MAX_STR_ENTRY][MAX_IDEN];	//str表

int tkind;
int ttype;
int tvalue;
int tsz;
int tpara;
int tx = 0;     //符号表当前指针
int btotal = 0; //0为全局
int qtx = 0;	//四元式表当前指针
int qltx = 0;	//四元式对应的label表指针
int tmpx = 0;	//临时变量计数器
int lbx = 0;	//label表当前指针
int stx = 0;	//str表当前指针

int error_flag = 0;	//编译过程是否出错
int ret_flag = 0;	//函数是否包含合法的返回语句
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
	3:STR(仅全局会有)
	*/
	int offset;
} allocable;

allocable alloctab[MAX_ASSIGN];;

typedef struct mfunc {
	char name[MAX_IDEN];//若为参数则为""
	int type;
	/*
	0:VOID
	1:INT
	2:CHAR
	*/
	int para;//若为函数则para为参数个数，若为参数则para为参数排位0-n
}mfunc;

mfunc functab[MAX_MFUNC_ENTRY];

int fx = 0;		//函数表栈顶指针
int vx = 0;      //变量地址分配表指针
int sp = 0;        //栈指针
int fp = 0;		//栈帧
int gp = 0;		//全局常量和变量的个数
int qidx = 0;     //四元式遍历指针
int saved = 0;		//是否保存过现场
int fidx = 0;	//当前函数表指针
int main_flag = 0;	//当前是否为main函数,1是，0否
FILE *mipsOut;

char refName[MAX_ASSIGN][MAX_IDEN];
int cntList[MAX_ASSIGN];
int refx = 0;	//ref表指针
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
	int suc[MAX_BLK_SUC];	//该基本块的后继，存储基本块入口语句在Qtab中的index
	int qidx;
	int sidx=0;	//后继表的栈顶指针
	char use[MAX_VAR][MAX_IDEN];
	int uidx = 0;
	char def[MAX_VAR][MAX_IDEN];
	int didx = 0;
	char in[MAX_VAR][MAX_IDEN];
	int iidx = 0;
	char out[MAX_VAR][MAX_IDEN];
	int oidx = 0;
}blkEntry;

Qt optQtab[MAX_QTAB_ENTRY];	//优化后的四元式表
//int optQltab[MAX_QTAB_ENTRY];	//优化后的四元式对应的label表
blkEntry blocks[20][500];	//基本块表

int oqtx = 0;	//优化后的四元式表指针
int blx = 0;	//基本块表外层指针，即函数序号

typedef struct dagNode {
	char op[MAX_IDEN];	//叶子结点或者符号结点
	int lchild;
	int rchild;
}DagNode;

DagNode Dtab[MAX_QTAB_ENTRY];	//dag图表

int dtx = 0;	//dag图表指针

typedef struct node {
	int no;
	char name[MAX_IDEN];
}Node;

Node Ntab[MAX_QTAB_ENTRY];	//dag图对应的结点表

int ntx = 0;	//结点表指针

char varList[MAX_VAR][MAX_IDEN];
int varListFlag[MAX_VAR] = {0};
int varidx;	//活跃变量分析的变量表指针
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