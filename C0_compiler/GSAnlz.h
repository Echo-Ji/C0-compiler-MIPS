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

int lookup(char *name) {//���ڷ��ű��д洢�˺����������ʲ������Դ�tx�鵽0
	int iter, iend;
	iend = tab.btab[btotal];
	for (iter = tx; iter>iend; iter--) {//�ֲ�����,����
		if (strcmpi(name, tab.symtab[iter].name) == 0) {
			return iter;
		}
	}

	for (iter = btotal; iter >= 1; iter--) {//��ѯ������
		if (strcmpi(name, tab.symtab[tab.btab[iter]].name) == 0) {
			return tab.btab[iter];
		}
	}

	iend = tab.btab[1] - 1;
	for (iter = iend; iter >= 0; iter--) {//ȫ�ֱ���
		if (strcmpi(name, tab.symtab[iter].name) == 0) {
			return iter;
		}
	}
	return -1;
}

//����name����Ϊ��ʱ��ʶ�����洢��iden��
int insertab(char *name) {//�Դ������,���������ظ���ֵ
	if (tx >= MAX_SYTAB_ENTRY) {//���ű�����������˳�����
		error(8);
	}

	int iter, iend;
	if (tkind != K_ARRAY) {
		tsz = 0;
	}
	if (tkind == K_FUNC) {//�����Ƿ�����
						  //���жϷֳ����
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
	else {//�򵥱���/����/�����Ƿ�����
		iter = tab.btab[btotal];//�ֲ����������뺯��������
		for (; iter<tx; iter++) {
			if (!strcmpi(name, tab.symtab[iter].name)) {
				error(7);
				return 0;
			}
		}
		//�����ڵľֲ�����ֻ��Ҫ���ұ������ظ��ͺ�
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

void pushfunc(int paranum) {//����������������
	tab.symtab[tab.btab[btotal]].para = paranum;
}

void popfunc() {//�ƶ�ָ�뵽�������һ��λ��
	int iend = tab.btab[btotal];
	tx = iend + tab.symtab[iend].para + 1;
}

//������Ԫʽ�й涨��ʽ�Ĳ�����
//����ֵΪsytype; nameΪ������������
int qtvargen(char *name, int type, int index, int itype, int loc) {
	int sytype;
	char tn1[MAX_IDEN], tn2[MAX_IDEN];
	int lloc = tab.btab[btotal];//level location
	int lpx = lloc + 1;
	int lvx = lloc + tab.symtab[lloc].para + 1;
	if (type > EX_ARRAY) {//��Ϊ���������ַ���ascii��
		sprintf(name, "%d", loc);
		sytype = type - 1;//��ex_type > 1ʱ��sytype = ex_type - 1
	}
	else if (type == EX_ARRAY) {//����
		if (loc < lvx) {//ȫ������ֱ��ȡ����
			strcpy(name, tab.symtab[loc].name);
		}
		else {//�ֲ�������˳������
			sprintf(name, "%%%d", loc - lvx);
		}

		if (itype) {//��index type��Ϊ��ʶ��,itype������Ϊ���飨1��
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
		if (tab.symtab[loc].kind == K_TMP) {//��ʱ����
			strcpy(name, tab.symtab[loc].name);
		}
		else if (loc >= lvx) {//�ֲ�����
			sprintf(name, "%%%d", loc - lvx);
		}
		else if (loc >= lpx  && loc < lvx) {//��Ϊ��������
			sprintf(name, "@%d", loc - lpx);
		}
		else {//ȫ�ֱ�ʶ�� loc < lpx
			strcpy(name, tab.symtab[loc].name);
		}
		sytype = tab.symtab[loc].type;
	}
	return sytype;
}

//��Ԫʽ��ʱ����������,���ظñ����ڷ��ű��е�λ��
//�����a+b*c
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

//��ǩ����label_0,label_1
int labgen() {
	sprintf(ltab[lbx], "label_%d", lbx);
	lbx++;
	return lbx - 1;
}


void setlab(int qidx, int label) {
	char llabel[MAX_IDEN], nlabel[MAX_IDEN];
	int i;
	if (qltab[qidx] != -1) {//���ô��б�ǩ
		//��ȡԭlabel
		strcpy(llabel, ltab[qltab[qidx]]);
		//��ȡ��label
		strcpy(nlabel, ltab[label]);
		//lbx--;//ɾ���±�ǩ
		//��ǩ�ظ���,��Ҫ�޸�var3Ϊnew label����Ԫʽ
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
	while (sym == CONSTTK) {//������˵���ϲ����˴�
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
				//tpara = 0;	//��ʼ���Ա���fpara�м�¼�βθ����Ӷ������tpara
				tkind = K_FUNC;
				tvalue = 0;	//�������ⶼΪ��
				tsz = 0;	//�������ⶼΪ��
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
				printf("<������>\n");
				fprintf(out, "<������>\n");
				if (sym != RBRACE) { error(11); }
				while (true) {
					if (end_flag) {
						if (isspace(ch)) {
							//printf("�﷨������ɣ������鿴�ļ�out.txt��������Ϣ��鿴err.txt\n");
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
			if (defhead()) {//�з���ֵ��������
				funcdef();
				if (ret_flag) {
					ret_flag = 0;
				}
				else {
					error(29);
				}
			}
			else {//��������
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
		printf("����ʧ�ܣ�������Ϣ��鿴�ļ�err.txt!\n");
	}
	else {
		printf("����ɹ���\n");
	}
}

int defhead() {//û��������
	int flag = 0;   //��Ϊ����ͷ����Ϊ1������ͷ����Ϊ0
	if (sym == IDEN) {
		getsym();
		if (sym == COMMA || sym == SEMICN || sym == LBRACK) {
			flag = 0;
		}
		else if (sym == LBRACE || sym == LPARENT) {
			flag = 1;
		}
		else {
			//����/�����������ַǷ��ַ�
			printf("defhead()���ַǷ��ַ�\n");
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
						if (insertab(iden)) {//��û���ض��壬��������Ԫʽ
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
	printf("<��������>\n");
	fprintf(out, "<��������>\n");
}

//emit-undebug
void vardef() {
	char tmp[MAX_IDEN];
	tkind = K_VAR;
	tvalue = 0;
	tpara = 0;
	tsz = 0;//����Ĭ��СΪ��
	if (sym == LBRACK) {
		tkind = K_ARRAY;
		//�����޷�������
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
			if (insertab(iden)) {//����insertab
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
		else {//�ʵ�����
			while (sym != SEMICN && sym != COMMA) {
				getsym();
			}
		}
	}
	else {  //SEMICN����COMMA
		if (insertab(iden)) { //�򵥱���insertab
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
				//������Ų���
				getsym();
				if (sym == PLUS || sym == MINU) {
					error(13);
					while (sym != INTCON) { getsym(); }
				}
				//�����޷�������
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
						//����ͼ򵥱���������
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
	printf("<��������>\n");
	fprintf(out, "<��������>\n");
}
//emit-undebug
void procdef() {//�ӱ�ʶ����ʼ
	tpara = 0;
	if (sym == IDEN) {//ԭ���ڴ˴����������������Ԫʽ��ʱ���������ƶ���funcdef��
		getsym();
		funcdef();
	}
	printf("<�޷���ֵ��������>\n");
	fprintf(out, "<�޷���ֵ��������>\n");
}
//emit-undebug
void funcdef () {//�ӱ�ʶ��֮��ʼ
	tpara = 0;	//��ʼ���Ա���fpara�м�¼�βθ����Ӷ������tpara
	tkind = K_FUNC;
	tvalue = 0;	//�������ⶼΪ��
	tsz = 0;	//�������ⶼΪ��

	if (insertab(iden)) {//�������
		emit(FUNC, iden, "", "");
	}

	if (sym == LPARENT) {//�в�������
		getsym();
		fparalist();
		if (sym != RPARENT) {
			while (sym == IDEN) { getsym(); }
			skip(sentbegsys, 9, 9); //Ӧ�������������忪ʼ
		}
		else { getsym(); }
	}
	
	pushfunc(tpara);    //����������������

	if (sym != LBRACE) {
		skip(sentbegsys, 18, 9);
	}
	else { getsym(); }//ֻ����LBRACE���޲�������
	compoundsent();
	popfunc();//�˱���������ĩ
	emit(EFUNC,tab.symtab[tab.btab[btotal]].name, "", "");
	if (sym != RBRACE){ error(11);}
	else { getsym(); }
	if (tab.symtab[tab.btab[btotal]].type != 0) {
		printf("<�з���ֵ��������>\n");
		fprintf(out, "<�з���ֵ��������>\n");
	}
}

void fparalist() {//�β��費��Ҫ������Ԫʽ
	tkind = K_PARA;
	tvalue = 0;
	if (sym == INTTK || sym == CHARTK) {
		ttype = sym == INTTK ? T_INT : T_CHAR;
		getsym();
		if (sym == IDEN) {
			tpara++;
			insertab(iden); //��������ű�
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
				insertab(iden); //��������ű�
				getsym();
			}
		}
	}
}

void compoundsent() {
	ret_flag = 0;
	while (sym == CONSTTK) {//������˵���ϲ����˴�
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
	printf("<�������>\n");
	fprintf(out, "<�������>\n");
}

//������У�   ::= ������䣾��
void sentlist() {
	while (isin(sym, sentbegsys, 9) >= 0) {
		sentence();
	}
	printf("<�����>\n");
	fprintf(out, "<�����>\n");
}

//��������䣾::= if ��(������������)������䣾else����䣾
//û��else�ͱ�������ִ֤�е���ȷ��
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
		printf("<�������>\n");
		fprintf(out, "<�������>\n");
	}
	else { skip(sentbegsys, 19, 9); }
}


//��������    ::=  �����ʽ������ϵ������������ʽ���������ʽ��
//���ش������ǩ����Ԫʽָ��,��Ԫʽ����ı��ʽΪ������ת,�������������Ҫȡ��
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

//��ѭ����䣾   ::=  while ��(������������)������䣾
void whilesent()
{
	int cdx, lpx;
	int label1, label2;
	getsym();
	if (sym != LPARENT) {
		if (sym != PLUS && sym != MINU) {//��Ϊ���ʽ�Ŀ�ʼ������Ҫ���ַ�
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
	setlab(lpx, label1);//ѭ������
	//qtab[qtx].label = label2;
	setlab(qtx, label2);

	printf("<ѭ�����>\n");
	fprintf(out, "<ѭ�����>\n");
}

//�������䣾  ::=  switch ��(�������ʽ����)�� ��{���������[��ȱʡ��] ��}��
void casesent() {
	int loc, type;
	getsym();
	if (sym != LPARENT) {
		if (sym != PLUS && sym != MINU) {//��Ϊ���ʽ�Ŀ�ʼ������Ҫ���ַ�
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
	printf("<������>\n");
	fprintf(out, "<������>\n");
}

//�������   ::=  ���������䣾{���������䣾}
//���������䣾  ::=  case��������������䣾
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

		getsym();//�з�����
		if (sym == PLUS || sym == MINU) {//����
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
		
		/////////////ѭ������ʣ�µ�case
		while(sym == CASETK){
			//qtab[qtx].label = label1;	
			setlab(qtx, label1);//setlab

			label1 = labgen();
			emit(JNE, var1, "", ltab[label1]);
			lack_v2 = qtx - 1;

			getsym();//�з�����
			if (sym == PLUS || sym == MINU) {//����
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
			emit(JMP, "", "", ltab[label0]);//��ת��������
		} 

		if (sym == DEFAULTTK) {
			//qtab[qtx].label = label1;
			setlab(qtx, label1);
			getsym();
			if (sym != COLON) { error(20); }
			getsym();
			sentence();
			//emit(JMP, "", "", ltab[label0]);//default��ת����һ������ʡ��
		}
		else{//��û��default���޸����һ��case
			strcpy(qtab[lack_v2].var3, ltab[label0]);//�޸�ʹ����ת����ȷ��λ��
			//lbx--;//ɾ�����һ��label,��Ϊ������
			qtx--;//ɾ�����һ��JMP,"","",label0
		}
		//qtab[qtx].label = label0;
		setlab(qtx, label0);//������
	}
}

//������䣾    ::=  scanf ��(������ʶ����{,����ʶ����}��)��
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
	printf("<�����>\n");
	fprintf(out, "<�����>\n");
}

//��д��䣾    ::= printf ��(�� ���ַ�����,�����ʽ�� ��)��| printf ��(�����ַ����� ��)��| printf ��(�������ʽ����)��
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
		qtvargen(var, type, 0, 0 ,loc);//���ɻ���ʱ��������ֿ����ҵ�����
		emit(WRITE, "", var, "");
	}

	if (sym != RPARENT) { skip(sentbegsys,9,9); }
	else if (sym != SEMICN) { getsym(); }
	printf("<д���>\n");
	fprintf(out, "<д���>\n");
}

//��������䣾   ::=  return[��(�������ʽ����)��]
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
	printf("<�������> \n");
	fprintf(out, "<�������> \n");
}

//����ֵ��䣾   ::=  ����ʶ�����������ʽ��|����ʶ������[�������ʽ����]��=�����ʽ��
void assignsent(int loc) {
	int tloc, type;
	int type1, type2;
	char var1[MAX_IDEN], var2[MAX_IDEN];

	if (tab.symtab[loc].kind != K_VAR && 
		tab.symtab[loc].kind != K_ARRAY &&
		tab.symtab[loc].kind != K_PARA) {//����Ϊ����/������ֵ
		error(26);
		while (sym != SEMICN) {
			getsym();
		}
		return;
	}

	if (sym == LBRACK) {//����Ԫ�ظ�ֵ	
		if (tab.symtab[loc].kind != K_ARRAY) { error(26); }
		getsym();
		tloc = expr(&type);
		type1 = qtvargen(var1, 1, tloc, type, loc);
		if (sym != RBRACK) { skip(sentbegsys, 10, 9); }
		else { getsym(); }
	}
	else {//������򵥱�����ֵ
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
		
		printf("<��ֵ���>\n");
		fprintf(out, "<��ֵ���>\n");
	}
	else {
		error(27);
		while (sym != SEMICN) getsym();
	}
}


//����䣾    ::= ��������䣾����ѭ����䣾| ��{��������У���}�������з���ֵ����������䣾;
//                           |���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|�������䣾����������䣾;
void sentence() {
	int loc;
	int flag;
	char tmp[MAX_IDEN];
	switch (sym) {
	case IDEN: {
		if ((loc = lookup(iden)) < 0) { //��������������
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
				if (tab.symtab[loc].para) {//��Ϊ�вκ���
					if (sym != LPARENT) {//������������,ֱ�Ӷ����ֺŽ���
						skip(sentbegsys, 16, 9);
					}
					else {
						getsym();
						flag = pfuncall(loc);
					}
				}
				else { //�޲κ�������
					flag = npfuncall(loc);
				}

				if (flag) {
					printf("<�з���ֵ�����������>\n");
					fprintf(out, "<�з���ֵ�����������>\n");
				}
				else {
					printf("<�޷���ֵ�����������>\n");
					fprintf(out, "<�޷���ֵ�����������>\n");
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
	case SEMICN: {//�����
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

//��ֵ������   ::= �����ʽ��{,�����ʽ��}
//δ�ж��Ƿ����β��б���ƥ��,locΪ�����ڷ��ű��е�λ��
void rparalist(int loc) {
	int rpara, type, i;
	int fpara = loc + 1;
	int vtx = 0;
	char varst[MAX_TMP_STACK][MAX_IDEN];
	char var[MAX_IDEN];
	//��һ�������������ж��Ƿ�ഫ���˲���
	rpara = expr(&type);
	/*if ((type > EX_ARRAY && type - 1 >= tab.symtab[fpara].type) ||
		(type <= EX_ARRAY && tab.symtab[rpara].type >= tab.symtab[fpara].type)) 
	{
		qtvargen(var, type, 0, 0, rpara);
		strcpy(varst[vtx], var);
		vtx++;
	}*/
	//ʵ���β����ͱ���һ�£����򱨴�
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
		if (fpara == tx || tab.symtab[fpara].kind != K_PARA) {//����������
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
	
	//vtx--;//������������洢
	//while(vtx >= 0) {
	//	emit(PARAV, varst[vtx], "", "");
	//	vtx--;
	//}
	//���������洢
	for (i = 0; i < vtx; i++) {
		emit(PARAV, varst[i], "", "");
	}
	vtx = 0;
}

//�޲κ������÷��غ�������ֵ����sytype
int npfuncall(int loc) {
	emit(CALL, tab.symtab[loc].name, "", "");
	return tab.symtab[loc].type;
}

//�вκ�������,���غ�������
int pfuncall(int loc) {
	int sytype;
	if (sym == RPARENT) {//���ݲ���̫�٣��򱨴�
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

//�����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
//type->0:VOID(��ʶ����������), 1 : ARRAY, 2 : INTI, 3 : CHARI
int expr(int *type) {
	int sign = 0;
	int index, itype;
	int loc1, loc2, type1, type2, sytype;
	enum qt_op op;
	char var1[MAX_IDEN], var2[MAX_IDEN];
	char tmp[MAX_IDEN];
	if (sym == PLUS || sym == MINU) {
		sign = sym == PLUS ? 0 : 1;//��Ϊ���ţ���signΪ1
		getsym();
	}
	loc1 = term(type,&index,&itype);
	if (*type == 1) {//��Ϊ���飬�Ƚ��������ʱ�������ٲ��������a[a[0]]����Ƕ������
		type1 = qtvargen(var1, *type, index, itype, loc1);
		loc1 = qtmpgen(tmp, type1);
		*type = 0;
		emit(MOV, var1, "", tmp);
		if (sign) {//�˴�var2�൱��tmp1��ֻ��һ����ʱ����
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
	printf("<���ʽ>\n");
	fprintf(out, "<���ʽ>\n");
	return loc1;
}

//���     ::= �����ӣ�{���˷�������������ӣ�}
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
	printf("<��>\n");
	fprintf(out, "<��>\n");
	return loc1;
}

//�����ӣ�    ::= ����ʶ����������ʶ������[�������ʽ����]��|��(�������ʽ����)������������|���ַ��������з���ֵ����������䣾
//typeΪ��������(0:VOID,1:ARRAY,2:INTI,3:CHARI),indexΪ�����±�(����һ����ʶ������Ϊ�ڷ��ű��е�λ��),itypeΪ�����±�����
//��Ϊ��ʶ���򷵻����ڷ��ű��е�λ�ã���Ϊ�������򷵻�ֵ
int factor(int *type, int *index, int *itype) {
	int loc, tloc;
	int tmpkind;
	int sign;
	char var[MAX_IDEN];
	char tmp[MAX_IDEN];
	*type = 0;//�����ͱ�ʶ����Ϊ0��void
	if (sym == IDEN) {//��ʶ��|��ʶ��[���ʽ]|�з���ֵ��������
		if ((loc = lookup(iden)) < 0) {//����δ����Ĵ���ֲ�����
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
				if (tab.symtab[loc].type == 0) {//���ú���Ϊvoid
					error(23);
				}
				getsym();
				if (tab.symtab[loc].para) {//��Ϊ�вκ���
					if (sym != LPARENT) {//������������,ֱ�Ӷ����ֺŽ���
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
				
				emit(MOV, "~$V0", "", tmp);//����ջ���
			}
			else if (tmpkind == K_ARRAY) {//���鴦��
				getsym();
				if (sym == LBRACK) {
					getsym();
					*index = expr(type);//��ȡ�±������Լ��±걾��
					*itype = *type;
					*type = 1;
					qtvargen(var, *type, *index, *itype, loc);//���ɷ��ű�λ��Ϊloc�ľֲ�������α��
					tloc = loc;
					loc = qtmpgen(tmp, tab.symtab[loc].type);//������ʱ����,������Ԫ�ش���
					emit(MOV, var, "", tmp);
					*type = 0;
					if (*itype > EX_ARRAY && (*index > tab.symtab[tloc].sz || *index < 0)) {
						error(24);
					}
					if (sym != RBRACK)   error(10);
					else if (sym != SEMICN) { getsym(); }
				}
			}
			else {//��ʶ��
				getsym();
			}
		}
	}
	else if (sym == LPARENT) {//��(�������ʽ����)��
		getsym();
		loc = expr(type);
		if (sym != RPARENT)  error(9);
		else if (sym != SEMICN) { getsym(); }
	}
	else {//��������|���ַ���
		if (sym == PLUS || sym == MINU) {//����
			sign = sym == PLUS ? 1 : -1;
			getsym();
			if (sym == INTCON) {
				*type = 2;//�������
				loc = sign * num;
				getsym();
			}
		}
		else if (sym == INTCON) {//�޷�������
			*type = 2;
			loc = num;
			getsym();
		}
		else if (sym == CHARCON) {//�ַ�
			*type = 3;
			loc = chcon;
			getsym();
		}
		else {
			error(15);
			getsym();
		}
	}
	printf("<����>\n");
	fprintf(out, "<����>\n");
	return loc;
}

