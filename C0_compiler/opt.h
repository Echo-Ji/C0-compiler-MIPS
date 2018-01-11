#pragma once
#include "global.h"

void optimize() {
	dagOpt();
	holeOpt();	//窥孔优化要放在dag之后
}

//窥孔优化
void holeOpt() {
	mergeQt();
	//cstSp();
}

//四元式加减乘除简单合并
void mergeQt() {
	int i;
	for (i = 0; i < qtx; i++) {
		if(qtab[i].op == ADD || qtab[i].op == SUB||
		qtab[i].op == MUL || qtab[i].op == DIVV){ 
			if (qtab[i+1].op == MOV && qtab[i].var3[0]=='$' &&	//非临时变量需要存储过程值
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
	//删除行没有标签则直接返回
	//if (qltab[i] == -1) return;
	if (qtab[i].label == -1) return;

	//更新行没有标签则直接将删除行标签移动至更新行,并清空删除行标签
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

	//先更改所有跳转到该语句的标签	
	char oldlbl[10], newlbl[10];
	int j;
	//strcpy(oldlbl, ltab[qltab[i]]);
	//strcpy(newlbl, ltab[qltab[i - 1]]);
	//for (j = 0; j < qtx; j++) {
	//	if (!strcmpi(qtab[j].var3, oldlbl)) {
	//		strcpy(qtab[j].var3, newlbl);
	//	}
	//}
	//qltab[i] = -1;	//删除行标签清理
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

//立即常量传播
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


//Dag图消除子表达式
void dagOpt() {
	int i, j;
	//划分基本块
	blkDiv(0);
	/*for (int i = 0; i < blx; i++) {
		for (int j = 0; blocks[i][j].qidx != -1; j++){
			fprintf(out, "流图%d:基本块%d入口:%4d\n", i, j, blocks[i][j].qidx);
			fprintf(out, "\t\t后继:");
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

//整个程序的基本块划分, flag=1求后继
void blkDiv(int flag) {
	int i;
	blx = 0;
	for (i = 0; i < qtx; i++) {	//跳过全局常量变量的声明
		if (qtab[i].op == FUNC || qtab[i].op == MAINF) break;
	}
	while (i < qtx && (qtab[i].op == FUNC || qtab[i].op == MAINF)) {
		i++;
		i = blkProc(i, flag);
		i++;
	}
}

//流图内基本块划分
int blkProc(int index, int flag) {
	int i, j, k, t;
	i = index;
	k = 0;	//k为blocks内层指针
	//声明语句跳读
	while (qtab[i].op == CONST || qtab[i].op == VAR) { i++; }
	//寻找所有入口语句
	blocks[blx][k++].qidx = i;	//1.程序的第一条语句是入口语句
	while (qtab[i].op != EFUNC && qtab[i].op != EMAINF) {
		if (isin(qtab[i].op, jmpSet, 9)<0) {//非跳转语句
			i++;
			continue;
		}
		else {
			//int blk = k - 1;//当前基本块号
			if (qtab[i+1].op != EFUNC && qtab[i+1].op != EMAINF) {//2.跳转语句的下一句
				for (j = 0; j < k; j++) {
					if (blocks[blx][j].qidx == i + 1) break;
				}
				if (j == k) {blocks[blx][k++].qidx = i + 1;}
			}
			if(qtab[i].op != RET) {//3.跳转语句跳到的第一条语句
				for (t = index; qtab[t].op != EFUNC && qtab[t].op != EMAINF; t++) {
					if (qtab[t].label != -1 && !strcmpi(qtab[i].var3, ltab[qtab[t].label])) {
						for (j = 0; j < k; j++) {
							if (blocks[blx][j].qidx == t) break;
						}
						if (j == k) {blocks[blx][k++].qidx = t;}
						break;	//只能跳转到一个地方
					}
				}
			}
		}
		i++;
	}

	//入口语句排序
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
	
	//函数结束点EFUNC/EMAINF记录
	if(i > index){
		blocks[blx][k++].qidx = i;
	}
	blocks[blx][k].qidx = -1;
	
	if (flag) {
		//求后继基本块
		int lastQt;
		for (j = 0; j < k; j++) {
			if (blocks[blx][j + 1].qidx < 0) { break; }	//B_exit无后继
			lastQt = blocks[blx][j + 1].qidx - 1;	//本块最后一个四元式下标
			if (qtab[lastQt].op == RET) {//RET后继为B_exit
				//blocks[blx][j].suc[blocks[blx][j].sidx++] = i;//此处换为块号---
				blocks[blx][j].suc[blocks[blx][j].sidx++] = getBlkNo(blx, k, i);
			}
			else if (qtab[lastQt].op == JMP) {//JMP后继为label_x
				for (t = index; qtab[t].op != EFUNC && qtab[t].op != EMAINF; t++) {
					if (qtab[t].label != -1 && !strcmpi(qtab[lastQt].var3, ltab[qtab[t].label])) {
						//blocks[blx][j].suc[blocks[blx][j].sidx++] = t;//-----
						blocks[blx][j].suc[blocks[blx][j].sidx++] = getBlkNo(blx, k, t);
					}
				}
			}
			else {//其他语句与分支后继为下一条[&label_x]
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

//用四元式下标获得基本块下标
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
	if (qtab[s].label != -1) {//若为跳转语句跳到的第一条语句，则将标签存储
		optQtab[oqtx].label = qtab[s].label;
	}

	for (i = s; i < e; i++) {
		if (qtab[i].op == ADD || qtab[i].op == SUB ||
			qtab[i].op == MUL || qtab[i].op == DIVV) {
			//处理左操作数
			lchild = lkupNode(qtab[i].var1);	//结点表中寻找x,立即数也该加入节点表
			if (lchild == -1) {//若未找到，dag图中新建叶结点，并加入结点表
				genDagNode(qtab[i].var1, CONST, name);
				lchild = insertDag(name, -1, -1);
				if (name[0] == '_') { insertNode(qtab[i].var1, lchild); }
			}
			else {//若找到，记录下x对应的dag图中的结点号
				lchild = Ntab[lchild].no;
			}
			//处理右操作数
			rchild = lkupNode(qtab[i].var2);
			if (rchild == -1) {
				genDagNode(qtab[i].var2, CONST, name);
				rchild = insertDag(name, -1, -1);
				if (name[0] == '_') { insertNode(qtab[i].var2, rchild); }
			}
			else {
				rchild = Ntab[rchild].no;
			}
			//处理符号
			genDagNode("", qtab[i].op, name);
			parent = lkupDag(name, lchild, rchild);
			if (parent == -1) {//若未找到，新建中间结点
				parent = insertDag(name, lchild, rchild);
			}
			//处理赋值结果
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
				//MOV,a[b],,$tn一定是赋值给临时变量，所以肯定需要新建node
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
				//-------若存在a[1] = 1;a[1] = 1;应该优化--------
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

//产生Dag图结点，CONST-叶子结点，VAR-数组运算
void genDagNode(char *name, enum qt_op op,char *nodeName) {
	if (op == CONST) {//叶子结点
		if (isdigit(name[0])){	
			strcpy(nodeName, name);
		}
		else{
			sprintf(nodeName, "_%s", name);
		}
	}
	else if(op == VAR) {	//数组运算
		strcpy(nodeName, name);
	}
	else{//中间结点
		strcpy(nodeName, qt_op_name[op]);
	}
}

//查询Dag图，找到返回下标，否则返回-1
int lkupDag(char *name, int lchild, int rchild) {
	int i;
	for (i = 0; i < dtx; i++) {
		if (!strcmp(Dtab[i].op, name) &&
			Dtab[i].lchild == lchild && Dtab[i].rchild == rchild)
			return i;
	}
	return -1;
}

//结点入Dag图，返回结点号
int insertDag(char *name, int lchild, int rchild) {
	strcpy(Dtab[dtx].op, name);
	Dtab[dtx].lchild = lchild;
	Dtab[dtx].rchild = rchild;
	dtx++;
	return (dtx - 1);
}

//查询结点表，找到返回下标，否则返回-1
int lkupNode(char *name) {
	int i;
	for (i = 0; i < ntx; i++) {
		if (!strcmp(Ntab[i].name, name))
			return i;
	}
	return -1;
}

//结点入结点表
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
	//处理叶子结点
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
		if (Dtab[i].lchild != -1) {	//左孩子为-1，右孩子一定为-1，存在左孩子不为-1右孩子为-1的情况(数组运算)
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
	if (Dtab[lchild].lchild == -1) {	//左孩子是叶结点
		if (Dtab[lchild].op[0] == '_') {strcpy(var1, &Dtab[lchild].op[1]);}
		else {strcpy(var1, Dtab[lchild].op);}
	}
	else {//左孩子是中间结点
		for (i = 0, j = -1; i < ntx; i++) {
			if (Ntab[i].no == lchild) {
				j = i;
				if (Ntab[i].name[0] != '$')	//不是临时变量则导出
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
	//op获取
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

//优化后的四元式入表
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

//全局寄存器分配
void regDis() {
	int i;
	blkDiv(1);
	for (i = 0; i < blx; i++) {
		LVAnalysis(i);
	}
}

//活跃变量分析 LV-analysis
void LVAnalysis(int procNum) {
	int i, j, k;
	i = procNum;
	//对每一个基本块求其def和use集
	getUD(procNum);
	do {
		j = getIO(procNum);
	}while (j);
	for (j = 0; blocks[i][j].qidx != -1; j++) {
		fprintf(out, "流图%d:基本块%d入口:%4d\n", i, j, blocks[i][j].qidx);
		fprintf(out, "\t\t后继:");
		for (k = 0; k < blocks[i][j].sidx; k++) {
			fprintf(out, "\tB%d", blocks[i][j].suc[k]);
		}
		fprintf(out, "\n");
		fprintf(out, "\t\tuse集合:");
		for (k = 0; k < blocks[i][j].uidx; k++) {
			fprintf(out, "\t%s", blocks[i][j].use[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
		fprintf(out, "\t\tdef集合:");
		for (k = 0; k < blocks[i][j].didx; k++) {
			fprintf(out, "\t%s", blocks[i][j].def[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
		fprintf(out, "\t\tout集合:");
		for (k = 0; k < blocks[i][j].oidx; k++) {
			fprintf(out, "\t%s", blocks[i][j].out[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
		fprintf(out, "\t\tin集合:");
		for (k = 0; k < blocks[i][j].iidx; k++) {
			fprintf(out, "\t%s", blocks[i][j].in[k]);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
	}
}

//获得USE集和DEF集
void getUD(int procNum) {
	int i, j, index;
	int varFlag;
	for (j = 0; blocks[procNum][j + 1].qidx != -1; j++) {	//流图内,不包括Bexit
		varidx = 0;
		for (i = blocks[procNum][j].qidx; i < blocks[procNum][j+1].qidx; i++) { //基本块内
			if (qtab[i].op == CALL) { continue; }

			if (strcmpi(qtab[i].var1, "") && qtab[i].var1[0] != '_' && qtab[i].var1[0] != '~' && //_可能是str或者v0
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

//该变量若出现过则返回1，否则返回0，若不是变量则返回-1
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

//得到in和out集
int getIO(int procNum) {
	int flag1 = 0, flag = 0;	//判断in,out是否有变化
	int i, j, blkNum, p;
	char tmp[MAX_VAR][MAX_IDEN];
	int tidx = 0;

	p = procNum;
	i = 0;
	while (blocks[p][i].qidx != -1) { i++; }
	blkNum = i-1;	//Bexit编号

	for (i = blkNum-1; i >= 0; i--) {
		tidx = 0;
		for (j = 0; j < blocks[p][i].sidx; j++) {//out[B] = U_B的后继块P in[P]
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
		//a[t][0] = '\0';	//从a中删除元素b[i]
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

//找到则返回下标，否则-1
int isin(char *item, char arr[][MAX_IDEN], int len) {
	for (int i = 0; i < len; i++) {
		if (!strcmpi(item, arr[i])) return i;
	}
	return -1;
}