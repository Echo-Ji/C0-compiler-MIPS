#pragma warning(disable:4996)
#include "global.h"
#include "lex.h"
#include "GSAnlz.h"
#include "opt.h"
#include "tomips.h"

void printtab() {
	int i, j, k;
	k = 0;
	for (i = 0; i < qtx; i++) {
		Qt entry = qtab[i];
		if (qtab[i].label != -1) {
			//fprintf(tabout, "%s:", ltab[qltab[i]]);
			fprintf(tabout, "%s:", ltab[entry.label]);
		}
		if (entry.op == ADD || entry.op == SUB ||
			entry.op == MUL || entry.op == DIVV) {
			fprintf(tabout, "%10s=\t%10s\t%10s\t%10s\n", entry.var3, entry.var1, qt_op_name[entry.op], entry.var2);
		}
		else if (entry.op == MOV) {
			fprintf(tabout, "%10s=\t%10s\n", entry.var3, entry.var1);
		}
		else {
			fprintf(tabout, "%10s\t%10s\t%10s\t%10s\n", qt_op_name[entry.op], entry.var1, entry.var2, entry.var3);
		}
		/*if (entry.var1[0] == '$' || entry.var2[0] == '$' ||
			entry.var3[0] == '$') { printf("%10s\t%10s\t%10s\t%10s\n", qt_op_name[entry.op], entry.var1, entry.var2, entry.var3); }*/
		/*if (entry.var1[0] == '$') { k++; }
		if (entry.var2[0] == '$') { k++; }
		if (entry.var3[0] == '$') { k++; }*/
	}
	//printf("all the tmp var:%d\t%d\n", k, tmpx*2);
}

int main() {
	char filename[MAX_FILE_NAME];
	int i;
	scanf("%s",filename);
	//strcpy(filename, "C:\\Users\\JJH\\Desktop\\in.txt");
	if ((in = fopen(filename, "rt")) == NULL) {
		perror("input file open error.\n");
		return 1;
	}
	if ((err = fopen(".\\err.txt", "wt+")) == NULL) {
		printf("err open error\n");
	}
	if ((out = fopen(".\\out.txt", "wt+")) == NULL) {
		printf("out open error\n");
	}
	if ((tabout = fopen(".\\stab.txt", "wt+")) == NULL) {
		printf("stab open error\n");
	}
	if ((mipsOut = fopen(".\\mipsOut.txt", "wt+")) == NULL) {
		printf("mipsOut open error\n");
	}

	//四元式对应的label表的初始化
	for (i = 0; i < MAX_QTAB_ENTRY; i++) {
		qltab[i] = -1;
	}

	/*int a[] = { 1,2,3,4,5 }, b[] = { 7,8,9,10,11 };
	int k = 1, j = 1;
	a[k++] = b[j++];
	for (i = 0; i < 5; i++) {
		printf("a[%d]:%d\vb[%d]:%d\t", i, a[i], i, b[i]);
	}
	printf("\n\nk:%d\tj:%d\n", k, j);*/
	getsym();
	program();
	printtab();
	optimize();
	fprintf(tabout, "AFTER OPTIMIZED:\n");
	printtab();
	//regDis();
	if (!error_flag) { 
		toMips(); 
		//printf("目标代码请查看mipsOut.txt");
	}
	fclose(in);
	fclose(out);
	fclose(err);
	fclose(tabout);
	return 0;
}



