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
		fprintf(err, "��%d\t��%d\t����λ�޷�������������ǰ����\n", line_num, cc);
		break;
	}
	case 1: {
		fprintf(err, "��%d\t��%d\t���ַ�����Ϊ�Ƿ��ַ�\n", line_num, cc);
		break;
	}
	case 2: {
		fprintf(err, "��%d\t��%d\t���ַ�����ȱ��ȱ�ٵ�����\n", line_num, cc);
		break;
	}
	case 3: {
		fprintf(err, "��%d\t��%d\t���Ƿ�Token\n", line_num, cc);
		break;
	}
	case 4: {
		fprintf(err, "��%d\t��%d\t���ַ������������Ƿ��ַ�\n", line_num, cc);
		break;
	}
	case 5: {
		fprintf(err, "��%d\t��%d\t���ַ�������Խ���ȱ��˫����\n", line_num, cc);
		break;
	}
	case 6: {
		fprintf(err, "��%d\t��%d\t����ʶ��δ����\n", line_num, cc);
		break;
	}
	case 7: {
		if (sym != IDEN) {
			fprintf(err, "��%d\t��%d\t����ʶ���ض���\n", line_num, cc-1);
		}
		else {
			fprintf(err, "��%d\t��%d\t����ʶ���ض���\n", line_num, cc);
		}
		break;
	}
	case 8: {
		fprintf(err, "��%d\t��%d\t�����ű����\n", line_num, cc);
		exit(0);
		break;
	}
	case 9: {
		fprintf(err, "��%d\t��%d\t��ȱʧ��С����\n", line_num, cc);
		break;
	}
	case 10: {
		fprintf(err, "��%d\t��%d\t��ȱʧ�ҷ�����\n", line_num, cc);
		break;
	}
	case 11: {
		fprintf(err, "��%d\t��%d\t��ȱʧ�һ�����\n", line_num, cc);
		break;
	}
	case 12: {
		fprintf(err, "��%d\t��%d\t��ȱʧ�ֺ�\n", line_num, cc);
		break;
	}
	case 13: {
		fprintf(err, "��%d\t��%d\t�����鷶ΧӦΪ�޷�������\n", line_num, cc);
		break;
	}
	case 14: {
		fprintf(err, "��%d\t��%d\t����ֵ����ұ߳��ַǷ�����\n", line_num, cc);
		break;
	}
	case 15: {
		fprintf(err, "��%d\t��%d\t����������ʧ\n", line_num, cc);
		break;
	}
	case 16: {
		fprintf(err, "��%d\t��%d\t��ȱʧ��С����\n", line_num, cc);
		break;
	}
	case 17: {
		fprintf(err, "��%d\t��%d\t��ȱʧ������\n", line_num, cc);
		break;
	}
	case 18: {
		fprintf(err, "��%d\t��%d\t��ȱʧ������\n", line_num, cc-1);
		break;
	}
	case 19: {
		fprintf(err, "��%d\t��%d\t��ȱʧ\"else\"�ؼ���\n", line_num, cc);
		break;
	}
	case 20: {
		fprintf(err, "��%d\t��%d\t��ȱʧð��\n", line_num, cc);
		break;
	}
	case 21: {
		fprintf(err, "��%d\t��%d\t���Ƿ�����\n", line_num, cc);
		break;
	}
	case 22: {
		fprintf(err, "��%d\t��%d\t�����Ϸ����\n", line_num, cc);
		break;
	}
	case 23: {
		fprintf(err, "��%d\t��%d\t�����ʽ���Ͳ�ƥ��\n", line_num, cc);
		break;
	}
	case 24: {
		fprintf(err, "��%d\t��%d\t������Խ��\n", line_num, cc);
		break;
	}
	case 25: {
		fprintf(err, "��%d\t��%d\t�����������в������ͻ������ƥ��\n", line_num, cc);
		break;
	}
	case 26: {
		fprintf(err, "��%d\t��%d\t���������ı�ʶ������\n", line_num, cc);
		break;
	}
	case 27: {
		fprintf(err, "��%d\t��%d\t����ֵ���ȱʧ\"=\"\n", line_num, cc);
		break;
	}
	case 28: {
		fprintf(err, "��%d\t��%d\t���ַ��������\n", line_num, cc);
		exit(0);
		break;
	}
	case 29: {
		fprintf(err, "��%d\t��%d\t���з���ֵ����ȱʧreturn���\n", line_num, cc);
		//exit(0);
		break;
	}
	case 30: {
		fprintf(err, "��%d\t��%d\t��������û������˵����\n", line_num, cc);
		break;
	}
	case 233: {
		fprintf(err, "��%d\t��%d\t��Դ�ļ�������\n", line_num, cc);
		break;
	}
	}
	error_flag = 1;
}

void getch()
{//�ļ�����������
	if (cc >= ll)
	{
		if (!fgets(line, MAX_LINE - 1, in))
		{//δ��ȡ������
			if (!end_flag)
			{
				end_flag = 1;
				ch = ' ';
				return;
			}
			else
			{
				printf("����ʧ�ܣ�������Ϣ��鿴�ļ�err.txt!\n");
				error(233);
				//printf("�ʷ�������ɣ������鿴�ļ�out.txt��������Ϣ��鿴err.txt\n");
				fclose(in);
				fclose(out);
				fclose(err);
				exit(0);
			}
		}
		ll = strlen(line);//���һ���ַ�Ϊ\n����ֹ���б���Գ�������
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
	int k = 0; //�൱��clearToken
	int i, j, r;
	while (isspace(ch)) {
		getch();
	}
	//��ʶ������
	if (isalpha(ch) || ch == '_')
	{
		k = 0;
		while (isalpha(ch) || isdigit(ch) || ch == '_') {
			iden[k++] = ch;
			getch();
		}
		iden[k] = '\0';
		//���ϴεĵ��ʳ�������ε�һ�£������������
		if (k>kk) kk = k;
		else {
			while (k<kk)
				iden[--kk] = '\0';
		}
		//Ѱ���Ƿ�Ϊ������
		i = 0;
		j = RESERVE_WORD_NUM - 1;
		while (i <= j) {
			k = (i + j) / 2;
			if (!(r = strcmp(iden, rword[k])))//�����
			{
				sym = rwsym[k];
				if (k == 1) {//CHARTK��rwsym�е�λ��
					type = CHARCON;
				}
				else if (k == 6) {//INTTK��rwsym�е�λ��
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
	//�޷�����������
	else if (isdigit(ch))
	{
		num = 0;
		if (ch == '0') {
			//���������㿪ʼ���޷�������
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
	//�ַ�����
	else if (ch == '\'')
	{	//�����Ŷ���ַ���ô��������û���ҵ�����
		sym = QMARK;
		getch();
		if (!(isalpha(ch) || isdigit(ch) || ch == '+' ||
			ch == '-' || ch == '*' || ch == '/')) {
			//�Ƿ��ַ�,��\0���
			error(1);
			chcon = '\0';
		}
		else chcon = ch;
		getch();
		if (ch != '\'')
		{	//�ַ�û���õ�����������,�������ֲ�����������ֱ��ֹͣ����
			error(2);
		}
		sym = CHARCON;
		getch();
	}
	//�ַ�������
	else if (ch == '\"')
	{
		sym = DQMARK;
		k = 0;
		strcon[k] = '\0'; //������ַ���
		getch();
		while (ch != '\"')
		{	//������û��˫������δ���:������ĩ����
			if (ch == 32 || ch == 33 || (ch > 34 && ch < 127)) {
				strcon[k++] = ch;
			}
			else {
				//�Ƿ��ַ�
				error(4);
			}
			if (cc >= ll || k == MAX_STR - 1) {
				//�ַ�����������
				error(5);
				break;
			}
			getch();
		}
		strcon[k] = '\0';
		sym = STRCON;
		getch();
	}
	//�����
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
	//�������
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
