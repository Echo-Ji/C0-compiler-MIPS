#pragma once
#include "global.h"

void emit(enum qt_op op, char* lopr, char* ropr, char* res) {
	qtab[qtx].op = op;
	strcpy(qtab[qtx].var1, lopr);
	strcpy(qtab[qtx].var2, ropr);
	strcpy(qtab[qtx].var3, res);
	qtx++;
}

void skip(enum symbol *symset, int errcode, int length) {
	error(errcode);
	while (isin(sym, symset, length)<0) {
		getsym();
	}
}

int isin(enum symbol sy, enum symbol *sys, int length) {
	int i = 0;
	int j = length;
	int k;
	while (i <= j) {
		k = (i + j) / 2;
		if (sy>sys[k]) {
			i = k + 1;
		}
		else if (sy < sys[k]) {
			j = k - 1;
		}
		else {
			return k;
		}
	}
	return -1;
}

int lookup(char *name) {//由于符号表中存储了函数参数，故不能无脑从tx查到0
	int iter, iend;
	iend = tab.btab[btotal];
	for (iter = tx; iter>iend; iter--) {//局部变量,反查
		if (strcmpi(name, tab.symtab[iter].name) == 0) {
			return iter;
		}
	}

	for (iter = btotal; iter >= 1; iter--) {//查询函数名
		if (strcmpi(name, tab.symtab[tab.btab[iter]].name) == 0) {
			return tab.btab[iter];
		}
	}

	iend = tab.btab[1] - 1;
	for (iter = iend; iter >= 0; iter--) {//全局变量
		if (strcmpi(name, tab.symtab[iter].name) == 0) {
			return iter;
		}
	}
	return -1;
}

//传递name是因为有时标识符不存储在iden中
int insertab(char *name) {//自带查表功能,常量不能重复赋值
	if (tx >= MAX_SYTAB_ENTRY) {//符号表溢出，报错并退出编译
		error(8);
	}

	int iter, iend;
	if (tkind != K_ARRAY) {
		tsz = 0;
	}
	if (tkind == K_FUNC) {//函数是否重名
						  //先判断分程序表
		for (iter = 1; iter <= btotal; iter++) {
			if (!strcmpi(name, tab.symtab[tab.btab[iter]].name)) {
				error(7);
				return 0;
			}
		}
		iend = tab.btab[1];
		for (iter = 0; iter < iend; iter++) {
			if (!strcmpi(name, tab.symtab[iter].name)) {
				error(7);
				return 0;
			}
		}

		tab.btab[++btotal] = tx;
	}
	else {//简单变量/数组/常量是否重名
		iter = tab.btab[btotal];//局部变量不能与函数名重名
		for (; iter<tx; iter++) {
			if (!strcmpi(name, tab.symtab[iter].name)) {
				error(7);
				return 0;
			}
		}
		//函数内的局部变量只需要查找本层无重复就好
		if (btotal == 0 && tkind != K_PARA) {
			iend = tab.btab[1];
			for (iter = 0; iter < iend; iter++) {
				if (!strcmpi(name, tab.symtab[iter].name)) {
					error(7);
					return 0;
				}
			}
		}
	}

	strcpy(tab.symtab[tx].name, name);
	tab.symtab[tx].kind = tkind;
	tab.symtab[tx].type = ttype;
	tab.symtab[tx].value = tvalue;
	tab.symtab[tx].sz = tsz;
	tab.symtab[tx].para = tpara;
	tx++;
	return 1;
}

int insertstr() {
	if (stx > MAX_STR_ENTRY) {
		error(28);
	}
	strcpy(stab[stx], strcon);
	stx++;
	return stx - 1;
}

void pushfunc(int paranum) {//函数参数个数反填
	tab.symtab[tab.btab[btotal]].para = paranum;
}

void popfunc() {//移动指针到参数最后一个位置
	int iend = tab.btab[btotal];
	tx = iend + tab.symtab[iend].para + 1;
}

//生成四元式中规定格式的操作数
//返回值为sytype; name为传出操作数名
int qtvargen(char *name, int type, int index, int itype, int loc) {
	int sytype;
	char tn1[MAX_IDEN], tn2[MAX_IDEN];
	int lloc = tab.btab[btotal];//level location
	int lpx = lloc + 1;
	int lvx = lloc + tab.symtab[lloc].para + 1;
	if (type > EX_ARRAY) {//若为立即数或字符的ascii码
		sprintf(name, "%d", loc);
		sytype = type - 1;//当ex_type > 1时，sytype = ex_type - 1
	}
	else if (type == EX_ARRAY) {//数组
		if (loc < lvx) {//全局数组直接取名字
			strcpy(name, tab.symtab[loc].name);
		}
		else {//局部变量按顺序自增
			sprintf(name, "%%%d", loc - lvx);
		}

		if (itype) {//若index type不为标识符,itype不可能为数组（1）
			sprintf(tn1, "[%d]", index);
		}
		else {
			qtvargen(tn2, 0, 0, 0, index);
			sprintf(tn1, "[%s]", tn2);
		}
		strcat(name, tn1);
		sytype = tab.symtab[loc].type;
	}
	else {
		if (tab.symtab[loc].kind == K_TMP) {//临时变量
			strcpy(name, tab.symtab[loc].name);
		}
		else if (loc >= lvx) {//局部变量
			sprintf(name, "%%%d", loc - lvx);
		}
		else if (loc >= lpx  && loc < lvx) {//若为函数参数
			sprintf(name, "@%d", loc - lpx);
		}
		else {//全局标识符 loc < lpx
			strcpy(name, tab.symtab[loc].name);
		}
		sytype = tab.symtab[loc].type;
	}
	return sytype;
}

//四元式临时变量生成器,返回该变量在符号表中的位置
//如计算a+b*c
//(MUL,b,c,$0)
//(MUL,a,$0,$1)
int qtmpgen(char *name, int type) {
	sprintf(name, "$%d", tmpx);
	tkind = K_TMP;
	ttype = type;
	tvalue = 0;
	tsz = 0;
	tpara = 0;
	insertab(name);
	tmpx++;
	return tx - 1;
}

//标签生成label_0,label_1
int labgen() {
	sprintf(ltab[lbx], "label_%d", lbx);
	lbx++;
	return lbx - 1;
}


void setlab(int qidx, int label) {
	char llabel[MAX_IDEN], nlabel[MAX_IDEN];
	int i;
	if (qltab[qidx] != -1) {//若该处有标签
		//获取原label
		strcpy(llabel, ltab[qltab[qidx]]);
		//获取新label
		strcpy(nlabel, ltab[label]);
		//lbx--;//删除新标签
		//标签重复了,需要修改var3为new label的四元式
		for (i = 0; i < qtx; i++) {
			if (!strcmpi(qtab[i].var3, nlabel)) {
				strcpy(qtab[i].var3, llabel);
			}
		}
		return;
	}
	
	qltab[qidx] = label;
}

void program() {
	while (sym == CONSTTK) {//将常量说明合并到此处
		tkind = K_CONST;
		getsym();
		constdef();
		if (sym != SEMICN) {
			skip(prgmabegsys, 12, 4);
		}
		else {
			getsym();
		}
	}
	while (sym == INTTK || sym == CHARTK || sym == VOIDTK) {
		if (sym == VOIDTK) {
			ttype = T_VOID;
			getsym();
			if (sym == MAINTK) {
				//tpara = 0;	//初始化以便在fpara中记录形参个数从而反填函数tpara
				tkind = K_FUNC;
				tvalue = 0;	//除常量外都为零
				tsz = 0;	//除数组外都为零
				if (insertab(iden)) {
					emit(MAINF, "", "", "");
				}
				getsym();
				if (sym == LPARENT) { 
					getsym();
					if (sym != RPARENT) { skip(sentbegsys, 9, 9); }
					else { getsym(); }
				}
				else { skip(sentbegsys, 16,9);}

				if (sym != LBRACE) { skip(sentbegsys, 18, 9); }
				else { getsym(); }
				pushfunc(0);
				compoundsent();
				popfunc();
				emit(EMAINF, "", "", "");
				printf("<主函数>\n");
				fprintf(out, "<主函数>\n");
				if (sym != RBRACE) { error(11); }
				while (true) {
					if (end_flag) {
						if (isspace(ch)) {
							//printf("语法分析完成，输出请查看文件out.txt，错误信息请查看err.txt\n");
							break;
						}
						else {
							error(233);
							break;
						}
					}
					else {
						if (!isspace(ch)) {
							error(233);
							break;
						}
					}
					getch();
				}
				break;
			}
			else {
				procdef();
			}
		}
		else {
			if (sym == INTTK) {
				ttype = T_INT;
			}
			else if (sym == CHARTK) {
				ttype = T_CHAR;
			}
			getsym();
			if (defhead()) {//有返回值函数声明
				funcdef();
				if (ret_flag) {
					ret_flag = 0;
				}
				else {
					error(29);
				}
			}
			else {//变量声明
				vardef();
				if (sym != SEMICN) error(12);
				getsym();
			}
		}
		if (sym != INTTK && sym != CHARTK && sym != VOIDTK) {
			skip(defbegsys, 30, 3);
		}
	}
	if (!end_flag || error_flag) {
		printf("编译失败：错误信息请查看文件err.txt!\n");
	}
	else {
		printf("编译成功！\n");
	}
}

int defhead() {//没有入表操作
	int flag = 0;   //若为函数头部则为1，变量头部则为0
	if (sym == IDEN) {
		getsym();
		if (sym == COMMA || sym == SEMICN || sym == LBRACK) {
			flag = 0;
		}
		else if (sym == LBRACE || sym == LPARENT) {
			flag = 1;
		}
		else {
			//变量/函数声明出现非法字符
			printf("defhead()出现非法字符\n");
		}
	}
	return flag;
}

//emit-undebug
void constdef() {
	int sign = 1, loc;
	char tmp[MAX_IDEN];
	tpara = 0;
	if (sym == INTTK) {
		ttype = T_INT;
		getsym();
		if (sym == IDEN) {
			getsym();
			if (sym == ASSIGN) {
				getsym();
				if (sym == PLUS || sym == MINU) {
					sign = sym == PLUS ? 1 : -1;
					getsym();
					if (sym == INTCON) {
						tvalue = sign * num;
						sprintf(tmp, "%d", tvalue);
						if (insertab(iden)) {//若没有重定义，则生成四元式
							if (btotal) { emit(CONST, "INT", "", tmp); }
							else { emit(CONST, "INT", iden, tmp); }
						}
						getsym();
					}
				}
				else if (sym == INTCON) {
					tvalue = num;
					sprintf(tmp, "%d", tvalue);
					if (insertab(iden)) {
						if (btotal) { emit(CONST, "INT", "", tmp); }
						else { emit(CONST, "INT", iden, tmp); }
					}
					getsym();
				}
				else {
					error(14);
				}
			}
		}
	}
	else if (sym == CHARTK) {
		ttype = T_CHAR;
		getsym();
		if (sym == IDEN) {
			getsym();
			if (sym == ASSIGN) {
				getsym();
				if (sym == CHARCON) {
					tvalue = chcon;
					sprintf(tmp, "%d", tvalue);
					if (insertab(iden)) {
						if (btotal) { emit(CONST, "CHAR", "", tmp); }
						else { emit(CONST, "CHAR", iden, tmp); }
					}
					getsym();
				}
				else {
					error(14);
				}
			}
		}
	}
	while (sym == COMMA) {
		if (ttype == T_INT) {
			getsym();
			if (sym == IDEN) {
				getsym();
				if (sym == ASSIGN) {
					getsym();
					if (sym == PLUS || sym == MINU) {
						sign = sym == PLUS ? 1 : -1;
						getsym();
						if (sym == INTCON) {
							tvalue = sign * num;
							sprintf(tmp, "%d", tvalue);
							if (insertab(iden)) {
								if (btotal) { emit(CONST, "INT", "", tmp); }
								else { emit(CONST, "INT", iden, tmp); }
							}
							getsym();
						}
					}
					else if (sym == INTCON) {
						tvalue = num;
						sprintf(tmp, "%d", tvalue);
						if (insertab(iden)) {
							if (btotal) { emit(CONST, "INT", "", tmp); }
							else { emit(CONST, "INT", iden, tmp); }
						}
						getsym();
					}
					else {
						error(14);
					}
				}
			}
		}
		else if (ttype == T_CHAR) {//T_CHAR
			getsym();
			if (sym == IDEN) {
				getsym();
				if (sym == ASSIGN) {
					getsym();
					if (sym == CHARCON) {
						tvalue = chcon;
						sprintf(tmp, "%d", tvalue);
						if (insertab(iden)) {
							if (btotal) { emit(CONST, "CHAR", "", tmp); }
							else { emit(CONST, "CHAR", iden, tmp); }
						}
						getsym();
					}
					else {
						error(14);
					}
				}
			}
		}
	}
	printf("<常量定义>\n");
	fprintf(out, "<常量定义>\n");
}

//emit-undebug
void vardef() {
	char tmp[MAX_IDEN];
	tkind = K_VAR;
	tvalue = 0;
	tpara = 0;
	tsz = 0;//数组默大小为零
	if (sym == LBRACK) {
		tkind = K_ARRAY;
		//处理无符号整数
		getsym();
		if (sym == PLUS || sym == MINU) {
			error(13);
			while (sym != INTCON) { getsym(); }
		}
		
		if (sym == INTCON) {
			tsz = num;
			sprintf(tmp,"%d",tsz);
			getsym();
			if (sym != RBRACK) {
				error(10);
			}
			else if (sym != SEMICN && sym != COMMA) {
				getsym();
			}
			if (insertab(iden)) {//数组insertab
				if (ttype == T_INT) { 
					if (btotal) { emit(VAR, "INT", "", tmp); }
					else { emit(VAR, "INT", iden, tmp); }
				}
				else {
					if(btotal){ emit(VAR, "CHAR", "", tmp); }
					else { emit(VAR, "CHAR", iden, tmp); }
				}
			}
		} 
		else {//适当跳读
			while (sym != SEMICN && sym != COMMA) {
				getsym();
			}
		}
	}
	else {  //SEMICN或者COMMA
		if (insertab(iden)) { //简单变量insertab
			if (ttype == T_INT) {
				if (btotal) { emit(VAR, "INT", "", ""); }
				else { emit(VAR, "INT", iden, ""); }
			}
			else{
				if (btotal) { emit(VAR, "CHAR", "", ""); }
				else { emit(VAR, "CHAR", iden, ""); }
			}
		}  
	}

	while (sym == COMMA) {
		tkind = K_VAR;
		getsym();
		if (sym == IDEN) {
			getsym();
			if (sym == LBRACK) {
				tkind = K_ARRAY;
				//处理符号部分
				getsym();
				if (sym == PLUS || sym == MINU) {
					error(13);
					while (sym != INTCON) { getsym(); }
				}
				//处理无符号整数
				if (sym == INTCON) {
					tsz = num;
					sprintf(tmp, "%d",tsz);
					getsym();
					if (sym != RBRACK)   error(10);
					else if (sym != SEMICN) {
						getsym();
					}
				}
				else {
					while (sym != SEMICN && sym != COMMA) {
						getsym();
					}
				}
			}
			if (sym == SEMICN || sym == COMMA) {
				if (insertab(iden)) {
					if (ttype == T_INT) {
						//数组和简单变量的区分
						if (tkind == K_ARRAY){
							if (btotal) { emit(VAR, "INT", "", tmp); }
							else { emit(VAR, "INT", iden, tmp); }
						}
						else { 
							if (btotal) { emit(VAR, "INT", "", ""); }
							else { emit(VAR, "INT", iden, ""); }
						}
					}
					else { 
						if (tkind == K_ARRAY) { 
							if (btotal) { emit(VAR, "CHAR", "", tmp); }
							else { emit(VAR, "CHAR", iden, tmp); }
						}
						else {
							if (btotal) { emit(VAR, "CHAR", "", ""); }
							else { emit(VAR, "CHAR", iden, ""); }
						}
					}
				}
			}
		}
	}
	printf("<变量定义>\n");
	fprintf(out, "<变量定义>\n");
}
//emit-undebug
void procdef() {//从标识符开始
	tpara = 0;
	if (sym == IDEN) {//原本在此处函数入表，在生成四元式的时候将入表操作移动到funcdef中
		getsym();
		funcdef();
	}
	printf("<无返回值函数定义>\n");
	fprintf(out, "<无返回值函数定义>\n");
}
//emit-undebug
void funcdef () {//从标识符之后开始
	tpara = 0;	//初始化以便在fpara中记录形参个数从而反填函数tpara
	tkind = K_FUNC;
	tvalue = 0;	//除常量外都为零
	tsz = 0;	//除数组外都为零

	if (insertab(iden)) {//函数入表
		emit(FUNC, iden, "", "");
	}

	if (sym == LPARENT) {//有参数函数
		getsym();
		fparalist();
		if (sym != RPARENT) {
			while (sym == IDEN) { getsym(); }
			skip(sentbegsys, 9, 9); //应该跳读到函数体开始
		}
		else { getsym(); }
	}
	
	pushfunc(tpara);    //函数参数个数反填

	if (sym != LBRACE) {
		skip(sentbegsys, 18, 9);
	}
	else { getsym(); }//只能是LBRACE，无参数函数
	compoundsent();
	popfunc();//退表到函数参数末
	emit(EFUNC,tab.symtab[tab.btab[btotal]].name, "", "");
	if (sym != RBRACE){ error(11);}
	else { getsym(); }
	if (tab.symtab[tab.btab[btotal]].type != 0) {
		printf("<有返回值函数定义>\n");
		fprintf(out, "<有返回值函数定义>\n");
	}
}

void fparalist() {//形参需不需要生成四元式
	tkind = K_PARA;
	tvalue = 0;
	if (sym == INTTK || sym == CHARTK) {
		ttype = sym == INTTK ? T_INT : T_CHAR;
		getsym();
		if (sym == IDEN) {
			tpara++;
			insertab(iden); //参数入符号表
			getsym();
		}
	}
	while (sym == COMMA) {
		getsym();
		if (sym == INTTK || sym == CHARTK) {
			ttype = sym == INTTK ? T_INT : T_CHAR;
			getsym();
			if (sym == IDEN) {
				tpara++;
				insertab(iden); //参数入符号表
				getsym();
			}
		}
	}
}

void compoundsent() {
	ret_flag = 0;
	while (sym == CONSTTK) {//将常量说明合并到此处
		tkind = K_CONST;
		getsym();
		constdef();
		if (sym != SEMICN) error(12);
		getsym();
	}
	while (sym == INTTK || sym == CHARTK) {
		ttype = sym == INTTK ? T_INT : T_CHAR;
		getsym();
		if (sym == IDEN) {
			getsym();
			vardef();
			if (sym != SEMICN) error(12);
			getsym();
		}
	}
	sentlist();
	printf("<复合语句>\n");
	fprintf(out, "<复合语句>\n");
}

//＜语句列＞   ::= ｛＜语句＞｝
void sentlist() {
	while (isin(sym, sentbegsys, 9) >= 0) {
		sentence();
	}
	printf("<语句列>\n");
	fprintf(out, "<语句列>\n");
}

//＜条件语句＞::= if ‘(’＜条件＞‘)’＜语句＞else＜语句＞
//没有else就报错，不保证执行的正确性
void ifsent() {
	int cdx;
	int label1, label2;
	getsym();
	if (sym != LPARENT) {
		if (sym != PLUS && sym != MINU) {
			skip(factorbegsys, 16, 4);
		}
		else { error(16); }
	}
	else { getsym(); }
	
	cdx = condsn();
	label1 = labgen();
	strcpy(qtab[cdx].var3, ltab[label1]);
	if (sym != RPARENT) { skip(sentbegsys,9,9); }
	else { getsym(); }
	sentence();
	if (sym == ELSETK) { 
		label2 = labgen();
		emit(JMP, "", "", ltab[label2]);
		//qtab[qtx].label = label1;
		setlab(qtx, label1);
		getsym();
		sentence();
		//qtab[qtx].label = label2;
		setlab(qtx, label2);
		printf("<条件语句>\n");
		fprintf(out, "<条件语句>\n");
	}
	else { skip(sentbegsys, 19, 9); }
}


//＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
//返回待填入标签的四元式指针,四元式代表的表达式为真则跳转,所以运算符都需要取反
int condsn() {
	int loc, type;
	char var1[MAX_IDEN], var2[MAX_IDEN];
	enum qt_op op;
	loc = expr(&type);
	qtvargen(var1, type, 0, 0, loc);
	if (sym != LSS && sym != LEQ && sym != GRE &&
		sym != GEQ && sym != EQL && sym != NEQ){
		emit(JZ, var1, "", "");
		return qtx - 1;
	}
	
	if (sym == LSS) { op = JGE;}
	else if (sym == LEQ) {op = JG;}
	else if (sym == GRE) { op = JLE; }
	else if (sym == GEQ) { op = JL; }
	else if (sym == EQL) { op = JNE; }
	else if (sym == NEQ) { op = JE; }

	getsym();
	loc = expr(&type);
	qtvargen(var2, type, 0, 0, loc);
	emit(op, var1, var2, "");
	return qtx - 1;
}

//＜循环语句＞   ::=  while ‘(’＜条件＞‘)’＜语句＞
void whilesent()
{
	int cdx, lpx;
	int label1, label2;
	getsym();
	if (sym != LPARENT) {
		if (sym != PLUS && sym != MINU) {//若为表达式的开始符则不需要读字符
			skip(factorbegsys, 16, 4);
		}
		else { error(16); }
	}
	else { getsym(); }

	label1 = labgen();
	//qtab[qtx].label = label1;
	lpx = qtx;
	cdx = condsn();
	label2 = labgen();
	strcpy(qtab[cdx].var3, ltab[label2]);

	if (sym != RPARENT) { skip(sentbegsys, 9, 9); }
	else { getsym(); }
	sentence();
	emit(JMP, "", "", ltab[label1]);
	setlab(lpx, label1);//循环跳回
	//qtab[qtx].label = label2;
	setlab(qtx, label2);

	printf("<循环语句>\n");
	fprintf(out, "<循环语句>\n");
}

//＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞[＜缺省＞] ‘}’
void casesent() {
	int loc, type;
	getsym();
	if (sym != LPARENT) {
		if (sym != PLUS && sym != MINU) {//若为表达式的开始符则不需要读字符
			skip(factorbegsys, 16, 4);
		}
		else { error(16); }
	}
	else { getsym(); }
	loc = expr(&type);

	if (sym != RPARENT) { skip(sentbegsys, 9, 9); }
	else { getsym(); }
	
	if (sym != LBRACE) { skip(sentbegsys,18,9); }
	else { getsym(); }

	casetabel(loc, type);
	
	if (sym != RBRACE) { skip(sentbegsys,11,9); }
	else { getsym(); }
	printf("<情况语句>\n");
	fprintf(out, "<情况语句>\n");
}

//＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
//＜情况子语句＞  ::=  case＜常量＞：＜语句＞
void casetabel(int loc, int type) {
	int label0, label1;
	char var1[MAX_IDEN],var2[MAX_IDEN];
	int label_value, lack_v2;
	int sign;
	if (sym == CASETK) {
		label0 = labgen();
		qtvargen(var1, type, 0, 0, loc);
		label1 = labgen();
		emit(JNE, var1,"", ltab[label1]);
		lack_v2 = qtx - 1;

		getsym();//有符号数
		if (sym == PLUS || sym == MINU) {//整数
			sign = sym == PLUS ? 1 : -1;
			getsym();
			if (sym == INTCON) {
				label_value = sign * num;
				getsym();
				if (sym != COLON) { skip(sentbegsys, 20, 9); }
				else { getsym(); }
				sentence();
			}
		}
		else if (sym == INTCON) {
			label_value = num;
			getsym();
			if (sym != COLON) { skip(sentbegsys,20,9); }
			else { getsym(); }
			sentence();
		}
		else if (sym == CHARCON) {
			label_value = chcon;
			getsym();
			if (sym != COLON) { skip(sentbegsys, 20, 9); }
			else { getsym(); }
			sentence();
		}
		else { error(21);
			while (sym != SEMICN) { getsym(); }
			return;
		}
		sprintf(var2,"%d",label_value);
		strcpy(qtab[lack_v2].var2, var2);
		emit(JMP, "", "", ltab[label0]);
		
		/////////////循环处理剩下的case
		while(sym == CASETK){
			//qtab[qtx].label = label1;	
			setlab(qtx, label1);//setlab

			label1 = labgen();
			emit(JNE, var1, "", ltab[label1]);
			lack_v2 = qtx - 1;

			getsym();//有符号数
			if (sym == PLUS || sym == MINU) {//整数
				sign = sym == PLUS ? 1 : -1;
				getsym();
				if (sym == INTCON) {
					label_value = sign * num;
					getsym();
					if (sym != COLON) { skip(sentbegsys, 20, 9); }
					else { getsym(); }
					sentence();
				}
			}
			else if (sym == INTCON) {
				label_value = num;
				getsym();
				if (sym != COLON) { skip(sentbegsys, 20, 9); }
				else { getsym(); }
				sentence();
			}
			else if (sym == CHARCON) {
				label_value = chcon;
				getsym();
				if (sym != COLON) { skip(sentbegsys, 20, 9); }
				else { getsym(); }
				sentence();
			}
			else {
				error(21);
				while (sym != SEMICN) { getsym(); }
				return;
			}
			
			sprintf(var2, "%d", label_value);
			strcpy(qtab[lack_v2].var2, var2);
			emit(JMP, "", "", ltab[label0]);//跳转到结束处
		} 

		if (sym == DEFAULTTK) {
			//qtab[qtx].label = label1;
			setlab(qtx, label1);
			getsym();
			if (sym != COLON) { error(20); }
			getsym();
			sentence();
			//emit(JMP, "", "", ltab[label0]);//default跳转到下一条语句可省略
		}
		else{//若没有default则修改最后一个case
			strcpy(qtab[lack_v2].var3, ltab[label0]);//修改使其跳转到正确的位置
			//lbx--;//删除最后一个label,因为撤销了
			qtx--;//删除最后一个JMP,"","",label0
		}
		//qtab[qtx].label = label0;
		setlab(qtx, label0);//结束处
	}
}

//＜读语句＞    ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
void readsent() {
	int loc;
	char var[MAX_IDEN];
	getsym();
	if (sym != LPARENT) { 
		error(16); 
		while (sym != IDEN || sym != RPARENT || sym != SEMICN) { getsym(); }
	}
	else { getsym(); }

	if (sym == IDEN) {
		if ((loc = lookup(iden)) < 0) { skip(sentbegsys, 6, 9); }
		else {
			if (tab.symtab[loc].kind != K_PARA && tab.symtab[loc].kind != K_VAR) {
				skip(sentbegsys, 26, 9);
				return;
			}
			qtvargen(var, 0, 0, 0, loc);
			emit(READ, "", "", var);
		}
		getsym();
	}

	while (sym == COMMA) {
		getsym();
		if (sym == IDEN) {
			if ((loc = lookup(iden)) < 0) { skip(sentbegsys, 6, 9); }
			else {
				if (tab.symtab[loc].kind != K_PARA && tab.symtab[loc].kind != K_VAR) {
					skip(sentbegsys, 26, 9);
					return;
				}
				qtvargen(var, 0, 0, 0, loc);
				emit(READ, "", "", var);
			}
			getsym();
		}
	}

	if (sym != RPARENT) { skip(sentbegsys,9,9); }
	else if (sym != SEMICN) { getsym(); }
	printf("<读语句>\n");
	fprintf(out, "<读语句>\n");
}

//＜写语句＞    ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞ ‘)’| printf ‘(’＜表达式＞‘)’
void writesent() {
	int type, loc, sidx;
	char var[MAX_IDEN];
	char str[MAX_IDEN];
	getsym();
	if (sym != LPARENT) {
		error(16); 
		while (sym == SEMICN ||sym == RPARENT ||
			sym == STRCON || sym == IDEN ||
			sym == PLUS || sym == MINU) {
			getsym();
		}
	}
	else { getsym(); }
	if (sym == STRCON) {
		sidx = insertstr();
		sprintf(str, "_str%d", sidx);
		getsym();
		if (sym == COMMA) {
			getsym();
			loc = expr(&type);
			qtvargen(var, type, 0, 0, loc);
			emit(WRITE, str, var, "");
		}
		else {
			emit(WRITE, str, "", "");
		}
	}
	else {
		loc = expr(&type);
		qtvargen(var, type, 0, 0 ,loc);//生成汇编的时候根据名字可以找到类型
		emit(WRITE, "", var, "");
	}

	if (sym != RPARENT) { skip(sentbegsys,9,9); }
	else if (sym != SEMICN) { getsym(); }
	printf("<写语句>\n");
	fprintf(out, "<写语句>\n");
}

//＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
void returnsent() {
	int type, loc;
	char var[MAX_IDEN];
	getsym();
	if (sym == LPARENT) {
		ret_flag = 1;
		getsym();
		loc = expr(&type);
		qtvargen(var, type, 0, 0, loc);
		emit(RET, var, "", "");
		if (sym != RPARENT) { skip(sentbegsys,9,9); }
		else if (sym != SEMICN) { getsym(); }
	}
	else {
		emit(RET, "", "", "");
	}
	printf("<返回语句> \n");
	fprintf(out, "<返回语句> \n");
}

//＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
void assignsent(int loc) {
	int tloc, type;
	int type1, type2;
	char var1[MAX_IDEN], var2[MAX_IDEN];

	if (tab.symtab[loc].kind != K_VAR && 
		tab.symtab[loc].kind != K_ARRAY &&
		tab.symtab[loc].kind != K_PARA) {//不能为常量/函数赋值
		error(26);
		while (sym != SEMICN) {
			getsym();
		}
		return;
	}

	if (sym == LBRACK) {//数组元素赋值	
		if (tab.symtab[loc].kind != K_ARRAY) { error(26); }
		getsym();
		tloc = expr(&type);
		type1 = qtvargen(var1, 1, tloc, type, loc);
		if (sym != RBRACK) { skip(sentbegsys, 10, 9); }
		else { getsym(); }
	}
	else {//参数或简单变量赋值
		if (tab.symtab[loc].kind != K_VAR &&
			tab.symtab[loc].kind != K_PARA) {
			error(26);
		}
		type1 = qtvargen(var1, 0, 0, 0, loc);
	}
	
	if (sym == ASSIGN) {
		getsym();
		tloc = expr(&type);
		type2 = qtvargen(var2, type, 0, 0, tloc);
		if (type2 > type1) { error(23); }
		else { emit(MOV, var2, "", var1); }
		
		printf("<赋值语句>\n");
		fprintf(out, "<赋值语句>\n");
	}
	else {
		error(27);
		while (sym != SEMICN) getsym();
	}
}


//＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;
//                           |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜情况语句＞｜＜返回语句＞;
void sentence() {
	int loc;
	int flag;
	char tmp[MAX_IDEN];
	switch (sym) {
	case IDEN: {
		if ((loc = lookup(iden)) < 0) { //若出错则将语句读完
			while (sym != SEMICN) {
				if (sym == IDEN && lookup(iden) < 0) {
					error(6);
				}
				getsym();
			}
			getsym();
		}
		else {
			getsym();
			if (tab.symtab[loc].kind == K_FUNC) {
				if (tab.symtab[loc].para) {//若为有参函数
					if (sym != LPARENT) {//函数无左括号,直接读到分号结束
						skip(sentbegsys, 16, 9);
					}
					else {
						getsym();
						flag = pfuncall(loc);
					}
				}
				else { //无参函数调用
					flag = npfuncall(loc);
				}

				if (flag) {
					printf("<有返回值函数调用语句>\n");
					fprintf(out, "<有返回值函数调用语句>\n");
				}
				else {
					printf("<无返回值函数调用语句>\n");
					fprintf(out, "<无返回值函数调用语句>\n");
				}

				if (sym != SEMICN) { skip(sentbegsys, 12, 9); }
				else { getsym(); }
			}
			else {
				assignsent(loc);
				if (sym != SEMICN) { 
					skip(sentbegsys, 12, 9); }
				else { getsym(); }
			}
		}
		break;
	}
	case IFTK: {
		ifsent();
		break;
	}
	case PRINTFTK: {
		writesent();
		if (sym != SEMICN) { skip(sentbegsys,12,9); }
		else { getsym(); }
		break;
	}
	case RETURNTK: {
		returnsent();
		if (sym != SEMICN) { skip(sentbegsys, 12, 9); }
		else { getsym(); }
		break;
	}
	case SCANFTK: {
		readsent();
		if (sym != SEMICN) { skip(sentbegsys, 12, 9); }
		else { getsym(); }
		break;
	}
	case SWITCHTK: {
		casesent();
		break;
	}
	case WHILETK: {
		whilesent();
		break;
	}
	case SEMICN: {//空语句
		getsym();
		break;
	}
	case LBRACE: {
		getsym();
		sentlist();
		if (sym != RBRACE) { skip(sentbegsys, 11, 9); }
		else { getsym(); }
		break;
	}
	default: {
		skip(sentbegsys, 22, 9);
		break;
	}
	}
}

//＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}
//未判断是否与形参列表相匹配,loc为函数在符号表中的位置
void rparalist(int loc) {
	int rpara, type, i;
	int fpara = loc + 1;
	int vtx = 0;
	char varst[MAX_TMP_STACK][MAX_IDEN];
	char var[MAX_IDEN];
	//第一个参数，无需判断是否多传递了参数
	rpara = expr(&type);
	/*if ((type > EX_ARRAY && type - 1 >= tab.symtab[fpara].type) ||
		(type <= EX_ARRAY && tab.symtab[rpara].type >= tab.symtab[fpara].type)) 
	{
		qtvargen(var, type, 0, 0, rpara);
		strcpy(varst[vtx], var);
		vtx++;
	}*/
	//实参形参类型必须一致，否则报错
	if ((type > EX_ARRAY && type - 1 == tab.symtab[fpara].type) ||
		(type <= EX_ARRAY && tab.symtab[rpara].type == tab.symtab[fpara].type))
	{
		qtvargen(var, type, 0, 0, rpara);
		strcpy(varst[vtx], var);
		vtx++;
	}
	else {
		error(25);
	}
	fpara++;
	while (sym == COMMA) {
		getsym();
		if (fpara == tx || tab.symtab[fpara].kind != K_PARA) {//参数传多了
			error(25);
			while(sym != RPARENT || sym != SEMICN) {
				getsym();
			}
			return;
		}

		rpara = expr(&type);
		if ((type > EX_ARRAY && type - 1 == tab.symtab[fpara].type) ||
			(type <= EX_ARRAY && tab.symtab[rpara].type == tab.symtab[fpara].type))
		{
			qtvargen(var, type, 0, 0, rpara);
			strcpy(varst[vtx], var);
			vtx++;
		}
		else { error(25); }
		fpara++;
	}
	if (tab.symtab[fpara].kind == K_PARA) {
		error(25);
	}
	
	//vtx--;//函数参数倒序存储
	//while(vtx >= 0) {
	//	emit(PARAV, varst[vtx], "", "");
	//	vtx--;
	//}
	//函数参数存储
	for (i = 0; i < vtx; i++) {
		emit(PARAV, varst[i], "", "");
	}
	vtx = 0;
}

//无参函数调用返回函数返回值类型sytype
int npfuncall(int loc) {
	emit(CALL, tab.symtab[loc].name, "", "");
	return tab.symtab[loc].type;
}

//有参函数调用,返回函数类型
int pfuncall(int loc) {
	int sytype;
	if (sym == RPARENT) {//传递参数太少，则报错
		error(25);
	}
	else{
		rparalist(loc);
	}
	sytype = npfuncall(loc);
	if (sym != RPARENT) {
		error(9);
	}
	else if (sym != SEMICN) { getsym(); }
	return sytype;
}

//＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
//type->0:VOID(标识符或函数调用), 1 : ARRAY, 2 : INTI, 3 : CHARI
int expr(int *type) {
	int sign = 0;
	int index, itype;
	int loc1, loc2, type1, type2, sytype;
	enum qt_op op;
	char var1[MAX_IDEN], var2[MAX_IDEN];
	char tmp[MAX_IDEN];
	if (sym == PLUS || sym == MINU) {
		sign = sym == PLUS ? 0 : 1;//若为负号，则sign为1
		getsym();
	}
	loc1 = term(type,&index,&itype);
	if (*type == 1) {//若为数组，先将项存入临时变量，再操作，解决a[a[0]]数组嵌套问题
		type1 = qtvargen(var1, *type, index, itype, loc1);
		loc1 = qtmpgen(tmp, type1);
		*type = 0;
		emit(MOV, var1, "", tmp);
		if (sign) {//此处var2相当于tmp1，只是一个临时变量
			loc1 = qtmpgen(var2,type1);
			//*type = 0;
			emit(NEG, tmp, "", var2);
		}
	}
	else { 
		if (sign) {
			type1 = qtvargen(var1, *type, index, itype, loc1);
			loc1 = qtmpgen(tmp, type1);
			*type = 0;
			emit(NEG, var1, "", tmp);
		}
	}

	while (sym == PLUS || sym == MINU) {
		op = sym == PLUS ? ADD : SUB;
		type1 = qtvargen(var1, *type, index, itype, loc1);
		getsym();
		loc2 = term(type, &index, &itype);
		type2 = qtvargen(var2, *type, index, itype, loc2);
		sytype = (type1 == T_CHAR && type2 == T_CHAR) ? T_CHAR : T_INT;
		loc1 = qtmpgen(tmp, sytype);
		*type = 0;
		emit(op, var1, var2, tmp);
	}
	printf("<表达式>\n");
	fprintf(out, "<表达式>\n");
	return loc1;
}

//＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
int term(int *type, int * index, int * itype) {
	int loc1, loc2, type1, type2, sytype;
	enum qt_op op;
	char var1[MAX_IDEN], var2[MAX_IDEN];
	char tmp[MAX_IDEN];
	loc1 = factor(type, index, itype);
	while (sym == MULT || sym == DIV) {
		op = sym == MULT ? MUL : DIVV;
		type1 = qtvargen(var1, *type, *index, *itype, loc1);
		getsym();
		loc2 = factor(type, index, itype);
		type2 = qtvargen(var2, *type, *index, *itype, loc2);
		sytype = (type1 == T_CHAR && type2 == T_CHAR) ? T_CHAR : T_INT;
		loc1 = qtmpgen(tmp, sytype);
		*type = 0;
		emit(op, var1, var2, tmp);
	}
	printf("<项>\n");
	fprintf(out, "<项>\n");
	return loc1;
}

//＜因子＞    ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’|‘(’＜表达式＞‘)’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
//type为因子类型(0:VOID,1:ARRAY,2:INTI,3:CHARI),index为数组下标(若是一个标识符，则为在符号表中的位置),itype为数组下标类型
//若为标识符则返回其在符号表中的位置，若为立即数则返回值
int factor(int *type, int *index, int *itype) {
	int loc, tloc;
	int tmpkind;
	int sign;
	char var[MAX_IDEN];
	char tmp[MAX_IDEN];
	*type = 0;//函数和标识符均为0：void
	if (sym == IDEN) {//标识符|标识符[表达式]|有返回值函数调用
		if ((loc = lookup(iden)) < 0) {//数组未定义的错误局部处理
			error(6);
			getsym();
			if (sym == LBRACK) {
				getsym();
				while (sym != RBRACK) {
					if (sym == IDEN && lookup(iden)<0) {
						error(6);
					}
					getsym();
				}
				getsym();
			}
			else if (sym == LPARENT) {
				getsym();
				while (sym != RPARENT) {
					if (sym == IDEN && lookup(iden)<0) {
						error(6);
					}
					getsym();
				}
				getsym();
			}
		}
		else {
			tmpkind = tab.symtab[loc].kind;
			if( tmpkind == K_FUNC) {
				if (tab.symtab[loc].type == 0) {//若该函数为void
					error(23);
				}
				getsym();
				if (tab.symtab[loc].para) {//若为有参函数
					if (sym != LPARENT) {//函数无左括号,直接读到分号结束
						error(16);
						while (sym != PLUS || sym != MINU || 
							sym != MULT || sym != DIV || sym != SEMICN) 
						{ getsym();}
					}else{
						getsym();
						loc = qtmpgen(tmp, pfuncall(loc));
					}
				}
				else { loc = qtmpgen(tmp, npfuncall(loc)); }
				
				emit(MOV, "~$V0", "", tmp);//运行栈相关
			}
			else if (tmpkind == K_ARRAY) {//数组处理
				getsym();
				if (sym == LBRACK) {
					getsym();
					*index = expr(type);//获取下标类型以及下标本身
					*itype = *type;
					*type = 1;
					qtvargen(var, *type, *index, *itype, loc);//生成符号表位置为loc的局部变量的伪名
					tloc = loc;
					loc = qtmpgen(tmp, tab.symtab[loc].type);//生成临时变量,将数组元素存入
					emit(MOV, var, "", tmp);
					*type = 0;
					if (*itype > EX_ARRAY && (*index > tab.symtab[tloc].sz || *index < 0)) {
						error(24);
					}
					if (sym != RBRACK)   error(10);
					else if (sym != SEMICN) { getsym(); }
				}
			}
			else {//标识符
				getsym();
			}
		}
	}
	else if (sym == LPARENT) {//‘(’＜表达式＞‘)’
		getsym();
		loc = expr(type);
		if (sym != RPARENT)  error(9);
		else if (sym != SEMICN) { getsym(); }
	}
	else {//＜整数＞|＜字符＞
		if (sym == PLUS || sym == MINU) {//整数
			sign = sym == PLUS ? 1 : -1;
			getsym();
			if (sym == INTCON) {
				*type = 2;//语义相关
				loc = sign * num;
				getsym();
			}
		}
		else if (sym == INTCON) {//无符号整数
			*type = 2;
			loc = num;
			getsym();
		}
		else if (sym == CHARCON) {//字符
			*type = 3;
			loc = chcon;
			getsym();
		}
		else {
			error(15);
			getsym();
		}
	}
	printf("<因子>\n");
	fprintf(out, "<因子>\n");
	return loc;
}

