#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//////////////////////////////////////////////////////////////
/*
Passos a serem seguidos para completar o projeto:

1 - Implementar o que foi projetado
2 - Testar os parametros empiricamente


Passo 1:
	-Modulos do Projeto:
		- Modulo de input
			- gera:
				- Instancia NSP
					- formado por:
						- Matriz de Preferencia
						- Cobertura
		AvaliadorDeSolucoes(Instancia NSP)
			- possui:
				- Modulo Solucao Inicial(Instancia NSP)
					- possui:
						- Modulo BAP
					- retorna:
						-SolucaoPossivel
				- Gerador de Estrutura de Vizinhanca(SolucaoPossivel)
					- possui:
						- Modulo BAP
					- contem:
						- CRP
						- K-Swap
					- retorna:
						-SolucaoPossivel
				- Modulo Buscador de Solucao
					-injeta:
						- Gerador de Estrutura de Vizinhanca
					-contem:
						- VND
						- Perturbacao
					-retorna: 
						- SolucaoPossivel
				- ILS(Instancia NSP)
					- injeta:
						- Modulo Solucao Inicial
						- Modulo Buscador Solucao
					- retorna:
						- melhor solucao do ILS
			- injetados:
				- Modulo de Restricoes
					- Restricoes Obrigatorias
					- Restricoes Desejaveis
			- retorna:
				melhor solucao
		Modulo de output(AvaliadorDeSolucoes):
			- gera:
				- saida formatada	

-- precisa de revisao e melhoria

*/
//////////////////////////////////////////////////////////////


///////////////////////////////////
/* TESTE

Existem 7290 arquivos para cada caso diverso N25,N50,N75,N100
Com Cases numerados de 1-8, gerando 8 instancias para cada arquivo

Existem 960 arquivos para cada caso realista N30,N60
Com Cases numerados de 9-16, gerando 8 instancias para cada arquivo


Como combinar cada Case com cada arquivo para gerar uma instancia?
*/
///////////////////////////////////

enum TURNO { DIA, TARDE, NOITE, FOLGA };

typedef struct {
	enum TURNO **solucao;
} estruturaSolucao;

typedef struct {
        enum TURNO *linha;
} linhaMatriz;


void moduloInput(char *arquivoEntrada, char *Case);
//estruturaSolucao* ILS();
//void GeracaoSaidaFormatada(estruturaSolucao *melhorSolucao,char *arquivoEntrada, char *Case);

/////////////////////////////////////////////////////////////////
// Variaveis Globais

// Variaveis que representam a instancia
// Retirado de cada arquivo .nsp de cada caso


int Nurses,	// quantidade de enfermeiras
    Days,	// quantidade de dias do periodo
    Shifts;	// quantidade de turnos por dia

int **matrizCobertura; // Matriz bidimensional que representa a quantidade minima de enfermeiras para certo dia d e turno t

int ***matrizPreferencia; // Matriz tridimensional que representa o custo de preferencia quando atribuido trabalho para enfermeira n
			  // no dia d no turno t

// Variaveis que representam as restricoes do problema
// Retirado de cada arquivo .gen de cada caso

int minAtribuicoes,				// minimo de atribuicoes, ou seja, minimo de dias trabalhados
    maxAtribuicoes,				// maximo de atribuicoes, ou seja, maximo de dias trabalhados
    minTurnosTrabalhadosConsecutivos,		// minimo de turnos consecutivos, minimo de dias de trabalho consecutivo 
    maxTurnosTrabalhadosConsecutivos;		// maximo de turnos consecutivos, maximo de dias de trabalho consecutivo


int *minMesmoTurnoTrabalhadoConsecutivos,	// minimo de turnos iguais consecutivos, minimo de dias trabalhados no mesmo turno consecutivos
    *maxMesmoTurnoTrabalhadoConsecutivos,	// maximo de turnos iguais consecutivos, minimo de dias trabalhados no mesmo turno consecutivos
    *minNumeroAtribuicoesTurno,			// minimo de atribuicoes por turno
    *maxNumeroAtribuicoesTurno;			// maximo de atribuicoes por turno

// Parametros utilizados para os buscadores e ILS



/////////////////////////////////////////////////////////////////



int main(int argc, char *argv[]){
	
	estruturaSolucao *melhorSolucao;
	
	// Leitura de todos os dados e construcao de todas as estruturas usadas no problema
	moduloInput(argv[1],argv[2]);

	//
	//melhorSolucao = ILS();

	// Geracao de arquivos ou saida formatada para analise de dados gerados
	//GeracaoSaidaFormatada(melhorSolucao,argv[1],argv[2]);
	return 0;
}

void moduloInput(char *arquivoEntrada, char *Case){
	
	int i,j,k;
	FILE *arquivoInstancia,*arquivoRestricoes;

	// Contrucao da instancia
	///////////////////////////////////////////////////////////


	arquivoInstancia  = fopen(arquivoEntrada,"r");
	
	// Leitura parametros da instancia
	fscanf(arquivoInstancia,"%d %d %d",&Nurses, &Days, &Shifts);
	
	// Construcao Matriz Cobertura
	matrizCobertura = (int **)malloc(sizeof(int *) * Days);
	
	for(j = 0; j < Days ; j += 1){
		matrizCobertura[j] = (int *)malloc(sizeof(int) * Shifts);
	}


	// Leitura Matriz Cobertura
	for(j = 0; j < Days; j += 1){
		for(k = 0; k < Shifts; k += 1){
			fscanf(arquivoInstancia,"%d",&matrizCobertura[j][k]);
		}
	}

	

	// Construcao Matriz de Preferencia
	matrizPreferencia = (int ***)malloc(sizeof(int **) * Nurses);

	for( i = 0; i < Nurses ; i += 1){
		matrizPreferencia[i] = (int **)malloc(sizeof(int *) * Days);
		for( j = 0; j < Days; j += 1){
			matrizPreferencia[i][j] = (int *)malloc(sizeof(int) * Shifts);
		}
	}

	// Leitura Matriz de Preferencia
	for(i = 0 ; i < Nurses ; i += 1){
		for(j = 0; j < Days ; j += 1){
			for(k = 0; k < Shifts; k += 1){
				fscanf(arquivoInstancia,"%d",&matrizPreferencia[i][j][k]);
			}
		}
	}
	
	fclose(arquivoInstancia);
	// Fim Construcao Instancia
	////////////////////////////////////////////////////////////
	

	// Inicio Contrucao Restricoes
	////////////////////////////////////////////////////////////
	arquivoRestricoes = fopen(Case,"r");

	// Leitura restricoes por enfermeira
	fscanf(arquivoRestricoes,"%d %d",&Days,&Shifts);
	fscanf(arquivoRestricoes,"%d %d",&minAtribuicoes,&maxAtribuicoes);
	fscanf(arquivoRestricoes,"%d %d",&minTurnosTrabalhadosConsecutivos,&maxTurnosTrabalhadosConsecutivos);

	// Construcao das restricoes
	minMesmoTurnoTrabalhadoConsecutivos = (int *)malloc(sizeof(int) * Shifts);
	maxMesmoTurnoTrabalhadoConsecutivos = (int *)malloc(sizeof(int) * Shifts);
	minNumeroAtribuicoesTurno 	    = (int *)malloc(sizeof(int) * Shifts);
	maxNumeroAtribuicoesTurno 	    = (int *)malloc(sizeof(int) * Shifts);

	// Leitura das Restricoes por turno
	for( k = 0; k < Shifts; k += 1){
		fscanf(arquivoRestricoes,"%d",&minMesmoTurnoTrabalhadoConsecutivos[k]);
		fscanf(arquivoRestricoes,"%d",&maxMesmoTurnoTrabalhadoConsecutivos[k]);
		fscanf(arquivoRestricoes,"%d",&minNumeroAtribuicoesTurno[k]);
		fscanf(arquivoRestricoes,"%d",&maxNumeroAtribuicoesTurno[k]);
	}

	fclose(arquivoRestricoes);
	// Fim Construcao Restricoes
	////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	// Printar todas as entradas para verificar se nao ocorreu erro de alocacao de memoria

	//print do arquivo de instancia
	
	//print nome dos arquivos
	printf("arquivo: %s\nCase: %s\n\n\n",arquivoEntrada,Case);

	//print parametros da instancia
	printf("Nurses: %d\nDays: %d\nShifts: %d\n\n\n",Nurses,Days,Shifts);


	//print matriz de cobertura
	printf("Matriz de cobertura\n\n");	
	for(j = 0; j < Days; j += 1){
		printf("Dia %d: \n\t",j+1);
                for(k = 0; k < Shifts; k += 1){
                        printf("Shift %d: %d ", k+1, matrizCobertura[j][k]);
                }
		printf("\n");
        }
	printf("\n\n\n");

	//print matriz de preferencia
	printf("Matriz de preferencia\n\n");
	for(i = 0 ; i < Nurses ; i += 1){
		printf("Nurse %d: \n", i+1);
                for(j = 0; j < Days ; j += 1){
			printf("\tDay %d: \n\t\t", j+1);
                        for(k = 0; k < Shifts; k += 1){
                                printf("Shift %d: %d ", k+1, matrizPreferencia[i][j][k]);
                        }
			printf("\n");
                }
        }
	printf("\n\n\n");

	//print do case

	//parametros do case
	printf("Dias: %d\nTurnos: %d\n", Days, Shifts);
        printf("Minimo de atribuicoes: %d\nMaximo de atribuicoes: %d\n", minAtribuicoes, maxAtribuicoes);
        printf("Minimo de turnos trabalhados consecutivos: %d\n", minTurnosTrabalhadosConsecutivos);
	printf("Maximo de turnos trabalhados consecutivos: %d\n", maxTurnosTrabalhadosConsecutivos);
	printf("\n\n\n");

	printf("Mesmo Turno Trabalhado consecutivos\n\n");	
	for( k = 0; k < Shifts; k += 1){
		printf("Turno %d: min %d max %d\n",k+1,minMesmoTurnoTrabalhadoConsecutivos[k], maxMesmoTurnoTrabalhadoConsecutivos[k]);
        }
	printf("\n\n");

	printf("Numero Atribuicoes por Turno\n\n");
	for( k = 0; k < Shifts; k += 1){
		printf("Turno %d: min %d max %d\n",k+1, minNumeroAtribuicoesTurno[k], maxNumeroAtribuicoesTurno[k]);
        }
	printf("\n\n");

	// Fim print
	/////////////////////////////////////////////////////////

}
/*
estruturaSolucao* ILS(){

	estruturaSolucao *solucaoInicial,
			 *solucaoCorrente,
			 *novaSolucao,
			 *novaSolucaoCorrente;

	int i,iteracoes = 10000;

	solucaoInicial = GerarSolucaoInicial();
	solucaoCorrente = VND(solucaoInicial);

	for( i = 0; i < iteracoes ; i += 1 ){
		novaSolucao = Perturbacao(solucaoCorrente);
		novaSolucaoCorrente = VND(novaSolucao);
		solucaoCorrente = CriterioAceitacao(solucaoCorrente,novaSolucaoCorrente);
	}

	return solucaoCorrente;	
}

linhaMatriz* constroiLinha(int day){

	linhaMatriz *l = (linhaMatriz *)malloc(sizeof(linhaMatriz));
	int N,quantidadeTurno,posicaoLinha = 0,i;
	enum TURNO T = DIA;

	l->linha = (TURNO *)calloc(Nurses,sizeof(TURNO));

	for(N = 0; N < Nurses; N += 1){
		quantidadeTurno = matrizCobertura[day][N];
		for(i = 0; i < quantidadeTurno; i += 1){
			l->linha[posicaoLinha] = T;
			posicaoLinha += 1;
		}
		if(T == DIA){
			T = TARDE;
		}else if(T == TARDE){
			T = NOITE;
		}else if(T == NOITE){
			T = FOLGA;
		}
	}

	// caso todas as linhas nao tenham sido completadas
	if(posicaoLinha < Nurses){
		for(i = posicaoLinha ; i < Nurses ; i += 1){
			l->linha[posicaoLinha] = T; // considerando que valor de T seja FOLGA
		}
	}

	return l;	
}

estruturaSolucao* GerarSolucaoInicial(){
	
	estruturaSolucao *solucaoInicial;
	linhaCobertura   *linhaMatriz;
	int 		**matrizCusto,
	int		**matrizAtribuicao,
			day,
			nurse,
			i,j;
	
	inicializarSolucao(solucaoInicial);

	for(day = 0; day < Days ; i += 1){
		linhaMatriz = constroiLinha(day);
		for(nurse = 0 ; nurse < Nurses ; nurse += 1){
			matrizCusto = constroiMatrizCustoInicial(solucaoInicial,dia);
			matrizAtribuicao = algoritmoHungaro(matrizCusto);// atribui turno a enfermeira, no dia d
			
			for( i = 0 ; i < Nurses ; i++){
				for(j = 0; j < Nurses ; j+= 1){
					if(matrizAtribuicao[i][j] == 1){// enfermeira i recebera turno j
						break;
					}
				}
				solucaoInicial[i][day] = linhaMatriz->linha[j];
			}
		}
	}

	return solucaoInicial;

}

// Dado matriz de ordem Nurses, retornar o resultado atrelando elemento
// por elemento
int** algoritmoHungaro(int **matrizPesos){
	
}

estruturaSolucao* VND(estruturaSolucao *solucaoBruta){
	
	int quantidadeEstruturasVizinhanca = 2, // Nesta solucao usamos apenas CRP e k-Swap
	    estruturaVizinhancaCorrente = 1;

	estruturaSolucao *novaSolucao;

	while (estruturaVizinhancaCorrente <= quantidadeEstruturasVizinhanca){
		novaSolucao = MelhorVizinhanca(solucaoBruta, estruturaVizinhancaCorrente);

		if(calculoCustoTotal(novaSolucao) < calculoCustoTotal(solucaoBruta)){
			solucaoBruta = novaSolucao;
			estruturaVizinhancaCorrente = 1;
		}else{
			estruturaVizinhancaCorrente += 1;
		}
	}
}


void CRP(int **matrizSolucao){
	int day;

	for(day = 0; day < Days - 1; day += 1){
		constroiMatriz();
		algoritmoHungaro();
		RearranjoSolucao();
	}
}

void k_swap(int **matrizSolucao){
	int day,
	    k = rand()%Days;
	
	for(day = 0; day < Days - k - 1){

	}

}

estruturaSolucao* Perturbacao(estruturaSolucao *solucao){

	estruturaSolucao *solucaoPerturbada = solucao;

	int i, nivelPerturbacao = 5;
	int perturbacao;

	// Pensar em operacoes de perturbacao
	// Trocar o turno de uma enfermeira em um dia
	// Trocar o turno de duas enfermeiras no mesmo dia
	// Trocar parte da escala entre duas enfermeiras
	// ... pensar em mais, se houver

	while( i = 0; i < nivelPerturbacao; i += 1){
		perturbacao = rand()%Nurses;

		if(perturbacao == 0){
			solucaoPerturbada = CRP(*solucaoPerturbada);
		}else{
			solucaoPerturbada = k_swap(*solucaoPerturbada, perturbacao);
		}
	}

	return solucaoPerturbada;
}


// Poderia ser alterado pela comparacao entre duas solucoes
// pela diferenca entre a enfermeira menos beneficiada e a mais beneficiada
// ao inves de simplesmente o menor custo possivel
estruturaSolucao* CriterioAceitacao(estruturaSolucao *solucaoCorrente,estruturaSolucao *novaSolucao){

	int rankSolucaoCorrente,
	    rankNovaSolucao;

	rankSolucaoCorrente = calculoCustoTotal(solucaoCorrente);
	rankNovaSolucao     = calculoCustoTotal(novaSolucao);

	if( rankSolucaoCorrente >= rankNovaSolucao ){
		return solucaoCorrente;
	}

	return novaSolucao;	

}

int calculoCustoTotal(estruturaSolucao *solucao){

	return 0;
}

void GeracaoSaidaFormatada(estruturaSolucao *melhorSolucao, char *arquivoEntrada, char *Case){
// Valores a colocar nos arquivos ou outputs
// Forma de referenciar qual arquivo de instancia e case testado
// Separar qual o grupo de instancias utilizada
// custo de preferencia de cada uma das enfermeiras
// custo de preferencia da enfermeira mais favorecida
// custo de preferencia da enfermeira menos favorecida
// os parametros utilizados nos buscadores de solucoes como no ILS
// aprender a fazer operacoes de SO em C

	

}
*/
