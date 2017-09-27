// Fases broca.cpp : Defines the entry point for the console application.
//Código base - Brocas e Vespas.cpp 
//Com taxa de morte por razões aleatórias ultima atualização -22/6/2016
/*utilizado
20/6/16
22/6/16
23/6/16
5/7/16
6/7/16 - nomes dos arquivos
12/7/16 - gráficos com taxa de morte da broca =0.0098
13/7/16 - predação proporcional - predxbroca/cotesia
14/7/16 - predação proporcional - predxbroca*cotesia/(broca+cotesia)^2
- predação proporcional - predxbroca/(cotesia+broca)
26/7/2016 - predação com (Taxa de predação) x(Cotesia)x(Broca)
27/7/2016 - Gráficos com predação proporcional - predxbroca/cotesia
- Gráficos com predação fixa
- imprimir qual predação está sendo usada nos parâmetros
28/7/2016
- MODIFICAÇÃO - brocas adultas em gráfico diferente do resto da população
- Broca com idade igual
14/8/2016 - acrescentando "levantamento do campo"
***********************************************************************
Começo do fases da broca
***********************************************************************
05/12/2016 - mata só se estiver vivo e se a idade for maior do que a fase do ovo (cotesia)
***********************************************************************
Otimização custo
***********************************************************************
19/05/2017 - inicio da integração com a otimização do custo
08/08/2017 - colocar tudo em funções
12/09/2017 - criando classes dos objetos vespa e brocas
*/

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "iostream"
#include "iomanip"
#include "string"
#include "map"
#include "random"
#include "cmath"
#include "conio.h"
#include "fstream"
using namespace std;
ofstream saida;
ofstream saidaV;
ofstream saidaPlano;
ofstream saidaId;
ofstream saidaIdCo;
ofstream parametros;
ofstream saidaAvaliacao;

char str[150];
char strV[150];
char strId[150];

#define valoresMedia 14			//quantidade de dados nas medias
#define amostraCamp 10			//amostras tiradas no campo
#define cana 100000				//canas em 1 hectare
/*
Diatraea
*/

#define tamanho_pop 2500   //tamanho da população inicial
#define tempo 200         //quantidade de dias
#define IndAdult 65		  //idade que vira adulto
#define proporcao 0.5	  //proporção entre machos e fêmeas	(abaixo do valor gera macho, acima gera fêmea)
#define ovo 15				//idade que larga de ser ovo e vira lagarta
#define vidaM 70 //56		  //expectativa de vida média
#define desvioVM 2 //3		  //desvio padrão da espectativa de vida
#define filhosM 60 //10		  //média de filhos por fêmea
#define desvioFM 20 //5		  //desvio padrão da média de filhos por fêmea
#define fertilInicial 1 //0.9	//proporção de indivíduos férteis inicialmente
#define mediaDados 20	  //quantidade de dados para média 
#define morteBroca 0.0098		//morte da broca por dia

/*
COTESIA
*/

#define Predacao 0.25		//Taxa de predação das brocas pelas vespas
#define Vesp_tam_pop 6000		//tamanho da população de cotesia
#define indAdultV	19		//idade que a vespa vira adulto
#define ProporcaoV 0.32		//proporção entre machos e fêmeas da vespa	(abaixo do valor gera macho, acima gera fêmea)
#define vidaMV 24		  //expectativa de vida média da vespa
#define desvioVMV 4		  //desvio padrão da espectativa de vida da vespa
#define filhosMV 28//41 		  //média de filhos por fêmea da vespa
#define desvioFMV 19//4		  //desvio padrão da média de filhos por fêmea da vespa
#define fertilInicialV 1	//proporção de indivíduos férteis inicialmente da vespa
#define morteCotesia 0.05	//morte da cotesia por dia


///*
class Data
{
public:
	int filhos;    //número de filhos
	int espVida;	//espectativa de vida
	int idade;     //idade
	int adulto;		//idade que vira adulto
	int ovolagarta;	//idade que larga de ser ovo
	int numero;     //qual indivíduo
	bool vivo;      //indentifica se o indivíduo está vivo
	bool femea;     //macho ou fêmea
	bool fertil;    //femea fértil
	bool lagarta;	//fase que a broca pode ser morta
	
	//static int QuantidadeVespa;
	//static int QuantidadeBrocas;
	static std::random_device rd;
	static void Inicializa_Gerador()
	{
		std::mt19937 gen(rd());
	}

};//*/


  ///*
class Vespa : public Data
{
public:
	static int QuantidadeVespa;
	//static std::random_device rd;
	/*
	static void Inicializa_Gerador()
	{
		std::mt19937 gen(rd());
	}//*/

	int dV()
	{
		static std::normal_distribution<> Ndistribution(filhosMV, desvioFMV);
		return Ndistribution(rd);
	}
	int fV()
	{
		static std::normal_distribution<> NfDistribution(vidaMV, desvioVMV);
		return NfDistribution(rd);
	}
	int IdadeInicialV()
	{
		static std::uniform_int_distribution<int> NidadeDistribution((int)0, (int)vidaMV);
		return NidadeDistribution(rd);
	}

	double aleatorioV()
	{
		static std::uniform_real_distribution<double> moeda(0.0, 1.0);
		return moeda(rd);
	}

	Vespa(double **media, int T)
	{
		numero = QuantidadeVespa++;
		filhos = abs((int)std::round(dV()));	     //gerador com distribuição normal para os filhos
		idade = (T == 0) ? indAdultV:0;				//gerador com distribuição normal para as idades
		adulto = indAdultV;							//idade que vira adulto (GERADO POR DISTRIBUIÇÃO OU FIXA???)
		
		espVida = (int)fV();							//espectativa de vida inicial
		if (adulto>espVida)							//checa se vira adulto antes da morte pra evitar erro no programa
			adulto = espVida - 1;
				//media[T][5] = (lagarta == true && vivo == true) ? media[T][5] + 1 : media[T][5];
		while (idade>espVida)
		{
			idade--;
		}

		vivo = true;						//diz que o indivíduo está vivo
		float genero = (float)aleatorioV();
		femea = (genero > ProporcaoV) ? true : false;
		fertil = (genero<fertilInicialV) ? true : false;
		media[T][5] += 1;					//acrescenta os indivíduos que foram introduzidos
	}
};

class Broca : public Data
{
public:
	static int QuantidadeBrocas;
	//static std::random_device rd;
	/*
	static void Inicializa_Gerador()
	{
		std::mt19937 gen(rd());
	}//*/

	int d()
	{
		static std::normal_distribution<> Ndistribution(filhosM, desvioFM);
		return Ndistribution(rd);
	}
	int f()
	{
		static std::normal_distribution<> NfDistribution(vidaM, desvioVM);
		return NfDistribution(rd);
	}
	int IdadeInicial()
	{
		static std::uniform_int_distribution<int> NidadeDistribution((int)0, (int)vidaM);
		return NidadeDistribution(rd);
	}
	
	double aleatorio()
	{
		static std::uniform_real_distribution<double> moeda(0.0, 1.0);
		return moeda(rd);
	}
	
	Broca( double **media, int T)
	{
		numero=QuantidadeBrocas++;
		//QuantidadeBrocas++;
		filhos = abs((int)std::round(d()));	     //gerador com distribuição normal para os filhos
		//if(T==0)
		idade = (T==0)? abs((int)IdadeInicial()):0;		//idade = std::round(f(gen));			//gerador com distribuição normal para as idades
		adulto = IndAdult;							//idade que vira adulto (GERADO POR DISTRIBUIÇÃO OU FIXA???)
		ovolagarta = ovo;								//idade que o ovo vira lagarta (GERADO POR DISTRIBUIÇÃO OU FIXA???)
		espVida = (int)f();							//espectativa de vida inicial
		if (adulto>espVida)							//checa se vira adulto antes da morte pra evitar erro no programa
			adulto = espVida - 1;
		lagarta = (idade < IndAdult&&idade >= ovolagarta) ? true : false;
		//lagarta = (lagarta == true && vivo == true) ? true : false; //só é lagarta se está viva
		media[T][10] = (lagarta == true) ? media[T][10] + 1 : media[T][10];// && vivo == true
		while (idade>espVida)
		{
			idade--;
		}
		
		vivo = true;						//diz que o indivíduo está vivo
		float genero = (float)aleatorio();
		femea = (genero > proporcao) ? true : false;
		fertil = (genero<fertilInicial) ? true : false;
		media[T][0] += 1;					//acrescenta os indivíduos que foram introduzidos
	}
};
//*/
//int Broca::QuantidadeBrocas;
//std::random_device Broca::rd;

typedef struct elem
{
	Data data;
	struct elem* next;
	struct elem* prev;
}
Elem;

Elem* alloc_Elem(Data x)
{
	Elem* e = (Elem*)malloc(sizeof(Elem));
	e->data = x;
	e->next = NULL;
	e->prev = NULL;
	return e;
}

/* #################################################################
############ Definition of list and list operations ################
#################################################################### */
typedef struct list
{
	Elem* first;
	Elem* last;
	int size;
}
List;


List* init_List()
{
	List* L = (List*)malloc(sizeof(List));
	L->size = 0;
	L->first = NULL;
	L->last = NULL;
	return L;
}

void insert_Elem(List* L, Data x)
{
	Elem* e = alloc_Elem(x);

	if (L->size == 0)
	{
		L->first = e;
	}
	
	else
	{
		e->prev = L->last;
		L->last->next = e;
	}

	L->last = e;
	(L->size)++;
}

void delete_Elem(List* L, Elem* to_erase)
{
	if (to_erase == L->first)
		L->first = L->first->next;
	else if (to_erase == L->last)
		L->last = L->last->prev;
	else
	{
		Elem* aux = to_erase->prev;
		aux->next = to_erase->next;
	}
	free(to_erase);
	L->size--;
}

void erase_List(List* L)
{
	while (L->size != 0)
		delete_Elem(L, L->first);
	free(L);
}

void update_Elem(Elem* e, Data y)
{
	e->data = y;
}
/*

/****************************************************************
Taxa de morte para o início
****************************************************************/
double taxa_morte_inicial(int t, double Tmorte)
{
	double morte = 0.0;
	morte = (double)1.0 - Tmorte;
	//cout << morte<<endl;
	morte = (double)pow((double)morte, (double)t);
	morte = 1.0 - morte;
	//cout <<IndAdult-t<<" "<<t<<" final " << morte << endl;
	//getchar();
	return morte;
}

//Imprime arquivo com parâmetros
void ImprimeParametros()
{
	parametros.open("Aparametros.txt");
	parametros << "Medias " << mediaDados << endl;
	parametros << "Tempo " << tempo << endl;
	parametros << endl;
	parametros << "-------------------------------------------------------" << endl;
	parametros << "Diatraea" << endl;
	parametros << endl;
	parametros << "Tamanho populacional " << tamanho_pop << endl;
	//parametros << "Idade Adulto " << IndAdult << endl;
	parametros << "Vida média " << vidaM << " Desvio padrão da vida média " << desvioVM << endl;
	parametros << "Filhos " << filhosM << " Desvio padrão dos filhos " << desvioFM << endl;
	parametros << "Morte por fatores aleatórios " << morteBroca << endl;
	parametros << "Fertilidade " << fertilInicial << " proporção " << proporcao << endl;
	parametros << "-------------------------------------------------------" << endl;
	parametros << "Cotesia" << endl;
	parametros << endl;
	parametros << "Predação " << Predacao << endl;
	parametros << "População inicial " << Vesp_tam_pop << endl;
	parametros << "Vida média " << vidaMV << " Desvio padrão da vida média " << desvioVMV << endl;
	parametros << "Filhos " << filhosMV << " Desvio padrão dos filhos " << desvioFMV << endl;
	parametros << "Mortalidade por fatores aleatórios " << morteCotesia << endl;
	parametros << "Fertilidade " << fertilInicialV << " proporção " << ProporcaoV << endl;
	parametros << "-------------------------------------------------------" << endl;
	parametros << "Modelo usado na predação" << endl;
	parametros << "predacaoProporcional = Predacao" << endl;
	parametros.close();
}


/**********************************************************
INICIALIZADOR DAS BROCAS
**********************************************************/
double** InicializadorBrocas(List *L)
{
	int popvivo = 0, poplagarta = 0, filhos=0, idade=0;
	float genero;
	Elem* aux1;
	//erase_List(L);
	
	/**********************************************************************
	Criação dinamica da matriz das medias
	********************************************************************/
	double **	media = (double**)calloc(tempo, sizeof(double*));	
	for(int i=0;i<tempo;i++)
		{
			/**************************************************************
			0-brocas vivas
			1-brocas mortas
			2-brocas femeas
			3-brocas macho
			4-brocas femeas ferteis
			5-vespas vivas
			6-vespas mortas
			7-vespas femeas
			8-vespas macho
			9-vespas femeas ferteis
			10-brocas lagartas
			11-desvio padrao brocas
			12-desvio padrão vespas
			13-brocas adultas
			**************************************************************/

			media[i] = (double*)calloc(valoresMedia, sizeof(double));
		}
	/*********************************************************************/

	popvivo = tamanho_pop;
	poplagarta = 0;
	//L = init_List();
	//aux1 = L->first;
	Broca::QuantidadeBrocas = 0;
	

	for (int i = 0; i<tamanho_pop; i++)
	{
		Broca x(media,0);
		insert_Elem(L, x);

		//cout << x.idade << " " << x.numero << " femea? "<<x.femea<<" fertil? "<<x.fertil<<" "<<x.filhos<<endl;
	}
	return media;
}

/**********************************************************
INICIALIZADOR DAS COTESIAS
**********************************************************/
void InicializadorVespas(List *Lv, double ** media)
{
	int popvivo = 0, filhos = 0, idade = 0;
	float genero;
	Elem* aux1;
	//erase_List(L);

	popvivo = Vesp_tam_pop;
	Vespa::QuantidadeVespa = 0;
	


	for (int i = 0; i<Vesp_tam_pop; i++)
	{
		Vespa v(media, 0);
		insert_Elem(Lv, v);
		//cout << v.idade << " " << v.numero << " femea? " << v.femea << " fertil? " << v.fertil << endl;
	}
}


int Vespa::QuantidadeVespa = 0;
int Broca::QuantidadeBrocas = 0;
std::random_device Vespa::rd;
//std::random_device Broca::rd;




int _tmain(int argc, _TCHAR* argv[])
{
	List* L; //= init_List();
	List* Lv;// = init_List();
	Elem* aux;
	Elem* aux1;
	Elem* aux2;
	Elem* aux3;
	Elem* dummy_elem;
	int i, j, k = 0, actual_time = 0, l = 0, idade, filhos, auxfilhos, popvivo = 0, popmorto = 0, popfemea = 0, popmacho = 0, popfemeaF = 0, poplagarta=0;
	int popvivoV = 0, popmortoV = 0, popfemeaV = 0, popmachoV = 0, popfemeaFV = 0, predado = 0, PopBrocAdul = 0;
	double **MediasMatriz;
	float genero, aleatóriopred, sorteio_morte;
	Broca::Inicializa_Gerador();
	Vespa::Inicializa_Gerador();
	//x.adulto = 0;
	/****************************************************************
	NOME COM PARAMETROS
	****************************************************************/
	/*
	sprintf_s(str, "Pd_%.3fBrocPop%d_VM%d_DVM_%d_Filh%d_dFilh%d_md%d_prop%.0f_Ad%dmorteAle%.2f.txt", Predacao, tamanho_pop, vidaM, desvioVM, filhosM, desvioFM, mediaDados, proporcao * 100, IndAdult, morteBroca * 100);
	sprintf_s(strV, "Pd_%.3fVesPop%d_VM%d_DVM_%d_Filh%d_dFilh%d_md%d_prop%.0f_Ad%dmorteAle%.2f.txt", Predacao, Vesp_tam_pop, vidaMV, desvioVMV, filhosMV, desvioFMV, mediaDados, ProporcaoV * 100, indAdultV, morteCotesia * 100);
	/****************************************************************
	NOME SEM PARAMETROS
	****************************************************************/
	sprintf_s(str, "ABrocas.txt");
	sprintf_s(strV, "ACotesias.txt");
	//sprintf_s(str, "dados.txt");
	//ofstream saida;
	//ofstream saidaV;
	saida.open(str);
	saidaV.open(strV);
	saidaPlano.open("Aplano.txt");
	ImprimeParametros();
	

	saidaAvaliacao.open("Broca-Cotesia-avaliacao.txt");
	saidaAvaliacao << "#Encontrado na Amostra; Brocas reais; tempo - levantamento - " << amostraCamp << " canas - " << cana << endl;
		std::random_device rd;
	std::random_device tf;
	std::mt19937 gen(rd());

	// values near the mean are the most likely
	// standard deviation affects the dispersion of generated values from the mean
	std::normal_distribution<> d(filhosM, desvioFM);
	std::normal_distribution<> f(vidaM, desvioVM);

	std::uniform_real_distribution<double> aleatorio(0.0, 1.0);
	std::uniform_int_distribution<int> IdadeInicial(0, (int)vidaM);

	/****************************************************************
	Vespa
	****************************************************************/
	std::normal_distribution<> dV(filhosMV, desvioFMV);
	std::normal_distribution<> fV(vidaMV, desvioVMV);
	std::uniform_real_distribution<double> aleatorioV(0.000, 1.000);
	std::uniform_real_distribution<> IdadeInicialV(1, (int)vidaMV);

	saida << "# tempo, população viva, população morta, fêmeas, machos, Broca Adulta" << endl;
	saidaV << "# tempo, população viva, população morta, fêmeas, machos" << endl;

	/*******************************************************************************
	inicializador Broca
	*******************************************************************************/
	
	for (int l = 0; l<mediaDados; l++)					//Repete o processo pelo número de vezes necessário
	{

		cout << "primeira coisa das brocas" << endl;
		cout << "Quantidade de brocas " << Broca::QuantidadeBrocas << endl;
		L = init_List();
		MediasMatriz=InicializadorBrocas(L);

		cout << "teste inicializador de brocas" << endl;
		//aux1 = L->first;
		cout << "segunda coisa" << endl;
		//cout << aux1->data.numero << endl;
		//getchar();

		
		/*******************************************************************************
		inicializador Vespa
		*******************************************************************************/
		cout << "Quantidade de brocas "<<Broca::QuantidadeBrocas << endl;
		cout << "primeira coisa da vespa" << endl;
		cout << "Quantidade de vespas " << Vespa::QuantidadeVespa << endl;
		//erase_List(Lv);
		Lv = init_List();
		InicializadorVespas(Lv, MediasMatriz);
		cout << "Quantidade de vespas " << Vespa::QuantidadeVespa << endl;
		aux1 = Lv->first;
//		cout <<"vespa numero "<< aux1->data.numero << endl;
		//getchar();


		/*******************************************************************************
		iterações
		*******************************************************************************/


		for (actual_time = 1; actual_time<tempo; actual_time++)                      //faz o tempo passar
		{

			/********************************************************************************
			PREDAÇÃO PROPORCIONAL
			*********************************************************************************/
			float predacaoProporcional = (float)Predacao;
			if (popvivoV > 0)
				predacaoProporcional = (float)Predacao*popvivo / popvivoV;//(popvivo*popvivoV / ((popvivoV+popvivo)*(popvivoV + popvivo)));
			//predacaoProporcional = Predacao;




			/********************************************************************************
			HISTOGRAMA ABAIXO
			*********************************************************************************/
			/*
			if (l == 0)
			{
			sprintf_s(strId, "histograma_tempo_%d.txt", actual_time);				//histograma
			saidaId.open(strId);													//histograma
			sprintf_s(strId, "histograma_cotesia_tempo_%d.txt", actual_time);		//histograma
			saidaIdCo.open(strId);													//histograma
			cout << actual_time << " " << strId << endl;

			}//*/
			if (actual_time == 500 || actual_time == 600)
			{
				cout << "tempo - " << actual_time << " poptotal " << popvivo << " cotesia " << popvivoV << endl;
			}
			if ((popvivo + popvivoV)>10000000)
			{
				cout << "tempo - " << actual_time << " poptotal " << popvivo << " cotesia " << popvivoV << endl;
				actual_time = tempo;
				//getchar();
			}
			/*******************************************************************************
			Iterações Broca
			*******************************************************************************/
			//Broca x(MediasMatriz, actual_time);
			//insert_Elem(L, x);                    //insert dummy element
			aux1 = L->first;                          //auxiliar pra percorrer todos os indivíduos
			dummy_elem = L->last;                     // last will be de dummy elem
			popmorto = 0;
			popvivo = 0;
			PopBrocAdul = 0;
			poplagarta = 0;
			//cout << " Interações das brocas" << endl;
			//cout << "Idade" << aux1->data.idade << ", Numero " << aux1->data.numero << endl;

			while (aux1 != NULL)                 //loop para percorrer os indivíduos
			{
				//cout << "entrou no loop das interacoes" << endl;
				//cout << "inicio do individuo numero " << aux1->data.numero << endl;
				/********************************************************************************
				HISTOGRAMA ABAIXO
				*********************************************************************************/
				/*
				if (l == 0)
				{
				if (aux1->data.vivo == true)
				{
				saidaId << aux1->data.idade << endl;		// imprime idade para histograma
				}
				}//*/
				/*******************************************************************************
				Morte da broca por fatores aleatórios
				*******************************************************************************/
				if (aux1->data.idade < aux1->data.espVida && aux1->data.vivo == true)
				{
					double morteV = 0.0;
					//int IdadeV = aux1->data.idade;
					//cout << aux1->data.idade<< " entrou na morte das brocas " << IdadeV<<endl;
					sorteio_morte = aleatorioV(rd);												//todos iguais
																								//morteV = (1.0 - (double)IdadeV/vidaM)*morteBroca;							//morte maior pra indivíduo mais velho
					morteV = (double)(actual_time == 0) ? taxa_morte_inicial(aux1->data.idade, morteBroca) : morteBroca;		//primeiro instante a distribuição é diferente
																															//cout << "entrou na morte das brocas " << morteV<< endl;
					aux1->data.vivo = (sorteio_morte > morteV) ? true : false;					// sorteia se a broca vai morrer
																							//cout << sorteio_morte << " " << morteBroca << " "<<morteV<<" "<< vidaM<< endl;
																							//cout << aux1->data.vivo << " "<< aux1->data.idade<< " "<< (double)IdadeV/vidaM <<endl;
																							//getchar();
				}


				/*******************************************************************************
				Morte da broca por Velhice e geração de filhos
				*******************************************************************************/
				if (aux1->data.idade >= aux1->data.espVida  && aux1->data.vivo == true)                                    //indivíduo morre de velhice
				{
					//printf("entrou na morte. filhos %d, idade %d, etiqueta %d ", aux1->data.filhos, aux1->data.idade, aux1->data.numero);
					//cout <<" femea? -  "<< aux1->data.femea << endl;
					//getchar();
					PopBrocAdul++;			//Conta Brocas ADULTAS
					aux2 = aux1;
					aux2->data.vivo = false;//delete_Elem(L, aux2);               //mata o indivíduo velho
										 //aux1 = aux1->next;
					if (aux2->data.femea == true && aux2->data.fertil == true)
					{
						auxfilhos = (aux2->data.filhos< 0) ? -aux2->data.filhos : aux2->data.filhos;
						//cout << "filhos "<<auxfilhos << endl;
						//getchar();
						aux3 = aux2;
						for (j = 0; j<auxfilhos; j++)    //configura os novos filhos
						{
							if (j > 200)
							{
								printf("loop infinito filhos %d, idade %d, etiqueta %d, j %d, auxfilhos %d \n", aux2->data.filhos, aux2->data.idade, aux2->data.numero, j, auxfilhos);
								//getchar();
							}
							Broca x( MediasMatriz, actual_time);
							
							insert_Elem(L, x);              //insere o indivíduo
							
							aux3 = aux3->next;
							//cout << "pop vivo " << popvivo << " individuo " << aux3->data.numero << " filhos de " << aux1->data.numero << endl;
															//printf("entrou no for. filhos %d, idade %d, etiqueta %d\n", aux2->data.filhos, aux2->data.idade, aux2->data.numero);

						}
					}

					//printf("antes de deletar. filhos %d, idade %d, etiqueta %d\n", aux2->data.filhos, aux2->data.idade, aux2->data.numero);
					//printf("deletou elemento no else no else. filhos %d, idade %d, etiqueta %d\n", aux1->data.filhos, aux1->data.idade, aux1->data.numero);
					//getchar();
				}
				if (aux1->data.idade <aux1->data.espVida  && aux1->data.vivo == true)                    //teste para ver se o indivíduo ainda tem algum dia de vida e está vivo
				{

					//printf("entrou no envelhecer. filhos %d, idade %d, etiqueta %d\n", aux1->data.filhos, aux1->data.idade, aux1->data.numero);
					//getchar();
					aux1->data.idade++;                     //em caso de ter dias, diminuir a expectativa de vida
														 //aux1 = aux1->next;
				}
				else
				{

					//printf("gente morta %d\n", j);
				}
				aux1->data.lagarta = (aux1->data.idade < IndAdult&&aux1->data.idade >= aux1->data.ovolagarta) ? true : false; // checa se é uma lagarta
				aux1->data.lagarta = (aux1->data.lagarta == true && aux1->data.vivo == true) ? true : false; //se for vivo é lagarta
				if (aux1->data.lagarta == true)
					poplagarta++;
				if (aux1->data.vivo == true)
					popvivo++;
				if (aux1->data.vivo == false)
					popmorto++;
				if (aux1->data.femea == true)
					popfemea++;
				if (aux1->data.femea == false)
					popmacho++;
				//cout << "final no individuo numero " << aux1->data.numero << endl;
				aux1 = aux1->next;


			}
			
			/*******************************************************************************
			iterações Cotesia
			*******************************************************************************/
			
			aux1 = Lv->first;                          //auxiliar pra percorrer todos os indivíduos
			popmortoV = 0;
			popvivoV = 0;
			while (aux1 != NULL)                 //loop para percorrer os indivíduos
			{
				
				/********************************************************************************
				HISTOGRAMA ABAIXO
				*********************************************************************************/
				/*
				if (l == 0)
				{
				if (aux1->data.vivo == true)
				{
				saidaIdCo << aux1->data.idade << endl;		// imprime idade para histograma
				}
				}//*/
				/*******************************************************************************
				Morte da cotesia por fatores aleatórios
				*******************************************************************************/
				if (aux1->data.idade < aux1->data.espVida && aux1->data.vivo == true)
				{
					double morteC = 0.0;
					//int IdadeC = aux1->data.idade;
					//cout << aux1->data.idade<< " entrou na morte das cotesias " << IdadeC<<endl;
					sorteio_morte = aleatorioV(rd);				//todos iguais
																//morteC = (1.0-(double)IdadeC / vidaMV)*morteCotesia;	//morte maior pra indivíduo mais velho
																//morteC = (double)(actual_time == 0) ? taxa_morte_inicial(aux1->data.idade, morteCotesia) : morteCotesia;	//primeiro instante a distribuição é diferente
					morteC = morteCotesia;

					aux1->data.vivo = (sorteio_morte > morteC) ? true : false;						// sorteia se a cotesia vai morrer
																								//cout << sorteio_morte << " " << morteCotesia << " "<<morteC<<" "<< vidaMV<< endl;
																								//cout << aux1->data.vivo << " "<< aux1->data.idade<< " "<< (double) IdadeC/ vidaMV <<endl;
																								//getchar();
				}
				if (aux1->data.idade >= aux1->data.espVida  && aux1->data.vivo == true)                                    //indivíduo morre de velhice
				{
					//printf("entrou na morte. filhos %d, idade %d, etiqueta %d ", aux1->data.filhos, aux1->data.idade, aux1->data.numero);
					//cout <<" femea? -  "<< aux1->data.femea << " fertil "<< aux1->data.fertil<< endl;
					//getchar();
					aux2 = aux1;
					aux2->data.vivo = false;//delete_Elem(L, aux2);               //mata o indivíduo velho
										 //aux1 = aux1->next;
					if (aux2->data.femea == true && aux2->data.fertil == true)
					{
						auxfilhos = (aux2->data.filhos< 0) ? -aux2->data.filhos : aux2->data.filhos;
						aleatóriopred = aleatorio(rd);

						/*******************************************************************************
						PREDAÇÃO
						*******************************************************************************/
						//if (Predacao > aleatóriopred && popvivo > 0)			//predação

						if (predacaoProporcional > aleatóriopred && poplagarta > 0)			//predação PROPORCIONAL
						{
							std::uniform_real_distribution<> IndPredado(1, (int)popvivo + popmorto); //sorteia qual broca vai morrer
							aux3 = L->first;
							predado = IndPredado(tf);
							//cout << "predado " << predado << endl;
							//getchar();
							k = 0;
							while (k<predado)//vai andando até encontrar a broca a ser predada
							{
								if (aux3 != L->last)
									aux3 = aux3->next;
								k++;
								//cout << k << endl;

							}
							/*if(aux3->data.adulto>=aux3->data.idade && aux3->data.idade>=aux3->data.ovolagarta)//determina se a broca está na fase de lagarta
							{
								aux3->data.lagarta = true;
							}else
							{
								aux3->data.lagarta = false;
								cout << "falso" << endl;
								cout << aux3->data.adulto << " " << aux3->data.idade << endl;
							}//*/
							aux3->data.lagarta = (aux3->data.idade < IndAdult&&aux3->data.idade >= aux3->data.ovolagarta) ? true : false; // checa se é uma lagarta
							aux3->data.lagarta = (aux3->data.lagarta == true && aux3->data.vivo == true) ? true : false; //só é lagarta se estiver viva

							//cout << " final - "<<aux3->data.vivo<< " "<< aux3->data.idade<<" "<< aux3->data.lagarta<<endl;
							//getchar();
							while (aux3->data.lagarta == false)//aux3->data.vivo == false||aux3->data.lagarta==false)//mata só se a broca estiver viva e estiver na fase da lagarta
							{
								if (aux3 != L->last)
								{
									aux3 = aux3->next;
								}
								else
								{
									//cout << "ultimo" << aux3->data.numero << " e na lista " << endl;
									aux3 = L->first;
								}
								k++;
								//cout <<" segundo teste"<< k << endl;
								//getchar();
							}
							//cout << " vai deletar " << aux3->data.numero << " femea? " << aux3->data.femea << " t - " << actual_time << " vivo ? " << aux3->data.vivo << endl << " idade=" << aux3->data.idade << " lagarta? " << aux3->data.lagarta << endl;
							//getchar();

							//delete_Elem(L, aux3);
							aux3->data.vivo = false;				//deleta brocas e nascem vespas
							aux3->data.lagarta = false;			//se morreu deixou de ser lagarta
							popvivo--;							//fala que matou 1 individuo da lista de brocas vivas
							poplagarta--;
							popmorto++;							//acrescenta 1 na lista de brocas mortas
							for (j = 0; j<auxfilhos; j++)    //configura os novos filhos
							{
								if (j > 200)
								{
									printf("loop infinito filhos %d, idade %d, etiqueta %d, j %d, auxfilhos %d \n", aux2->data.filhos, aux2->data.idade, aux2->data.numero, j, auxfilhos);
									getchar();
								}
								Vespa v(MediasMatriz, actual_time);
	
								insert_Elem(Lv, v);              //insere o indivíduo
																 //printf("entrou no for. filhos %d, idade %d, etiqueta %d\n", aux2->data.filhos, aux2->data.idade, aux2->data.numero);

							}
						}
					
					}

					//printf("antes de deletar. filhos %d, idade %d, etiqueta %d\n", aux2->data.filhos, aux2->data.idade, aux2->data.numero);
					//printf("deletou elemento no else no else. filhos %d, idade %d, etiqueta %d\n", aux1->data.filhos, aux1->data.idade, aux1->data.numero);
					//getchar();
				}
				if (aux1->data.idade <aux1->data.espVida  && aux1->data.vivo == true)                    //teste para ver se o indivíduo ainda tem algum dia de vida e está vivo
				{

					aux1->data.idade++;                     //em caso de ter dias, diminuir a expectativa de vida
														 //aux1 = aux1->next;
				}
				else
				{
					//printf("gente morta %d\n", j);
				}

				if (aux1->data.vivo == true)
					popvivoV++;
				if (aux1->data.vivo == false)
					popmortoV++;
				if (aux1->data.femea == true)
					popfemeaV++;
				if (aux1->data.femea == false)
					popmachoV++;
				aux1 = aux1->next;
			}
			//delete_Elem(Lv, dummy_elem);



			//printf("gente morta %d, gente viva %d\n", popmorto, popvivo);
			//getchar();
			// escreve em arquivo de saida os dados da lista
			/*
			if (actual_time == 10)
			{
			i = 0;
			aux1 = L->first;
			while (aux1 != L->last)
			{
			fprintf(ages, "%d %d %d %d\n", actual_time, i++, aux1->data.idade, aux1->data.filhos);
			aux1 = aux1->next;
			}
			//fprintf(ages, "%d %d %d %d\n", actual_time, i++, aux1->data.idade, aux1->data.filhos);
			}*/
			//fprintf(pop, "%d %d\n", actual_time, L->size);
			aux1 = L->first;
			popvivo = 0;
			poplagarta = 0;
			popmorto = 0;
			popfemea = 0;
			popmacho = 0;
			popfemeaF = 0;
			while (aux1!=NULL)
			{
				/********************************************************************************
				HISTOGRAMA ABAIXO - não usar
				*********************************************************************************/
				/*
				if (aux1->data.vivo == true)
				{
				saidaId << aux1->data.idade << endl;		// imprime idade para histograma
				}//*/

				//cout << actual_time << endl;
				//cout << "broca " << aux1->data.idade << " indivíduo "<< aux1->data.numero<<" vivo -> "<< aux1->data.vivo << endl;
				//getchar();
				if (aux1->data.lagarta == true)
					poplagarta++;
				if (aux1->data.vivo == true)
					popvivo++;
				if (aux1->data.vivo == false)
					popmorto++;
				if (aux1->data.femea == true && aux1->data.vivo == true)
					popfemea++;
				if (aux1->data.femea == false && aux1->data.vivo == true)
					popmacho++;
				if (aux1->data.femea == true && aux1->data.vivo == true && aux1->data.fertil == true)
					popfemeaF++;
				aux1 = aux1->next;
			}//*/
			 /******************************************************
			 Vespa
			 *********************************************************/
			aux1 = Lv->first;
			popvivoV = 0;
			popmortoV = 0;
			popfemeaV = 0;
			popmachoV = 0;
			popfemeaFV = 0;
			while (aux1 != NULL)
			{

				if (aux1->data.vivo == true)
					popvivoV++;
				if (aux1->data.vivo == false)
					popmortoV++;
				if (aux1->data.femea == true && aux1->data.vivo == true)
					popfemeaV++;
				if (aux1->data.femea == false && aux1->data.vivo == true)
					popmachoV++;
				if (aux1->data.femea == true && aux1->data.vivo == true && aux1->data.fertil == true)
					popfemeaFV++;
				aux1 = aux1->next;
			}//*/

			MediasMatriz[actual_time][0] += popvivo;
			MediasMatriz[actual_time][1] += popmorto;
			MediasMatriz[actual_time][2] += popfemea;
			MediasMatriz[actual_time][3] += popmacho;
			MediasMatriz[actual_time][4] += popfemeaF;
			MediasMatriz[actual_time][5] += popvivoV;
			MediasMatriz[actual_time][6] += popmortoV;
			MediasMatriz[actual_time][7] += popfemeaV;
			MediasMatriz[actual_time][8] += popmachoV;
			MediasMatriz[actual_time][9] += popfemeaFV;
			MediasMatriz[actual_time][10] += poplagarta;
			MediasMatriz[actual_time][11] += pow(popvivo, 2.0);
			MediasMatriz[actual_time][12] += popvivoV*popvivoV;
			MediasMatriz[actual_time][13] += PopBrocAdul;
			
			/*********************************************************************************
			Avaliação do campo - probabilidade
			*********************************************************************************/
			if (l == 0)
			{

				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<double> aleatorio(0.000, 1.000);


				{

					int n, BrocasEncontradas = 0;
					double probabilidadeBroca = 0, sorteio = 0, BrocasAmostra;
		//			probabilidadeBroca = (double)media[actual_time][0] / cana;
					for (n = 0; n < amostraCamp; n++)
					{
						sorteio = aleatorio(rd);
						//sorteio = 1.0;
						if (probabilidadeBroca > sorteio)
						{
							BrocasEncontradas++;
						}
						//cout << media[actual_time][0] << " " << probabilidadeBroca << " " << sorteio << " " << BrocasEncontradas << " " << n << " " << actual_time << endl;
						//getchar();
					}
					BrocasAmostra = BrocasEncontradas*cana / amostraCamp;
					//cout << "Quantidade de brocas amostradas = " << BrocasAmostra << " Brocas reais = " << V << endl;
					//getchar();
					//saidaAvaliacao << BrocasAmostra << " " << media[actual_time][0] << " " << actual_time << " " << BrocasEncontradas << endl;
				}
			}

			//cout << popvivo << " " << media[actual_time][0] << " " << media[actual_time][10] <<" "<< popvivo*popvivo<< endl;
			//getchar();
			/********************************************************************************
			HISTOGRAMA ABAIXO
			*********************************************************************************/
			/*
			if(l==0)
			saidaId.close();					//histograma
			saidaIdCo.close();					//histograma
			//*/
		}cout << "media " << l << " de " << mediaDados << endl;
		cout << "antes de apagar" << endl;
		//aux1 = L->last;
		//cout <<"Ultima broca "<< aux1->data.numero << endl;
		cout << endl;
		//cout << media[tempo - 1][5] << endl;
		erase_List(L);
		erase_List(Lv);

	}
	double DesvioB = 0;
	double DesvioC = 0;

	//	cout << media[tempo-1][0] << endl;
	for (actual_time = 0; actual_time<tempo; actual_time++)
	{
		for (int k = 0; k<= 10; k++)
		{
			//media[actual_time][k] = (double)media[actual_time][k] / mediaDados;
			//cout << media[actual_time][k] << " tempo - " << actual_time<< " k - "<< k<< endl;

		}
		//media[actual_time][13] = (double)media[actual_time][13] / mediaDados;
		//cout << "saiu primeiro loop - funcionando " << actual_time << endl;
		//getchar();
		if (actual_time > 0)
		{

			DesvioB = pow((double)(MediasMatriz[actual_time][11] - MediasMatriz[actual_time][0] * MediasMatriz[actual_time][0] * mediaDados) / (mediaDados - 1), 0.5);
			DesvioC = pow((double)(MediasMatriz[actual_time][12] - MediasMatriz[actual_time][5] * MediasMatriz[actual_time][5] * mediaDados) / (mediaDados - 1), 0.5);
		}
		//cout << DesvioB << endl;
		saida << actual_time << " " << MediasMatriz[actual_time][0] << " " << MediasMatriz[actual_time][1] << " " << MediasMatriz[actual_time][2] << " " << MediasMatriz[actual_time][3] << " " << MediasMatriz[actual_time][4] << " " << DesvioB << " " << MediasMatriz[actual_time][13] << endl;
		//cout << media[actual_time][10] << " " << media[actual_time][0] * media[actual_time][0] * mediaDados << " "<< media[actual_time][0]<<endl;
		//getchar();
		saidaV << actual_time << " " << MediasMatriz[actual_time][5] << " " << MediasMatriz[actual_time][6] << " " << MediasMatriz[actual_time][7] << " " << MediasMatriz[actual_time][8] << " " << MediasMatriz[actual_time][9] << " " << DesvioC << endl;
		//cout << "gravou no arquivo " << endl;
		saidaPlano << MediasMatriz[actual_time][0] << " " << MediasMatriz[actual_time][5] << endl;
	}
	//cout << media[tempo - 1][5] << "\a /a" << endl;
	saidaAvaliacao.close();
	system("PAUSE");
	//return 0;
}






