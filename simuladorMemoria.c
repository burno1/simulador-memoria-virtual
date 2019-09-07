#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define ARQUIVO "processos.txt"

typedef char texto[4];

typedef struct _MEMORIA {
	int *paginas;
	texto *frameProcId;
	int *tempo;
	int *tempoInalterado;
	int *bitRef;
} MEMORIA;

typedef struct _PROCESSO {
	texto id;
	int *tabelaPagina;
	unsigned long int tamProcesso;
	int qtdePagina;
	
} PROCESSO;

int main (int argc, char* argv[]) {
	int escolha,tamPagina, tamMemoria, qtdeProcessos, paginasQnt;
	PROCESSO *processo; 
	
	do{
	printf("A pagina e a memoria devem ser multiplos entre si\n");
	printf("Qualquer valor nao multiplo nao sera valido\n");
	printf("---------------------------------------------------------------------\n");
	printf("Pressione ENTER para continuar\n");
	printf("---------------------------------------------------------------------\n");
	getch();
	printf("Informe tamanho de memoria: ");
	scanf("%d", &tamMemoria);
	printf("Informe tamanho de pagina: ");
	scanf("%d",&tamPagina);
	
	
	} while (verificarParametros(&tamPagina,&tamMemoria));
	
	printf("---------------------------------------------------------------------\n");
	qtdeProcessos = contaProcessos(); // quantidade de processoos que ser√£o criados
	paginasQnt = tamMemoria / tamPagina; // quantos paginas na mem√≥ria principal
	printf("\nMemoria: %d\t\tQTDE de paginas da memoria: %d\n\n",tamMemoria, tamPagina, paginasQnt);
	processo = (PROCESSO*)malloc(sizeof(PROCESSO) * qtdeProcessos);
	processoScale(processo); // ler os tamanhos dos processoos
	inicializarTabelas(processo, tamPagina, qtdeProcessos); // inicializar mem√≥ria principal
	printf("---------------------------------------------------------------------\n");
	printf("Pressione ENTER para continuar\n");
	printf("---------------------------------------------------------------------");
	getch();
	
	
	limparTela();
	printf("----------------------------\n");
	printf("Qual algoritmo deseja usar ?\n");
	printf("----------------------------\n");
	
	printf("1 - LRU \n");
	printf("2 - LRU APROXIMADO\n");
	printf("3 - SEGUNDA CHANCE\n");
	
	scanf("%d",&escolha);
	
	switch(escolha){
		case 1:
			limparTela();
			printf("---------------------------LRU---------------------------------------\n");
			simuladorMemoria(tamPagina, paginasQnt,  processo, qtdeProcessos, escolha);
			printf("\nPressione ENTER para encerrar\n");
			getchar();
			break;
		case 2:
			limparTela();
			printf("\n-------------------------LRU-Aproximado----------------------------\n");
			simuladorMemoria(tamPagina, paginasQnt,  processo, qtdeProcessos, escolha);
			printf("\nPressione ENTER para encerrar\n");
			getchar();
			break;	
		case 3:
			limparTela();
			printf("\n------------------------Segunda-Chance-----------------------------\n");
			simuladorMemoria(tamPagina, paginasQnt,  processo, qtdeProcessos, escolha);
			printf("\nPressione ENTER para encerrar\n");
			getchar();
			break;
					
	}
	
	liberaMemoria(processo, qtdeProcessos);
	return 0;
}

//Simula a memoria com seus algoritmos, usando parametros adquiridos no main para tamanho de processo e a lista de processos passada como ponteiro
int simuladorMemoria (int tamPagina, int paginasQnt,PROCESSO *processo, int processoCont, int modo){
	MEMORIA memoriaPrincipal; 
	int flag1, flag2, cont = 0, contPaginas, pos, numPage, numFrame, j, i;	
	int contador = 0, contadorSemTrocas = 0, pageFaults = 0;
	PROCESSO processos[processoCont];	
		
	//Variaveis para leitura de arquivo
	FILE *arq;
	int tamanho, posicaoMemoria;
	texto nomeProcesso, processoOperacao, tipoTamanho; 

	inicializarMemoria(&memoriaPrincipal, paginasQnt); 
	if (!(arq = fopen(ARQUIVO, "rt"))) {
	 	printf("\nProblemas na leitura do arquivo. Saindo\n");
	 	exit(-1);
	}

	//Zera as tabelas de Pagina para o vetor de structs
	for (i = 0; i < processoCont; i++) {
		for (j = 0; j < processo[i].qtdePagina; j++) {
			processo[i].tabelaPagina[j] = -1;
		}
	}

	//Zera a tabela da memoria principal
	for (i = 0; i < paginasQnt; i++) {
		memoriaPrincipal.paginas[i] = -1;
		strcpy(memoriaPrincipal.frameProcId[i], "");
		memoriaPrincipal.tempo[i] = 0;
		memoriaPrincipal.tempoInalterado[i] = 0;
		memoriaPrincipal.bitRef[i] = 0;
	}
				
	while (!feof(arq)) {
		//Verifica Se a pagina ja foi referenciada, e incrementa o tempo decorrido
		flag1 = flag2 = 0;//flag 0 significa que nao foi encontrada
		fscanf(arq, "%s", nomeProcesso);
		fscanf(arq, "%s", processoOperacao);
		if (strcmp(processoOperacao,"C") != 0) { 			
			fscanf(arq, "%x", &posicaoMemoria);
			numPage = posicaoMemoria / tamPagina;
			cont++;	
			//Verifica se a pagina ja foi acessada
			for (i = 0; i < processoCont; i++) { 
				//Verifica se esta na tabela de paginas
				if (strcmp(processo[i].id, nomeProcesso) == 0 && (processo[i].tabelaPagina[numPage] != -1)) {
						memoriaPrincipal.tempo[processo[i].tabelaPagina[numPage]] = ++contador;
						memoriaPrincipal.bitRef[processo[i].tabelaPagina[numPage]] = 1;
						flag1 = flag2 = 1;
						break;
				}
			}

			
			//Se nada foi encontrado (flag1) ele insere
			if (flag1 == 0){
				for (contPaginas = 0; contPaginas < paginasQnt; contPaginas++){
					if (memoriaPrincipal.paginas[contPaginas] == -1) {					
						memoriaPrincipal.paginas[contPaginas] = numPage;
						strcpy(memoriaPrincipal.frameProcId[contPaginas], nomeProcesso);
						for (i = 0; i < processoCont; i++){
							if ((strcmp(processo[i].id, nomeProcesso)) == 0){
								processo[i].tabelaPagina[numPage] = contPaginas;
								
								break;
							}
						}				
						memoriaPrincipal.tempoInalterado[contPaginas] = ++contadorSemTrocas;
						memoriaPrincipal.tempo[contPaginas] = ++contador;						
						pageFaults++;											
						flag2 = 1;												
						break;
					}	
				}
			}

			//se n„o encontrou, procura por um espaÁo vazio e insere
			if (flag2 == 0){
				switch (modo)
				{
					//LRU
					case 1:
						pos = LRU(memoriaPrincipal.tempo, paginasQnt);
						for (i = 0; i < processoCont; i++){ 
							if ((strcmp(processo[i].id, memoriaPrincipal.frameProcId[pos])) == 0){
								
								processo[i].tabelaPagina[memoriaPrincipal.paginas[pos]] = -1;
								
								break;
							}
						}		
						memoriaPrincipal.paginas[pos] = numPage;
						strcpy(memoriaPrincipal.frameProcId[pos], nomeProcesso);
						for (i = 0; i < processoCont; i++){
							if ((strcmp(processo[i].id, nomeProcesso)) == 0){
								processo[i].tabelaPagina[numPage] = pos;
								
								break;
							}
						}
						memoriaPrincipal.tempo[pos] = ++contador;					
						pageFaults++;										
						break;	
	
						// LRU Aproximado
					case 2:						
						pos = LRU_bitRef(memoriaPrincipal.tempo, memoriaPrincipal.bitRef, paginasQnt);	
						for (i = 0; i < processoCont; i++){ //Apaga a pagina que sera trocada
							if ((strcmp(processo[i].id, memoriaPrincipal.frameProcId[pos])) == 0){
								processo[i].tabelaPagina[memoriaPrincipal.paginas[pos]] = -1;
								break;
							}
						}		
						memoriaPrincipal.paginas[pos] = numPage;
						strcpy(memoriaPrincipal.frameProcId[pos], nomeProcesso);
						for (i = 0; i < processoCont; i++){
							if ((strcmp(processo[i].id, nomeProcesso)) == 0){
								
								processo[i].tabelaPagina[numPage] = pos;
								break;
							}
						}
						memoriaPrincipal.tempo[pos] = ++contador;					
						pageFaults++;										
						break;
					
					//Segunda Chance
					case 3:			
						pos = SecondChance(memoriaPrincipal.tempoInalterado, memoriaPrincipal.bitRef, paginasQnt);
						for (i = 0; i < processoCont; i++){
							if ((strcmp(processo[i].id, memoriaPrincipal.frameProcId[pos])) == 0){
								
								processo[i].tabelaPagina[memoriaPrincipal.paginas[pos]] = -1;
								break;
							}
						}		
						memoriaPrincipal.paginas[pos] = numPage;
						strcpy(memoriaPrincipal.frameProcId[pos], nomeProcesso);
						for (i = 0; i < processoCont; i++){
							if ((strcmp(processo[i].id, nomeProcesso)) == 0){
								
								processo[i].tabelaPagina[numPage] = pos;
								break;
							}
						}
						memoriaPrincipal.tempoInalterado[pos] = ++contadorSemTrocas;					
						pageFaults++;										
						break;				
				}
			}
			printf("\n");

			if (modo == 1){			
			int i,qtde;
			limparTela();
			printf("---------------------------------------------------------------------\n");
			printf("\nEndereco %d, acessado pelo processo: %s\n", posicaoMemoria, nomeProcesso);
			printf("\nPagina acessada: %d\n", numPage);
			printf("---------------------------------------------------------------------\n");
				for (contPaginas = 0; contPaginas < paginasQnt; ++contPaginas)
					
					printf("| [%d]: Pagina: [%d] |\t Processo: %s|\t\n",contPaginas, memoriaPrincipal.paginas[contPaginas], memoriaPrincipal.frameProcId[contPaginas]);
					printf("|Page faults: %d\n", pageFaults);
					getch();
					
					
			} else
			if (modo == 2){
			limparTela();
			printf("---------------------------------------------------------------------\n");
			printf("\nEndereco %d, acessado pelo processo: %s\n", posicaoMemoria, nomeProcesso);
			printf("\nPagina acessada: %d\n", numPage);
			printf("---------------------------------------------------------------------\n");
				for (contPaginas = 0; contPaginas < paginasQnt; ++contPaginas)
					printf("| [%d]: Pagina: [%d ]|\t Processo: %s |\t BitReferencia: %d |\t \n",contPaginas, memoriaPrincipal.paginas[contPaginas], memoriaPrincipal.frameProcId[contPaginas], memoriaPrincipal.bitRef[contPaginas]);
					printf("|Page faults: %d\n", pageFaults);
					getch();
					limparTela();
			} else
			if (modo == 3){
			limparTela();
			printf("---------------------------------------------------------------------\n");
			printf("\nEndereco %d, acessado pelo processo: %s\n", posicaoMemoria, nomeProcesso);
			printf("\nPagina acessada: %d\n", numPage);
			printf("---------------------------------------------------------------------\n");
				for (contPaginas = 0; contPaginas < paginasQnt; ++contPaginas)
				
					printf("| [%d]: Pagina: [%d]|\t Processo: %s |\t BitReferencia: %d |\t\n",contPaginas, memoriaPrincipal.paginas[contPaginas], memoriaPrincipal.frameProcId[contPaginas], memoriaPrincipal.bitRef[contPaginas]);
					printf("|Page faults: %d\n", pageFaults);
					getch();
					limparTela();
			}
		}else {
			fscanf(arq,"%d", &tamanho);
			fscanf(arq,"%s", tipoTamanho);
			
		}
	}
	
	
		
	free(memoriaPrincipal.paginas);
	free(memoriaPrincipal.frameProcId);
	free(memoriaPrincipal.tempo);
	free(memoriaPrincipal.tempoInalterado);
	free(memoriaPrincipal.bitRef);
	
	fclose(arq);
	return 1;
}
  
//Faz a verificacao dos tamanhos de memoria e pagina, retorna 1 para parametros validos

int verificarParametros( int *tamPagina, int *tamMemoria) {
	int i;
	char *pageFrame, *memory;
	
	if (((*tamMemoria) % (*tamPagina)) != 0) {
		return 1;
	}else if (((*tamPagina) <= 0) ||((*tamMemoria) <= 0)) {
		return 1;
	} else return 0;
}


//Funcao de conversao de tio de arquivo para Bytes
unsigned long int toByte(texto tamUnidade, int tamProcesso) {
	if (strcmp(tamUnidade, "KB") == 0){
	return tamProcesso * 1000;
	}else if (strcmp(tamUnidade, "B") == 0) {
	return tamProcesso;
	}else if (strcmp(tamUnidade, "MB") == 0){
	return tamProcesso * 1000000;
	}else if (strcmp(tamUnidade, "GB") == 0){
	 return tamProcesso * 1000000000;
	}else if (strcmp(tamUnidade, "TB") == 0) {
	return tamProcesso * 1000000000000;
	}else 
	printf("\nTamanho invalido\n");
	exit(-1);
}

//Funcao que salva os processos no ponteiro, de acordo com que vai lendo, para depois usar em tabela de paginas
void processoScale(PROCESSO *p) {
	int cont = 0, tamanho, posicaoMemoria;
	texto nomeProcesso, processoOperacao, tipoTamanho;
	FILE *arq;
	if (!(arq = fopen(ARQUIVO, "rt"))) {
	 	printf("\nProblemas na leitura do arquivo. Saindo\n");
	 	exit(-1);
	}
	while (!feof(arq)) {
		fscanf(arq,"%s", nomeProcesso);
		fscanf(arq,"%s", processoOperacao);
		if (strcmp (processoOperacao,"R") == 0 || strcmp (processoOperacao,"W") == 0){
			fscanf(arq,"%x", &posicaoMemoria);		
		}
		if (strcmp (processoOperacao,"C") == 0){
			fscanf(arq,"%d", &tamanho);
			fscanf(arq,"%s", tipoTamanho);
			strcpy(p[cont].id, nomeProcesso);
			p[cont].tamProcesso = toByte(tipoTamanho, tamanho);
			cont++;
		}
	}
	fclose(arq);
}

	
//Faz a leitura da quantidade de criaÁıes e operaÁıes, fazendo a conta de quantos processos ser„o utilizados, e usando destes para
//alocar memoria na funcao

int contaProcessos(){
	int cont = 0, tamanho, posicaoMemoria;
	texto nomeProcesso, processoOperacao, tipoTamanho;
	FILE *arq;
	if (!(arq = fopen(ARQUIVO, "rt"))) {
	 	printf("\nProblemas na leitura do arquivo. Saindo\n");
	 	exit(-1);
	}
	while (!feof(arq)) {
		fscanf(arq,"%s", nomeProcesso);
		fscanf(arq,"%s", processoOperacao);
		if (strcmp (processoOperacao,"R") == 0 || strcmp (processoOperacao,"W") == 0){
			fscanf(arq,"%x", &posicaoMemoria);		
		}
		if (strcmp (processoOperacao,"C") == 0){
			cont++;
			fscanf(arq,"%d", &tamanho);
			fscanf(arq,"%s", tipoTamanho);
		}
	}
	fclose(arq);
	return cont;
}

//Itera o vetor de struct e para cada iteracao faz a alocacao na tabela de paginas de acordo com as paginas do processo

void inicializarTabelas(PROCESSO *p, int tamPagina, int processoCont){
	int i, j;
	for (i = 0; i < processoCont; i++) {
		p[i].qtdePagina = p[i].tamProcesso / tamPagina;
		if ((p[i].tamProcesso % tamPagina) > 0){
		p[i].qtdePagina += 1;
		} 
		p[i].tabelaPagina = (int*)malloc(sizeof(int) * p[i].qtdePagina);	
	}
}

//inicializa vetor de memoria de acordo com a quantidade de paginas fornecida
void inicializarMemoria(MEMORIA *m, int paginasQnt) {
	int i;
	m->bitRef = (int*)malloc(sizeof(int) * paginasQnt);
	m->frameProcId = (texto*)malloc(sizeof(texto) * paginasQnt);
	m->tempoInalterado = (int*)malloc(sizeof(int) * paginasQnt);
	m->paginas = (int*)malloc(sizeof(int) * paginasQnt);
	m->tempo = (int*)malloc(sizeof(int) * paginasQnt);
	for (i = 0; i < paginasQnt; i++) {
		m->paginas[i] = -1;
		strcpy(m->frameProcId[i], "");
		m->tempo[i] = 0;
		m->tempoInalterado[i] = 0;
		m->bitRef[i] = 0;
	}
}


// Implementacao do algoritmo LRU, usada com o tempo da variavel ao inves de pilha ou uma lista encadeada 
int LRU (int *tempo, int n)
{
	int cont, minimo = tempo[0], pos = 0;		

	for (cont = 1; cont < n; ++cont)	
		if (tempo[cont] < minimo)		
		{								
			minimo = tempo[cont];		
			pos = cont;					
		}	
	return pos;							
}

// Implementacao do algoritmo LRU aproximado, onde ele procura os bits como 0 para retirada 
int LRU_bitRef (int *tempo, int *bitRef, int n){
	int cont, minimo = tempo[0], pos = 0;	

	for (cont = 1; cont < n; ++cont)	
		if (tempo[cont] < minimo)		
		{	
			if (bitRef[cont] == 0)		
			{
				minimo = tempo[cont];	
				pos = cont;				
			}
			if (bitRef[cont] == 1)		
				bitRef[cont] = 0;
		}	
	return pos;							
}


int SecondChance (int *tempo, int *bitRef, int n)
{
	int cont, minimo = tempo[0], pos = 0;	

	for (cont = 1; cont < n; ++cont)	
		if (tempo[cont] < minimo)		
		{	
			if (bitRef[cont] == 0)		
			{
				minimo = tempo[cont];	
				pos = cont;				
			}
			if (bitRef[cont] == 1)		
				bitRef[cont] = 0;
		}	
	return pos;							
}

//Libera memoria dos processos de acordo com a contagem de processos feitas
void liberaMemoria(PROCESSO *p, int processoCont) {
	int i;
	for (i = 0; i < processoCont; i++) {
		free(p[i].tabelaPagina);
	}
	free(p);
}

// limpa tela kkk
void limparTela(){
   system("cls");
}

void liberaMemoria(PROCESSO *p, int processoCont); 
void inicializarMemoria(MEMORIA *m, int paginasQnt);
void limpaTela();
void processoScale(PROCESSO *p);
void inicializarTabelas(PROCESSO *p, int tamPagina, int processoCont);
int contaProcessos();
int simuladorMemoriaVirtual(PROCESSO *processo, int paginasQnt, int processoCont, int modo, int tamPagina);
int LRU(int *tempo, int n);
int LRUAproximado(int *tempo, int *bitRef, int n);
int segundaChance(int *tempo, int *bitRef, int n);
unsigned long int toByte(texto tamUnidade, int tamProcesso);

