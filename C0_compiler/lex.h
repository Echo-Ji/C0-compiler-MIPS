#pragma once
#include "global.h"
//----------for homework----------------//
char * word_out[] = {
	"IDEN", "INTCON", "CHARCON", "STRCON", "CASETK", "CHARTK", "CONSTTK", "DEFAULTTK",
	"ELSETK", "IFTK", "INTTK", "MAINTK", "PRINTFTK", "RETURNTK", "SCANFTK", "SWITCHTK",
	"VOIDTK", "WHILETK","PLUS", "MINU", "MULT", "DIV", "LSS", "LEQ", "GRE", "GEQ", "EQL",
	"NEQ", "ASSIGN", "COLON", "SEMICN", "COMMA", "QMARK", "DQMARK","LPARENT", "RPARENT",
	"LBRACK", "RBRACK", "LBRACE", "RBRACE"
};

enum symbol ropsym[] = {
	PLUS, MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ
};

char * rop[] = {
	"+", "-", " *", "/", "<", "<=", ">", ">=", "==", "!="
};

enum symbol rptsym[] = {
	ASSIGN, COLON, SEMICN, COMMA, QMARK, DQMARK, LPARENT, RPARENT,
	LBRACK, RBRACK, LBRACE, RBRACE
};

char * rpt[] = {
	"=", ":", ";", ",", "'", "\"", "(", ")", "[", "]", "{", "}"
};
//----------end for homework----------------//


void error(int errcode) {
	switch (errcode) {
	case 0: {
		fprintf(err, "行%d\t列%d\t：多位无符号整数不能有前导零\n", line_num, cc);
		break;
	}
	case 1: {
		fprintf(err, "行%d\t列%d\t：字符常量为非法字符\n", line_num, cc);
		break;
	}
	case 2: {
		fprintf(err, "行%d\t列%d\t：字符常量缺少缺少单引号\n", line_num, cc);
		break;
	}
	case 3: {
		fprintf(err, "行%d\t列%d\t：非法Token\n", line_num, cc);
		break;
	}
	case 4: {
		fprintf(err, "行%d\t列%d\t：字符串常量包含非法字符\n", line_num, cc);
		break;
	}
	case 5: {
		fprintf(err, "行%d\t列%d\t：字符串常量越界或缺少双引号\n", line_num, cc);
		break;
	}
	case 6: {
		fprintf(err, "行%d\t列%d\t：标识符未声明\n", line_num, cc);
		break;
	}
	case 7: {
		if (sym != IDEN) {
			fprintf(err, "行%d\t列%d\t：标识符重定义\n", line_num, cc-1);
		}
		else {
			fprintf(err, "行%d\t列%d\t：标识符重定义\n", line_num, cc);
		}
		break;
	}
	case 8: {
		fprintf(err, "行%d\t列%d\t：符号表溢出\n", line_num, cc);
		exit(0);
		break;
	}
	case 9: {
		fprintf(err, "行%d\t列%d\t：缺失右小括号\n", line_num, cc);
		break;
	}
	case 10: {
		fprintf(err, "行%d\t列%d\t：缺失右方括号\n", line_num, cc);
		break;
	}
	case 11: {
		fprintf(err, "行%d\t列%d\t：缺失右花括号\n", line_num, cc);
		break;
	}
	case 12: {
		fprintf(err, "行%d\t列%d\t：缺失分号\n", line_num, cc);
		break;
	}
	case 13: {
		fprintf(err, "行%d\t列%d\t：数组范围应为无符号整数\n", line_num, cc);
		break;
	}
	case 14: {
		fprintf(err, "行%d\t列%d\t：赋值语句右边出现非法符号\n", line_num, cc);
		break;
	}
	case 15: {
		fprintf(err, "行%d\t列%d\t：立即数丢失\n", line_num, cc);
		break;
	}
	case 16: {
		fprintf(err, "行%d\t列%d\t：缺失左小括号\n", line_num, cc);
		break;
	}
	case 17: {
		fprintf(err, "行%d\t列%d\t：缺失左方括号\n", line_num, cc);
		break;
	}
	case 18: {
		fprintf(err, "行%d\t列%d\t：缺失左花括号\n", line_num, cc-1);
		break;
	}
	case 19: {
		fprintf(err, "行%d\t列%d\t：缺失\"else\"关键字\n", line_num, cc);
		break;
	}
	case 20: {
		fprintf(err, "行%d\t列%d\t：缺失冒号\n", line_num, cc);
		break;
	}
	case 21: {
		fprintf(err, "行%d\t列%d\t：非法常量\n", line_num, cc);
		break;
	}
	case 22: {
		fprintf(err, "行%d\t列%d\t：不合法语句\n", line_num, cc);
		break;
	}
	case 23: {
		fprintf(err, "行%d\t列%d\t：表达式类型不匹配\n", line_num, cc);
		break;
	}
	case 24: {
		fprintf(err, "行%d\t列%d\t：数组越界\n", line_num, cc);
		break;
	}
	case 25: {
		fprintf(err, "行%d\t列%d\t：函数调用中参数类型或个数不匹配\n", line_num, cc);
		break;
	}
	case 26: {
		fprintf(err, "行%d\t列%d\t：不期望的标识符类型\n", line_num, cc);
		break;
	}
	case 27: {
		fprintf(err, "行%d\t列%d\t：赋值语句缺失\"=\"\n", line_num, cc);
		break;
	}
	case 28: {
		fprintf(err, "行%d\t列%d\t：字符串表溢出\n", line_num, cc);
		exit(0);
		break;
	}
	case 29: {
		fprintf(err, "行%d\t列%d\t：有返回值函数缺失return语句\n", line_num, cc);
		//exit(0);
		break;
	}
	case 30: {
		fprintf(err, "行%d\t列%d\t：此声明没有类型说明符\n", line_num, cc);
		break;
	}
	case 233: {
		fprintf(err, "行%d\t列%d\t：源文件不完整\n", line_num, cc);
		break;
	}
	}
	error_flag = 1;
}

void getch()
{//文件结束的问题
	if (cc >= ll)
	{
		if (!fgets(line, MAX_LINE - 1, in))
		{//未读取到东西
			if (!end_flag)
			{
				end_flag = 1;
				ch = ' ';
				return;
			}
			else
			{
				printf("编译失败：错误信息请查看文件err.txt!\n");
				error(233);
				//printf("词法分析完成，输出请查看文件out.txt，错误信息请查看err.txt\n");
				fclose(in);
				fclose(out);
				fclose(err);
				exit(0);
			}
		}
		ll = strlen(line);//最后一个字符为\n，防止联行编译对程序过解读
						  //ll--;
		cc = 0;
		line_num++;
		//printf("%d:	%s",line_num, line);
	}
	ch = line[cc];
	cc++;
}

void getsym()
{
	int k = 0; //相当于clearToken
	int i, j, r;
	while (isspace(ch)) {
		getch();
	}
	//标识符读入
	if (isalpha(ch) || ch == '_')
	{
		k = 0;
		while (isalpha(ch) || isdigit(ch) || ch == '_') {
			iden[k++] = ch;
			getch();
		}
		iden[k] = '\0';
		//若上次的单词长度与这次的一致，则不做多余操作
		if (k>kk) kk = k;
		else {
			while (k<kk)
				iden[--kk] = '\0';
		}
		//寻找是否为保留字
		i = 0;
		j = RESERVE_WORD_NUM - 1;
		while (i <= j) {
			k = (i + j) / 2;
			if (!(r = strcmp(iden, rword[k])))//若相等
			{
				sym = rwsym[k];
				if (k == 1) {//CHARTK在rwsym中的位置
					type = CHARCON;
				}
				else if (k == 6) {//INTTK在rwsym中的位置
					type = INTCON;
				}
				break;
			}
			else if (r>0) {
				i = k + 1;
			}
			else {
				j = k - 1;
			}
		}
		if (r) {
			sym = IDEN;
		}
	}
	//无符号整数读入
	else if (isdigit(ch))
	{
		num = 0;
		if (ch == '0') {
			//不能有以零开始的无符号整数
			getch();
			if (isdigit(ch)) {
				error(0);
			}
		}
		while (isdigit(ch)) {
			num = num * 10 + ch - '0';
			getch();
		}
		sym = INTCON;
	}
	//字符读入
	else if (ch == '\'')
	{	//单引号多个字符怎么处理，或者没有右单引号
		sym = QMARK;
		getch();
		if (!(isalpha(ch) || isdigit(ch) || ch == '+' ||
			ch == '-' || ch == '*' || ch == '/')) {
			//非法字符,用\0填充
			error(1);
			chcon = '\0';
		}
		else chcon = ch;
		getch();
		if (ch != '\'')
		{	//字符没有用单引号括起来,错误做局部化处理，不能直接停止编译
			error(2);
		}
		sym = CHARCON;
		getch();
	}
	//字符串读入
	else if (ch == '\"')
	{
		sym = DQMARK;
		k = 0;
		strcon[k] = '\0'; //清理掉字符串
		getch();
		while (ch != '\"')
		{	//若本行没有双引号如何处理:读到行末报错
			if (ch == 32 || ch == 33 || (ch > 34 && ch < 127)) {
				strcon[k++] = ch;
			}
			else {
				//非法字符
				error(4);
			}
			if (cc >= ll || k == MAX_STR - 1) {
				//字符串无右引号
				error(5);
				break;
			}
			getch();
		}
		strcon[k] = '\0';
		sym = STRCON;
		getch();
	}
	//运算符
	else if (ch == '+') {
		sym = PLUS;
		getch();
	}
	else if (ch == '-') {
		sym = MINU;
		getch();
	}
	else if (ch == '*') {
		sym = MULT;
		getch();
	}
	else if (ch == '/') {
		sym = DIV;
		getch();
	}
	else if (ch == '<') {
		sym = LSS;
		getch();
		if (ch == '=') {
			sym = LEQ;
			getch();
		}
	}
	else if (ch == '>') {
		sym = GRE;
		getch();
		if (ch == '=') {
			sym = GEQ;
			getch();
		}
	}
	else if (ch == '=') {
		sym = ASSIGN;
		getch();
		if (ch == '=') {
			sym = EQL;
			getch();
		}
	}
	else if (ch == '!') {
		getch();
		if (ch == '=') {
			sym = NEQ;
			getch();
		}
		else {
			error(3);
		}
	}
	//其余符号
	else if (ch == ':') {
		sym = COLON;
		getch();
	}
	else if (ch == ';') {
		sym = SEMICN;
		getch();
	}
	else if (ch == ',') {
		sym = COMMA;
		getch();
	}
	else if (ch == '(') {
		sym = LPARENT;
		getch();
	}
	else if (ch == ')') {
		sym = RPARENT;
		getch();
	}
	else if (ch == '[') {
		sym = LBRACK;
		getch();
	}
	else if (ch == ']') {
		sym = RBRACK;
		getch();
	}
	else if (ch == '{') {
		sym = LBRACE;
		getch();
	}
	else if (ch == '}') {
		sym = RBRACE;
		getch();
	}
	else {
		error(3);
	}
}
