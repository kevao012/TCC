/**********************************************************
** PROGRAMA EM C PARA RESOLVER O PROBLEMA DA MOCHILA POR **
** MEIO DE META-HEURÍSTICAS E HEURÍSTICAS DE BUSCA LOCAL **
**                                                       **
** AUTOR: ANTONIO AUGUSTO CHAVES (FEG/UNESP)             **
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#pragma hdrstop


#define MAX(x,y) ((x)<(y) ? (y) : (x))

#define n 50                 //numero de objetos
#define capacidade 100       //capacidade da mochila



//Tipo Solução
struct TSol
{
  int obj[n];
  int fo;
};

struct TSolRK
{
  float obj[n];
  int fo;
};



//Variáveis Globais
int B[n],                   //vetor com os benefícios dos objetos
    P[n];                   //vetor com os pesos dos objetos


//Declaração de Funções
void LerArquivos();
int CalcularFO(TSol s);
int Decoder(TSolRK s);
TSol GerarSolucaoInicial();
TSol GerarSolucaoInicialViavel();
TSol Construcao(float alfa);
void ImprimirMelhorSol(TSol s);
TSol TrocarBit(TSol s, int i);
TSol SubidaTrocaBit(TSol s);


void GA();
void BRKGA();
void SA();
void TS();
void GRASP();
void VNS();
void ILS();
void ACO();
void LNS();

double randomico(double min, double max);
int irandomico(int min, int max);




#pragma argsused
/************************************************************************************
*** Método: Main()                                                                ***
*** Função: Programa principal                                                    ***
*************************************************************************************/
int main()
{
  int opcao = 0, opcao2;
  LerArquivos();

  do
  {
	//Menu principal
    system("cls");
    printf("\n-------------------------------------------------------------\n");
    printf("\nResolucao do Problema da Mochila por meio de Meta-heuristicas\n");
    printf("\nAutor: Antonio Augusto Chaves (FEG\\UNESP)\n");
    printf("\nDados do problema-teste: \n  numero de objetos: 50 \n");
    printf("  capacidade da mochila: 100 \n  solucao otima: 149 \n");
    printf("\n-------------------------------------------------------------");
    printf("\n\nMENU \n");
    printf("\n[01] Heuristicas");
    printf("\n[02] Algoritmo Genetico");
    printf("\n[03] Simulated Annealing");
    printf("\n[04] Busca Tabu");
    printf("\n[05] GRASP");
    printf("\n[06] VNS");
    printf("\n[07] ILS");
    printf("\n[08] Colonia de Formigas");
    printf("\n[09] LNS");
    printf("\n[10] BRKGA");
    printf("\n[00] Sair");
    printf("\n\nOpcao: ");

    scanf("%d", &opcao);
    
    switch(opcao)
    {
      case 1:
        system("cls");
        printf("\n-------------------------------------------------------------\n");
        printf("\n\nSUB-MENU HEURISTICAS\n");
        printf("\n[1] Solucao Aleatoria");
        printf("\n[2] Solucao Aleatoria Viavel");
        printf("\n[3] Solucao Gulosa");
        printf("\n[4] Solucao Parcialmente Gulosa");        
        printf("\n\nOpcao: ");
        scanf("%d", &opcao2);       
    
        switch(opcao2)
        {
          case 1: 
            ImprimirMelhorSol(GerarSolucaoInicial());
            break;
          
       	  case 2:
            ImprimirMelhorSol(GerarSolucaoInicialViavel());
            break;
            
          case 3:
            ImprimirMelhorSol(Construcao(1));
            break;
          
          case 4:
            float alfa;
            printf("\nDigite o valor de alfa: ");
            scanf("%d", &alfa); 
            ImprimirMelhorSol(Construcao(alfa));
            break;
    
          default:
            printf("\n\nOpcao invalida! Pressione alguma tecla para retornar ao menu...");
            fflush(stdin);
            getchar();
        }
        break;                  
                 
      case 2: 
        GA();
        break;
      
   	  case 3:
        SA();
        break;
        
      case 4:
        TS();
        break;
        
      case 5:
        GRASP();
        break;
        
      case 6:
        VNS();
        break;
        
      case 7:
        ILS();
        break;
        
      case 8:
        ACO();
        break;
        
      case 9:
        LNS();     
        break;           

      case 10:
        BRKGA();     
        break;
                        
      case 0:
        printf("\n\nPressione alguma tecla para encerrar o programa...");
        break;

      default:
        printf("\n\nOpcao invalida! Pressione alguma tecla para retornar ao menu...");
        fflush(stdin);
        getchar();
    }
  } while (opcao != 0);
   
  fflush(stdin);
  getchar();
  return 0;
}

/************************************************************************************
*** Método: LerArquivos()                                                         ***
*** Função: Ler os arquivos de beneficios e pesos dos objetos                     ***
*************************************************************************************/
void LerArquivos()
{
  int j, valor;
  FILE *arquivo;

  char nomearqB[50] = "beneficio50.txt";
  char nomearqP[50] = "peso50.txt";
  
  //Ler os benefícios dos objetos
  arquivo = fopen(nomearqB,"r");
	if (!arquivo) {
     printf("O Arquivo %s nao pode ser aberto.\n", nomearqB);
     getchar();
     exit(1);
  }
  j=0;
  while (!feof(arquivo)){
    fscanf(arquivo, "%d", &valor);
    B[j] = valor;
    j++;
  }
  fclose(arquivo);
  
  //Ler os pesos
  arquivo = fopen(nomearqP,"r");
  if (!arquivo) {
     printf("O Arquivo %s nao pode ser aberto.\n", nomearqP);
     getchar();
     exit(1);
  }
  j=0;
  while (!feof(arquivo)){
    fscanf(arquivo, "%d", &valor);
    P[j] = valor;
    j++;
  }
  fclose(arquivo);  
}

/************************************************************************************
*** Método: CalcularFO(TSol s)                                                    ***
*** Função: Calcula o valor da função objetivo da solução s                       ***
*************************************************************************************/
int CalcularFO(TSol s)
{
  int fo,
      beneficio = 0,
      peso = 0,
      inviabilidade = 0,
      penalidade = 400;
      
  for (int i=0; i<n; i++)
  {
    if (s.obj[i] == 1)
    {
      beneficio += B[i];
      peso += P[i];
    }
  }
  
  inviabilidade = MAX(0, peso - capacidade);
  fo = beneficio - (penalidade * inviabilidade);
  
  return fo;
}

/************************************************************************************
*** Método: ImprimirMelhorSol()                                                   ***
*** Função: Imprimir a melhor solucao encontrada                                  ***
*************************************************************************************/
void ImprimirMelhorSol(TSol s)
{
  printf("\n\n\nMELHOR SOLUCAO ENCONTRADA");
  printf("\n*************************");
  printf("\n\nFO: %d", s.fo);
  printf("\n\nObjetos selecionados: \n");
  printf("--------------------\n");
  for (int i=0; i<n; i++)
    if (s.obj[i])
      printf(" %d ",i);

  printf("\n\nObjetos nao selecionados: \n");
  printf("------------------------\n");
  for (int i=0; i<n; i++)
    if (!s.obj[i])
      printf(" %d ",i);
      
  printf("\n\n\nPressione alguma tecla para retornar ao menu...");
  fflush(stdin);
  getchar();
}

/************************************************************************************
*** Método: ImprimirMelhorSol2()                                                   ***
*** Função: Imprimir a melhor solucao encontrada                                  ***
*************************************************************************************/
void ImprimirMelhorSolRK(TSolRK s)
{
  printf("\n\n\nMELHOR SOLUCAO ENCONTRADA");
  printf("\n*************************");
  printf("\n\nFO: %d", s.fo);
  printf("\n\nObjetos selecionados: \n");
  printf("--------------------\n");
  for (int i=0; i<n; i++)
    if (s.obj[i] > 0.5)
      printf(" %d ",i);

  printf("\n\nObjetos nao selecionados: \n");
  printf("------------------------\n");
  for (int i=0; i<n; i++)
    if (s.obj[i] <= 0.5)
      printf(" %d ",i);
      
  printf("\n\n\nPressione alguma tecla para retornar ao menu...");
  fflush(stdin);
  getchar();
}

/************************************************************************************
*** Método: TrocarBit(TSol s, int i)                                              ***
*** Função: Troca um bit da solução s                                             ***
*************************************************************************************/
TSol TrocarBit(TSol s, int i)
{
  if (s.obj[i] == 1)
    s.obj[i] = 0;
  else
    s.obj[i] = 1;
    
  return s;
}


/************************************************************************************
*** Método: GerarSolucaoInicialViavel()                                                 ***
*** Função: Gerar Solucao Inicial Viavel Aleatorimante                                   ***
*************************************************************************************/
TSol GerarSolucaoInicialViavel()
{
  TSol s;
  int ins, //indicador da inserção (1) ou não (0) de um objeto
      peso = 0; //para verificar restrição de capacidade
  
	//inicializacao da solucao
	for (int i=0; i<n; i++)
	   s.obj[i] = 0;
	   
    for (int j=0; j<n; j++)
	{
       ins = irandomico(0,1);
       if (ins == 1)
       {
           peso+= P[j];
           if (peso <= capacidade)
              s.obj[j] = 1;
           else
           {
              peso -= P[j];
              break;
           }
       } 
    }
    // printf("Solucao inicial viavel: peso = %d\n", peso);
    s.fo = CalcularFO(s);
    return s;
}


/************************************************************************************
*** Método: GerarSolucaoInicial()                                                 ***
*** Função: Gerar Solucao Inicial Aleatorimante                                   ***
*************************************************************************************/
TSol GerarSolucaoInicial()
{
  TSol s;
  for (int j=0; j<n; j++)
    s.obj[j] = irandomico(0,1);
    
  s.fo = CalcularFO(s);
    
  return s;
}

/************************************************************************************
*** Método: GerarSolucaoInicial2()                                                ***
*** Função: Gerar Solucao Inicial Aleatorimante float                             ***
*************************************************************************************/
TSolRK GerarSolucaoInicialRK()
{
  TSolRK s;
  for (int j=0; j<n; j++)
    s.obj[j] = randomico(0,1);
    
  s.fo = Decoder(s);
    
  return s;
}

/************************************************************************************
*** Método: Card()                                                                ***
*** Função: Retorna a cardinalidade de um vetor em temos de elementos não nulos   ***
*************************************************************************************/
int Card(int C[])
{
    int c=0;
    for(int i=0; i<n; i++)
       if(C[i]==1) c++;
    return c; // quantos elementos sao iguais a 1
}

/************************************************************************************
*** Método: Construcao()                                                          ***
*** Função: Fase de construcao do algoritmo GRASP                                 ***
*************************************************************************************/
TSol Construcao(float alfa)
// alfa: parâmetro usado na construção da LCR
// (Lista de Candidatos Restrita)
{
  
  TSol s;
  int C[n]; // vetor de candidatos à solucao
  
  // inicialização do conjunto C de candidados
  // C[i]=1 => objeto 'i' nao incluido na solucao
  for(int i=0; i<n; i++)
     C[i]=1;
  
  // inicialização da solucao inicial   
  for (int j=0; j<n; j++)
    s.obj[j] = 0;
  
  // relação "custo/benefício": benefício/peso
  float R[n];
  for (int i=0; i<n; i++)
     R[i]=B[i]/P[i];
     
  // construcao da solucao
  int peso = 0; // para verificar capacidade da mochila
  while (Card(C) != 0)
  {
     // valores máximo e minimo da função adaptativa gulosa
     float Rmin = +999999;
     float Rmax = -999999;
     for(int i=0; i<n; i++)
        if (C[i]==1) { // i-esimo candidato ainda nao adicionado aa solucao
           if (R[i] <= Rmin) Rmin = R[i];
           if (R[i] >= Rmax) Rmax = R[i];
        }
           
      // construção da lista de candidatos restrita 
      int LCR[n];
      // faixa valida funcao de adaptação para um candidato poder ser escolhido
      // float alfa = randomico(0,1);
      // printf("alfa = %f\n", alfa);
  
      float delta = Rmin + alfa * (Rmax - Rmin);
  
      // construcao da LCR
      for (int i=0; i<n; i++)
         if (C[i]==1)
            if(R[i] >= delta) {
               LCR[i]=1;
               // C[i] = 0; // candidato adicionado a LCR
               }
            else
               LCR[i]=0;
         else LCR[i]=0;
               
      int nLCR = Card(LCR); // quantos candidatos na LCR
  
      // printf("\nCardinalidade, LCR = %d\n", nLCR);
      // printf("\nCardinalidade, C = %d\n", Card(C));
  // fflush(stdin);
  // getchar();
        
      while(nLCR > 0) {
        // seleciona, aleatoriamente, um elemento t da LCR        
        int pt = irandomico(1,nLCR); // 'ordem' do valor selecionado aleatoriamente 

        // printf("\nValor de pt = %d", pt);   
        for (int j=0; j<n; j++)
         if (LCR[j]==1) {
            pt--;
            if (pt==0) {
               peso += P[j];
               if (peso <= capacidade) {
                  s.obj[j]=1; // elemento de LCR adicionado à solucao
                  C[j]=0;     // candidato adicionado à solucao
                  nLCR=0;
                  }
               else { // peso > capacidade
                    peso -=P[j]; // desconsidera peso do j-esimo candidato
                    LCR[j] = 0;  // candidato nao pode ser adicionado aa solucao
                    C[j]=0; 
                    nLCR--;
                    }
               break;     
               }               
         } 
      }    // while nLCR > 0                                  
  }                 
  s.fo = CalcularFO(s);
  return s;
}



/************************************************************************************
*** Método: SubidaTrocaBit()                                                      ***
*** Função: Heuristica de subida que examina a troca de todos os bits             ***
*************************************************************************************/
TSol SubidaTrocaBit(TSol s)
{
  int melhorou = 1;
  TSol sLocal = s;       //armazena a melhor solucao vizinha
  
  while (melhorou)
  {
    melhorou = 0;
    
    for (int i=0; i<n; i++)
    {
      //trocar um bit
      s = TrocarBit(s, i);
      s.fo = CalcularFO(s);
      
			//armazenar a melhor troca se a solucao melhorar
      if (s.fo > sLocal.fo)
      {
        sLocal = s;
        melhorou = 1;
      }
        
      //desfazer a troca e continuar a busca
      s = TrocarBit(s, i);      
    }
    
    //continuar a busca a partir da melhor solucao
    s = sLocal;
  }
  
  return s;
}

/************************************************************************************
*** Método: limpar_solucao()                                                      ***
*** Função: Inicializa todos os indivíduos de uma população como "nulos"          ***
*************************************************************************************/
void limpar_populacao(TSol Pop[], int tamPop)
{
     for (int i=0; i<tamPop; i++)
     {
         for (int j=0; j<n; j++)
            Pop[i].obj[j]=0; 
         Pop[i].fo = CalcularFO(Pop[i]);
         }
     return;
}
    
/************************************************************************************
*** Método: limpar_solucao2()                                                      ***
*** Função: Inicializa todos os indivíduos de uma população como "nulos"          ***
*************************************************************************************/
void limpar_populacaoRK(TSolRK Pop[], int tamPop)
{
     for (int i=0; i<tamPop; i++)
     {
         for (int j=0; j<n; j++)
            Pop[i].obj[j]=0; 
         Pop[i].fo = 0;
         }
     return;
}        
        
/************************************************************************************
*** Método: GA()                                                                  ***
*** Função: Aplica a metaheuristica Algoritmo Genetico no Problema da Mochila     ***
*************************************************************************************/
void GA()
{
  int tamPop = 100;
  float probCros = 0.95,
        probMut = 0.005;

  printf("\n\nDigite o tamanho da populacao: ");
  scanf("%d", &tamPop);

  printf("\n\nDigite a probabilidade de crossover: ");
  scanf("%f", &probCros);

  printf("\n\nDigite a probabilidade de mutacao: ");
  scanf("%f", &probMut);

  TSol Pop[tamPop];
  TSol PopInter[tamPop];
  TSol PopFilhos[tamPop];
  
  TSol melhorInd;
  TSol melhorFilho;
  melhorInd.fo = -9999999;

  limpar_populacao(Pop, tamPop);
  limpar_populacao(PopInter, tamPop);
  limpar_populacao(PopFilhos, tamPop);   
          
  //Gerar a população inicial
  for (int i=0; i<tamPop; i++)
  {
    TSol ind = GerarSolucaoInicial();
    Pop[tamPop-i-1]=ind;
  }


  int numGeracoes = 0;
  while (numGeracoes < 1000)
  {
    numGeracoes++;

    //Selecionar os pais por meio do método de seleção por TORNEIO
    for (int j=0; j<tamPop; j++)
    {
      int p1, p2, p3, melhor;

      p1 = irandomico(0,tamPop-1);
      p2 = irandomico(0,tamPop-1);
      p3 = irandomico(0,tamPop-1);

      if ((Pop[p1].fo > Pop[p2].fo) && (Pop[p1].fo > Pop[p3].fo))
				melhor = p1;
      else
      if (Pop[p2].fo > Pop[p3].fo)
        melhor = p2;
			else
        melhor = p3;

			//inserir o melhor indivíduo na população intermediaria 
			PopInter[tamPop-j-1]=Pop[melhor];
		}


    //***********************************************************************
    //  realizar crossover de 2 pontos entre os pares de pais selecionados
    //***********************************************************************
    int pcorte1 = 0;
    int pcorte2 = 0;

    //realizar o crossover até o número de filhor ser igual a tamPop
    for (int i=0; i<tamPop; i=i+2)
    {
      PopFilhos[tamPop-i-2] = PopInter[i];
      PopFilhos[tamPop-i-1] = PopInter[i+1];

      //verificar a probalidade de crossover
      if (randomico(0,1) <= probCros)
      {
        pcorte1 = irandomico(1,n-1);
        pcorte2 = irandomico(1,n-1);

        if (pcorte1 > pcorte2)
				{
          int temp = pcorte1;
          pcorte1 = pcorte2;
          pcorte2 = temp;
        }

        for (int j=pcorte1; j<pcorte2; j++)
        {
          PopFilhos[i].obj[j] = PopInter[i+1].obj[j];
          PopFilhos[i+1].obj[j] = PopInter[i].obj[j];
        }
      }
    }

    //***********************************************************************
	//       realizar mutação bit a bit nos filhos gerados - trocar bit
	//***********************************************************************
	for (int i=0; i<tamPop; i++)
	{
   	  //verificar a probabilidade de realizar mutacao em cada bit do filho
      for (int j=0; j<n; j++)
      {
        if (randomico(0,1) <= probMut)
          PopFilhos[i] = TrocarBit(PopFilhos[i], j);
      }
    }


    //Avaliar a populaçao de filhos
    melhorFilho.fo = -99999999;
    int mediaFilhos = 0;
    for (int i=0; i<tamPop; i++)
    {
      PopFilhos[i].fo = CalcularFO(PopFilhos[i]);

			mediaFilhos += PopFilhos[i].fo;

      //encontrar o melhor filho desta geracao
      if (PopFilhos[i].fo > melhorFilho.fo)
				melhorFilho = PopFilhos[i];

      //encontrar o melhor filho do GA
      if (PopFilhos[i].fo > melhorInd.fo)
        melhorInd = PopFilhos[i];
    }

	printf("Geracao: %d  media = %d   melhor individuo = %d  \n", numGeracoes, (mediaFilhos/tamPop), melhorInd.fo);

    //Limpar Pop e PopInter
    limpar_populacao(Pop, tamPop);
    limpar_populacao(PopInter, tamPop);
   
     
    //substituir a população pelos filhos gerados
    for (int i=0; i<tamPop; i++)
       Pop[i] = PopFilhos[i];

    //Elitismo - atribuir o melhor individuo na Pop
    int pos1 = irandomico(0,tamPop-1);
		Pop[pos1] = melhorInd;

    // PopFilhos.clear();
    limpar_populacao(PopFilhos, tamPop);
            
  }

  ImprimirMelhorSol(melhorInd); 
}

/************************************************************************************
*** Método: Particao()                                                            ***
*** Função: Metodo utilizado pelo Quick sort para ordenar uma particao do vetor   ***
*************************************************************************************/
/*int Particao(int p, int r)
{
	TSolRK pivo = Pop[p];
	int topo = p;
	
	for (int i=p+1; i<=r; i++)
	{
		if (Pop[i].fo > pivo.fo)
		{
			Pop[topo] = Pop[i];
			Pop[i] = Pop[topo+1];
			topo++;
		}
	}	
	Pop[topo] = pivo;
	
	return topo;
}*/

/************************************************************************************
*** Método: QuickSort()                                                           ***
*** Função: Ordena a populacao do BRKGA                                           ***
*************************************************************************************/
/*void QuickSort(int p, int r)
{
	int q;	
	if (p < r)
	{
		q = Particao(p,r);
		QuickSort(p, q-1);
		QuickSort(q+1, r);
	}
}*/

/************************************************************************************
*** Método: Decoder()                                                             ***
*** Função: Trasnforma uma solucao de chaves aleatorias em uma solucao do problema***
*************************************************************************************/
int Decoder(TSolRK s)
{
  int fo,
      beneficio = 0,
      peso = 0,
      inviabilidade = 0,
      penalidade = 400;
      
  for (int i=0; i<n; i++)
  {
    if (s.obj[i] > 0.5)
    {
      beneficio += B[i];
      peso += P[i];
    }
  }
  
  inviabilidade = MAX(0, peso - capacidade);
  fo = beneficio - (penalidade * inviabilidade);
  
  return fo;
}

/************************************************************************************
*** Método: BRKGA()                                                               ***
*** Função: Aplica a metaheuristica BRKGA no Problema da Mochila                  ***
*************************************************************************************/
void BRKGA()
{
  //int n = 50;		    // size of chromosomes
  int p = 100;	        // size of population
  double pe = 0.20;	    // fraction of population to be the elite-set
  double pm = 0.10;	    // fraction of population to be replaced by mutants
  double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent  
  
  int Tpe;
  int Tpm;   
      
  printf("\n\nDigite o tamanho da populacao: ");
  scanf("%d", &p);

  printf("\n\nDigite a fracao da elite: ");
  scanf("%f", &pe);

  printf("\n\nDigite a fracao de mutantes: ");
  scanf("%f", &pm);
  
  printf("\n\nDigite a probalidade de herdar alelos do pai elite: ");
  scanf("%f", &rhoe);


  //calcular o tamanho das populacoes elite e mutantes
  Tpe = (int)(p*pe);
  Tpm = (int)(p*pm);

  TSolRK Pop[p];  //populacao do BRKGA
  TSolRK PopInter[p];
  
  TSolRK melhorInd;
  melhorInd.fo = -9999999;

  limpar_populacaoRK(Pop, p);
  limpar_populacaoRK(PopInter, p);
          
  //Gerar a população inicial
  for (int i=0; i<p; i++)
  {
    TSolRK ind = GerarSolucaoInicialRK();
    Pop[p-i-1] = ind;
  }
   
  //ordenar a populacao inicial (quick sort)   
  //QuickSort(0,p-1);   
 
   
  //ordenar a populacao inicial (bubble sort)
  for(int i=p-1; i>0; i--)
  {
    for(int j=0; j<i; j++)
    {
        if(Pop[j+1].fo > Pop[j].fo)
        {				 
            TSolRK aux = Pop[j];
            Pop[j] = Pop[j+1];
            Pop[j+1] = aux;            
        }
    }
  }


  int numGeracoes = 0;
  while (numGeracoes < 1000)
  {
    numGeracoes++;

	// We now will set every chromosome of 'current', iterating with 'i':
	int i = 0;	// Iterate chromosome by chromosome
	int j = 0;	// Iterate allele by allele

	// The 'pe' best chromosomes are maintained, so we just copy these into 'current':
	while(i < Tpe) {
		PopInter[i] = Pop[i];
		++i;
	}

	// We'll mate 'p - pe - pm' pairs; initially, i = pe, so we need to iterate until i < p - pm:
	while(i < p - Tpm) {  
		// Select an elite parent:
		int eliteParent = irandomico(0,Tpe - 1);

		// Select a non-elite parent:
		int noneliteParent = Tpe + irandomico(0, p - Tpe - 1);

		// Mate:
		for(j = 0; j < n; ++j) {
			int sourceParent;
            if (randomico(0,1) < rhoe)
               sourceParent = eliteParent;
            else 
               sourceParent = noneliteParent;

			PopInter[i].obj[j] = Pop[sourceParent].obj[j];
		}

		++i;
	}

	// We'll introduce 'pm' mutants:
	while(i < p) {
		for(j = 0; j < n; ++j) 
        { 
           PopInter[i].obj[j] = randomico(0,1); 
        }
		++i;
	}
	
    //Calculate fitness
    for (i=0; i<p; i++)
    {
      PopInter[i].fo = Decoder(PopInter[i]);

      //encontrar o melhor filho do BRKGA
      if (PopInter[i].fo > melhorInd.fo)
        melhorInd = PopInter[i];
    }    

	printf("Geracao: %d  melhor individuo = %d  \n", numGeracoes, melhorInd.fo);

    //Limpar Pop
    limpar_populacaoRK(Pop, p);   
     
    //substituir a população pelos filhos gerados
    for (int i=0; i<p; i++)
       Pop[i] = PopInter[i];
       
    //ordenar a populacao 
    //QuickSort(0,p-1);  
    for(int i=p-1; i>0; i--)
    {
        for(int j=0; j<i; j++)
        {
            if(Pop[j+1].fo > Pop[j].fo)
            {				 
                TSolRK aux = Pop[j];
                Pop[j] = Pop[j+1];
                Pop[j+1] = aux;            
            }
        }
    }       

    //Limpar a PopInter   
    limpar_populacaoRK(PopInter, p);            
  }

  ImprimirMelhorSolRK(melhorInd); 
}


/************************************************************************************
*** Método: SA()                                                                  ***
*** Função: Aplica a metaheuristica Simulated Annealing no Problema da Mochila    ***
*************************************************************************************/
void SA()
{
  TSol s,                   //solução corrente do SA
       sMelhor,             //melhor solução do SA
       sViz;                //solução vizinha

  int delta;

  //Ler os parametros
  float T0 = 1000000,         //temperatura inicial
        T,                    //temperatura corrente
		alfa = 0.95;          //taxa de resfriamento

  int SAmax = 1000,           //numero maximo de iteracoes
      IterT;                  //iteracao corrente

  //Usuario entra com os parametros (comentar para testar)
  printf("\nDigite a temperatura inicial: ");
  scanf("%f", &T0);

  printf("\nDigite o numero de iteracoes (SAmax): ");
  scanf("%d", &SAmax);

  printf("\nDigite a taxa de resfriamento: ");
  scanf("%f", &alfa);


  //Gerar uma solucao inicial aleatoriamente
  s = GerarSolucaoInicial();
  printf("\nSol Inicial: SA: ");
  ImprimirMelhorSol(s);

  sMelhor = s;
  IterT = 0;
  T = T0;

  while (T > 0.0001)
  {
    while (IterT < SAmax)
    {
      IterT++;
      //escolher um objeto aleatoriamente
      int pos = irandomico(0, n-1);

      //gerar vizinho
      sViz = TrocarBit(s, pos);

      //calcular a FO
      sViz.fo = CalcularFO(sViz);

      delta = sViz.fo - s.fo;

      //definir a partir de qual solucao continuar a busca
      if (delta > 0)
      {
        s = sViz;
        if (s.fo > sMelhor.fo)
          sMelhor = s;
      }
      else
      {
        double x = randomico(0,1);

        if ( x < (exp(delta/T)) )       //1 + delta/T
          s = sViz;
      }
    } //Fim-SAmax

    printf("\nT = %.4f \t FO = %d \t melhorFO = %d", T, s.fo, sMelhor.fo);

    T = T * alfa;
    IterT = 0;

  } //Fim-T

  ImprimirMelhorSol(sMelhor);
}



/************************************************************************************
*** Método: TS()                                                                  ***
*** Função: Aplica a metaheuristica Busca Tabu no Problema da Mochila             ***
*************************************************************************************/
void TS()
{
  TSol s,
       sMelhor,
       sViz,
       sMelhorViz;

  int iter,                   //numero de iteracoes
      melhorIter,             //iteracao na qual foi obtida a melhor solucao
      BTmax,                  //numero maximo de iteracoes sem melhora
      A,                      //funcao de aspiracao
      tabu,                   //tamanho da lista tabu
      pos,                    //posicao do bit trocado no melhor vizinho
      numTabu;                //numero de movimentos tabu em uma iteracao

  int T[n];                   //lista tabu


  //definicao dos parametros
  iter = 0;
  melhorIter = 0;
  BTmax = 3000;
  tabu = 5;

  printf("\n\nDigite o tamanho da lista tabu: ");
  scanf("%d",&tabu);

  printf("\nDigite o numero maximo de iteracoes sem melhora: ");
  scanf("%d",&BTmax);


  //Gerar uma solucao inicial aleatoriamente
  s = GerarSolucaoInicial();

  sMelhor = s;

  //inicializar lista tabu
  for(int i=0; i<n; i++)
    T[i] = 0;

  //função de aspiração
  A = sMelhor.fo;


  while (iter - melhorIter <= BTmax)
  {
    iter++;

    //encontrar o melhor vizinho s' que não seja tabu ou satisfaça o criterio de aspiracao
    sMelhorViz.fo = -9999999;
    numTabu = 0;
    for (int i=0; i<n; i++)
    {
      //trocar bit
      sViz = TrocarBit(s, i);
      sViz.fo = CalcularFO(sViz);

      //movimento nao tabu ou criterio de aspiracao atendido
      if ( (T[i] == 0) || (iter - T[i] > tabu) || (sViz.fo > A) )
      {
				//verficar se é o melhor vizinho
        if (sViz.fo > sMelhorViz.fo)
        {
          sMelhorViz = sViz;
          pos = i;
        }
      }
      else
        numTabu++;
    }

    //atualizar a lista tabu
    if (numTabu < n)
    {
      T[pos] = iter;
    }
    else
    {
      //escolher o movimento mais antigo
      int antigo = 999999;
      for (int i=0; i<n; i++)
        if (T[i] < antigo)
          antigo = i;

      //realizar o movimento mais antigo
      sMelhorViz = TrocarBit(s,antigo);
      T[antigo] = iter;
    }

    //continuar a busca a partir da melhor solucao vizinha
    s = sMelhorViz;

    //verificar se é a melhor solucao encontrada até o momento
    if (s.fo > sMelhor.fo)
    {
      sMelhor = s;
      melhorIter = iter;
    }

    //atualizar a funcao de aspiracao
    A = sMelhor.fo;

    printf("\nIteracao: %04d \t FO: %d \t melhorFO: %d", iter, s.fo, sMelhor.fo);
  }

  ImprimirMelhorSol(sMelhor);
}




/************************************************************************************
*** Método: GRASP()                                                               ***
*** Função: Aplica a metaheuristica GRASP no Problema da Mochila                  ***
*************************************************************************************/
void GRASP()
{
  TSol s, sMelhorViz, sMelhor; // s: sol. construida;
                               // sMelhorViz: melhor vizinho encontrado a partir de s
                               // sMelhor: solucao com maior beneficio 

  // definicao dos parametros
  float alfa = 0.99; // para a fase de construção
  int GRASPmax = 1000;  // numero maximo de iterações
  
  // printf("\n\nMeta-heuristica ainda nao implementada!");
  printf("\nMeta-heuristica sendo implementada ... ");
  
  printf("\n\nDigite o valor de alfa p/ a fase de construcao (intervalo [0,1]: ");
  scanf("%f",&alfa);

  printf("\nDigite o numero maximo de iteracoes para o metodo GRASP: ");
  scanf("%d",&GRASPmax);
  
  // aplicacao das fases da metaheuristica
  sMelhor.fo = -999999;
  
  for (int iter=1; iter <= GRASPmax; iter++)
  {
     // construcao de uma solucao "gulosa" com base em alfa
     s = Construcao(alfa);
     // printf("\n\n=====================\n");
     // printf("\n\n\nSolucao construida: ");
     // ImprimirMelhorSol(s);
     
     // busca de um otimo local na vizinhança escolhida
     sMelhorViz = SubidaTrocaBit(s);
     // printf("\nSolucao melhorada, iteracao %d: ", iter);
     // ImprimirMelhorSol(sMelhorViz);
     
     // verifica qualidade da solucao encontrada
     if(sMelhorViz.fo > sMelhor.fo) 
        sMelhor = sMelhorViz;                 
  }
  s = sMelhor;
  
  // solucao final obtida 
  ImprimirMelhorSol(s);
    
}




/************************************************************************************
*** Método: VNS()                                                                 ***
*** Função: Aplica a metaheuristica VNS no Problema da Mochila                    ***
*************************************************************************************/
void VNS()
{
  TSol s, sViz, sMelhor;

  int k, r;

  //numero de estruturas de vizinhaça
	r = 6;

  //GerarSolucaoInicial
  s = GerarSolucaoInicial();
  sMelhor = s;

  int numIteracoes=0;
  while (numIteracoes <= 5000)
  {
    //tipo de estrurura de vizinhança
    k = 1;

    while (k <= r)
    {
      //comecar a busca a partir da melhor solucao
      sViz = sMelhor;

      //gerar o vizinho aleatoriamente na vizinhança k
      numIteracoes++;
      
	  for (int i=0; i<k; i++)
      {
        int pos = irandomico(0,n-1);
        sViz = TrocarBit(sViz, pos);
      }
      
      //aplicar a busca local no vizinho gerado
      sViz = SubidaTrocaBit(sViz);
			sViz.fo = CalcularFO(sViz);

      //se melhorar, continuar a busca a partir da solucao vizinha e retornar para 1a vizinhança
      if (sViz.fo > sMelhor.fo)
      {        
		s = sViz;
        sMelhor = s;
        k = 1;
      }
      //senao, continuar a busca a partir da solucao anterior e partir para proxima vizinhança
      else
      {
        k++;
      }

      printf("\nVizinho: %d  fo: %8d  Melhor Solucao = %d ", numIteracoes, sMelhor.fo, s.fo);
    }
  }
  
  ImprimirMelhorSol(sMelhor);
}






/*****************************************************************************
*** Método: ITERATED LOCAL SEARCH                                          ***
*** Funcao: ILS implementado para resolver o problema da mochila           ***
******************************************************************************/
void ILS()
{
  double beta = 0;                            //taxa de perturbação
  int Iter = 0;
  int IterMax = 5000;
  int IterMelhora = 0;
  int delta = 0;

  TSol s,
       sMelhor,     
       sViz,
       sMelhorViz;

  //***Criar a solucao inicial do ILS
  s = GerarSolucaoInicial();
  
  //aplicar busca local
  sMelhor = SubidaTrocaBit(s);
  sMelhor.fo = CalcularFO(sMelhor);

  while (Iter < IterMax)
  // while(Iter-IterMelhora < IterMax) // alteracao Galeno
  {
    Iter++;

		//s' <- perturbar a melhor solucao s*
    beta = randomico(0.2,0.4);

    //perturbar a solucao corrente (sMelhor)
    double pert = n * beta;
    sViz = sMelhor;

    for (int i=0; i<pert; i++)
    {
      int pos = irandomico(0,n-1);
      sViz = TrocarBit(sViz,pos);
    }

    sViz.fo = CalcularFO(sViz);

    //s*' <- busca local (s')
    sMelhorViz = SubidaTrocaBit(sViz);
    sMelhorViz.fo = CalcularFO(sMelhorViz);


    //s* <- criterio de aceitação (s*,s*', historico)
    delta = sMelhorViz.fo - sMelhor.fo;
    if (delta > 0)
    {
      sMelhor = sMelhorViz;
      IterMelhora = Iter;
		}

    //aplicar history
    if (Iter - IterMelhora > 1000)   //1000 iteracoes sem melhora
    {
      //reiniciar a busca
//      sMelhor = GerarSolucaoInicial();
//      IterMelhora = Iter;
    }

    printf("\nIter: %d \t fo: %8d ", Iter, sMelhor.fo);

  } //fim-while_ILS

  ImprimirMelhorSol(sMelhor);
}




/************************************************************************************
*** Método: ObjetosViaveisFormiga()                                               ***
*** Função: Constroi um vetor contendo os objetos capazes de levar a uma          ***
***    solução viável com relação à capacidade da mochila                         ***
*************************************************************************************/
void ObjetosViaveisFormiga(TSol s, int peso, int N[])
// N[]: vetor contendo todos os objetos ainda não incluido em s
// e capazes de levar a uma solucao viavel
{
   for(int i=0; i<n; i++)
      if (s.obj[i]==0) // objeto nao adicionado
         if((peso+P[i]) <= capacidade)
            N[i] = 1;
         else
            N[i]=0; // i-esimo objeto excede a capacidade
      else
         N[i]=0; // ojeto já escolhido    
   return;    
}



/************************************************************************************
*** Método: GerarSolucaoFormiga()                                                 ***
*** Função: Constroi uma solucao para a k-ésima formiga                           ***
*************************************************************************************/
TSol GerarSolucaoFormiga(int k, float Fer[], float Heur[], float alfa, float beta)
// k: k-esima formiga
// Fer[]: vetor contendo os valores de feromônio depositados
// Heur[]: vetor contendo informação heurística definida a priori
// alfa, beta: parâmetros p/ o cálculo da probabilidade de escolha de um objeto
{
   TSol s;  // solução a ser gerada pela k-esima formiga 
   // inicializacao da solucao
   for (int i=0; i<n; i++)
      {s.obj[i]=0; s.fo=0;}
   
   // objeto inicial: k-esima formiga
   int pos = irandomico(0, n-1);
   s.obj[pos] = 1;

   // peso do objeto: capacidade da mochila
   int peso = P[pos];
   
   // conjunto de objetos não incluidos que podem levar a uma solucao viavel
   int N[n];
   ObjetosViaveisFormiga(s,peso,N);
   int nObj = Card(N);

   // escolha dos próximos objetos 
   // ainda não incluidos e satiafazendo a restrição de capacidade)
   while (nObj > 0)
   {
     // printf("\nSolucao Formiga: nObj = %d\n", nObj);
    // fflush(stdin);
    // getchar();
      // escolha probabilística do próximo objeto
      double SomaFerHeur = 0;
      for (int j=0; j<n; j++)
         if (N[j]==1) // objeto valido
         {
            // printf("\nCalculo da soma de probabilidades: ");
            // printf("\nObjeto: %d Feromonio: %f  Heuristica: %f\n", j, Fer[j], Heur[j]);
            SomaFerHeur += (pow(Fer[j],alfa) * pow(Heur[j],beta)); 
         }
      // printf("\nSoma probabilidades: %f\n", SomaFerHeur);
      // identifica objeto escolhido
      for (int j=0; j<n; j++)
      {
          double x = randomico(0,1);   
          if (N[j] == 1) // objeto valido
          {
             double pr_obj = (pow(Fer[j],alfa) * pow(Heur[j],beta))/SomaFerHeur;
             // printf("\n x = %f\n", x);
             // printf("\nprobabilidade = %f\n", pr_obj);
             if (x < pr_obj)
             {
                s.obj[j]=1;
                peso += P[j];
                // printf("\nObjeto escolhido ... ");
                break;
             }
          }
      }
      // atualiza conjunto de objetos não incluidos
      ObjetosViaveisFormiga(s,peso,N);
      nObj = Card(N);
   } // while
   s.fo = CalcularFO(s);
   return s;
} // GerarSolucaoFormiga




/************************************************************************************
*** Método: Destruir()                                                            ***
*** Função: método p/ destruição de uma solução, conduzindo a uma sol. inviavel   ***
*************************************************************************************/
TSol Destruir(TSol s, int A[])
{

   int peso = 0; // p/ verificar restrição de capacidade
   for(int i=0; i<n; i++)
      if (s.obj[i]==1) // i-esimo objeto na solucao atual
          peso += P[i]; // acumula peso do objeto i
   
   // Inicialização do vetor de objetos adicionados
   for (int i=0; i<n; i++)
      A[i] = 0;
          
   // destroi a sol corrente, gerando uma solução que viola a restricao de capacidade
   // quantidade maxima de objetos a adicionar
   int nObj = (int) (0.10*n) > n-Card(s.obj) ? n - Card(s.obj) : (int) (0.10*n);
   int ind; // indice de objeto a ser acrescentado

   // printf("\nObjetos a incluir ... %d\n", nObj);
   while (peso <= capacidade)
   {       
      for (int i=1; i<=nObj; i++) // controle do nro de objetos a modificar
      {
         do
            ind = irandomico(0, n-1);
         while (s.obj[ind]==1); // objeto ja faz parte da solucao
         if (s.obj[ind]==0) // obj. nao faz parte da solucao
         {
            s.obj[ind]=1; // acrescenta objeto
            A[ind] = 1; // atualiza vetor c/ objetos adicionados
            peso += P[ind];
         }
      }
      // printf("\nPeso = %d\n", peso);
   } // while
   /* printf("\nOPeso da mochila: %d\n", peso);
   printf("\nQuant. objetos adicionados: "); */

   // IMPRESSAO DE CONTROLE DO NRO DE OBJETOS ADICIONADOS ---- APAGAR
   if (Card(A) > (int) (0.1*n))
      printf("\nCardinalidade de A = %d\n", Card(A));

   // solucao destruida
   s.fo = CalcularFO(s);
   return(s);
}



/************************************************************************************
*** Método: Construir()                                                            ***
*** Função: método p/ reparação de uma solução, obtendo uma solução viavel  a      ***
***         partir de uma solução inviável                                         ***
*************************************************************************************/
TSol Reparar(TSol s, int A[])
{
   // restricao de capacidade: peso da solucao atual 
   int peso = 0;
   for (int i=0; i<n; i++)
      if (s.obj[i]==1) peso += P[i];
   
   // retira objetos adicionados um a um até que restrição seja satisfeita
   // printf("\npeso - inicio reparacao: %d", peso);          
   while (peso > capacidade)
   {
      int ind = irandomico(0,n-1);
      if (s.obj[ind]==1) // objeto i pertence à solução
      {
            // if (A[ind]==1) {
               s.obj[ind] = 0; // retira o i-ésimo elemento
               peso -= P[ind]; // atualizacao do peso        
            // }
      }
      // printf("\npeso - durante reparacao: %d", peso);
   } // while 
   
   // solucao reparada
   s.fo = CalcularFO(s);
   return(s);
}



/************************************************************************************
*** Método: Aceitar()                                                             ***
*** Função: Compara duas soluções sinalizando de uma sol. temporária deve         ***
***         ser aceita ou não                                                     ***
*************************************************************************************/
int Aceitar(TSol sTemp, TSol s, float T)
{
      int delta = sTemp.fo - s.fo;

      // verificar se uma solução deve ser aceita ou não
      // com probabilidade da metaheurística "Simulated Annealing"
      if (delta > 0)
         return 1; // sol. aceita: aumenta o valor da fo
      else
      {
        double x = randomico(0,1);
        if ( x < (exp(delta/T)) )   // Sol. aceita c/ prob. da metaheurística SA
          return 1;
      }
  return 0;  // Sol. não aceita
}



/************************************************************************************
*** Método: ACO()                                                                 ***
*** Função: Aplica a metaheuristica Colonia de Formigas no Problema da Mochila    ***
*************************************************************************************/
void ACO()
{
	printf("\n\nMeta-heuristica sendo implementada!");
  // printf("\n\nPressione alguma tecla para retornar ao menu...");
  
  TSol s, sMelhor; // sol. obtida para cada formiga; melhor solucao
  float DeltaFer[n], // qtd de feromonio acumulada em cada passo 
        DeltaFerk, // qtd de rastro deixado pela k-esima formiga
        Fer[n], // qtd de feromonio em cada objeto
        Heur[n], // informacao heuristica disp. a priori p/ cada objeto
        alfa=0.5, // parametro, calc. probabilidade escolha (feromonio)
        beta=0.5, // parametro, calc. probabilidade escolha (heuristica)
        ro=0.3; // parametro: taxa de evaporação de feromônio
  int ACOmax = 3000, // numero maximo de iterações sem melhora da solução
      m=2;  // quantidade de formigas
        
  // Quantidade inicial de feromônio em cada objeto
  float Fer0 = 1; // FO = 100 => 1/FO = 0.01
  // Inicialização
  for (int i=0; i<n; i++)
  {
     DeltaFer[i] = 0;  // quantidade de feromônio depositada em cada objeto
     Fer[i] = Fer0; // quantidade de feromonio inicial em cada objeto
     // Heur[i] = ((float) B[i])/P[i]; // info. heuristica disp. a priori p/ cada objeto
  }
  // informacao heuristica disponivel a priori para cada objeto
  int Bmax = B[0];
  for (int i=1; i<n; i++)
     if (Bmax < B[i]) Bmax=B[i];
  for (int i=1; i<n; i++)
     Heur[i] = ((float) B[i])/Bmax;
  
  // entrada de dados: parametros para aplicacao da metaheuristica
  printf("\n\nValor de alfa p/ calc de probabilidade de escolha (feromonio): ");
  scanf("%f",&alfa);
  printf("\n\nValor de beta p/ calc de probabilidade de escolha (heuristica): ");
  scanf("%f",&beta);
  printf("\n\nValor de ro: taxa de evaporacao de feromonio: ");
  scanf("%f",&ro);
  printf("\nDigite a quantidade de formigas: ");
  scanf("%d",&m);
  printf("\nDigite o numero maximo de iteracoes para o metodo ACO: ");
  scanf("%d",&ACOmax);
  
  
  // aplicacao dos passo da metaheuristica
  int Iter=0, MelhorIter=0;
  sMelhor.fo=-999999;
  while(Iter-MelhorIter <= ACOmax)
  {
     Iter++;
     // obtenção de uma solução para cada formiga
     for (int k=1; k<=m; k++)
     {
         s = GerarSolucaoFormiga(k, Fer, Heur, alfa, beta);
         // printf("\nSolucao: formiga %d: \n", k);
         // ImprimirMelhorSol(s);
         if (s.fo > sMelhor.fo)
         {
            sMelhor.fo=s.fo;
            MelhorIter = Iter;         
         }
         // calculo da quantidade de rastro deixado pela formiga k
         for (int i=0; i<n; i++)
         {
            if (s.obj[i]==1) // i-esimo objeto escolhido
               DeltaFerk = 1.0/s.fo; // qtd feromonio depositada formiga k
            else DeltaFerk = 0.0;
            DeltaFer[i] += DeltaFerk;
         } // for i
     } // for k
     // Atualizacao das quantidades de feromonio em cada objeto
     for (int j=0; j<n; j++)
        Fer[j] = (1 - ro) * Fer[j] + DeltaFer[j];   
        
    printf("\nIter: %d \t fo: %8d ", Iter, sMelhor.fo);
  } // while
  
  // saida melhor solucao obtida
  ImprimirMelhorSol(sMelhor);

}




/************************************************************************************
*** Método: LNS()                                                                 ***
*** Função: Aplica a metaheuristica "Large Neighborhood Search"                   ***
***         no Problema da Mochila                                                ***
*************************************************************************************/
void LNS()
{
	printf("\n\nMeta-heuristica sendo implementada!");
  // printf("\n\nPressione alguma tecla para retornar ao menu...");
  
  TSol s,         //  sol. inicial; sol. aceita a cada iteração;
       sMelhor,   //  melhor solucao
       sTemp1, sTemp2,    //  soluções temporárias 
       sAnt;      //  copia da solucao de entrada numa iteração
  float Heur[n];     // informação heurística disponível a priori p/ cada obj.
  int  A[n],  // vetor c/ info sobre os elementos adicionados em "Destruir"
       LNSmax = 3000;// numero maximo de iterações sem melhora da solução
  
  // dados da metaheuristica SA p/ condicao de aceitacao
  float T0 = 100000,         //temperatura inicial
        T,                    //temperatura corrente
    	alfa = 0.95;          //taxa de resfriamento
  
  // informacao heuristica disponivel a priori para cada objeto
  int Bmax = B[0];
  for (int i=1; i<n; i++)
     if (Bmax < B[i]) Bmax=B[i];
  for (int i=1; i<n; i++)
     Heur[i] = ((float) B[i])/Bmax;
  
  // entrada de dados: parametros para aplicacao da metaheuristica
  printf("\nDigite o numero maximo de iteracoes para o metodo LNS: ");
  scanf("%d",&LNSmax);
  
  // aplicacao dos passos da metaheuristica
  // geração da solução inicial
  s = GerarSolucaoInicialViavel();
  printf("\nSol inicial: "); 
  ImprimirMelhorSol(s);
  sMelhor = s;
  // parâmetros: critério de parada
  int Iter=0, MelhorIter=0;
  T = T0;
  while(Iter-MelhorIter <= LNSmax)
  {
     Iter++;
     // obtenção de uma solução temporaria
     sAnt = s;
     sTemp1 = Destruir(s,A);  // otendo uma sol. inviavel
     // printf("\nSolucao apos destruicao: \n");
     // ImprimirMelhorSol(sTemp);
     sTemp2 = Reparar(sTemp1,A);   // voltando para um sol. viavel
     // verifica se sol temporaria é aceita
     if (T < 0.001) T = 0.001;
     if (Aceitar(sTemp2,sAnt,T))
     {
         s = sTemp2;
        //  printf("\nSolucao aceita: ");
        //  ImprimirMelhorSol(s);
     }
     else 
        {
            s = sAnt;    
           //  printf("\nSolucao não aceita: ");
           //  ImprimirMelhorSol(s);
         }
     // verifica se a nova solução é melhor do que a melhor solução conhecida
     if (sTemp2.fo > sMelhor.fo)
     {
        sMelhor = sTemp2;
        MelhorIter = Iter;
     } 
     T *= alfa;  
  } // while
  
  // saida melhor solucao obtida
  ImprimirMelhorSol(sMelhor);

}



/************************************************************************************
*** Método: randomico(double min, double max)                                     ***
*** Função: Gera um numero aleatorio entre min e max                              ***
*************************************************************************************/
double randomico(double min, double max)
{
	return ((double)(rand()%10000)/10000.0)*(max-min)+min;
}

/************************************************************************************
*** Método: irandomico(int min, int max)                                          ***
*** Função: Gera um numero inteiro aleatorio entre min e max                      ***
*************************************************************************************/
int irandomico(int min, int max)
{
	return (int)randomico(0,max-min+1.0) + min;
}
