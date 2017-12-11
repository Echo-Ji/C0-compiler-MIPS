#pragma warning(disable:4996)
#include "global.h"
#include "lex.h"
#include "GSAnlz.h"
#include "tomips.h"

void printtab() {
	int i, j ;
	/*for (i = 0; i < tx; i++) {
		symentry entry = tab.symtab[i];
		fprintf(stab, "%s\t%d\t%d\t%d\t%d\t%d\t%d\n", entry.name,
			entry.kind, entry.type, entry.value, entry.sz, entry.address, entry.para);
	}*/
	for (i = 0; i < qtx; i++) {
		Qt entry = qtab[i];
		if (qltab[i] != -1) {
			fprintf(tabout, "%s:\n", ltab[qltab[i]]);
		}
		fprintf(tabout, "%10s\t%10s\t%10s\t%10s\n", qt_op_name[entry.op], entry.var1, entry.var2, entry.var3);
	}
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

	for (i = 0; i < MAX_QTAB_ENTRY; i++) {
		qltab[i] = -1;
	}

	getsym();
	program();
	printtab();
	toMips();
	/*output the table
	int i = 0;
	while (!end_flag) {
	getsym();
	fprintf(out, "%d\t", ++i);
	switch (sym) {
	case IDEN: {
	fprintf(out, "%s\t\t%s\n", word_out[sym], iden);
	break;
	}
	case 1: {
	fprintf(out, "%s\t\t%d\n", word_out[sym], num);
	break;
	}
	case 2: {
	fprintf(out, "%s\t\t%c\n", word_out[sym], chcon);
	break;
	}
	case 3: {
	fprintf(out, "%s\t\t%s\n", word_out[sym], strcon);
	break;
	}
	case 4:case 5:case 6:case 7: case 8: case 9:case 10:
	case 11:case 12:case 13:case 14:case 15:case 16:case 17:{
	fprintf(out, "%s\t\t%s\n", word_out[sym], rword[sym - 4]);
	break;
	}
	case 18:case 19:case 20:case 21:case 22:case 23:
	case 24:case 25:case 26:case 27: {
	fprintf(out, "%s\t\t%s\n", word_out[sym], rop[sym - 18]);
	break;
	}
	case 28:case 29:case 30:case 31:case 32:case 33:
	case 34:case 35:case 36:case 37:case 38:case 39:{
	fprintf(out, "%s\t\t%s\n", word_out[sym], rpt[sym - 28]);
	break;
	}
	}
	}*/

	//getsym();
	//program();
	fclose(in);
	fclose(out);
	fclose(err);
	fclose(tabout);
	return 0;
}



