#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define HUNGARIAN_NOT_ASSIGNED 0 
#define HUNGARIAN_ASSIGNED 1

#define HUNGARIAN_MODE_MINIMIZE_COST   0
#define HUNGARIAN_MODE_MAXIMIZE_UTIL 1

#define INF (0x7FFFFFFF)
#define verbose (0)

#define hungarian_test_alloc(X) do {if ((void *)(X) == NULL) fprintf(stderr, "Out of memory in %s, (%s, line %d).\n", __FUNCTION__, __FILE__, __LINE__); } while (0)

#define RED   "\x1B[31m"
#define RESET "\x1B[0m"

enum TURNO { DIA, TARDE, NOITE, FOLGA };
enum BOOLEAN { FALSE, TRUE };

typedef struct {
	enum TURNO **solucao;
} estruturaSolucao;

typedef struct {
        enum TURNO *linha;
} linhaMatriz;

/***** Hungarian Struct ********/
typedef struct {
  int num_rows;
  int num_cols;
  int** cost;
  int** assignment;  
} hungarian_problem_t;
//////////////////////////////////



void moduloInput(char *arquivoEntrada, char *Case);
void printaParametros(char *arquivoEntrada, char *Case);
estruturaSolucao* ILS();
estruturaSolucao* inicializarSolucao();
estruturaSolucao* gerarSolucaoInicial();
estruturaSolucao* VND(estruturaSolucao *solucaoBruta);
estruturaSolucao* MelhorVizinhanca(estruturaSolucao *solucaoBruta, int estruturaEscolhida);
void copiaSolucao(estruturaSolucao *solucao, estruturaSolucao *copia);
void CRP(estruturaSolucao *solucao);
void k_swap(estruturaSolucao *solucao, int k_s);
linhaMatriz* constroiLinha(int day);
int** constroiMatrizCustoInicialDia(linhaMatriz *linha, estruturaSolucao* solucaoInicial, int dia);
int** constroiMatrizCustoQuebraUmDia(estruturaSolucao* solucao, int diaDeQuebra);
int** constroiMatrizCustoQuebraDoisDias(estruturaSolucao* solucao, int diaDeQuebra1, int diaDeQuebra2);
int calculoCustoRoteiro(enum TURNO *roteiro, int nurse, int dia);
estruturaSolucao* CriterioAceitacao(estruturaSolucao *solucaoCorrente,estruturaSolucao *novaSolucao);
int calculoCustoTotal(estruturaSolucao *solucao);
/*********************************/
/// Hungarian algorithm functions

int** hungarianAlgorithm(int** matrizCusto);

/** This method initialize the hungarian_problem structure and init 
 *  the  cost matrices (missing lines or columns are filled with 0).
 *  It returns the size of the quadratic(!) assignment matrix. **/
int hungarian_init(hungarian_problem_t* p, 
		   int** cost_matrix, 
		   int rows, 
		   int cols, 
		   int mode);
  
/** Free the memory allocated by init. **/
void hungarian_free(hungarian_problem_t* p);

/** This method computes the optimal assignment. **/
void hungarian_solve(hungarian_problem_t* p);

/** Print the computed optimal assignment. **/
void hungarian_print_assignment(hungarian_problem_t* p);

/** Print the cost matrix. **/
void hungarian_print_costmatrix(hungarian_problem_t* p);

/** Print cost matrix and assignment matrix. **/
void hungarian_print_status(hungarian_problem_t* p);

///End Hungarian algorithm functions
/**********************************/

void geracaoSaidaFormatada(estruturaSolucao *melhorSolucao,char *arquivoEntrada, char *Case);
void liberarMemoria();
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

int violacaoRestricaoObrigatoria =  1000,
    violacaoRestricaoDesejavel   =  100;

enum BOOLEAN DEBUG = FALSE;

/////////////////////////////////////////////////////////////////

void moduloInput(char *arquivoEntrada, char *Case){
	
	int i,j,k;
	FILE *arquivoInstancia,*arquivoRestricoes;

	// Contrucao da instancia
	///////////////////////////////////////////////////////////

	arquivoInstancia  = fopen(arquivoEntrada,"r");
	
	if(arquivoInstancia == NULL){
		printf("arquivo instancia nao existe\n");
		return;
	}

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

	if(arquivoRestricoes == NULL){
		printf("arquivo Case nao existe\n");
		return;
	}

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

	if(DEBUG) printaParametros(arquivoEntrada, Case);

}

void printaParametros(char *arquivoEntrada, char *Case){

	int i,j,k;

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

estruturaSolucao* ILS(){

	estruturaSolucao *solucaoInicial,
			 *solucaoCorrente,
			 *novaSolucao,
			 *novaSolucaoCorrente;

	int i,iteracoes = 10000;

	solucaoInicial = gerarSolucaoInicial();
	solucaoCorrente = VND(solucaoInicial);

	/*for( i = 0; i < iteracoes ; i += 1 ){
		novaSolucao = Perturbacao(solucaoCorrente);
		novaSolucaoCorrente = VND(novaSolucao);
		solucaoCorrente = CriterioAceitacao(solucaoCorrente, novaSolucaoCorrente);
	}*/

	return solucaoCorrente;

}

estruturaSolucao* inicializarSolucao(){

	int i,j;
	estruturaSolucao *solucao = calloc(1 , sizeof(estruturaSolucao));
	solucao->solucao = (enum TURNO**)calloc(Nurses, sizeof(enum TURNO*));

	for( i = 0 ; i < Nurses ; i += 1 ){
		solucao->solucao[i] = (enum TURNO*)calloc(Days, sizeof(enum TURNO));
	}

	if(DEBUG){
		printf("Verificando os valores iniciais da solucao inicial\n\n");
		for( i = 0 ; i < Nurses ; i +=1 ){
			for( j = 0 ; j < Days ; j +=1 ){
				printf("%d ",solucao->solucao[i][j]);
			}
			printf("\n");
		}
	}

	return solucao;
}

estruturaSolucao* gerarSolucaoInicial(){
	
	estruturaSolucao *solucaoInicial;
	linhaMatriz      *linhaCobertura;
	int 		**matrizCusto,
			**matrizAtribuicao,
			day,i,j;
	
	solucaoInicial = inicializarSolucao();

	for(day = 0; day < Days ; day += 1){
		if(DEBUG) printf("Passando por gerarSolucaoInicial.constroiLinha\n\n");
		linhaCobertura = constroiLinha(day);// linha que representa as colunas de turnos de um determinado dia
		if(DEBUG) printf("Passando por gerarSolucaoInicial.constroiMatrizCustoInicialDia\n\n");
		matrizCusto = constroiMatrizCustoInicialDia(linhaCobertura, solucaoInicial, day);// matriz nurses x nurses,
		if(DEBUG) printf("Passando por gerarSolucaoInicial.hungarianAlgorithm\n\n");
		matrizAtribuicao = hungarianAlgorithm(matrizCusto);// atribui turno a enfermeira, no dia d
		printf("passa do hungarianAlgorithm do dia %d\n",day);
		for( i = 0 ; i < Nurses ; i += 1){
			for(j = 0; j < Nurses ; j += 1){
				if(matrizAtribuicao[i][j] == 1){// enfermeira i recebera turno j no dia day
					break;
				}
			}
			//printf("Valores i = %d day = %d j = %d linhaCobertura->linha[j] = %d\n",i,day,j,linhaCobertura->linha[j]);
			solucaoInicial->solucao[i][day] = linhaCobertura->linha[j];
			//printf("atribui turno para enfermeira %d no dia %d\n\n",i,day);
		}

		if(DEBUG) printf("Dando free em cada linha de gerarSolucaoInicial.matrizCusto\n\n");
		for( i = 0 ; i < Nurses ; i+= 1){
			free(matrizCusto[i]);
		}
		/*if(DEBUG) printf("Dando free em gerarSolucaoInicial.matrizCusto\n\n");
		free(matrizCusto);
		if(DEBUG) printf("Dando free em cada linha de gerarSolucaoInicial.matrizAtribuicao\n\n");
		for( i = 0 ; i < Nurses ; i+= 1){
			free(matrizAtribuicao[i]);
		}
		if(DEBUG) printf("Dando free em gerarSolucaoInicial.matrizAtribuicao\n\n");
		free(matrizAtribuicao);
		free(linhaCobertura->linha);*/
		//free(linhaCobertura);
	}

	return solucaoInicial;

}

void copiaSolucao(estruturaSolucao *solucao, estruturaSolucao *copia){

	int i,j;

	for( i = 0 ; i < Nurses ; i += 1 ){
		for( j = 0 ; j < Days ; j += 1 ){
			copia->solucao[i][j] = solucao->solucao[i][j];
		}
	}
}

estruturaSolucao* VND(estruturaSolucao *solucaoBruta){
	
	int quantidadeEstruturasVizinhanca = 2, // Nesta solucao usamos apenas CRP e k-Swap
	    estruturaVizinhancaCorrente = 1;

	estruturaSolucao *novaSolucao,*melhorSolucao = inicializarSolucao();

	copiaSolucao(solucaoBruta,melhorSolucao);

	printf("melhorSolucao inicial: %d\n",calculoCustoTotal(melhorSolucao));

	while (estruturaVizinhancaCorrente <= quantidadeEstruturasVizinhanca){
		novaSolucao = MelhorVizinhanca(melhorSolucao, estruturaVizinhancaCorrente);

		printf("Valor novaSolucao: %d e melhorSolucao anterior: %d\n",calculoCustoTotal(novaSolucao),calculoCustoTotal(melhorSolucao));

		if(calculoCustoTotal(novaSolucao) < calculoCustoTotal(melhorSolucao)){
			printf("%d menor que %d\n",calculoCustoTotal(novaSolucao),calculoCustoTotal(melhorSolucao));
			copiaSolucao(novaSolucao,melhorSolucao);
			estruturaVizinhancaCorrente = 1;
		}else{
			printf("Testando outra estrutura\n");
			estruturaVizinhancaCorrente += 1;
		}
		printf("Estrutura de vizinhanca %d\n",estruturaVizinhancaCorrente);
	}

	return melhorSolucao;
}

estruturaSolucao* MelhorVizinhanca(estruturaSolucao *solucaoBruta, int estruturaEscolhida){

	estruturaSolucao *melhorVizinhanca = inicializarSolucao();

	copiaSolucao(solucaoBruta, melhorVizinhanca);

	if(estruturaEscolhida == 1){
		printf("Executa CRP, estrutura %d\n",estruturaEscolhida);
		CRP(melhorVizinhanca);
	}else if(estruturaEscolhida == 2){

		estruturaSolucao *novaSolucao = inicializarSolucao();
		int i;


		for( i = 1 ; i < Days - 1; i += 1 ){//tamanho do bloco k

			copiaSolucao(solucaoBruta, novaSolucao);

			if( i == 1 ){
				printf("Executa CRP, estrutura %d\n",estruturaEscolhida);
				CRP(novaSolucao);
			}else{
				printf("Executa k_swap, estrutura %d\n",estruturaEscolhida);
				k_swap(novaSolucao,i);
			}
			if( calculoCustoTotal(novaSolucao) < calculoCustoTotal(melhorVizinhanca) ){
				copiaSolucao(novaSolucao, melhorVizinhanca);
			}
		}
	}

	return melhorVizinhanca;
}


void CRP(estruturaSolucao *solucao){

	estruturaSolucao *solucaoCopia = inicializarSolucao();
	int 		 **matrizCusto,
			 **matrizAtribuicao,
			 day,i,j,k;


	for(day = 0; day < Days - 1; day += 1){
		if(DEBUG) printf("Passando por CRP.constroiLinha\n\n");
		// talvez a forma de criar a linhaCobertura seja diferente;
		// pode ser usando constroiLinha ou utilizando os turnos ja atribuidos
		//linhaCobertura = getLinhaSolucaoDia(solucao, day);// linha que representa as colunas de turnos de um determinado dia
		if(DEBUG) printf("Passando por CRP.constroiMatrizCustoQuebraUmDia\n\n");
		matrizCusto = constroiMatrizCustoQuebraUmDia( solucao, day );// matriz nurses x nurses,
		if(DEBUG) printf("Passando por CRP.hungarianAlgorithm\n\n");
		matrizAtribuicao = hungarianAlgorithm(matrizCusto);// atribui turno a enfermeira, no dia d

		copiaSolucao(solucao,solucaoCopia);
	
		// Dado a matriz de atribuicao, verificar quais as enfermeiras e turnos que precisam ser rearranjados
		for( i = 0 ; i < Nurses ; i += 1){
			for(j = 0; j < Nurses ; j += 1){
				if(matrizAtribuicao[i][j] == 1){// enfermeira i recebera turno j no dia day
					break;
				}
			}
			// printf("Valores i = %d day = %d j = %d linhaCobertura->linha[j] = %d\n",i,day,j,linhaCobertura->linha[j]);
			// solucaoInicial->solucao[i][day] = linhaCobertura->linha[j];
			// printf("atribui turno para enfermeira %d no dia %d\n\n",i,day);
			for( k = day + 1 ; day < Days ; day += 1 ){
				solucao->solucao[i][k] = solucaoCopia->solucao[j][k];
			}
		}
		if(DEBUG) printf("Dando free em cada linha de CRP.matrizCusto\n\n");
		for( i = 0 ; i < Nurses ; i+= 1){
			free(matrizCusto[i]);
		}
	}
}

void k_swap(estruturaSolucao *solucao, int k_s){
	
	estruturaSolucao *solucaoCopia = inicializarSolucao();
	int 		 **matrizCusto,
			 **matrizAtribuicao,
			 day,i,j,k;


	for(day = 0; day < Days - k_s - 1 ; day += 1){
		if(DEBUG) printf("Passando por CRP.constroiLinha\n\n");
		// talvez a forma de criar a linhaCobertura seja diferente;
		// pode ser usando constroiLinha ou utilizando os turnos ja atribuidos
		//linhaCobertura = getLinhaSolucaoDia(solucao, day);// linha que representa as colunas de turnos de um determinado dia
		if(DEBUG) printf("Passando por CRP.constroiMatrizCustoQuebraUmDia\n\n");
		matrizCusto = constroiMatrizCustoQuebraDoisDias( solucao, day, day + k_s );// matriz nurses x nurses,
		if(DEBUG) printf("Passando por CRP.hungarianAlgorithm\n\n");
		matrizAtribuicao = hungarianAlgorithm(matrizCusto);// atribui turno a enfermeira, no dia d

		copiaSolucao(solucao,solucaoCopia);

		// Dado a matriz de atribuicao, verificar quais as enfermeiras e turnos que precisam ser rearranjados
		for( i = 0 ; i < Nurses ; i += 1){
			for(j = 0; j < Nurses ; j += 1){
				if(matrizAtribuicao[i][j] == 1){// enfermeira i recebera turno j no dia day
					break;
				}
			}
			// printf("Valores i = %d day = %d j = %d linhaCobertura->linha[j] = %d\n",i,day,j,linhaCobertura->linha[j]);
			// solucaoInicial->solucao[i][day] = linhaCobertura->linha[j];
			// printf("atribui turno para enfermeira %d no dia %d\n\n",i,day);
			for( k = day + 1 ; day < Days - k_s; day += 1 ){
				solucao->solucao[i][k] = solucaoCopia->solucao[j][k];
			}
		}
		if(DEBUG) printf("Dando free em cada linha de CRP.matrizCusto\n\n");
		for( i = 0 ; i < Nurses ; i+= 1){
			free(matrizCusto[i]);
		}
	}
}

linhaMatriz* constroiLinha(int day){// constroi a linha que representa a coluna da matriz de custo

	linhaMatriz *l = (linhaMatriz *)malloc(sizeof(linhaMatriz));
	int S,quantidadeTurno,posicaoLinha = 0,i,randomico;
	enum TURNO T = DIA;
	time_t t;

	l->linha = (enum TURNO*)calloc(Nurses,sizeof(enum TURNO));

	for(S = 0; S < Shifts; S += 1){
		quantidadeTurno = matrizCobertura[day][S];
		for(i = 0; i < quantidadeTurno; i += 1){
			l->linha[posicaoLinha] = T;
			posicaoLinha += 1;
		}
		if(T == DIA){
			T = TARDE;
		}else if(T == TARDE){
			T = NOITE;
		}else if(T == NOITE){
			T = FOLGA;// criar um novo tipo flexivel, assim, no calculo poderao ser escolhidos o melhor turno
		}
	}

	// caso todas as linhas nao tenham sido completadas
	srand((unsigned) time(&t));
	if(posicaoLinha < Nurses){
		for(i = posicaoLinha ; i < Nurses ; i += 1){
			randomico = rand()%4;

			if( randomico == 0 ) T = DIA;
			if( randomico == 1 ) T = TARDE;
			if( randomico == 2 ) T = NOITE;
			if( randomico == 3 ) T = FOLGA;

			l->linha[i] = T; // considerando que valor de T seja FOLGA, talvez escolher estes valores aleatoriamente
		}
	}

	if(DEBUG){
		printf("Coluna da matriz de custo para o dia %d\n\n", day + 1);
		for( i = 0 ; i < Nurses ; i +=1 ){
			printf("%d ",l->linha[i]);
		}
		printf("\n\n");
	}


	return l;
}

int** constroiMatrizCustoInicialDia(linhaMatriz *linha, estruturaSolucao* solucaoInicial, int dia){

	//utiliza matriz de preferencia
	int **matrizCusto;
	int i,j,k;
	enum TURNO shift;
	enum TURNO *roteiro;

	if(DEBUG) printf("Alocando constroiMatrizCustoInicialDia.matrizCusto\n\n");
	matrizCusto = (int **)malloc(Nurses*sizeof(int*));

	if(DEBUG) printf("Alocando as colunas de constroiMatrizCustoInicialDia.matrizCusto\n\n");
	for( i = 0 ; i < Nurses ; i += 1){
		matrizCusto[i] = (int *)malloc(Nurses*sizeof(int));
	}

	if(DEBUG) printf("Alocando constroiMatrizCustoInicialDia.roteiro\n\n");
	roteiro = (enum TURNO*)calloc(dia+1,sizeof(enum TURNO));

	for( i = 0 ; i < Nurses ; i += 1 ){
		for( j = 0 ; j< Nurses ; j += 1 ){// mesmo iterando sobre a coluna de turnos, ha turnos adicionais,
						  // portanto, deve ser igual o numero de enfermeiras
			shift = linha->linha[j];

			if( dia != 0 ){
				
				for( k = 0 ; k <= dia ; k += 1){ // roteiro do primeiro ate o dia anterior ao atual
					if( k == dia ) roteiro[k] = shift;
					else           roteiro[k] = solucaoInicial->solucao[i][k];
				}

				//if(DEBUG) printf("Chamando constroiMatrizCustoInicialDia.calculoCustoRoteiro\n\n");
				matrizCusto[i][j] = calculoCustoRoteiro(roteiro,i,dia);
			}else{
				matrizCusto[i][j] = matrizPreferencia[i][dia][shift];
			}
		}
	}
 
	return matrizCusto;

}


int** constroiMatrizCustoQuebraUmDia(estruturaSolucao* solucao, int diaDeQuebra){
	
	int i,j,k;
	int **matrizCusto;
	enum TURNO *roteiro;

	if(DEBUG) printf("Alocando constroiMatrizCustoQuebraUmDia.matrizCusto\n\n");
	matrizCusto = (int **)malloc(Nurses*sizeof(int*));

	if(DEBUG) printf("Alocando as colunas de constroiMatrizCustoQuebraUmDia.matrizCusto\n\n");
	for( i = 0 ; i < Nurses ; i += 1){
		matrizCusto[i] = (int *)malloc(Nurses*sizeof(int));
	}

	if(DEBUG) printf("Alocando constroiMatrizCustoInicialDia.roteiro\n\n");
	roteiro = (enum TURNO*)calloc(Days,sizeof(enum TURNO));

	for( i = 0 ; i < Nurses ; i += 1 ){
		for ( j = 0 ; j < Nurses ; j += 1 ){
			// roteiro da esquerda sera ate o dia diaDeQuebra
			for( k = 0 ; k < Days ; k += 1){
				if( k <= diaDeQuebra )  roteiro[k] = solucao->solucao[i][k];
				else			roteiro[k] = solucao->solucao[j][k];
			}

			//if(DEBUG) printf("Chamando constroiMatrizCustoQuebraUmDia.calculoCustoRoteiro\n\n");
			matrizCusto[i][j] = calculoCustoRoteiro(roteiro,i, Days - 1);
		}
	}

	return matrizCusto;
}

int** constroiMatrizCustoQuebraDoisDias(estruturaSolucao* solucao, int diaDeQuebra1, int diaDeQuebra2){
	
	int i,j,k;
	int **matrizCusto;
	enum TURNO *roteiro;

	if(DEBUG) printf("Alocando constroiMatrizCustoQuebraDoisDias.matrizCusto\n\n");
	matrizCusto = (int **)malloc(Nurses*sizeof(int*));

	if(DEBUG) printf("Alocando as colunas de constroiMatrizCustoQuebraDoisDias.matrizCusto\n\n");
	for( i = 0 ; i < Nurses ; i += 1){
		matrizCusto[i] = (int *)malloc(Nurses*sizeof(int));
	}

	if(DEBUG) printf("Alocando constroiMatrizCustoInicialDia.roteiro\n\n");
	roteiro = (enum TURNO*)calloc(Days,sizeof(enum TURNO));

	for( i = 0 ; i < Nurses ; i += 1 ){
		for ( j = 0 ; j < Nurses ; j += 1 ){
			// roteiro da esquerda sera ate o dia diaDeQuebra1
			// roteiro da direita sera diaDeQuebra2 ate Days - 1
			for( k = 0 ; k < Days ; k += 1){
				if( k <= diaDeQuebra1 || k >= diaDeQuebra2 )  roteiro[k] = solucao->solucao[i][k];
				else					      roteiro[k] = solucao->solucao[j][k];
			}

			//if(DEBUG) printf("Chamando constroiMatrizCustoQuebraDoisDias.calculoCustoRoteiro\n\n");
			matrizCusto[i][j] = calculoCustoRoteiro(roteiro,i, Days - 1);
		}
	}

	return matrizCusto;
}

// Tem como entrada o roteiro ate o dia atual(tamanho dia + 1, vai ate posicao dia), sendo o dia atual dado por dia e a enfermeira
int calculoCustoRoteiro(enum TURNO *roteiro, int nurse, int dia){

	int i, custoPreferencia = 0;
	enum TURNO shiftCorrente,
		   shiftAnterior = FOLGA;

	int quantidadeRestricoesObrigatoriasVioladas = 0,
	    quantidadeRestricoesDesejaveisVioladas = 0;

	int quantidadeAtribuicoes = 0,
    	    quantidadeTurnosTrabalhadosConsecutivos = 0;

	int quantidadeTurnosTrabalhadosConsecutivosCorrente = 0;

	int quantidadeMesmoTurnoTrabalhadoConsecutivos[Shifts],
    	    quantidadeNumeroAtribuicoesTurno[Shifts];

	int quantidadeMesmoTurnoTrabalhadoConsecutivosCorrente[Shifts];
	
	memset(quantidadeMesmoTurnoTrabalhadoConsecutivos,0,sizeof(quantidadeMesmoTurnoTrabalhadoConsecutivos));
	memset(quantidadeNumeroAtribuicoesTurno,0,sizeof(quantidadeNumeroAtribuicoesTurno));
	memset(quantidadeMesmoTurnoTrabalhadoConsecutivosCorrente,0,sizeof(quantidadeMesmoTurnoTrabalhadoConsecutivosCorrente));

	// Acumulo dos dados para o calculo de custo do roteiro mais o dia atual
	for( i = 0 ; i <= dia ; i += 1){
		
		shiftCorrente = roteiro[i];

		if(i != 0){ // verificar violacao de restricao obrigatoria
			if((shiftAnterior == TARDE && shiftCorrente == DIA) ||
			   (shiftAnterior == NOITE && (shiftCorrente == DIA || shiftCorrente == TARDE))){
				quantidadeRestricoesObrigatoriasVioladas += 1;
			}
		}
			
		if(shiftCorrente != FOLGA){
			quantidadeAtribuicoes += 1;
		}

		quantidadeNumeroAtribuicoesTurno[shiftCorrente] += 1;
		
		if( i != 0 && shiftCorrente == shiftAnterior ){
			quantidadeTurnosTrabalhadosConsecutivosCorrente += 1;
			quantidadeMesmoTurnoTrabalhadoConsecutivosCorrente[shiftCorrente] += 1;

			if( quantidadeTurnosTrabalhadosConsecutivosCorrente > quantidadeTurnosTrabalhadosConsecutivos ){
				quantidadeTurnosTrabalhadosConsecutivos = quantidadeTurnosTrabalhadosConsecutivosCorrente;
			}

			if( quantidadeMesmoTurnoTrabalhadoConsecutivosCorrente[shiftCorrente] > 
				quantidadeMesmoTurnoTrabalhadoConsecutivos[shiftCorrente]){
				quantidadeMesmoTurnoTrabalhadoConsecutivos[shiftCorrente] = 
					quantidadeMesmoTurnoTrabalhadoConsecutivosCorrente[shiftCorrente];
			}
		}else{
			quantidadeTurnosTrabalhadosConsecutivosCorrente = 1;
			quantidadeMesmoTurnoTrabalhadoConsecutivosCorrente[shiftCorrente] = 1;
		}

		custoPreferencia += matrizPreferencia[nurse][i][shiftCorrente];

		shiftAnterior = shiftCorrente;
	}
	
	// Calculo do custo baseado na matriz de preferencia e nos dados do roteiro
	if(quantidadeAtribuicoes < minAtribuicoes || quantidadeAtribuicoes > maxAtribuicoes){
		quantidadeRestricoesDesejaveisVioladas += 1;
	}

	if(quantidadeTurnosTrabalhadosConsecutivos < minTurnosTrabalhadosConsecutivos || 
	   quantidadeTurnosTrabalhadosConsecutivos > maxTurnosTrabalhadosConsecutivos){
		quantidadeRestricoesDesejaveisVioladas += 1;
	}

	/*for( i = 0 ; i < Shifts ; i += 1){
		if( quantidadeMesmoTurnoTrabalhadoConsecutivos[i] < minMesmoTurnoTrabalhadoConsecutivos[i] ||
		    quantidadeMesmoTurnoTrabalhadoConsecutivos[i] > maxMesmoTurnoTrabalhadoConsecutivos[i] ){
			quantidadeRestricoesDesejaveisVioladas += 1;
		}
	}*/

	for( i = 0 ; i < Shifts ; i += 1){
		if( quantidadeNumeroAtribuicoesTurno[i] < minNumeroAtribuicoesTurno[i] ||
		    quantidadeNumeroAtribuicoesTurno[i] > maxNumeroAtribuicoesTurno[i] ){
			quantidadeRestricoesDesejaveisVioladas += 1;
		}
	}
	
	//printf("%d %d %d\n",custoPreferencia,quantidadeRestricoesObrigatoriasVioladas,quantidadeRestricoesDesejaveisVioladas);

	return custoPreferencia + 
	       quantidadeRestricoesObrigatoriasVioladas * violacaoRestricaoObrigatoria + 
	       quantidadeRestricoesDesejaveisVioladas * violacaoRestricaoDesejavel; // verificar se precisa considerar custo ja na construcao
}

// Poderia ser alterado pela comparacao entre duas solucoes
// pela diferenca entre a enfermeira menos beneficiada e a mais beneficiada
// ao inves de simplesmente o menor custo possivel
estruturaSolucao* CriterioAceitacao(estruturaSolucao *solucaoCorrente,estruturaSolucao *novaSolucao){

	int rankSolucaoCorrente,
	    rankNovaSolucao;

	rankSolucaoCorrente = calculoCustoTotal(solucaoCorrente);
	rankNovaSolucao     = calculoCustoTotal(novaSolucao);

	if( rankSolucaoCorrente <= rankNovaSolucao ){
		return solucaoCorrente;
	}

	return novaSolucao;

}

int calculoCustoTotal(estruturaSolucao *solucao){

	int i, custoTotal = 0;

	for( i = 0 ; i < Nurses ; i += 1 ){
		custoTotal += calculoCustoRoteiro(solucao->solucao[i], i, Days - 1);
		//printf("Custo Total na iteracao %d: %d, somando %d\n",i,custoTotal,calculoCustoRoteiro(solucao->solucao[i], i, Days - 1));
	}

	return custoTotal;
}




/**********************Hungarian Algorithm*******************/

int** hungarianAlgorithm(int** matrizCusto){

	hungarian_problem_t p;

	hungarian_init(&p, matrizCusto , Nurses, Nurses, HUNGARIAN_MODE_MINIMIZE_COST);

	if(DEBUG) hungarian_print_costmatrix(&p);
	hungarian_solve(&p);
	if(DEBUG) hungarian_print_assignment(&p);
	return p.assignment;

}


void hungarian_print_matrix(int** C, int rows, int cols) {
  int i,j;
  fprintf(stderr , "\n");
  for(i=0; i<rows; i++) {
    fprintf(stderr, " [");
    for(j=0; j<cols; j++) {
      if(C[i][j] != 0) fprintf(stderr, RED);
      fprintf(stderr, "%3d ",C[i][j]);
      if(C[i][j] != 0) fprintf(stderr, RESET);
    }
    fprintf(stderr, "]\n");
  }
  fprintf(stderr, "\n");
}

void hungarian_print_assignment(hungarian_problem_t* p) {
  hungarian_print_matrix(p->assignment, p->num_rows, p->num_cols) ;
}

void hungarian_print_costmatrix(hungarian_problem_t* p) {
  hungarian_print_matrix(p->cost, p->num_rows, p->num_cols) ;
}

void hungarian_print_status(hungarian_problem_t* p) {
  
  fprintf(stderr,"cost:\n");
  hungarian_print_costmatrix(p);

  fprintf(stderr,"assignment:\n");
  hungarian_print_assignment(p);
  
}

int hungarian_imax(int a, int b) {
  return (a<b)?b:a;
}

int hungarian_init(hungarian_problem_t* p, int** cost_matrix, int rows, int cols, int mode) {

  int i,j, org_cols, org_rows;
  int max_cost;
  max_cost = 0;
  
  org_cols = cols;
  org_rows = rows;

  // is the number of cols  not equal to number of rows ? 
  // if yes, expand with 0-cols / 0-cols
  rows = hungarian_imax(cols, rows);
  cols = rows;
  
  p->num_rows = rows;
  p->num_cols = cols;

  p->cost = (int**)calloc(rows,sizeof(int*));
  hungarian_test_alloc(p->cost);
  p->assignment = (int**)calloc(rows,sizeof(int*));
  hungarian_test_alloc(p->assignment);

  for(i=0; i<p->num_rows; i++) {
    p->cost[i] = (int*)calloc(cols,sizeof(int));
    hungarian_test_alloc(p->cost[i]);
    p->assignment[i] = (int*)calloc(cols,sizeof(int));
    hungarian_test_alloc(p->assignment[i]);
    for(j=0; j<p->num_cols; j++) {
      p->cost[i][j] =  (i < org_rows && j < org_cols) ? cost_matrix[i][j] : 0;
      p->assignment[i][j] = 0;

      if (max_cost < p->cost[i][j])
	max_cost = p->cost[i][j];
    }
  }


  if (mode == HUNGARIAN_MODE_MAXIMIZE_UTIL) {
    for(i=0; i<p->num_rows; i++) {
      for(j=0; j<p->num_cols; j++) {
	p->cost[i][j] =  max_cost - p->cost[i][j];
      }
    }
  }
  else if (mode == HUNGARIAN_MODE_MINIMIZE_COST) {
    // nothing to do
  }
  else 
    fprintf(stderr,"%s: unknown mode. Mode was set to HUNGARIAN_MODE_MINIMIZE_COST !\n", __FUNCTION__);
  
  return rows;
}




void hungarian_free(hungarian_problem_t* p) {
  int i;
  for(i=0; i<p->num_rows; i++) {
    free(p->cost[i]);
    free(p->assignment[i]);
  }
  free(p->cost);
  free(p->assignment);
  p->cost = NULL;
  p->assignment = NULL;
}



void hungarian_solve(hungarian_problem_t* p)
{
  int i, j, m, n, k, l, s, t, q, unmatched, cost;
  int* col_mate;
  int* row_mate;
  int* parent_row;
  int* unchosen_row;
  int* row_dec;
  int* col_inc;
  int* slack;
  int* slack_row;

  cost=0;
  m =p->num_rows;
  n =p->num_cols;

  col_mate = (int*)calloc(p->num_rows,sizeof(int));
  hungarian_test_alloc(col_mate);
  unchosen_row = (int*)calloc(p->num_rows,sizeof(int));
  hungarian_test_alloc(unchosen_row);
  row_dec  = (int*)calloc(p->num_rows,sizeof(int));
  hungarian_test_alloc(row_dec);
  slack_row  = (int*)calloc(p->num_rows,sizeof(int));
  hungarian_test_alloc(slack_row);

  row_mate = (int*)calloc(p->num_cols,sizeof(int));
  hungarian_test_alloc(row_mate);
  parent_row = (int*)calloc(p->num_cols,sizeof(int));
  hungarian_test_alloc(parent_row);
  col_inc = (int*)calloc(p->num_cols,sizeof(int));
  hungarian_test_alloc(col_inc);
  slack = (int*)calloc(p->num_cols,sizeof(int));
  hungarian_test_alloc(slack);

  for (i=0;i<p->num_rows;i++) {
    col_mate[i]=0;
    unchosen_row[i]=0;
    row_dec[i]=0;
    slack_row[i]=0;
  }
  for (j=0;j<p->num_cols;j++) {
    row_mate[j]=0;
    parent_row[j] = 0;
    col_inc[j]=0;
    slack[j]=0;
  }

  for (i=0;i<p->num_rows;++i)
    for (j=0;j<p->num_cols;++j)
      p->assignment[i][j]=HUNGARIAN_NOT_ASSIGNED;

  // Begin subtract column minima in order to start with lots of zeroes 12
  if (verbose)
    fprintf(stderr, "Using heuristic\n");
  for (l=0;l<n;l++)
    {
      s=p->cost[0][l];
      for (k=1;k<m;k++) 
	if (p->cost[k][l]<s)
	  s=p->cost[k][l];
      cost+=s;
      if (s!=0)
	for (k=0;k<m;k++)
	  p->cost[k][l]-=s;
    }
  // End subtract column minima in order to start with lots of zeroes 12

  // Begin initial state 16
  t=0;
  for (l=0;l<n;l++)
    {
      row_mate[l]= -1;
      parent_row[l]= -1;
      col_inc[l]=0;
      slack[l]=INF;
    }
  for (k=0;k<m;k++)
    {
      s=p->cost[k][0];
      for (l=1;l<n;l++)
	if (p->cost[k][l]<s)
	  s=p->cost[k][l];
      row_dec[k]=s;
      for (l=0;l<n;l++)
	if (s==p->cost[k][l] && row_mate[l]<0)
	  {
	    col_mate[k]=l;
	    row_mate[l]=k;
	    if (verbose)
	      fprintf(stderr, "matching col %d==row %d\n",l,k);
	    goto row_done;
	  }
      col_mate[k]= -1;
      if (verbose)
	fprintf(stderr, "node %d: unmatched row %d\n",t,k);
      unchosen_row[t++]=k;
    row_done:
      ;
    }
  // End initial state 16
 
  // Begin Hungarian algorithm 18
  if (t==0)
    goto done;
  unmatched=t;
  while (1)
    {
      if (verbose)
	fprintf(stderr, "Matched %d rows.\n",m-t);
      q=0;
      while (1)
	{
	  while (q<t)
	    {
	      // Begin explore node q of the forest 19
	      {
		k=unchosen_row[q];
		s=row_dec[k];
		for (l=0;l<n;l++)
		  if (slack[l])
		    {
		      int del;
		      del=p->cost[k][l]-s+col_inc[l];
		      if (del<slack[l])
			{
			  if (del==0)
			    {
			      if (row_mate[l]<0)
				goto breakthru;
			      slack[l]=0;
			      parent_row[l]=k;
			      if (verbose)
				fprintf(stderr, "node %d: row %d==col %d--row %d\n",
				       t,row_mate[l],l,k);
			      unchosen_row[t++]=row_mate[l];
			    }
			  else
			    {
			      slack[l]=del;
			      slack_row[l]=k;
			    }
			}
		    }
	      }
	      // End explore node q of the forest 19
	      q++;
	    }
 
	  // Begin introduce a new zero into the matrix 21
	  s=INF;
	  for (l=0;l<n;l++)
	    if (slack[l] && slack[l]<s)
	      s=slack[l];
	  for (q=0;q<t;q++)
	    row_dec[unchosen_row[q]]+=s;
	  for (l=0;l<n;l++)
	    if (slack[l])
	      {
		slack[l]-=s;
		if (slack[l]==0)
		  {
		    // Begin look at a new zero 22
		    k=slack_row[l];
		    if (verbose)
		      fprintf(stderr, 
			     "Decreasing uncovered elements by %d produces zero at [%d,%d]\n",
			     s,k,l);
		    if (row_mate[l]<0)
		      {
			for (j=l+1;j<n;j++)
			  if (slack[j]==0)
			    col_inc[j]+=s;
			goto breakthru;
		      }
		    else
		      {
			parent_row[l]=k;
			if (verbose)
			  fprintf(stderr, "node %d: row %d==col %d--row %d\n",t,row_mate[l],l,k);
			unchosen_row[t++]=row_mate[l];
		      }
		    // End look at a new zero 22
		  }
	      }
	    else
	      col_inc[l]+=s;
	  // End introduce a new zero into the matrix 21
	}
    breakthru:
      // Begin update the matching 20
      if (verbose)
	fprintf(stderr, "Breakthrough at node %d of %d!\n",q,t);
      while (1)
	{
	  j=col_mate[k];
	  col_mate[k]=l;
	  row_mate[l]=k;
	  if (verbose)
	    fprintf(stderr, "rematching col %d==row %d\n",l,k);
	  if (j<0)
	    break;
	  k=parent_row[j];
	  l=j;
	}
      // End update the matching 20
      if (--unmatched==0)
	goto done;
      // Begin get ready for another stage 17
      t=0;
      for (l=0;l<n;l++)
	{
	  parent_row[l]= -1;
	  slack[l]=INF;
	}
      for (k=0;k<m;k++)
	if (col_mate[k]<0)
	  {
	    if (verbose)
	      fprintf(stderr, "node %d: unmatched row %d\n",t,k);
	    unchosen_row[t++]=k;
	  }
      // End get ready for another stage 17
    }
 done:

  // Begin doublecheck the solution 23
  for (k=0;k<m;k++)
    for (l=0;l<n;l++)
      if (p->cost[k][l]<row_dec[k]-col_inc[l])
	exit(0);
  for (k=0;k<m;k++)
    {
      l=col_mate[k];
      if (l<0 || p->cost[k][l]!=row_dec[k]-col_inc[l])
	exit(0);
    }
  k=0;
  for (l=0;l<n;l++)
    if (col_inc[l])
      k++;
  if (k>m)
    exit(0);
  // End doublecheck the solution 23
  // End Hungarian algorithm 18

  for (i=0;i<m;++i)
    {
      p->assignment[i][col_mate[i]]=HUNGARIAN_ASSIGNED;
      /*TRACE("%d - %d\n", i, col_mate[i]);*/
    }
  for (k=0;k<m;++k)
    {
      for (l=0;l<n;++l)
	{
	  /*TRACE("%d ",p->cost[k][l]-row_dec[k]+col_inc[l]);*/
	  p->cost[k][l]=p->cost[k][l]-row_dec[k]+col_inc[l];
	}
      /*TRACE("\n");*/
    }
  for (i=0;i<m;i++)
    cost+=row_dec[i];
  for (i=0;i<n;i++)
    cost-=col_inc[i];
  if (verbose)
    fprintf(stderr, "Cost is %d\n",cost);


  //free(slack);
  free(col_inc);
  free(parent_row);
  free(row_mate);
  free(slack_row);
  free(row_dec);
  free(unchosen_row);
  free(col_mate);
}





/*******************End Hungarian Algorithm******************/

void geracaoSaidaFormatada(estruturaSolucao *melhorSolucao, char *arquivoEntrada, char *Case){
// Valores a colocar nos arquivos ou outputs
// Forma de referenciar qual arquivo de instancia e case testado
// Separar qual o grupo de instancias utilizada
// custo de preferencia de cada uma das enfermeiras
// custo de preferencia da enfermeira mais favorecida
// custo de preferencia da enfermeira menos favorecida
// os parametros utilizados nos buscadores de solucoes como no ILS
// aprender a fazer operacoes de SO em C

	int i,j,acum;
	
	printf("Arquivo de instancia %s\n\n",arquivoEntrada);
	printf("Arquivo de case %s\n\n",Case);


	printf("Resultado do algoritmo\n\n");

	for( i = 0 ; i < Nurses ; i += 1){
		printf("Nurse %d:\n\t",i);
		for( j = 0 ; j < Days ; j += 1){
			enum TURNO turno = melhorSolucao->solucao[i][j];
			if(turno == DIA){
				printf("DIA   ");
			}else if(turno == TARDE){
				printf("TARDE ");
			}else if(turno == NOITE){
				printf("NOITE ");
			}else if(turno == FOLGA){
				printf("FOLGA ");
			}
		}
		printf("\n");
	}

	printf("\n\n\n");

	for( i = 0 ; i < Nurses ; i += 1 ){
		acum = calculoCustoRoteiro(melhorSolucao->solucao[i], i, Days - 1);
		printf("Enfermeira %d tem roteiro com custo %d\n",i,acum);
	}

	printf("Custo da melhor solucao %d\n\n",calculoCustoTotal(melhorSolucao));
	
}

void liberarMemoria(){

	int i,j;

	if(DEBUG) printf("Liberando memoria de matrizCobertura\n\n");
	for(j = 0; j < Days ; j += 1){
		free(matrizCobertura[j]);
	}
	free(matrizCobertura);

	if(DEBUG) printf("Liberando memoria de matrizPreferencia\n\n");
	for( i = 0; i < Nurses ; i += 1){
		for( j = 0; j < Days; j += 1){
			free(matrizPreferencia[i][j]);
		}
		free(matrizPreferencia[i]);
	}
	free(matrizPreferencia);

	if(DEBUG) printf("Liberando memoria dos dados de Case\n\n");
	free(minMesmoTurnoTrabalhadoConsecutivos);
	free(maxMesmoTurnoTrabalhadoConsecutivos);
	free(minNumeroAtribuicoesTurno);
	free(maxNumeroAtribuicoesTurno);

}

int main(int argc, char *argv[]){
	
	estruturaSolucao *melhorSolucao;
	
	// Leitura de todos os dados e construcao de todas as estruturas usadas no problema
	moduloInput(argv[1],argv[2]);

	//
	melhorSolucao = ILS();

	// Geracao de arquivos ou saida formatada para analise de dados gerados
	geracaoSaidaFormatada(melhorSolucao,argv[1],argv[2]);

	liberarMemoria();
	return 0;
}


// ideia: ajuste na criacao de otimo local com o VND
// Exemplo: Verificar os roteiros com restricoes, e alterar por turnos para verificar se diminui custo
/*

estruturaSolucao* Perturbacao(estruturaSolucao *solucao){

	estruturaSolucao *solucaoPerturbada = solucao;

	int i, nivelPerturbacao = 5;
	int perturbacao;

	// Pensar em operacoes de perturbacao
	// Trocar o turno de uma enfermeira em um dia
	// Trocar o turno de duas enfermeiras no mesmo dia
	// Trocar parte da escala entre duas enfermeiras
	// ... pensar em mais, se houver
	CRP_Perturbado // mudanca nos turnos, respeitando a cobertura minima
	k_swap_Perturbado // mudanca nos turnos, respeitando a cobertura minima

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




*/
