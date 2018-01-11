#pragma once
#include "global.h"

void optimize() {
	dagOpt();
	holeOpt();	//�����Ż�Ҫ����dag֮��
}

//�����Ż�
void holeOpt() {
	mergeQt();
	//cstSp();
}

//��Ԫʽ�Ӽ��˳��򵥺ϲ�
void mergeQt() {
	int i;
	for (i = 0; i < qtx; i++) {
		if(qtab[i].op == ADD || qtab[i].op == SUB||
		qtab[i].op == MUL || qtab[i].op == DIVV){ 
			if (qtab[i+1].op == MOV && qtab[i].var3[0]=='$' &&	//����ʱ������Ҫ�洢����ֵ
				!strcmpi(qtab[i].var3, qtab[i + 1].var1)) {
				procLbl(i+1);
				strcpy(qtab[i].var3, qtab[i + 1].var3);
				delQt(i+1);
			}
		}

		if (qtab[i].op == JMP && !strcmpi(qtab[i].var3, ltab[qtab[i+1].label])&&
			qtab[i].label == -1) {
			delQt(i);
		}
	}
}

void procLbl(int i) {
	//ɾ����û�б�ǩ��ֱ�ӷ���
	//if (qltab[i] == -1) return;
	if (qtab[i].label == -1) return;

	//������û�б�ǩ��ֱ�ӽ�ɾ���б�ǩ�ƶ���������,�����ɾ���б�ǩ
	/*if (qltab[i - 1] == -1) {
		qltab[i - 1] = qltab[i];
		qltab[i] = -1;
		return;
	}*/
	if (qtab[i - 1].label == -1) {
		qtab[i - 1].label = qtab[i].label;
		qtab[i].label = -1;
		return;
	}

	//�ȸ���������ת�������ı�ǩ	
	char oldlbl[10], newlbl[10];
	int j;
	//strcpy(oldlbl, ltab[qltab[i]]);
	//strcpy(newlbl, ltab[qltab[i - 1]]);
	//for (j = 0; j < qtx; j++) {
	//	if (!strcmpi(qtab[j].var3, oldlbl)) {
	//		strcpy(qtab[j].var3, newlbl);
	//	}
	//}
	//qltab[i] = -1;	//ɾ���б�ǩ����
	strcpy(oldlbl, ltab[qtab[i].label]);
	strcpy(newlbl, ltab[qtab[i - 1].label]);
	for (j = 0; j < qtx; j++) {
		if (!strcmpi(qtab[j].var3, oldlbl)) { strcpy(qtab[j].var3, newlbl); }
	}
	qtab[i].label = -1;
	return;
}

void delQt(int i) {
	int j;
	for (j = i; j < qtx; j++) {
		qtab[j] = qtab[j + 1];
		//qltab[j] = qltab[j + 1];
	}
	qtx--;
	//qltx--;
}

//������������
void cstSp() {
	int i;
	int a, b, c;
	char tmp[10];
	for (i = 0; i < qtx; i++) {
		if (qtab[i].var1[0] == '-' || isdigit(qtab[i].var1[0])) {
			if (qtab[i].op == NEG) {
				c = -atoi(qtab[i].var1);
				sprintf(tmp, "%d", c);
				for (int j = i+1; qtab[j].op != EFUNC && qtab[j].op != EMAINF && strcmpi(qtab[j].var3, qtab[i].var3); j++) {
					if (!strcmpi(qtab[j].var1, qtab[i].var3)) { strcpy(qtab[j].var1, tmp); }
					if (!strcmpi(qtab[j].var2, qtab[i].var3)) { strcpy(qtab[j].var2, tmp); }
				}
			}
			else if (qtab[i].op == MOV) {
				c = atoi(qtab[i].var1);
				sprintf(tmp, "%d", c);
				for (int j = i + 1; qtab[j].op != EFUNC && qtab[j].op != EMAINF && strcmpi(qtab[j].var3, qtab[i].var3); j++) {
					if (!strcmpi(qtab[j].var1, qtab[i].var3)) { strcpy(qtab[j].var1, tmp); }
					if (!strcmpi(qtab[j].var2, qtab[i].var3)) { strcpy(qtab[j].var2, tmp); }
				}
			}
			else {
				if ((qtab[i].var2[0] == '-' || isdigit(qtab[i].var2[0])) &&
					(qtab[i].op == ADD || qtab[i].op == SUB ||
						qtab[i].op == MUL || qtab[i].op == DIVV)) {
					a = atoi(qtab[i].var1);
					b = atoi(qtab[i].var2);
					switch (qtab[i].op) {
					case ADD: {
						c = a + b;
						break;
					}case SUB: {
						c = a - b;
						break;
					}case MUL: {
						c = a*b;
						break;
					}case DIVV: {
						c = a / b;
						break;
					}
					}
					sprintf(tmp, "%d", c);
					for (int j = i+1; qtab[j].op != EFUNC && qtab[j].op != EMAINF && strcmpi(qtab[j].var3,qtab[i].var3); j++) {
						if (!strcmpi(qtab[j].var1, qtab[i].var3)) { strcpy(qtab[j].var1, tmp); }
						if (!strcmpi(qtab[j].var2, qtab[i].var3)) { strcpy(qtab[j].var2, tmp); }
					}
				}
			}
		}
	}
}


//Dagͼ�����ӱ��ʽ
void dagOpt() {
	int i, j;
	//���ֻ�����
	blkDiv(0);
	/*for (int i = 0; i < blx; i++) {
		for (int j = 0; blocks[i][j].qidx != -1; j++){
			fprintf(out, "��ͼ%d:������%d���:%4d\n", i, j, blocks[i][j].qidx);
			fprintf(out, "\t\t���:");
			for (int k = 0; k < blocks[i][j].sidx; k++)
				fprintf(out, "\t%d", blocks[i][j].suc[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
	}*/

	for (i = 0, j = 0; i < blx; i++) {
		while (j < blocks[i][0].qidx) {
			optQtab[oqtx++] = qtab[j];
			j++;
		}
		for (j = 0; blocks[i][j + 1].qidx != -1; j++)
			bldDag(blocks[i][j].qidx, blocks[i][j + 1].qidx);
		j = blocks[i][j].qidx;	//END OF A FUNC
	}
	while (j < qtx) {	//END OF MAIN
		optQtab[oqtx++] = qtab[j];
		j++;
	}
	copyOptab();
}

//��������Ļ����黮��, flag=1����
void blkDiv(int flag) {
	int i;
	blx = 0;
	for (i = 0; i < qtx; i++) {	//����ȫ�ֳ�������������
		if (qtab[i].op == FUNC || qtab[i].op == MAINF) break;
	}
	while (i < qtx && (qtab[i].op == FUNC || qtab[i].op == MAINF)) {
		i++;
		i = blkProc(i, flag);
		i++;
	}
}

//��ͼ�ڻ����黮��
int blkProc(int index, int flag) {
	int i, j, k, t;
	i = index;
	k = 0;	//kΪblocks�ڲ�ָ��
	//�����������
	while (qtab[i].op == CONST || qtab[i].op == VAR) { i++; }
	//Ѱ������������
	blocks[blx][k++].qidx = i;	//1.����ĵ�һ�������������
	while (qtab[i].op != EFUNC && qtab[i].op != EMAINF) {
		if (isin(qtab[i].op, jmpSet, 9)<0) {//����ת���
			i++;
			continue;
		}
		else {
			//int blk = k - 1;//��ǰ�������
			if (qtab[i+1].op != EFUNC && qtab[i+1].op != EMAINF) {//2.��ת������һ��
				for (j = 0; j < k; j++) {
					if (blocks[blx][j].qidx == i + 1) break;
				}
				if (j == k) {blocks[blx][k++].qidx = i + 1;}
			}
			if(qtab[i].op != RET) {//3.��ת��������ĵ�һ�����
				for (t = index; qtab[t].op != EFUNC && qtab[t].op != EMAINF; t++) {
					if (qtab[t].label != -1 && !strcmpi(qtab[i].var3, ltab[qtab[t].label])) {
						for (j = 0; j < k; j++) {
							if (blocks[blx][j].qidx == t) break;
						}
						if (j == k) {blocks[blx][k++].qidx = t;}
						break;	//ֻ����ת��һ���ط�
					}
				}
			}
		}
		i++;
	}

	//����������
	blkEntry tmp;
	for (j = 0; j < k; j++) {
		for (t = k - 1; t > j; t--) {
			if (blocks[blx][t].qidx < blocks[blx][t - 1].qidx) {
				tmp = blocks[blx][t];
				blocks[blx][t] = blocks[blx][t - 1];
				blocks[blx][t - 1] = tmp;
			}
		}
	}
	
	//����������EFUNC/EMAINF��¼
	if(i > index){
		blocks[blx][k++].qidx = i;
	}
	blocks[blx][k].qidx = -1;
	
	if (flag) {
		//���̻�����
		int lastQt;
		for (j = 0; j < k; j++) {
			if (blocks[blx][j + 1].qidx < 0) { break; }	//B_exit�޺��
			lastQt = blocks[blx][j + 1].qidx - 1;	//�������һ����Ԫʽ�±�
			if (qtab[lastQt].op == RET) {//RET���ΪB_exit
				//blocks[blx][j].suc[blocks[blx][j].sidx++] = i;//�˴���Ϊ���---
				blocks[blx][j].suc[blocks[blx][j].sidx++] = getBlkNo(blx, k, i);
			}
			else if (qtab[lastQt].op == JMP) {//JMP���Ϊlabel_x
				for (t = index; qtab[t].op != EFUNC && qtab[t].op != EMAINF; t++) {
					if (qtab[t].label != -1 && !strcmpi(qtab[lastQt].var3, ltab[qtab[t].label])) {
						//blocks[blx][j].suc[blocks[blx][j].sidx++] = t;//-----
						blocks[blx][j].suc[blocks[blx][j].sidx++] = getBlkNo(blx, k, t);
					}
				}
			}
			else {//����������֧���Ϊ��һ��[&label_x]
				blocks[blx][j].suc[blocks[blx][j].sidx++] = getBlkNo(blx, k, lastQt + 1);
				if (isin(qtab[lastQt].op, jmpSet, 9) < 0) { continue; }
				for (t = index; qtab[t - 1].op != EFUNC && qtab[t - 1].op != EMAINF; t++) {
					if (qtab[t].label != -1 && !strcmpi(qtab[lastQt].var3, ltab[qtab[t].label])) {
						//blocks[blx][j].suc[blocks[blx][j].sidx++] = t;//
						blocks[blx][j].suc[blocks[blx][j].sidx++] = getBlkNo(blx, k, t);
					}
				}
			}
		}
	}
	blx++;
	return i;
}

//����Ԫʽ�±��û������±�
int getBlkNo(int proc, int blks, int qtIndex) {
	for (int i = 0; i < blks; i++) {
		if (blocks[proc][i].qidx == qtIndex) { return i; }
	}
	return -1;
}

int isin(enum qt_op sy, enum qt_op *sys, int length) {
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

void bldDag(int s, int e) {
	int i;
	int lchild, rchild, parent, result;
	char name[MAX_IDEN], arrayName[MAX_IDEN], index[MAX_IDEN];
	if (qtab[s].label != -1) {//��Ϊ��ת��������ĵ�һ����䣬�򽫱�ǩ�洢
		optQtab[oqtx].label = qtab[s].label;
	}

	for (i = s; i < e; i++) {
		if (qtab[i].op == ADD || qtab[i].op == SUB ||
			qtab[i].op == MUL || qtab[i].op == DIVV) {
			//�����������
			lchild = lkupNode(qtab[i].var1);	//������Ѱ��x,������Ҳ�ü���ڵ��
			if (lchild == -1) {//��δ�ҵ���dagͼ���½�Ҷ��㣬���������
				genDagNode(qtab[i].var1, CONST, name);
				lchild = insertDag(name, -1, -1);
				if (name[0] == '_') { insertNode(qtab[i].var1, lchild); }
			}
			else {//���ҵ�����¼��x��Ӧ��dagͼ�еĽ���
				lchild = Ntab[lchild].no;
			}
			//�����Ҳ�����
			rchild = lkupNode(qtab[i].var2);
			if (rchild == -1) {
				genDagNode(qtab[i].var2, CONST, name);
				rchild = insertDag(name, -1, -1);
				if (name[0] == '_') { insertNode(qtab[i].var2, rchild); }
			}
			else {
				rchild = Ntab[rchild].no;
			}
			//�������
			genDagNode("", qtab[i].op, name);
			parent = lkupDag(name, lchild, rchild);
			if (parent == -1) {//��δ�ҵ����½��м���
				parent = insertDag(name, lchild, rchild);
			}
			//����ֵ���
			result = lkupNode(qtab[i].var3);
			if (result == -1) {
				insertNode(qtab[i].var3, parent);
			}
			else {
				Ntab[result].no = parent;
			}
		}
		else if (qtab[i].op == NEG) {
			lchild = lkupNode(qtab[i].var1);
			if (lchild == -1) {
				genDagNode(qtab[i].var1, CONST, name);
				lchild = insertDag(name, -1, -1);
				if (name[0] == '_') {
					insertNode(qtab[i].var1, lchild);
				}
			}
			else {
				lchild = Ntab[lchild].no;
			}

			genDagNode("", qtab[i].op, name);
			parent = lkupDag(name, lchild, -1);
			if (parent == -1) {
				parent = insertDag(name, lchild, -1);
			}
			
			result = lkupNode(qtab[i].var3);
			if (result == -1) {
				insertNode(qtab[i].var3, parent);
			}
			else {
				Ntab[result].no = parent;
			}
		}
		else if (qtab[i].op == MOV) {
			if (!strcmp(qtab[i].var1, "~$V0")) { 
				optQtab[oqtx++] = qtab[i];
				continue;
			}

			if (isArrayItem(qtab[i].var1, arrayName, index)) {
				lchild = lkupNode(arrayName);
				if (lchild == -1) {
					genDagNode(arrayName, CONST, name);
					lchild = insertDag(name, -1, -1);
					if (name[0] == '_') { insertNode(arrayName, lchild); }
				}
				else {
					lchild = Ntab[lchild].no;
				}

				rchild = lkupNode(index);
				if (rchild == -1) {
					genDagNode(index, CONST, name);
					rchild = insertDag(name, -1, -1);
					if (name[0] == '_') { insertNode(index, rchild); }
				}
				else {
					rchild = Ntab[rchild].no;
				}

				genDagNode("[]", VAR, name);
				parent = lkupDag(name, lchild, rchild);
				if (parent == -1) {
					parent = insertDag(name, lchild, rchild);
				}
				//MOV,a[b],,$tnһ���Ǹ�ֵ����ʱ���������Կ϶���Ҫ�½�node
				insertNode(qtab[i].var3, parent);
			}
			else if (isArrayItem(qtab[i].var3, arrayName, index)) {
				lchild = lkupNode(index);
				if (lchild == -1) {
					genDagNode(index, CONST, name);
					lchild = insertDag(name, -1, -1);
					if (name[0] == '_') { insertNode(index, lchild); }
				}
				else {
					lchild = Ntab[lchild].no;
				}

				rchild = lkupNode(qtab[i].var1);
				if (rchild == -1) {
					genDagNode(qtab[i].var1, CONST, name);
					rchild = insertDag(name, -1, -1);
					if (name[0] == '_') { insertNode(qtab[i].var1, rchild); }
				}
				else {
					rchild = Ntab[rchild].no;
				}

				genDagNode("[]=", VAR, name);
				parent = lkupDag(name, lchild, rchild);
				if (parent == -1) {
					parent = insertDag(name, lchild, rchild);
				}
				//-------������a[1] = 1;a[1] = 1;Ӧ���Ż�--------
				insertNode(arrayName, parent);
			}
			else {
				lchild = lkupNode(qtab[i].var1);
				if (lchild == -1) {
					genDagNode(qtab[i].var1, CONST, name);
					lchild = insertDag(name, -1, -1);
					if (name[0] == '_') {
						insertNode(qtab[i].var1, lchild);
					}
				}
				else {
					lchild = Ntab[lchild].no;
				}

				result = lkupNode(qtab[i].var3);
				if (result == -1) {
					insertNode(qtab[i].var3, lchild);
				}
				else {
					Ntab[result].no = lchild;
				}
			}
		}
		else if (qtab[i].op == READ || qtab[i].op == WRITE) {
			dagOut();
			optQtab[oqtx++] = qtab[i];
		}
		else if (qtab[i].op == PARAV) {
			dagOut();
			do {
				optQtab[oqtx++] = qtab[i];
				i++;
			} while (qtab[i].op != CALL);
			optQtab[oqtx++] = qtab[i];
		}
		else {
			dagOut();
			optQtab[oqtx++] = qtab[i];
		}
	}
	if (dtx || ntx) {
		dagOut();
	}
}

//����Dagͼ��㣬CONST-Ҷ�ӽ�㣬VAR-��������
void genDagNode(char *name, enum qt_op op,char *nodeName) {
	if (op == CONST) {//Ҷ�ӽ��
		if (isdigit(name[0])){	
			strcpy(nodeName, name);
		}
		else{
			sprintf(nodeName, "_%s", name);
		}
	}
	else if(op == VAR) {	//��������
		strcpy(nodeName, name);
	}
	else{//�м���
		strcpy(nodeName, qt_op_name[op]);
	}
}

//��ѯDagͼ���ҵ������±꣬���򷵻�-1
int lkupDag(char *name, int lchild, int rchild) {
	int i;
	for (i = 0; i < dtx; i++) {
		if (!strcmp(Dtab[i].op, name) &&
			Dtab[i].lchild == lchild && Dtab[i].rchild == rchild)
			return i;
	}
	return -1;
}

//�����Dagͼ�����ؽ���
int insertDag(char *name, int lchild, int rchild) {
	strcpy(Dtab[dtx].op, name);
	Dtab[dtx].lchild = lchild;
	Dtab[dtx].rchild = rchild;
	dtx++;
	return (dtx - 1);
}

//��ѯ�����ҵ������±꣬���򷵻�-1
int lkupNode(char *name) {
	int i;
	for (i = 0; i < ntx; i++) {
		if (!strcmp(Ntab[i].name, name))
			return i;
	}
	return -1;
}

//��������
void insertNode(char *name, int no) {
	strcpy(Ntab[ntx].name, name);
	Ntab[ntx].no = no;
	ntx++;
}

int isArrayItem(char *var, char *arrayName, char *index) {
	int i, j;
	for (i = 0,j =0; var[i] != '\0' && var[i] != '['; i++) {
		arrayName[j++] = var[i];
	}
	arrayName[j] = '\0';
	if (var[i] == '\0') return 0;
	for (i=i+1,j = 0; var[i] != ']'; i++) {
		index[j++] = var[i];
	}
	index[j] = '\0';
	return 1;
}

void dagOut() {
//	int opNodes[MAX_QTAB_ENTRY];
//	int opNum;
	int i, j;
	char tmp[MAX_IDEN];
	//opNum = selOpNodes(opNodes);
	//����Ҷ�ӽ��
	for (i = 0; i < dtx; i++) {
		if (Dtab[i].lchild == -1) {
			for (j = 0; j < ntx; j++) {
				if (Ntab[j].no == i && Ntab[j].name[0] != '$') {
					if (Dtab[i].op[0] == '_') { strcpy(tmp, &Dtab[i].op[1]); }
					else { strcpy(tmp, Dtab[i].op); }

					if (strcmp(Ntab[j].name, tmp)) { insertOptab(MOV, tmp, "", Ntab[j].name); }
				}
			}
		}
	}

	for (i = 0; i < dtx; i++) {
		if (Dtab[i].lchild != -1) {	//����Ϊ-1���Һ���һ��Ϊ-1���������Ӳ�Ϊ-1�Һ���Ϊ-1�����(��������)
			writeOptQtab(i);
		}
	}
	
	dtx = 0;
	ntx = 0;
}

//int selOpNodes(int opNodes[]) {
//	int i, j;
//	for (i = 0; i < dtx; i++) {
//		if (Dtab[i].lchild != -1) {
//			writeOptQtab();
//		}
//	}
//}

void writeOptQtab(int node) {
	int i, j, flag;
	int lchild, rchild;
	char var1[MAX_IDEN], var2[MAX_IDEN], tmp[MAX_IDEN];
	enum qt_op op;

	lchild = Dtab[node].lchild;
	rchild = Dtab[node].rchild;
	if (Dtab[lchild].lchild == -1) {	//������Ҷ���
		if (Dtab[lchild].op[0] == '_') {strcpy(var1, &Dtab[lchild].op[1]);}
		else {strcpy(var1, Dtab[lchild].op);}
	}
	else {//�������м���
		for (i = 0, j = -1; i < ntx; i++) {
			if (Ntab[i].no == lchild) {
				j = i;
				if (Ntab[i].name[0] != '$')	//������ʱ�����򵼳�
					break;
			}
		}
		strcpy(var1, Ntab[j].name);
	}

	if (rchild != -1) {
		if (Dtab[rchild].lchild == -1) {
			if (Dtab[rchild].op[0] == '_') { strcpy(var2, &Dtab[rchild].op[1]); }
			else { strcpy(var2, Dtab[rchild].op); }
		}
		else {
			for (i = 0, j = -1; i < ntx; i++) {
				if (Ntab[i].no == rchild) {
					j = i;
					if (Ntab[i].name[0] != '$')
						break;
				}
			}
			strcpy(var2, Ntab[j].name);
		}
	}
	else { var2[0] = '\0'; }
	//op��ȡ
	if (!strcmp(Dtab[node].op, "ADD")) { op = ADD; }
	else if (!strcmp(Dtab[node].op, "SUB")) { op = SUB; }
	else if (!strcmp(Dtab[node].op, "MUL")) { op = MUL; }
	else if (!strcmp(Dtab[node].op, "DIVV")) { op = DIVV; }
	else if (!strcmp(Dtab[node].op, "NEG")) { op = NEG; }
	else if (!strcmp(Dtab[node].op, "[]") ||
		!strcmp(Dtab[node].op, "[]=")) {
		op = MOV;
	}
	else {
		printf("unexpected op!\n");
	}
	flag = 1;
	for (i = 0, j = -1; i < ntx; i++) {
		if (Ntab[i].no == node) {
			if (Ntab[i].name[0] == '$') { j = i; }
			else {
				flag = 0;
				if (op == MOV) {
					if (!strcmp(Dtab[node].op, "[]")) {
						sprintf(tmp, "%s[%s]", var1, var2);
						insertOptab(op, tmp, "", Ntab[i].name);
					}
					else {
						sprintf(tmp, "%s[%s]", Ntab[i].name, var1);
						insertOptab(op, var2, "", tmp);
					}
				}
				else { insertOptab(op, var1, var2, Ntab[i].name); }
			}
		}
	}

	if (flag) {
		if (op == MOV) {
			if (!strcmp(Dtab[node].op, "[]")) {
				sprintf(tmp, "%s[%s]", var1, var2);
				insertOptab(op, tmp, "", Ntab[j].name);
			}
			else {
				sprintf(tmp, "%s[%s]", Ntab[j].name, var1);
				insertOptab(op, var2, "", tmp);
			}
		}
		else { insertOptab(op, var1, var2, Ntab[j].name); }
	}
}

//�Ż������Ԫʽ���
void insertOptab(enum qt_op op, char *var1, char *var2, char *var3) {
	optQtab[oqtx].op = op;
	strcpy(optQtab[oqtx].var1, var1);
	strcpy(optQtab[oqtx].var2, var2);
	strcpy(optQtab[oqtx].var3, var3);
	oqtx++;
}

void copyOptab() {
	int i, j;
	for (i = 0, j = 0; i < oqtx; i++) {
		qtab[j++] = optQtab[i];
	}
	qtx = j;
}

//ȫ�ּĴ�������
void regDis() {
	int i;
	blkDiv(1);
	for (i = 0; i < blx; i++) {
		LVAnalysis(i);
	}
}

//��Ծ�������� LV-analysis
void LVAnalysis(int procNum) {
	int i, j, k;
	i = procNum;
	//��ÿһ������������def��use��
	getUD(procNum);
	do {
		j = getIO(procNum);
	}while (j);
	for (j = 0; blocks[i][j].qidx != -1; j++) {
		fprintf(out, "��ͼ%d:������%d���:%4d\n", i, j, blocks[i][j].qidx);
		fprintf(out, "\t\t���:");
		for (k = 0; k < blocks[i][j].sidx; k++) {
			fprintf(out, "\tB%d", blocks[i][j].suc[k]);
		}
		fprintf(out, "\n");
		fprintf(out, "\t\tuse����:");
		for (k = 0; k < blocks[i][j].uidx; k++) {
			fprintf(out, "\t%s", blocks[i][j].use[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
		fprintf(out, "\t\tdef����:");
		for (k = 0; k < blocks[i][j].didx; k++) {
			fprintf(out, "\t%s", blocks[i][j].def[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
		fprintf(out, "\t\tout����:");
		for (k = 0; k < blocks[i][j].oidx; k++) {
			fprintf(out, "\t%s", blocks[i][j].out[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
		fprintf(out, "\t\tin����:");
		for (k = 0; k < blocks[i][j].iidx; k++) {
			fprintf(out, "\t%s", blocks[i][j].in[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
	}
}

//���USE����DEF��
void getUD(int procNum) {
	int i, j, index;
	int varFlag;
	for (j = 0; blocks[procNum][j + 1].qidx != -1; j++) {	//��ͼ��,������Bexit
		varidx = 0;
		for (i = blocks[procNum][j].qidx; i < blocks[procNum][j+1].qidx; i++) { //��������
			if (qtab[i].op == CALL) { continue; }

			if (strcmpi(qtab[i].var1, "") && qtab[i].var1[0] != '_' && qtab[i].var1[0] != '~' && //_������str����v0
				!isdigit(qtab[i].var1[0]) && qtab[i].var1[0] != '-') {
				strcpy(varList[varidx], qtab[i].var1);
				varListFlag[varidx++] = 0;
			}
			if (strcmpi(qtab[i].var2, "") && 
				!isdigit(qtab[i].var2[0]) && qtab[i].var2[0] != '-') {
				strcpy(varList[varidx], qtab[i].var2);
				varListFlag[varidx++] = 0;
			}
			if (strcmpi(qtab[i].var3, "") && strncmp(qtab[i].var3, "label", 5)) {
				strcpy(varList[varidx], qtab[i].var3);
				varListFlag[varidx++] = 0;
			}
		}

		for (i = blocks[procNum][j].qidx; i < blocks[procNum][j+1].qidx; i++) {
			if (!getFlag(qtab[i].var1, &index)) {
				varListFlag[index] = 1;
				strcpy(blocks[procNum][j].use[blocks[procNum][j].uidx++], qtab[i].var1);
			}
			if (!getFlag(qtab[i].var2, &index)) {
				varListFlag[index] = 1;
				strcpy(blocks[procNum][j].use[blocks[procNum][j].uidx++], qtab[i].var2);
			}
			if (!getFlag(qtab[i].var3, &index)) {
				varListFlag[index] = 1;
				strcpy(blocks[procNum][j].def[blocks[procNum][j].didx++], qtab[i].var3);
			}
		}
	}
	
}

//�ñ��������ֹ��򷵻�1�����򷵻�0�������Ǳ����򷵻�-1
int getFlag(char *name, int *index) {
	int i;
	for (i = 0; i < varidx; i++) {
		if (!strcmpi(varList[i], name)) {
			*index = i;
			return varListFlag[i];
		}
	}
	*index = -1;
	return -1;
}

//�õ�in��out��
int getIO(int procNum) {
	int flag1 = 0, flag = 0;	//�ж�in,out�Ƿ��б仯
	int i, j, blkNum, p;
	char tmp[MAX_VAR][MAX_IDEN];
	int tidx = 0;

	p = procNum;
	i = 0;
	while (blocks[p][i].qidx != -1) { i++; }
	blkNum = i-1;	//Bexit���

	for (i = blkNum-1; i >= 0; i--) {
		tidx = 0;
		for (j = 0; j < blocks[p][i].sidx; j++) {//out[B] = U_B�ĺ�̿�P in[P]
			flag = getUnion(blocks[p][blocks[p][i].suc[j]].in, blocks[p][blocks[p][i].suc[j]].iidx, 
				blocks[p][i].out, blocks[p][i].oidx, blocks[p][i].out, &blocks[p][i].oidx);
		}
		getPoor(blocks[p][i].out, blocks[p][i].oidx, blocks[p][i].def, blocks[p][i].didx, tmp, &tidx);
		flag1 = getUnion(blocks[p][i].use, blocks[p][i].uidx, tmp, tidx, blocks[p][i].in, &blocks[p][i].iidx);
	}

	return (flag || flag1);
}

//c = a - b
void getPoor(char a[][MAX_IDEN], int len_a, char b[][MAX_IDEN], int len_b, char c[][MAX_IDEN], int *len_c) {
	int i, t;
	int ai[MAX_VAR] = {0};
	for (i = 0; i < len_b; i++) {
		if ((t = isin(b[i], a, len_a)) < 0) { continue; }
		//a[t][0] = '\0';	//��a��ɾ��Ԫ��b[i]
		ai[t] = -1;
	}
	t = *len_c;
	for (i = 0; i < len_a; i++) {
		if ((ai[i] != -1) && (isin(a[i], c, t)<0)) {
			strcpy(c[t++], a[i]);
		}
	}
	*len_c = t;
}

//c = a U b
int getUnion(char a[][MAX_IDEN], int len_a, char b[][MAX_IDEN], int len_b, char c[][MAX_IDEN], int *len_c) {
	int i, k, flag = 0;
	k = *len_c;
	for (i = 0; i < len_a; i++) {
		if ((isin(a[i], b, len_b)<0) && (isin(a[i], c, k)<0)) {
			flag = 1;
			strcpy(c[k++], a[i]);
		}
	}

	for (i = 0; i < len_b; i++) {
		if (isin(b[i], c, k) < 0) {
			flag = 1;
			strcpy(c[k++], b[i]);
		}
	}
	*len_c = k;
	return flag;
}

//�ҵ��򷵻��±꣬����-1
int isin(char *item, char arr[][MAX_IDEN], int len) {
	for (int i = 0; i < len; i++) {
		if (!strcmpi(item, arr[i])) return i;
	}
	return -1;
}