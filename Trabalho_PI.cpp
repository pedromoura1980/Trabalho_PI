#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h> // Foi adicionado por causa do comando getch();
#include <ctype.h> //para usar o toupper;

#define TAMANHO 100  // Definição da constante TAMANHO com valor 100

//VARIAVEIS GLOBAIS
int total_meios_ = 0; 	//total de meios existente
int total_pedidos_ = 0;	//total de pedidos existente
int total_pendentes_ = 0;	//total de pedidos pendentes

typedef struct // Struct para todos os transportes
{
	int codigo; 
	char tipo [30];
	float custo;
	int autonomia;
} transportes_ ;



typedef struct  // Struct para pedidos
{
	int n_ordem;
	char nif[10];
	int cod_meio;
	int tempo;
	int distancia;
} pedidos_ ;

typedef struct  // Struct para pedidos pendentes, pedidos sem atribuição de meio
{
	int n_ordem;
	int distancia;
	char tipo[30];
} pendentes_ ;

// Protótipos 
int menu();
int existe_meio();
void limpar();
int inserir_atribuir_meio(transportes_ meios[], int qt, char tipo[], float custo, int autono);
int inserir_atribuir_pedido(pedidos_ pedidos[], int qt_, char nif_[], int tempo_, int distancia_);
void grava_meios_txt(transportes_ meios[], int qtd);
void menu_aux (char nome[]);
int ler_meios_txt(transportes_ meios[]);
int ler_meios_array(transportes_ meios[], int qtd);
void menu_inserir_meios (transportes_ meios[]);
void menu_inserir_pedidos (pedidos_ pedidos[], transportes_ meios[]) ;
void menu_listar_meios (transportes_ meios[]);
void menu_listar_pedidos(pedidos_ pedidos[], pendentes_ pendentes[]);
int convert_ID (char id_[]);
int formato_codigo_meio (char id_[]);
int formato_codigo_pedido (char id_[]);
int valida_id_meio (transportes_ meios[] , char cod_[] );
int valida_id_pedido (pedidos_ pedidos[] , char n_ordem_[] );
int valida_tipo_meio (transportes_ meios[] , char tipo_[] );
int ler_pedidos_txt(pedidos_ pedidos[]);
int ler_pedidos_array(pedidos_ pedidos[], int qtd, pendentes_ pendentes[]);
int novo_id_meio(transportes_ meios[], int qt);
int novo_id_pedidos(pedidos_ pedidos[], int qt);
void grava_pedidos_txt(pedidos_ pedidos[], int qtd);
void menu_eliminar_pedidos (pedidos_ pedidos[] , pendentes_ pendentes[] );
void grava_pendentes_txt(pendentes_ meios[], int qtd);


//Limpar a tela...
void limpar()
{
    system ("clear || cls");
}


//FUNCOES

//procurar o id do meio se existe
int valida_id_meio (transportes_ meios[] , char cod_[] )
{
	
	int aj_result =0;
	int codigo_ =0;
	int i=0;
	
	aj_result = formato_codigo_meio(cod_);

	if (aj_result==1)
	{
		
		aj_result=0 ; //Para garantir que se o código for encontrado, passa novamente a 1
		
	 	codigo_ = convert_ID(cod_) ; //converter o formato id M_000 em 000
	 	
	 	for (i=0;i<total_meios_;i++) 
	 	{
	
			if (meios[i].codigo == codigo_ && codigo_ !=0  )
			{
				
				//Lista a informação do id
			 	printf ("************************************************************\n");
			 	printf ("\n Codigo		Tipo		Custo(Eur)	Autonomia(Km)\n");
			 	
			 	if (strlen(meios[i].tipo) <=7 )
					printf(" M_%d		%s		%.2f		%d\n",meios[i].codigo,meios[i].tipo,meios[i].custo,meios[i].autonomia);
				else
					printf(" M_%d		%s	%.2f		%d\n",meios[i].codigo,meios[i].tipo,meios[i].custo,meios[i].autonomia);
	
				printf ("\n************************************************************\n");

				return (1) ;
			}
		
	    }
	
	}

	return (0);
	
	//0 meio não encontrado
	//1 meio encontrado e listado
	
}

//por id_ de meio, verifica a disponibilidade de autonimia com base já nos pedidos existentes
int calcula_autonia(pedidos_ pedidos[], int id_)
{
	int i=0;
	int soma_dist=0;
	
	for (i=0;i<total_pedidos_ ;i++) 
 	{
		if (pedidos[i].n_ordem != 0 && pedidos[i].cod_meio == id_)
		{
			soma_dist = soma_dist + pedidos[i].distancia ;
		}
		
	}
	
	return (soma_dist);
}
//procurar o tipo de meio se existe(trotinete carro...etc...
int valida_tipo_meio (transportes_ meios[] , char tipo_[] , pedidos_ pedidos[] )
{
	int aj_ =0;
	int codigo_ =0;
	int i=0;
	int encontrou=0;
	
	int auto_disp_ =0;
	
 	for (i=0;i<total_meios_;i++) 
 	{

		if (meios[i].codigo != 0 && strcmpi(meios[i].tipo,tipo_) ==0 )
		{
			if (aj_==0)//Cabeçalho apenas mostra 1 vez
				{
					//Lista a informação de todos os ids do tipo de meio escolhido
				 	printf ("\n***************** Disponibilidade dos Meios ******************\n");
				 	printf ("\n Codigo		Tipo		Custo(Eur)	Auto.Disponivel(Km)\n");
				 	aj_=1;
				}
			
			auto_disp_ =0;
			auto_disp_ = calcula_autonia(pedidos, meios[i].codigo ) ;
			
			if (auto_disp_<0) //na possibilidade de ter valores negativos
				auto_disp_ =0;
				
		 	if (strlen(meios[i].tipo) <=7 )
				printf(" M_%d		%s		%.2f		%d\n",meios[i].codigo,meios[i].tipo,meios[i].custo, (meios[i].autonomia -auto_disp_ ) );
			else
				printf(" M_%d		%s	%.2f		%d\n",meios[i].codigo,meios[i].tipo,meios[i].custo, (meios[i].autonomia - auto_disp_) );

			encontrou =1 ;
		}
	
    }
	
	printf ("\n************************************************************\n");
	return (encontrou);
	
	//0 - meio não encontrado
	//1 - meio encontrado e listado
	
}

//Identifica qual o ID to meio que será para atribuir ao pedido
int atribui_id_meio_ao_pedido (transportes_ meios[] , char tipo_[] , pedidos_ pedidos[] , int distancia_, int aj_  )
{
	int i=0,j=0;
	int auto_disp_ =0; //autonomia disponivel
	
	int matriz[total_meios_][2] ={}; //Para gravar os meios e a respectiva autonomia 'colunas |id meio | autonomia|
	int aj_matriz =0; //contabiliza as vezes que entrou no matriz
	int ad_ =0; //se entrou, ajuda a não continuar a acrescentar
	
	
 	for (i=0;i<total_meios_;i++) 
 	{

		if (meios[i].codigo != 0 && strcmpi(meios[i].tipo,tipo_) ==0 )
		{
			auto_disp_ =0;
			auto_disp_ = calcula_autonia(pedidos, meios[i].codigo ) ;

			if (auto_disp_<0) //na possibilidade de ter valores negativos
				auto_disp_ =0;
				
			ad_=0;
			for (j=0;j<total_meios_  && ad_ ==0 ;j++) //grava por id qual a autonomia disponivel
			{
				if (matriz[j][1] == 0)
				{
					matriz[j][1] = meios[i].codigo;
					matriz[j][2] = (meios[i].autonomia -auto_disp_ ) ;
					
					aj_matriz++;
					ad_=1;
					
				}
			}
			
		}
	
    }
	
	int return_ =0;
	int dist_min=0;
	
	//varifica se existiram dados, e faz a analise qual o meio que mais se aproxima do pedido
	if (aj_matriz!=0 )
	{
		for (j=0;j<aj_matriz;j++ )
		{	
			//tem ter uma autonomia superior à distancia que o pediddo pretende
			// e caso ja tenha encontra um id, apenas atribui outro, se esse outro ainda for mais proximo da distancia pretendida
			
			if ( distancia_ <= matriz [j][2] ) 
			{
				if (dist_min == 0) //primeira entrada, não precisa de confirmar mais nada
				{
					return_ =	matriz [j][1];
					dist_min =	matriz [j][2];
				}

				else if ( matriz [j][2] < dist_min )
				{
					return_ =	matriz [j][1];
					dist_min =	matriz [j][2];
				}
				
			}
		}	
	}
	
	if (aj_==0) //so mostra esta informação se for 0
	{
		if (return_ !=0)
			printf ("\nFoi-lhe atribuido o ID do Meio M_%d (%s):", return_, tipo_);
		else
			{
				printf ("\nNeste momento nao existe nenhum Meio para satisfazer o seu pedido.");
				printf ("\nAssim que existir um Meio disponivel o mesmo sera atribuido.");	
			}
	
		printf ("\n************************************************************\n");
	}
	
	return (return_);

	
}

//funcao para atribuir novo ID ao meio
int novo_id_meio(transportes_ meios[], int qt)
{	
	int i=0;
	int novo_id =0;

 	for (i=0;i<qt;i++)
	{
		if ( meios[i].codigo > novo_id)
		{
			novo_id =meios[i].codigo;
		}
 	}
 	
 	novo_id ++;
 	
 return(novo_id);
}

//funcao para atribuir novo ID ao pedido
int novo_id_pedido(pedidos_ pedidos[], int qt)
{	
	int i=0;
	int novo_id =0;

 	for (i=0;i<qt;i++)
	{
		if ( pedidos[i].n_ordem > novo_id)
		{
			novo_id =pedidos[i].n_ordem;
		}
 	}
 	
 	novo_id ++;
 	
 return(novo_id);
}
//Converte formato M_000 ou P_000 em numero
int convert_ID(char id_[])
{
	
//funcao tirada da net
//Em uma string, considera apenas os numericos
//conseguir saber qual o ID de um meio com o format M_000
char c; 
int i,digit,number=0; 

for(i=0;i<strlen(id_);i++) 
{ 
	c = id_[i]; 
	
	if(c>= '0' && c <='9') //Verificar se é numerico
	{ 
		digit = c - '0'; 
		number = number*10 + digit; 
	} 
} 
	

	return number ;
	  

}

//Converser o MD_ID para ID
int formato_codigo_meio (char id_[])
{	
	int aj_validar =0;

	
	if (('M' == id_[0]) || ('m' == id_[0]) )
		aj_validar =1;
	else
		aj_validar =0;
			
	if (aj_validar ==1)	
		if ('_' == id_[1] )
			aj_validar =1;
		else
			aj_validar =0;

		
    return aj_validar;

}


//Converser o P_000 para P_
int formato_codigo_pedido (char id_[])
{	
	int aj_validar =0;

	
	if (('P' == id_[0]) || ('p' == id_[0]) )
		aj_validar =1;
	else
		aj_validar =0;
			
	if (aj_validar ==1)	
		if ('_' == id_[1] )
			aj_validar =1;
		else
			aj_validar =0;

		
    return aj_validar;

}

//funcao inserir novo meio
int inserir_atribuir_meio(transportes_ meios[], int qt_, char tipo[], float custo, int autono)
{
	int aj_qt =0;
	
	if (qt_ < TAMANHO) 
	{
	
	
		aj_qt = qt_;
		
		meios[aj_qt].codigo = novo_id_meio(meios, qt_);
	 	meios[aj_qt].custo = custo;
		meios[aj_qt].autonomia = autono;
		strcpy(meios[aj_qt].tipo,tipo);	
		
	  	return(1);
	 }
	 else return(0);
}

//funcao inserir novo pedido
int inserir_atribuir_pedido(pedidos_ pedidos[], int qt_, char nif_[], int tempo_, int distancia_, transportes_ meios[], char tipo_[])
{
	int aj_qt =0;
	int cod_meio_ =0;
	int cod_pedido_ =0;
	//fazer funcao para atribuir meio com base no pedido
	
	if (qt_ < TAMANHO) 
	{
	
		cod_meio_ = atribui_id_meio_ao_pedido(meios,tipo_,pedidos,distancia_,0) ;
		cod_pedido_=novo_id_pedido(pedidos, qt_);
		
		aj_qt = qt_;
		
		pedidos[aj_qt].n_ordem = cod_pedido_;
	 	pedidos[aj_qt].cod_meio = cod_meio_;
		pedidos[aj_qt].tempo = tempo_;
		pedidos[aj_qt].distancia = distancia_;
		strcpy(pedidos[aj_qt].nif,nif_);	
		
	  	return(cod_pedido_);
	 }
	 else return(0);
}


//funcao que verifica se existe algum pedido com 0 id meios atribuido, e atribui esse mesmo meio
void atribuir_pendentes(pedidos_ pedidos[], transportes_ meios[], pendentes_ pendentes[])
{

	int cod_meio_ =0;
	int i=0,j=0;
	int distancia_ =0;
	char tipo_[30]="";
	
	int aj_pend_=0;
	
	
	if (total_pendentes_ != 0)
	{
		for (i=0; i<total_pendentes_ ; i++)
		{
			if ( pendentes[i].n_ordem !=0)
			{
				
				strcpy(tipo_, pendentes[i].tipo);
				distancia_=pendentes[i].distancia;
				
				cod_meio_ = atribui_id_meio_ao_pedido(meios,tipo_,pedidos,distancia_ ,1) ;
				
			 	for (j=0;j<total_pedidos_ && cod_meio_ !=0 ;j++)
			 	{
			 		if (pendentes[i].n_ordem==pedidos[j].n_ordem)
			 		{
			 			pedidos[j].cod_meio = cod_meio_;
			 			pendentes[i].n_ordem =0;  //depois de atribuir o pedido, coloca a posicao do array a 0

			 			break;
					}
				}

					
			}

		}
		
	
	  	grava_pedidos_txt(pedidos, total_pedidos_  );
	  	grava_pendentes_txt(pendentes,total_pendentes_) ;
	 }
	

}

int valida_se_meio_atribuido ( pedidos_ pedidos[] , int codigo_ )
{
	int aj =1;
	int i=0;
	
	for (i;i<total_pedidos_; i++)
	{
		if (pedidos[i].cod_meio == codigo_ && pedidos[i].n_ordem !=0 )
			{
				aj =0;
				break;	
			}
	}
	
	//retrun = 1 --Não tem id inserido em nenhum pedido, pode continuar
	//return = 0 --Tem id inserido não pode continuar	
	
	return aj;
	
}

//funcao eliminar meio
int eliminar_meio(transportes_ meios[], int qt_, char cod_[], pedidos_ pedidos[])
{
	//optamos por não ocupar a posição eliminada com os restantes dados do array
	//a estrategia é colocar os códigos eliminados a 0 e usar o usar como referencia para saber o que não deve ser considerado
	
	int codigo_;
	char tipo_[30];
	float custo_;
	int autono_;
 	
 	int aj_result =0, i=0;
 	int aj_result_valida =0;
 	
 	char resp_[1] ="" ; //Ajuda na questão se deseja mesmo eliminar o meio
 	
 	aj_result = formato_codigo_meio(cod_);
 	
 	
 	if (aj_result==1)
 	{
		
		aj_result=0 ; //Para garantir que se o código for encontrado, passa novamente a 1
		
	 	codigo_ = convert_ID(cod_) ; //converter o formato id M_000 em 000
	 	
	 	for (i=0;i<qt_;i++) 
	 	{

			if (meios[i].codigo == codigo_ && codigo_ !=0  )
			{
				
				aj_result_valida = valida_se_meio_atribuido (pedidos , codigo_  );
				
				if (aj_result_valida ==1) //se o código não estiver em nenhum pedido, pode ser eliminado
				{

					//Lista a informação que será eliminada
				 	printf ("************************************************************\n");
				 	printf ("\n Codigo		Tipo		Custo(Eur)	Autonomia(Km)\n");
				 	
				 	if (strlen(meios[i].tipo) <=7 )
						printf(" M_%d		%s		%.2f		%d\n",meios[i].codigo,meios[i].tipo,meios[i].custo,meios[i].autonomia);
					else
						printf(" M_%d		%s	%.2f		%d\n",meios[i].codigo,meios[i].tipo,meios[i].custo,meios[i].autonomia);
	
					printf ("\n************************************************************\n");
	
					printf("Prima S, se desejar eliminar o codigo %s: ", cod_);
					scanf("%s", resp_);	   
			
			
					if ( (strcmp (resp_,"S") == 0) || (strcmp (resp_,"s") == 0) ) //0 significa que é igual
					{
						
						meios[i].codigo = 0; //0 serve para controlar o que é para eliminar
						return (1) ;
						
					}
					else
						return (2) ; //ao colocar 2, não elimina nada e apenas questiona se quer continuar no meu de eliminação de meios
	
					
				}
				else
					return (3) ; //Não pode eliminar, meio está atribuido a um pedido

			}	
			 		
	    }

	}

	return (aj_result) ;
	
	
	//retorno 0 - não encontrou o id
	//retorno 1 - encontrou, é para eliminar da lista
	//retorno 2 - decidiu não eliminar
	//retorno 3 - Não pode eliminar, meio está atribuido a um pedido
}

//funcao eliminar pedidos
int eliminar_pedido(pedidos_ pedidos[], int qt_, char cod_[])
{
	//optamos por não ocupar a posição eliminada com os restantes dados do array
	//a estrategia é colocar os códigos eliminados a 0 e usar o usar como referencia para saber o que não deve ser considerado
	
	int codigo_;
 	int aj_result =0, i=0;
 	
 	char resp_[1] ="" ; //Ajuda na questão se deseja mesmo eliminar o meio
 	
 	aj_result = formato_codigo_pedido(cod_);
 	
 	
 	if (aj_result==1)
 	{
		
		aj_result=0 ; //Para garantir que se o código for encontrado, passa novamente a 1
		
	 	codigo_ = convert_ID(cod_) ; //converter o formato id P_000 em 000
	 	
	 	for (i=0;i<qt_;i++) 
	 	{

			if (pedidos[i].n_ordem == codigo_ && codigo_ !=0  )
			{
				
					printf ("************************************************************\n");
				 	printf ("\n N.Ordem	NIF		Codigo		Tmp(min)	Dist.(km)	\n");

					printf(" P_%d		%s	M_%d		%d		%d\n",pedidos[i].n_ordem,pedidos[i].nif,pedidos[i].cod_meio,pedidos[i].tempo,pedidos[i].distancia);

					printf ("\n************************************************************\n");
				  				

				printf("Prima S, se desejar eliminar o Pedido %s: ", cod_);
				scanf("%s", resp_);	   
		
		
				if ( (strcmp (resp_,"S") == 0) || (strcmp (resp_,"s") == 0) ) //0 significa que é igual
				{
					
					pedidos[i].n_ordem = 0; //0 serve para controlar o que é para eliminar
					return (1) ;
					
				}
				else
					return (2) ; //ao colocar 2, não elimina nada e apenas questiona se quer continuar no meu de eliminação de meios
			}	
			 		
	    }

	}

	return (aj_result) ;
	
	
	//retorno 0 - não encontrou o id
	//retorno 1 - encontrou, é para eliminar da lista
	//retorno 2 - decidiu não eliminar
}

//Funcao para escrever o conteudo do array das meios no ficheiro de texto
void grava_meios_txt(transportes_ meios[], int qtd)
{
	int i;
 	
	FILE* fp_;
 
 	fp_ = fopen("meios_transporte.txt","wt");   // "wt" write text  "rt" read text  "at" append
 	
	for(i=0;i<qtd;i++)
 		{		
 			if (meios[i].codigo!= 0) //caso seja 0 não grava. Estrategia para eliminar posiçoes do array
			 {
				fprintf(fp_,"%d;",meios[i].codigo);
				fprintf(fp_,"%.2f;",meios[i].custo);
				fprintf(fp_,"%d;",meios[i].autonomia);
				fprintf(fp_,"%s\n",meios[i].tipo);			 	
			 }

		}
 
 	fclose(fp_);
}


//Funcao para escrever o conteudo do array das pendentes no ficheiro de texto
void grava_pendentes_txt(pendentes_ pendentes[], int qtd)
{
	int i;
 	
	FILE* fp_;
 
 	fp_ = fopen("pendentes.txt","wt");   // "wt" write text  "rt" read text  "at" append
 	
	for(i=0;i<qtd;i++)
 		{		
 			if (pendentes[i].n_ordem!= 0) //caso seja 0 não grava. Estrategia para eliminar posiçoes do array
			 {
				fprintf(fp_,"%d;",pendentes[i].n_ordem);
				fprintf(fp_,"%d;",pendentes[i].distancia);
				fprintf(fp_,"%s\n",pendentes[i].tipo);			 	
			 }

		}
 
 	fclose(fp_);
}
//Funcao para escrever o conteudo do array das pedidos no ficheiro de texto
void grava_pedidos_txt(pedidos_ pedidos[], int qtd)
{
	int i;
 	
	FILE* fp_;
 
 	fp_ = fopen("pedidos.txt","wt");   // "wt" write text  "rt" read text  "at" append
 	
	for(i=0;i<qtd;i++)
 		{		
 			if (pedidos[i].n_ordem!= 0) //caso seja 0 não grava. Estrategia para eliminar posiçoes do array
			 {
				fprintf(fp_,"%d;",pedidos[i].n_ordem);
				fprintf(fp_,"%d;",pedidos[i].cod_meio);
				fprintf(fp_,"%d;",pedidos[i].tempo);
				fprintf(fp_,"%d;",pedidos[i].distancia);		
				fprintf(fp_,"%s\n",pedidos[i].nif);	 	
			 }

		}
 
 	fclose(fp_);
}


//Função server para ler os meios inseridos no ficheiro meios_transporte.txt
int ler_meios_txt(transportes_ meios[])
{
	
	FILE* fp_;
 
 	int qt_ler,i_=0;

	int codigo_;
	char tipo_[30];
	float custo_;
	int autono_;
 	
 	qt_ler = 0;
 	
 	if (fp_ = fopen("meios_transporte.txt","rt") ) //se ficheiro existir
 	{

		 while(!feof(fp_))  // feof -> end of file
	 	{
		 	fscanf(fp_,"%d;%f;%d;%s\n",&codigo_,&custo_,&autono_,tipo_);
			
			if (codigo_ != 0)
			{
				meios[qt_ler].codigo = codigo_;
			 	meios[qt_ler].custo =  custo_;
				meios[qt_ler].autonomia = autono_;
				strcpy(meios[qt_ler].tipo,tipo_);	
				
				qt_ler++; //retorna o total de registos ja inseridos no ficheiro
			}

	
			
	 	}
	 	
		fclose(fp_);
	
	 }
 
 	
 	
 
 	return(qt_ler);
}


//Função server para ler os pedidos inseridos no ficheiro pedidos.txt
int ler_pedidos_txt(pedidos_ pedidos[])
{
	
	FILE* fp_;
 
 	int qt_ler,i_=0;

	int cod_meio_=0;
	int dist_=0;
	char nif_[10]="";
	int n_ordem_=0;
	int tempo_;
 	char tipo_[30]=""; //será para usar no cado de não atribuir pedido, saber que tipo de meio foi solicitado.
 	
 	qt_ler = 0;
 	
 	if (fp_ = fopen("pedidos.txt","rt") ) //se ficheiro existir
 	{

		 while(!feof(fp_))  // feof -> end of file
	 	{
		 	fscanf(fp_,"%d;%d;%d;%d;%s\n",&n_ordem_,&cod_meio_,&tempo_,&dist_,nif_);
			
			if (n_ordem_ != 0)
			{

				pedidos[qt_ler].n_ordem = n_ordem_;
			 	strcpy( pedidos[qt_ler].nif,nif_);
				pedidos[qt_ler].cod_meio = cod_meio_;
				pedidos[qt_ler].tempo = tempo_;
				pedidos[qt_ler].distancia = dist_ ;
	
				
				qt_ler++; //retorna o total de registos ja inseridos no ficheiro
								
			}

	
			
	 	}
	 	
		fclose(fp_);
	
	 }
 

 	return(qt_ler);
}

//Função server para ler os pedidos pendentes no ficheiro pendentes.txt
int ler_pendentes_txt(pendentes_ pendentes[])
{
	
	FILE* fp_;
 
 	int qt_ler,i_=0;

	int dist_=0;
	int n_ordem_=0;
	char tipo_[30]="";
 	qt_ler = 0;
 	
 	if (fp_ = fopen("pendentes.txt","rt") ) //se ficheiro existir
 	{

		 while(!feof(fp_))  // feof -> end of file
	 	{
		 	fscanf(fp_,"%d;%d;%s\n",&n_ordem_,&dist_,tipo_);
			
			if (n_ordem_ != 0)
			{
				pendentes[qt_ler].n_ordem = n_ordem_;
			 	strcpy( pendentes[qt_ler].tipo,tipo_);
				pendentes[qt_ler].distancia = dist_ ;
		
				
				qt_ler++; //retorna o total de registos ja inseridos no ficheiro
			}

	
			
	 	}
	 	
		fclose(fp_);
	
	 }
 

 	return(qt_ler);
}

//Função server para ler os meios inseridos no array
int ler_meios_array(transportes_ meios[], int qt)
{

	int codigo_;
	char tipo_[30];
	float custo_;
	int autono_;
 	int total_reg_ =0;
 	

 	printf ("	****  Lista de Meios Existentes  *****\n");
 	printf ("************************************************************\n");
 	printf ("\n Codigo		Tipo		Custo(Eur)	Autonomia(Km)\n");

 	
 	int i=0;

 	for (i=0;i<qt;i++)
	{

		if (meios[i].codigo != 0) //Apenas lista posições com códigos no array. o 0 é usado como estrategia para eliminar posicoes do array
			{
				if (strlen(meios[i].tipo) <=7 )
					printf(" M_%d		%s		%.2f		%d\n",meios[i].codigo,meios[i].tipo,meios[i].custo,meios[i].autonomia);
				else
					printf(" M_%d		%s	%.2f		%d\n",meios[i].codigo,meios[i].tipo,meios[i].custo,meios[i].autonomia);
				
				total_reg_ ++;				
			}

 	}
 	
	printf ("\n************************************************************\n");
  				

 	return(total_reg_);
}

//Função serve para ler os meios inseridos no array
int ler_pedidos_array(pedidos_ pedidos[], int qt, pendentes_ pendentes[], transportes_ meios[] )
{

	int cod_meio=0;
	int dist_=0;
	int nif_=0;
	int n_ordem=0;
	int tempo_;
 	int total_reg_ =0;
 	

 	printf ("	****  Lista de Pedidos Existentes  *****\n");
 	printf ("************************************************************\n");
 	printf ("\n N.Ordem	NIF		Codigo	Tipo		Tmp(min)	Dist.(km)		\n");

 	
 	int i=0;
 	
 	//VARIAVEIS PARA OS REGISTOS PENDENTES
	int j=0;
	char tipo_[30];
	
 	//VARIAVEIS PARA PROCURAR O TIPO DE MEIO
	int k=0;
	
	//no caso de ser um pedido pendente assinala o meio com ** 
	char char_1 [3] ="**";
	int aj_ =0; //se passar para 1 tem pedidos pendentes, adiciona legenda
	//
 	
	 for (i=0;i<qt;i++)
	{

		if (pedidos[i].n_ordem != 0) //Apenas lista posições com códigos no array. o 0 é usado como estrategia para eliminar posicoes do array
			{
				strcpy(tipo_,""); //LIMPAR VARIVAEL
				
				//vai ao array do meio, faz pesquisa por cod do meio, e retira o tipo de meio
				for (k =0;k < total_meios_  ; k++ )
				{
					if (meios[k].codigo == pedidos[i].cod_meio)
					{
						strcpy(tipo_,meios[k].tipo);
						break;
					}
				}
				
				//vai ao array dos pendentes, faz pesquisa por n_ordem(pedido), e retira o tipo de meio	pendente			
				for (j =0;j < total_pendentes_ ; j++ )
				{
					if (pendentes[j].n_ordem == pedidos[i].n_ordem)
					{
						strcpy( tipo_ ,pendentes[j].tipo);
						
						strcat( tipo_, char_1);
						aj_=1;
						break;
					}
				}
				
				if (strlen(tipo_) <=7 )
				{
					printf(" P_%d		%s	M_%d	%s		%d		%d\n",pedidos[i].n_ordem,pedidos[i].nif,pedidos[i].cod_meio,tipo_,pedidos[i].tempo,pedidos[i].distancia);
				}
				else
				{
					printf(" P_%d		%s	M_%d	%s	%d		%d\n",pedidos[i].n_ordem,pedidos[i].nif,pedidos[i].cod_meio,tipo_,pedidos[i].tempo,pedidos[i].distancia);
				}
				total_reg_ ++;				
			}

 	}
 	
 	if (aj_ !=0)
 		printf ("\n ** [Pedidos Pendentes de atribuicao de meio] \n");
 		
	printf ("\n************************************************************\n");
  				

 	return(total_reg_);
}
//MENU PRINCIPAL
int menu()
{
	int opcao;
	
	limpar();
	
	 do { 
	 		printf ("************************************************************\n");
	    	printf ("		*****  MOBILIDADE VERDE  *****\n");
	        printf ("************************************************************\n");
			
			printf ("\n");
			
	      	//printf("1  - Lista de Pedidos \n");
	      	//printf("2  - Listar Meios de Mobilidade \n");
	      	printf("1  - Inserir Meio de Mobilidade \n");  
	      	printf("2  - Eliminar Meio de Mobilidade \n");
	      	printf("3  - Inserir Pedido \n");
	      	printf("4  - Remover Pedido \n");
	      	printf("5  - Listar Meios de Mobilidade \n");
	      	printf("6  - Listar Pedidos de Utilizacao \n");
	      	printf("7  - Calculo do Custo do Pedido \n");
	      	//printf("8  - Distribuicao de Pedidos \n");
	      	printf("8  - Listagem Plano de Utilizacao \n");
	      	//printf("12 - Armazenar Pedidos \n");
			//printf("13 - Armazenar Meios de Mobilidade \n");
			
			printf ("\n");
			
	 		printf ("************************************************************\n");
	    	printf ("		*****  0 - Sair  *****\n");
	        printf ("************************************************************\n");
	
	
	      	printf("Opcao? ");
	      
		  	scanf("%d",&opcao);
	    } 
	 while ((opcao<0)||(opcao>8));
	 
	 return(opcao);
}


void menu_aux (char nome[]) //Mostra o menu onde altera com base apenas na opção nome
{
	
	printf ("************************************************************\n");
	printf ("		*****  MOBILIDADE VERDE  *****\n");
	printf ("************************************************************\n");
	printf ("%s\n",nome);
	printf ("************************************************************\n");
}



//caso a opção no menu seja 1 //INSERCAO MEIO DE MOBILIDADE
void menu_inserir_meios (transportes_ meios[])
{
	char aj_menu_1[31] ="		*****  NOVO MEIO  *****";
	char resp[1] ="" ; //Ajuda no controlo da resposta se S ou s para continuar, caso contrario sai
	int aj_resul =0; 

	char tipo_[30];
	float custo_;
	int autono_;
	

	do  
	{
	
		limpar();

  		menu_aux (aj_menu_1);
  		
		ler_meios_array (meios , total_meios_); //ANTES ABRIR A OPÇÃO DE INSERSÃO, MOSTRAS OS MEIOS INSERIDOS
		
		
		//printf ("\n************************************************************\n");
		printf ("\nInsira os dados referentes ao novo meio:\n");
		
	   	printf("\nTipo: ");
   		scanf("%s", tipo_);
   		
   		printf("Custo(Eur): ");
   		scanf("%f",&custo_);
   		
   		printf("Autonomia(km): ");
   		scanf("%d",&autono_);
   		
   		//colocar verificação se dados estão de acordo para serem gravados
   		aj_resul = inserir_atribuir_meio(meios, total_meios_  ,tipo_,custo_,autono_);
		
    	if (aj_resul==1) 
       	{
       		
       		total_meios_++;
       		
       		grava_meios_txt(meios, total_meios_);
       		
       		printf("Novo Meio Inserido....");
       		printf ("\n************************************************************\n");
       		printf("Prima S, se desejar inserir um novo meio: ");
			scanf("%s", resp);

	   	}

	
	 } while ( (strcmp (resp,"s") == 0) || (strcmp (resp,"S") == 0) ) ;// Executa o loop sempre que a reposta seja S ou s
	
}

//caso a opção no menu seja 2 //INSERCAO MEIO DE MOBILIDADE
void menu_eliminar_meios (transportes_ meios[], pedidos_ pedidos[] )
{
	char aj_menu_2[33] ="		*****  ELIMINAR MEIO  *****"; //Titulo do Menu
	
	char resp_[1] ="" ; //Ajuda no controlo da resposta se S ou s para continuar, caso contrario sai
	int aj_resul =0; 
	char codigo_[5];
	int total_reg_ =0;
	
	do  
	{
	
		limpar();

  		menu_aux (aj_menu_2);
  		
		total_reg_ = ler_meios_array (meios , total_meios_); //ANTES ABRIR A OPÇÃO DE INSERSÃO, MOSTRAS OS MEIOS INSERIDOS
		
		
		if (total_reg_ != 0 )
		{
			//printf ("\n************************************************************\n");
			printf ("\nIdentifique qual o codigo do meio que pretende eliminar:\n");
			
		   	printf("\nCodigo : ");
	   		scanf("%s", codigo_);
	   		
	   		if (strlen(codigo_)>=2) //garatir que insere um código com tamanho para válido para validacao
			{
				
		   		aj_resul = eliminar_meio(meios, total_meios_  , codigo_, pedidos);
				
		    	if (aj_resul==1) 
		       	{
		       		//ao eliminar a variavel total_meios_ permanece com o mesmo valor. 
					//Nada será eliminado do array, execpto o codigo será alterado para 0
		       		
		       		grava_meios_txt(meios, total_meios_);
		       		
		       		printf("Meio Eliminado....");
			   	}
				else if (aj_resul==0)
				{
					printf("O meio %s nao existe, verifique se inseriu o codigo correto.", codigo_  );
				}
				else if (aj_resul==3) 
				{
					printf("O meio %s nao pode ser eliminado. Esta a ser usado em um pedido.", codigo_  );
				}						
			}
			else
				printf("O meio %s nao existe, verifique se inseriu o codigo correto.", codigo_  );
				
				
	       	printf ("\n************************************************************\n");
	   		printf("Prima S, se desejar eliminar novo meio: ");
			scanf("%s", resp_);	
				
		}
		else
		{
			printf ("	*****   SEM REGISTOS PARA LISTAR   *****");
			printf ("\n************************************************************\n");
			printf("Pressione qualquer tecla para continuar...");
			getch();
			
			break; //forçar saida

		}

		

		
	 } while ( (strcmp(resp_,"S" ) == 0) || (strcmp(resp_,"s" ) == 0) ) ;// Executa o loop sempre que a reposta seja S
	
}

//caso a opção no menu seja 3 ////Inserir Pedido
//Colocamos tb o array de meios, para listar os meios existente para fazer o pedido
//E verificar se está a usar um código de meio correto
void menu_inserir_pedidos (pedidos_ pedidos[], transportes_ meios[]) 
{
	//pedidos
	char aj_menu_3[31] ="		*****  NOVO PEDIDO  *****";
	char resp_[1] ="" ; //Ajuda no controlo da resposta se S ou s para continuar, caso contrario sai
	 
	int cod_meio=0;
	int dist_=0;
	char nif_ [10];
	int tempo_;

	int aj_resul =0;
	
	//meios
	char tipo_ [30];
	int total_reg_ =0;
	
 
	do  
	{
	
		limpar();

  		menu_aux (aj_menu_3);

		total_reg_ = ler_meios_array (meios , total_meios_ ); //ANTES ABRIR A OPÇÃO DE INSERSÃO, MOSTRAS OS MEIOS INSERIDOS
		
		
		if (total_reg_ != 0 )
		{
			//printf ("\n************************************************************\n");
			printf ("\nEscreva o Tipo de Meio que pretende alugar(exemplo: Carro..Mota.. etc..).\n");
			
		   	printf("\nTipo de Meio : ");
	   		scanf("%s", tipo_);
	   		
			aj_resul = valida_tipo_meio(meios,tipo_, pedidos);

	    	if (aj_resul==1) 
	       	{
				
				aj_resul =0;
				
				//printf ("\n************************************************************\n");
				printf ("\nInsira os dados referentes ao pedido:\n");
				
			   	printf("\nNIF: ");
		   		scanf("%s", nif_);
		   		
		   		printf("Tempo(min) que deseja utilizar o meio: ");
		   		scanf("%d",&tempo_);
		   		
		   		printf("Qual a Distancia(km) que pretende percorrer: ");
		   		scanf("%d",&dist_);
		   		
		   		//colocar verificação se dados estão de acordo para serem gravados
		   		aj_resul =inserir_atribuir_pedido (pedidos , total_pedidos_, nif_, tempo_,dist_,meios,tipo_) ;
				
				if (aj_resul!=0) 
		       	{
		       		
		       		total_pedidos_++;
		       		
		       		grava_pedidos_txt(pedidos, total_pedidos_);

		       		printf("O seu pedido ficou registado com o numero P_%d ", aj_resul );
//			       		printf ("\n************************************************************\n");
//			       		printf("Prima S, se desejar inserir novo pedido: ");
//						scanf("%s", resp_);
		
			   	}

		   	}
			else if (aj_resul==0)
			{
				printf("Tipo de meio %s nao existe, verifique se escreveu correctamente.", tipo_  );
			}
					

				
	       	printf ("\n************************************************************\n");
	   		printf("Prima S, se desejar fazer novo pedido: ");
			scanf("%s", resp_);	
				
		}
		else
		{
			printf ("	*****   SEM MEIOS PARA LISTAR   *****");
			printf ("\n************************************************************\n");
			printf("Pressione qualquer tecla para continuar...");
			getch();
			
			break; //forçar saida

		}
		  		

	 } while ( (strcmp (resp_,"s") == 0) || (strcmp (resp_,"S") == 0) ) ;// Executa o loop sempre que a reposta seja S ou s
	
}

//caso a opção no menu seja 4 //INSERCAO MEIO DE MOBILIDADE
void menu_eliminar_pedidos (pedidos_ pedidos[], pendentes_ pendentes[] , transportes_ meios[])
{
	char aj_menu_4[33] ="		*****  ELIMINAR PEDIDOS  *****"; //Titulo do Menu
	
	char resp_[1] ="" ; //Ajuda no controlo da resposta se S ou s para continuar, caso contrario sai
	int aj_resul =0; 
	char codigo_[5];
	int total_reg_ =0;
	
	do  
	{
	
		limpar();

  		menu_aux (aj_menu_4);
  		
		total_reg_ = ler_pedidos_array (pedidos , total_pedidos_, pendentes, meios); //ANTES ABRIR A OPÇÃO DE INSERSÃO, MOSTRAS OS PEDIDOS INSERIDOS
		
		
		if (total_reg_ != 0 )
		{
			//printf ("\n************************************************************\n");
			printf ("\nIdentifique qual o N. Ordem do pedido que pretende eliminar:\n");
			
		   	printf("\nN.Ordem : ");
	   		scanf("%s", codigo_);
	   		
	   		if (strlen(codigo_)>=2) //garatir que insere um código com tamanho para válido para validacao
			{
				
		   		aj_resul = eliminar_pedido(pedidos, total_pedidos_  ,codigo_);
				
		    	if (aj_resul==1) 
		       	{
		       		//ao eliminar a variavel total_pedidos_ permanece com o mesmo valor. 
					//Nada será eliminado do array, execpto o codigo será alterado para 0
		       		
		       		grava_pedidos_txt(pedidos, total_pedidos_);
		       		
		       		printf("Pedido Eliminado....");
			   	}
				else if (aj_resul==0)
				{
					printf("O pedido %s nao existe, verifique se inseriu o codigo correto.", codigo_  );
				}
						
			}
			else
				printf("O pedido %s nao existe, verifique se inseriu o codigo correto.", codigo_  );
				
				
	       	printf ("\n************************************************************\n");
	   		printf("Prima S, se desejar eliminar mais pedidos: ");
			scanf("%s", resp_);	
				
		}
		else
		{
			printf ("	*****   SEM REGISTOS PARA LISTAR   *****");
			printf ("\n************************************************************\n");
			printf("Pressione qualquer tecla para continuar...");
			getch();
			
			break; //forçar saida

		}

		

		
	 } while ( (strcmp(resp_,"S" ) == 0) || (strcmp(resp_,"s" ) == 0) ) ;// Executa o loop sempre que a reposta seja S
	
}
//caso a opção no menu seja 5 //Listar Meios de Mobilidade
void menu_listar_meios (transportes_ meios[])
{
	char aj_menu_5[31] ="		*****  LISTA MEIOS  *****";
	int total_reg_ =0;
	
	limpar();
	   			
	menu_aux (aj_menu_5);

	total_reg_ = ler_meios_array (meios , total_meios_); //ANTES ABRIR A OPÇÃO DE INSERSÃO, MOSTRAS OS MEIOS INSERIDOS

	if (total_reg_ == 0 )
	{
		printf ("	*****   SEM REGISTOS PARA LISTAR   *****");
		printf ("\n************************************************************\n");
	}
			
	
	printf("Pressione qualquer tecla para continuar...");
	getch();
	
}

//caso a opção no menu seja 6 ////Listar Pedidos de Utilizacao
void menu_listar_pedidos (pedidos_ pedidos[], pendentes_ pendentes[] , transportes_  meios[])
{
	char aj_menu_6[30] ="		*****  LISTA PEDIDOS  *****";
	int total_reg_ =0;
	
	limpar();
	   			
	menu_aux (aj_menu_6);

	total_reg_ = ler_pedidos_array (pedidos , total_pedidos_, pendentes,meios ); //ANTES ABRIR A OPÇÃO DE INSERSÃO, MOSTRAS OS MEIOS INSERIDOS

	if (total_reg_ == 0 )
	{
		printf ("	*****   SEM REGISTOS PARA LISTAR   *****");
		printf ("\n************************************************************\n");
	}
			
	
	printf("Pressione qualquer tecla para continuar...");
	getch();
	
}

int main()
{
 int opcao; //variavel para registar a opção escolhida

 transportes_ MEIOS_ [TAMANHO]; //Atribir a variavel Meios a struct transportes_
 pedidos_ PEDIDOS_ [TAMANHO]; //Atribir a variavel PEDIDOS_ a struct pedidos_
 pendentes_ PENDENTES_ [TAMANHO]; //Atribir a variavel PENDENTES_ a struct pendentes_
 
 total_meios_ = ler_meios_txt (MEIOS_); //ler_meios_txt alem de abrir o txt com os meios, tb faz o return do total de meios inseridos. Atribuindo à variavel total_meios_ esse valor 
 total_pedidos_ = ler_pedidos_txt(PEDIDOS_ ) ; //ler_pedidos_txt alem de abrir o txt com os pedidos, tb faz o return do total de pedidos inseridos. Atribuindo à variavel total_pedidos_ esse valor 
 total_pendentes_ = ler_pendentes_txt(PENDENTES_) ; //ler_pedidos_txt alem de abrir o txt com os pedidos, tb faz o return do total de pedidos inseridos. Atribuindo à variavel total_pedidos_ esse valor 
 
 atribuir_pendentes(PEDIDOS_ , MEIOS_,  PENDENTES_); //analisar se tem pedidos pendentes e se podem ser atribuidos
 
 do
 {
 	opcao = menu();

	switch(opcao)
	  {

			   	
	   case 1: //INSERCAO MEIO DE MOBILIDADE
	   			menu_inserir_meios(MEIOS_);
	   			atribuir_pendentes(PEDIDOS_ , MEIOS_,  PENDENTES_);
		   		break;
	   case 2:  //Eliminar Meio de Mobilidade
		   		menu_eliminar_meios(MEIOS_, PEDIDOS_);
		   		break;
	   case 3: //Inserir Pedido
		   		menu_inserir_pedidos(PEDIDOS_,MEIOS_);
		   		break;	   		
	   case 4: //Remover Pedido
		   		menu_eliminar_pedidos(PEDIDOS_, PENDENTES_, MEIOS_  );
		   		atribuir_pendentes(PEDIDOS_ , MEIOS_,  PENDENTES_);
		   		break;
	   case 5: //Listar Meios de Mobilidade
		   		menu_listar_meios(MEIOS_);
		   		break;
	   case 6: //Listar Pedidos de Utilizacao
	   		   	menu_listar_pedidos(PEDIDOS_,PENDENTES_, MEIOS_ );
		   		break;
	   case 7: //Calculo do Custo do Pedido

			   	break;

//	   case 8: //Distribuicao de Pedidos
//	 
//		   		break;
	   case 8: //Listagem Plano de Utilizacao
	
		   		break;	   		
	  } 
} while (opcao !=0);
 
 return(0);
}


