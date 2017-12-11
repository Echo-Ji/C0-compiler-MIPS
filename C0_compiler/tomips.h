#pragma once
#include "global.h"

//将变量/常量存入表中,len单位为字,相当于为其alloc内存（移动sp）,返回该变量在内存中的起始地址
int insertAlloctab(char *name, int type, int len) 
{
	int lsp;
	strcpy(alloctab[vx].name, name);
	alloctab[vx].type = type;
	alloctab[vx].offset = sp;
	lsp = sp;
	sp -= 4 * len;
	vx++;
	return lsp;
}

//返回查询变量的类型1:int,2:char,3:str
int lkupType(char *name) {
	int type;
	int i, j;
	char buf[MAX_IDEN], aname[MAX_IDEN];
	//若为数组，获取数组名
	for (i = 0, j = 0; name[i] != '\0' && name[i] != '['; i++) {
		aname[j] = name[i];
		j++;
	}
	aname[j] = '\0';

	if (name[0] == '-' || isdigit(name[0])) {
		return AT_INT;
	}
	else if (name[0] == '@') {//函数参数
		//符号表查找当前函数
		i = atoi(&name[1]);
		return functab[fidx + 1 + i].type;
	}
	else if (name[0] == '$' || name[0] == '%') {//局部变量
		for (i = gp; i < vx; i++) {
			if (!strcmpi(aname, alloctab[i].name)) {
				return alloctab[i].type;
			}
		}
		return -1;
	}
	else {//全局变/常量/函数
		//查函数表
		for (i = 0; i < fx; i++) {
			if (!strcmpi(aname, functab[i].name)) {
				return functab[i].type;
			}
		}
		//全局变/常量
		sprintf(buf, "_%s", aname);	//全局变量加标记
		for (i = 0; i < gp; i++) {
			if (!strcmpi(buf, alloctab[i].name)) {
				return alloctab[i].type;
			}
		}
		return  -1;
	}
}

void toMips() {
	mipsData();
	mipsCode();
}

void mipsData() {
	int i, j, k, t;;
	char tmp[MAX_IDEN];
	fprintf(mipsOut, ".data\n");
	//全局变量入表只为存储类型以便打印时用,其地址由data段分配,不需要手动分配
	while (qtab[qidx].op == CONST) {
		sprintf(tmp, "_%s", qtab[qidx].var2);
		if (strcmpi(qtab[qidx].var1, "INT") == 0) {//const int 
			insertAlloctab(tmp, AT_INT, 0);
			fprintf(mipsOut, "%s:\t.word\t%d\t#const int\n", tmp, atoi(qtab[qidx].var3));
		}
		else {//const char
			insertAlloctab(tmp, AT_CHAR, 0);
			fprintf(mipsOut, "%s:\t.word\t\'%c\'\t#const char\n", tmp, atoi(qtab[qidx].var3));
		}
		qidx++;
	}
	while (qtab[qidx].op == VAR) {
		sprintf(tmp, "_%s", qtab[qidx].var2);
		if (!strcmpi(qtab[qidx].var3, "")) {//若为简单变量
			if (strcmpi(qtab[qidx].var1, "INT") == 0) {
				insertAlloctab(tmp, AT_INT, 0);
				fprintf(mipsOut, "%s:\t.word\t0\t#int var\n", tmp);
			}
			else {
				insertAlloctab(tmp, AT_CHAR, 0);
				fprintf(mipsOut, "%s:\t.word\t0\t#char var\n", tmp);
			}
			
		}
		else {//数组
			if (strcmpi(qtab[qidx].var1, "INT") == 0) {
				insertAlloctab(tmp, AT_INT, 0);
				fprintf(mipsOut, "%s:\t.space\t%d\t#array of int\n", tmp, 4 * atoi(qtab[qidx].var3));
			}
			else {//按字访问为了统一处理---------------
				insertAlloctab(tmp, AT_CHAR, 0);
				fprintf(mipsOut, "%s:\t.space\t%d\t#array of char\n", tmp, 4 * atoi(qtab[qidx].var3));
			}
			
		}
		qidx++;
	}
	//str处理
	for (i = 0; i < stx; i++) {
		sprintf(tmp, "__str%d", i);
		insertAlloctab(tmp, AT_STR, 20);//str_max为80所以为20个字
		fprintf(mipsOut, "%s:\t.ASCIIZ\t\"%s\"\n", tmp, stab[i]);
	}
	gp = vx;	//全局变量栈顶
	
	//函数表建立
	for (i = 1, j = 0; i <= btotal; i++){
		t = tab.btab[i];
		strcpy(functab[j].name, tab.symtab[t].name);
		functab[j].type = tab.symtab[t].type;
		functab[j].para = tab.symtab[t].para;
		j++;
		for (k = 0; k < tab.symtab[t].para; k++) {
			strcpy(functab[j].name, "");
			functab[j].type = tab.symtab[t+k+1].type;
			functab[j].para = k;
			j++;
		}
	}
	fx = j;	//函数表栈顶指针设置
	fprintf(mipsOut, "\n");
}

void mipsCode() {
	char tmp[MAX_IDEN];
	char tmp_var[MAX_TMP_VAR][MAX_IDEN];
	int  tmpx;	//tmpx为tmp_var表栈顶指针
	int len; //存储数组大小
	int contx;	//常量栈指针
	int paranum;	//函数参数个数
	int i, j;
	int lsp;

	fprintf(mipsOut, ".text\n");
	fprintf(mipsOut, "addi\t$sp,$sp,-40\n");
	fprintf(mipsOut, "add\t$fp,$sp,$0\n");	//先存储当前活动记录基地址到$fp
	fprintf(mipsOut, "j\tmain\n");
	fprintf(mipsOut, "\n");
	while (qtab[qidx].op == FUNC || qtab[qidx].op == MAINF) {
		if (qltab[qidx] != -1) {
			fprintf(mipsOut, "%s:\n", ltab[qltab[qidx]]);
		}

		if (qtab[qidx].op == MAINF) {
			main_flag = 1;
			fprintf(mipsOut, "main:\n");
		}
		else {
			main_flag = 0;
			fprintf(mipsOut, "%s:\n", qtab[qidx].var1);	//函数名作为函数起始标签
		}
			
		sp = 0;
		vx = gp;	//vx不能覆盖全局变量

		//非主函数参数空间预留
		if (!main_flag) {
			i = lookup(qtab[qidx].var1);
			paranum = tab.symtab[i].para;
			if (paranum) {
				sp -= 4 * paranum;
				fprintf(mipsOut, "sub\t$sp,$sp,%d\t#formal parameters alloc\n", (4 * paranum));
			}
		}

		qidx++;
		//局部const入alloc表
		if (qtab[qidx].op == CONST) { fprintf(mipsOut, "######\n"); }
		while (qtab[qidx].op == CONST) {
			//常量将地址存入栈中
			sprintf(tmp, "%%%d", vx - gp);
			if (!strcmpi(qtab[qidx].var1, "INT")) {
				lsp = insertAlloctab(tmp, AT_INT, 1);
			}
			else {
				lsp = insertAlloctab(tmp, AT_CHAR, 1);
			}
			//类型在输出的时候查看从而决定如何填写$v0寄存器
			fprintf(mipsOut, "ori\t$t0,$0,%d\n", atoi(qtab[qidx].var3));
			fprintf(mipsOut, "sw\t$t0,%d($sp)\n",lsp);
			qidx++;
		}
		contx = sp;

		if (sp != 0) { fprintf(mipsOut, "add\t$sp,$sp,%d\t#local const alloc\n\n", sp); }
		//局部var入alloc表
		while (qtab[qidx].op == VAR) {
			//声明只需要移动栈指针，不需要填写值
			sprintf(tmp, "%%%d", vx - gp);
			if (!strcmpi(qtab[qidx].var3, "")) {
				len = 1;
			}
			else {
				len = atoi(qtab[qidx].var3);
			}

			if (!strcmpi(qtab[qidx].var1, "INT")) {
				insertAlloctab(tmp, AT_INT, len);
			}
			else {
				insertAlloctab(tmp, AT_CHAR, len);
			}
			qidx++;
		}
		if (sp != contx) { fprintf(mipsOut, "add\t$sp,$sp,%d\t#local var alloc\n\n", sp - contx); }
	
		//局部临时变量处理
		tmpx = 0;	//tmp表指针清空
		for (i = qidx; qtab[i].op != EFUNC && qtab[i].op != EMAINF; i++) {
			if (qtab[i].var3[0] != '$')	//若不是临时变量
				continue;
			for (j = 0; j < tmpx; j++) {
				if (!strcmpi(qtab[i].var3, tmp_var[j]))	break;
			}

			if (j == tmpx) {
				//insertAlloctab(qtab[i].var3,)
				strcpy(tmp_var[tmpx], qtab[i].var3);
				tmpx++;
			}
		}
		if (tmpx != 0) { fprintf(mipsOut, "add\t$sp,$sp,%d\t#alloc memory for temp var\n", (-4 * tmpx)); }

		/*all the sentence*/
		while (qtab[qidx].op != EFUNC && qtab[qidx].op != EMAINF) {
			if (qltab[qidx] != -1) {
				fprintf(mipsOut, "%s:\n", ltab[qltab[qidx]]);
			}
			switch (qtab[qidx].op) {
			case ADD:case SUB:case MUL:case DIVV: {
				mipsComp();
				break;
			}
			case NEG: {
				mipsNeg();
				break;
			}
			case MOV: {
				mipsMov();
				break;
			}
			case JZ:case JL:case JLE:case JG:case JGE:case JE:case JNE: 
			{
				mipsBrch();
				break;
			}
			case RET: {
				mipsRet();
				break;
			}
			case WRITE: {
				mipsWrite();
				break;
			}
			case READ: {
				mipsRead();
				break;
			}
			case PARAV: {
				mipsParav();
				break;
			}
			case CALL: {
				mipsCall();
				break;
			}
			case JMP: {
				mipsJmp();
				break;
			}
			}
			qidx++;
		}
		//处理EFUNC或EMAINF处有标签的情况
		if (qltab[qidx] != -1) {
			fprintf(mipsOut, "%s:\n", ltab[qltab[qidx]]);
		}

		//处理EFUNC或EMAINF处无返回语句的情况
		if (main_flag) {
			fprintf(mipsOut, "li\t$v0,10\n");
			fprintf(mipsOut, "syscall\n");
		}
		else {
			fprintf(mipsOut, "jr\t$ra\n");
		}

		qidx++;
	}
}

//将数组元素地址取出存储到$t2中
void mipsArray(char *name, int offset, int ioffset, int itype) {
	int i, j;
	char aname[MAX_IDEN], iname[MAX_IDEN];

	for (i = 0, j = 0; name[i] != '['; i++, j++) {
		aname[j] = name[i];
	}
	aname[j] = '\0';

	if (offset == -1) {//全局数组
		//求出下标存$t2寄存器
		if (ioffset == -1) {//全局下标
			for (j = 0, i++; name[i] != ']'; i++, j++) {
				iname[j] = name[i];
			}
			iname[j] = '\0';
			//下标处理
			if (isdigit(iname[0])) {//立即数不能为负数
				fprintf(mipsOut, "ori\t$t1,$0,%d\n", atoi(iname));
			}
			else {//全局变常量
				fprintf(mipsOut, "lw\t$t1,_%s\n", iname);
			}
		}
		else {//局部下标
			fprintf(mipsOut, "lw\t$t1,%d($fp)\n", ioffset);
		}

		fprintf(mipsOut, "sll\t$t2,$t1,2\n");	//数组元素相对于数组起始地址的偏移量
		fprintf(mipsOut, "la\t$t2,_%s($t2)\n", aname);//全局的加_
	}
	else {//局部数组
		fprintf(mipsOut, "addi\t$t0,$fp,%d\n", offset);	//取出数组起始地址
		if (ioffset == -1) {//全局下标
			for (j = 0, i++; name[i] != ']'; i++, j++) {
				iname[j] = name[i];
			}
			iname[j] = '\0';
			//下标处理
			if (isdigit(iname[0])) {//立即数
				fprintf(mipsOut, "ori\t$t1,$0,%d\n", atoi(iname));
			}
			else {//全局变常量
				fprintf(mipsOut, "lw\t$t1,_%s\n", iname);
			}
		}
		else {//局部下标
			fprintf(mipsOut, "lw\t$t1,%d($fp)\n", ioffset);	//取出数组下标
		}

		fprintf(mipsOut, "#######array addr compute\n");
		fprintf(mipsOut, "sll\t$t2,$t1,2\n");
		fprintf(mipsOut, "add\t$t2,$t2,$t0\n");
		//fprintf(mipsOut, "lw\t$s0,($t2)\n");
	}
}

//itype下标类型0:para,1:var;ioffset下标地址
int lkupArrayAddr(char *name, int * ioffset, int *itype) {
	int i, j;
	int offset;
	char buf[MAX_IDEN];
	for (i = 0, j = 0; name[i] != '\0' && name[i] != '['; i++, j++) {
		buf[j] = name[i];
	}
	if (name[i] == '\0') {//不是数组
		*ioffset = -1;
		*itype = -1;
		return -2;
	}

	buf[j] = '\0';
	offset = lkupAddr(buf);
	for (j = 0, i++; name[i] != ']'; i++, j++) {
		buf[j] = name[i];
	}
	buf[j] = '\0';
	*ioffset = lkupAddr(buf);	//可能是立即数,ioffset为-1
								//if(ioffset)
	if (buf[0] == '@') { *itype = 0; }
	else { *itype = 1; }

	return offset;
}

//寻找函数参数或简单变量相对于栈帧$fp的偏移(数组带[])
int lkupAddr(char *name) {
	int i, j;
	int addr = 0;
	//若为函数参数
	if (name[0] == '@') {
		j = atoi(&name[1]);
		for (i = 0; i < j; i++) {
			addr -= 4;//fp向下分配空间
		}
		return addr;
	}
	for (i = gp; i < vx; i++) {
		if (!strcmpi(alloctab[i].name, name)) {
			return alloctab[i].offset;
		}
	}
	return -1;
}

/*
sRegister s0-s7
------
lastfp
-------
ret addres
-------
para1        fp
-------
paraX
...
-------
sp
-------
*/

void mipsRet() {
	int offset;

	if (main_flag) {
		fprintf(mipsOut, "li\t$v0,10\n");
		fprintf(mipsOut, "syscall\n");
	}
	else {
		if (strcmpi(qtab[qidx].var1, "")) {//var1不为空
			offset = lkupAddr(qtab[qidx].var1);
			if (offset == -1) {
				if (qtab[qidx].var1[0] == '-' || isdigit(qtab[qidx].var1[0])) {//立即数
					fprintf(mipsOut, "ori\t$s0,$0,%d\n", atoi(qtab[qidx].var1));
				}
				else {//全局变常量
					fprintf(mipsOut, "lw\t$s0,_%s\n", qtab[qidx].var1);
				}
			}
			else {//函数参数或变量
				fprintf(mipsOut, "lw\t$s0,%d($fp)\n", offset);	//从内存取出实参	
			}
			fprintf(mipsOut, "add\t$v0,$s0,$0\n");
		}
		/*else {
			fprintf(mipsOut, "li\t$v0,0\n");
		}*/
		fprintf(mipsOut, "jr\t$ra\n");
	}
}

void mipsParav() {
	int vtx = 0;	//存储参数个数
	int offset, ioffset, itype;
	//下面一定是call语句,但由于需要存储函数参数，故先保存现场
	//保存现场
	fprintf(mipsOut, "#######save the spot\n");
	fprintf(mipsOut, "addi\t$sp,$sp,-40\n");
	fprintf(mipsOut, "sw\t$ra,4($sp)\n");
	fprintf(mipsOut, "sw\t$fp,8($sp)\n");	//栈帧保存
	//save regs
	fprintf(mipsOut, "sw\t$s7,12($sp)\n");
	fprintf(mipsOut, "sw\t$s6,16($sp)\n");
	fprintf(mipsOut, "sw\t$s5,20($sp)\n");
	fprintf(mipsOut, "sw\t$s4,24($sp)\n");
	fprintf(mipsOut, "sw\t$s3,28($sp)\n");
	fprintf(mipsOut, "sw\t$s2,32($sp)\n");
	fprintf(mipsOut, "sw\t$s1,36($sp)\n");
	fprintf(mipsOut, "sw\t$s0,40($sp)\n");
	
	saved = 1;		//表示已经保存过现场

	//函数参数栈空间申请
	fprintf(mipsOut, "#######real parameter pass\n");
	while (qtab[qidx].op == PARAV) {
		//---------需要分配寄存器算法-----------------//
		offset = lkupAddr(qtab[qidx].var1);
		if (offset == -1) {//考虑全局变常量&立即数
			//offset = lkupArrayAddr(qtab[qidx].var1, &ioffset, &itype);
			//if (offset == -2) {//全局变常量
				if (qtab[qidx].var1[0] == '-' || isdigit(qtab[qidx].var1[0])) {//立即数
					fprintf(mipsOut, "ori\t$s0,$0,%d\n", atoi(qtab[qidx].var1));
					//fprintf(mipsOut, "sw\t$s0,%d($sp)\n", (-4 * vtx));	//将实参填入形参相应位置
					}
				else {//全局变常量
					fprintf(mipsOut, "lw\t$s0,_%s\n", qtab[qidx].var1);
					//fprintf(mipsOut, "sw\t$s0,%d($sp)\n", (-4 * vtx));	//将实参填入形参相应位置
				}
			//}
			//else {//数组(全局或局部)
			//	mipsArray(qtab[qidx].var1, offset, ioffset, itype);
			//	//fprintf(mipsOut, "sw\t$s0,%d($sp)\n", (-4 * vtx));
			//}
		}
		else {//函数参数或变量
			fprintf(mipsOut, "lw\t$s0,%d($fp)\n", offset);	//从内存取出实参
			//fprintf(mipsOut, "sw\t$s0,%d($sp)\n", (-4 * vtx));	//将实参填入形参相应位置
		}

		fprintf(mipsOut, "sw\t$s0,%d($sp)\n", (-4 * vtx));	//将实参填入形参相应位置
		vtx++;
		qidx++;
	}
	qidx--;
	fprintf(mipsOut, "\n");
	fprintf(mipsOut, "\n");

	fprintf(mipsOut, "add\t$fp,$sp,$0\n");	//进入新的栈帧
	fprintf(mipsOut, "addi\t$sp,$sp,%d\n", (-4*vtx));
}

void mipsCall() {
	if (!saved) {
		//保存现场
		fprintf(mipsOut, "#######save the spot\n");
		fprintf(mipsOut, "addi\t$sp,$sp,-40\n");
		fprintf(mipsOut, "sw\t$ra,4($sp)\n");
		fprintf(mipsOut, "sw\t$fp,8($sp)\n");	//栈帧保存
		//save regs
		fprintf(mipsOut, "sw\t$s7,12($sp)\n");
		fprintf(mipsOut, "sw\t$s6,16($sp)\n");
		fprintf(mipsOut, "sw\t$s5,20($sp)\n");
		fprintf(mipsOut, "sw\t$s4,24($sp)\n");
		fprintf(mipsOut, "sw\t$s3,28($sp)\n");
		fprintf(mipsOut, "sw\t$s2,32($sp)\n");
		fprintf(mipsOut, "sw\t$s1,36($sp)\n");
		fprintf(mipsOut, "sw\t$s0,40($sp)\n");
		//新栈帧建立
		fprintf(mipsOut, "add\t$fp,$sp,$0\n");
	}

	fprintf(mipsOut, "jal\t%s\n", qtab[qidx].var1);
	fprintf(mipsOut, "nop\n");

	//设置当前函数指针
	for (fidx = 0; fidx < fx; fidx++) {
		if (!strcmpi(qtab[qidx].var1, functab[fidx].name)) { break; }
	}

	//恢复现场
	//restore $ra, $fp, $sp
	fprintf(mipsOut, "#######restore the spot\n");
	fprintf(mipsOut, "addi\t$sp,$fp,40\n");
	fprintf(mipsOut, "lw\t$ra,-36($sp)\n");
	fprintf(mipsOut, "lw\t$fp,-32($sp)\n");
	//restore save regs
	fprintf(mipsOut, "lw\t$s7,-28($sp)\n");
	fprintf(mipsOut, "lw\t$s6,-24($sp)\n");
	fprintf(mipsOut, "lw\t$s5,-20($sp)\n");
	fprintf(mipsOut, "lw\t$s4,-16($sp)\n");
	fprintf(mipsOut, "lw\t$s3,-12($sp)\n");
	fprintf(mipsOut, "lw\t$s2,-8($sp)\n");
	fprintf(mipsOut, "lw\t$s1,-4($sp)\n");
	fprintf(mipsOut, "lw\t$s0,0($sp)\n");

	saved = 0;
}

void mipsJmp() {
	fprintf(mipsOut, "j\t%s\n", qtab[qidx].var3);
}

void mipsComp() {
	int offset1, offset2, offset3;
	int type1, type2, type3;
	char tmp1[MAX_IDEN], tmp2[MAX_IDEN], tmp3[MAX_IDEN];

	strcpy(tmp1, qtab[qidx].var1);
	strcpy(tmp2, qtab[qidx].var2);
	strcpy(tmp3, qtab[qidx].var3);
	offset1 = lkupAddr(tmp1);
	offset2 = lkupAddr(tmp2);
	type1 = lkupType(tmp1);
	type2 = lkupType(tmp2);

	//临时变量入表
	if (tmp3[0] == '$') {
		type3 = type1 == type2 ? type1 : AT_INT;
		insertAlloctab(tmp3, type3, 1);
	}
	offset3 = lkupAddr(tmp3);

	//取出操作数1
	if (offset1 == -1) {//全局的
		if (tmp1[0] == '-' || isdigit(tmp1[0])) {//立即数
			fprintf(mipsOut, "ori\t$t1,$0,%d\n", atoi(tmp1));
		}
		else {//全局变量或常量(无数组)
			fprintf(mipsOut, "lw\t$t1,_%s\n", tmp1);
		}
	}
	else {//局部的
		fprintf(mipsOut, "lw\t$t1,%d($fp)\n", offset1);
	}
	//取出操作数2
	if (offset2 == -1) {
		if (tmp2[0] == '-' || isdigit(tmp2[0])) {
			fprintf(mipsOut, "ori\t$t2,$0,%d\n", atoi(tmp2));
		}
		else {
			fprintf(mipsOut, "lw\t$t2,_%s\n", tmp2);
		}
	}
	else {
		fprintf(mipsOut, "lw\t$t2,%d($fp)\n", offset2);
	}

	//计算结果
	switch (qtab[qidx].op) {
	case ADD: {
		fprintf(mipsOut, "add\t$t3,$t1,$t2\n");	//add
		break;
	}
	case SUB: {
		fprintf(mipsOut, "sub\t$t3,$t1,$t2\n");	//sub
		break;
	}
	case MUL: {
		fprintf(mipsOut, "mul\t$t3,$t1,$t2\n");	//mul
		break;
	}
	case DIVV: {
		fprintf(mipsOut, "div\t$t1,$t2\n");	//div
		fprintf(mipsOut, "mflo\t$t3\n");
		break;
	}
	}

	//存储结果
	if (offset3 == -1) {//全局,不可能是立即数,只可能是变量
		fprintf(mipsOut, "sw\t$t3,_%s\n", tmp3);
	}
	else {//参数或局部变量
		fprintf(mipsOut, "sw\t$t3,%d($fp)\n", offset3);
	}
}

void mipsNeg(){
	int offset1, offset3;
	int type1;

	offset1 = lkupAddr(qtab[qidx].var1);
	type1 = lkupType(qtab[qidx].var1);

	if (qtab[qidx].var3[0] == '$') {
		insertAlloctab(qtab[qidx].var3, type1, 1);
	}
	offset3 = lkupAddr(qtab[qidx].var3);

	//取出操作数
	if (offset1 == -1) {
		if (qtab[qidx].var1[0] == '-' || isdigit(qtab[qidx].var1[0])) {
			fprintf(mipsOut, "ori\t$t1,$0,%d\n", atoi(qtab[qidx].var1));
		}
		else {
			fprintf(mipsOut, "lw\t$t1,_%s\n", qtab[qidx].var1);
		}
	}
	else {
		fprintf(mipsOut, "lw\t$t1,%d($fp)\n", offset1);
	}
	
	//计算结果
	fprintf(mipsOut, "sub\t$t3,$0,$t1\n");

	//存储结果
	if (offset3 == -1) {
		fprintf(mipsOut, "sw\t$t3,_%s\n", qtab[qidx].var3);
	}
	else {
		fprintf(mipsOut, "sw\t$t3,%d($fp)\n", offset3);
	}
}

void mipsMov() {
	int offset1, offset3;
	int type1;
	int  ioffset, itype;
	char tmp1[MAX_IDEN], tmp3[MAX_IDEN];

	strcpy(tmp1, qtab[qidx].var1);
	strcpy(tmp3, qtab[qidx].var3);
	offset1 = lkupAddr(tmp1);

	if (tmp1[0] == '~') {//操作数为函数返回值时需要访问上一条函数调用语句来获取类型
		type1 = lkupType(qtab[qidx - 1].var1);
	}
	else {
		type1 = lkupType(tmp1);
	}

	if (tmp3[0] == '$') {
		insertAlloctab(tmp3, type1, 1);
	}
	offset3 = lkupAddr(tmp3);

	//取出操作数
	if (tmp1[0] == '~') {//操作数为函数返回值的情况
		fprintf(mipsOut, "move\t$s0,$v0\n");
	}
	else if (offset1 == -1) {//全局&数组&立即数
		offset1 = lkupArrayAddr(tmp1, &ioffset, &itype);
		if (offset1 == -2) {//全局&立即数
			if (tmp1[0] == '-' || isdigit(tmp1[0])) {
				fprintf(mipsOut, "ori\t$s0,$0,%d\n", atoi(tmp1));
			}
			else {//全局的
				fprintf(mipsOut, "lw\t$s0,_%s\n", tmp1);
			}
		}
		else {//数组元素取出
			mipsArray(tmp1, offset1, ioffset, itype);
			fprintf(mipsOut, "lw\t$s0,($t2)\n");
		}
	}
	else {//局部简单变量& 参数
		fprintf(mipsOut, "lw\t$s0,%d($fp)\n", offset1);
	}

	//存储结果
	if (offset3 == -1) {//全局变量
		offset3 = lkupArrayAddr(tmp3, &ioffset, &itype);
		if (offset3 == -2) {//全局变量赋值
			fprintf(mipsOut, "sw\t$s0,_%s\n", tmp3);
		}
		else {//数组元素赋值
			mipsArray(tmp3, offset3, ioffset, itype);
			fprintf(mipsOut, "sw\t$s0,($t2)\n");
		}
	}
	else {//局部简单变量&参数
		fprintf(mipsOut, "sw\t$s0,%d($fp)\n", offset3);
	}
}

//未检查
void mipsBrch() {
	int offset1, offset2;
	char tmp1[MAX_IDEN], tmp2[MAX_IDEN], label[MAX_IDEN];

	strcpy(tmp1, qtab[qidx].var1);
	strcpy(label, qtab[qidx].var3);
	offset1 = lkupAddr(tmp1);
	
	//取出操作数1
	if (offset1 == -1) {//全局的
		if (tmp1[0] == '-' || isdigit(tmp1[0])) {//立即数
			fprintf(mipsOut, "ori\t$t1,$0,%d\n", atoi(tmp1));
		}
		else {//全局变量或常量(无数组)
			fprintf(mipsOut, "lw\t$t1,_%s\n", tmp1);
		}
	}
	else {//局部的
		fprintf(mipsOut, "lw\t$t1,%d($fp)\n", offset1);
	}
	//取出操作数2
	if (qtab[qidx].op != JZ) {//JZ没有var2
		strcpy(tmp2, qtab[qidx].var2);
		offset2 = lkupAddr(tmp2);
		if (offset2 == -1) {
			if (tmp2[0] == '-' || isdigit(tmp2[0])) {
				fprintf(mipsOut, "ori\t$t2,$0,%d\n", atoi(tmp2));
			}
			else {
				fprintf(mipsOut, "lw\t,$t2,_%s\n", tmp2);
			}
		}
		else {
			fprintf(mipsOut, "lw\t$t2,%d($fp)\n", offset2);
		}
	}

	switch (qtab[qidx].op) {
	case JZ: {
		fprintf(mipsOut, "beq\t$t1,$0,%s\n", label);
		break;
	}
	case JLE: {
		fprintf(mipsOut, "ble\t$t1,$t2,%s\n", label);
		break;
	}
	case JL: {
		fprintf(mipsOut, "blt\t$t1,$t2,%s\n", label);
		break;
	}
	case JGE: {
		fprintf(mipsOut, "bge\t$t1,$t2,%s\n", label);
		break;
	}
	case JG: {
		fprintf(mipsOut, "bgt\t$t1,$t2,%s\n", label);
		break;
	}
	case JE: {
		fprintf(mipsOut, "beq\t$t1,$t2,%s\n", label);
		break;
	}
	case JNE:{
		fprintf(mipsOut, "bne\t$t1,$t2,%s\n", label);
		break;
	}
	default: {
		printf("error operation in mipsBrch.\n");
		break;
	}
	}

}

void mipsWrite() {
	int type, offset;
	if (strcmpi(qtab[qidx].var1, "")) {//若有str，则先打印str
		fprintf(mipsOut, "li\t$v0,4\n");
		fprintf(mipsOut, "la\t$a0,_%s\n", qtab[qidx].var1);
		fprintf(mipsOut, "syscall\n");
	}
	if (strcmpi(qtab[qidx].var2, "")) {//WRITE,[str],expr,""
		offset = lkupAddr(qtab[qidx].var2);
		if (offset == -1) {
			if (qtab[qidx].var2[0] == '-' || isdigit(qtab[qidx].var2[0])) {//立即数
				fprintf(mipsOut, "ori\t$a0,$0,%d\n", atoi(qtab[qidx].var2));
			}
			else {//全局变量或常量（不能为数组）
				fprintf(mipsOut, "lw\t$a0,_%s\n", qtab[qidx].var2);
			}
		}
		else {
			fprintf(mipsOut, "lw\t$a0,%d($fp)\n", offset);
		}

		type = lkupType(qtab[qidx].var2);
		if (type == AT_INT) {
			fprintf(mipsOut, "li\t$v0,1\n");
		}
		else if(type == AT_CHAR) {
			fprintf(mipsOut, "li\t$v0,11\n");
		}
		else {///////////////未处理
			printf("%d\t%10s\t%10s\t%10s\t%10s\n",qidx, qt_op_name[qtab[qidx].op], qtab[qidx].var1, qtab[qidx].var2, qtab[qidx].var3);
			printf("type %d is wrong in write.\n", type);
		}
		fprintf(mipsOut, "syscall\n");
	}
}

//READ,"","",标识符(没有数组)
void mipsRead() {
	int offset, type;

	type = lkupType(qtab[qidx].var3);
	if (type == AT_INT) {
		fprintf(mipsOut, "li\t$v0,5\n");
		fprintf(mipsOut, "syscall\n");
	}
	else if (type == AT_CHAR) {
		fprintf(mipsOut, "li\t$v0,12\n");
		fprintf(mipsOut, "syscall\n");
	}
	else {
		printf("%d\t%10s\t%10s\t%10s\t%10s\n", qidx,qt_op_name[qtab[qidx].op], qtab[qidx].var1, qtab[qidx].var2, qtab[qidx].var3);
		printf("type %d is wrong in read.\n", type);
	}

	//从$t0寄存器取出读入数据
	fprintf(mipsOut, "add\t$s0,$0,$v0\n");
	offset = lkupAddr(qtab[qidx].var3);
	if (offset == -1) {//若为全局的
		//全局变量存储
		fprintf(mipsOut, "sw\t$s0,_%s\n", qtab[qidx].var3);
	}
	else {//局部变量存储
		fprintf(mipsOut, "sw\t$s0,%d($fp)\n", offset);
	}
}