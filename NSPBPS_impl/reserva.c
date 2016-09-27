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

struct Solucao{
	int **solucao;
} estruturaSolucao;

void moduloInput(char *arquivoEntrada, char *Case);
estruturaSolucao* AvaliadorSolucoes();
void GeracaoSaidaFormatada(estruturaSolucao *melhorSolucao,char *arquivoEntrada, char *Case);

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
	
	estruturaSolucao melhorSolucao;
	
	// Leitura de todos os dados e construcao de todas as estruturas usadas no problema
	moduloInput(argv[1],argv[2]);

	//
	melhorSolucao = AvaliadorSolucoes();

	// Geracao de arquivos ou saida formatada para analise de dados gerados
	GeracaoSaidaFormatada(melhorSolucao,argv[1],argv[2]);
	return 0;
}

void moduloInput(char *arquivoEntrada, *Case){
	
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
	**matrizPreferencia = (int ***)malloc(sizeof(int **) * Nurses);

	for( i = 0; i < Nurses ; i += 1){
		*matrizPreferencia[i] = (int **)malloc(sizeof(int *) * Days);
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
	fscanf(arquicoRestricoes,"%d %d",&minTurnosTrabalhadosConsecutivos,&maxTurnosTrabalhadosConsecutivos);

	// Construcao das restricoes
	minMesmoTurnoTrabalhadoConsecutivos = (int *)malloc(sizeof(int) * Shifts);
	maxMesmoTurnoTrabalhadoConsecutivos = (int *)malloc(sizeof(int) * Shifts);
	minNumeroAtribuicoesTurno 	    = (int *)malloc(sizeof(int) * Shifts);
	minNumeroAtribuicoesTurno 	    = (int *)malloc(sizeof(int) * Shifts);

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

}

estruturaSolucao* AvaliadorSolucoes(){

	estruturaSolucao *solucaoInicial,
			 *solucaoCorrente,
			 *novaSolucao,
			 *novaSolucaoCorrente;

	int i,iteracoes = 10000;

	solucaoInicial = GerarSolucaoInicial();
	solucaoCorrente = VND(solucaoInicial);

	for( i = 0; i < iteracoes ; i += 1 /*criterios de parada a serem definidos*/){
		novaSolucao = Perturbacao(solucaoCorrente);
		novaSolucaoCorrente = VND(novaSolucao);
		solucaoCorrente = CriterioAceitacao(solucaoCorrente,novaSolucaoCorrente);
	}

	return solucaoCorrente;	
}

estruturaSolucao* GerarSolucaoInicial(){

	

}

estruturaSolucao* VND(estruturaSolucao *solucaoBruta){

}

estruturaSolucao* Perturbacao(estruturaSolucao *solucao){

	estruturaSolucao *solucaoPerturbada;
	
	int i, nivelPerturbacao = 5;

	while( i = 0; i < nivelPerturbacao; i += 1){
		solucaoPerturbada = CRP_Randomico(*solucaoPerturbada);
		solucaoPerturbada = k_swap_Randomico(*solucaoPerturbada);
	}

	return solucaoPerturbada;
}

estruturaSolucao* VND(estruturaSolucao *solucao){

	estruturaSolucao *solucaoOtimoLocal;

	return NULL;
}

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
