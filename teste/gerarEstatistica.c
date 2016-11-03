#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
int main(int args, char *argv[]){

	FILE *f;
	int pos1 = 0, pos2 = 0,i = 0, quantInstancias = 0;
	char arquivoEntrada[20], grupo[10], c[10], arquivoGerar[20];

	grupo[0] = '\0';
	c[0] = '\0';
	arquivoGerar[0] = '\0';

	char instancia[10];
	int calculoCusto,calculoCustoSemPenalidades,somaPenalidades,somaQuantPenalidades,menor,maior,diferenca;
	float time;

	int 	somaCalculoCusto = 0 ,
		somaCalculoCustoSemPenalidades = 0 ,
		sSomaPenalidades = 0 ,
		sSomaQuantPenalidades = 0 ,
		somaMenor = 0 ,
		somaMaior = 0 ,
		somaDiferenca = 0 ;

	float somaTime = 0.0;


	strcpy(arquivoEntrada,argv[1]);

	f = fopen(arquivoEntrada,"r");

	while(fscanf(f,"%s %d %d %d %d %f %d %d %d",instancia,&calculoCusto,&calculoCustoSemPenalidades,&somaPenalidades,&somaQuantPenalidades,&time,&menor,&maior,&diferenca) == 9){
		quantInstancias += 1;
		
		somaCalculoCusto += calculoCusto ;
                somaCalculoCustoSemPenalidades += calculoCustoSemPenalidades ;
                sSomaPenalidades += somaPenalidades ;
                sSomaQuantPenalidades += somaQuantPenalidades ;
                somaMenor += menor ;
                somaMaior += maior ;
                somaDiferenca += diferenca ;
		somaTime += time ;
	}
	
	fclose(f);

	i = 0;
        while(arquivoEntrada[i] != '\0'){
                if(arquivoEntrada[i] == '/'){
                        pos1 = i;
                }
                if(arquivoEntrada[i] == '.'){
                        pos2 = i;
                }
                i++;
        }

	printf("%s\n",arquivoEntrada);
	strncat(grupo, arquivoEntrada, pos1);
	strncat(c    , arquivoEntrada + pos1 + 1, pos2 - pos1 - 1);

	printf("%d\n%d\n",pos1,pos2);
	printf("%s\n",grupo);
	printf("%s\n",c);

	strcat(arquivoGerar,grupo);
	strcat(arquivoGerar,"/");
	strcat(arquivoGerar,c);
	strcat(arquivoGerar,"statistics.txt");

	printf("%s\n",arquivoGerar);

	f = fopen(arquivoGerar,"w");

	fprintf(f,"%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",(somaCalculoCusto * 1.0) / quantInstancias,
		(somaCalculoCustoSemPenalidades * 1.0) / quantInstancias,
		(sSomaPenalidades * 1.0) / quantInstancias,
                (sSomaQuantPenalidades * 1.0) / quantInstancias,
		(somaTime * 1.0) / quantInstancias,
                (somaMenor * 1.0) / quantInstancias,
		(somaMaior * 1.0) / quantInstancias,
                (somaDiferenca * 1.0) / quantInstancias);

	fclose(f);

	return 0;
}
