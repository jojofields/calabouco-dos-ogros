#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <conio2.h>
#include <string.h>
#include <windows.h>
#include "jogo.h"
#define N_LINHA 11
#define N_COLUNA 27
#define INIMIGOS 4
#define PAREDE '#'
#define INIMIGO 'O'
#define JOGADOR 'J'
#define MOEDA 'M'
#define PM_ATIVADA 'A'
#define PM_DESATIVADA 'D'
#define ALAVANCA 'B'
#define CHAVE 'C'
#define VAZIO ' '

//Codigos de acoes disponiveis para o teclado
#define CIMA 10
#define BAIXO 11
#define ESQUERDA 12
#define DIREITA 13
#define APERTA_ALAVANCA 14
//Codigos para tratar teclas especiais e invalidas do jogo
#define TAB 9
#define SETA_DIR 77
#define SETA_ESQ 75
#define SETA_CIMA 72
#define SETA_BAIXO 80

#define ACAO_INVALIDA 999

//Codigos para tratar os itens pegados
#define MAISVIDA 1
#define MENOSVIDA -1
#define MAISCHAVE 2
#define MAISMOEDA 3
#define ACHOUSAIDA 4

//Codigos para gerenciar o fim de cada fase
#define PERDEU 100
#define GANHOU 101
#define OPCAO_SAIR 102

//Alguns codigos para chamada de execucao do teclado
#define CHAMA_TECLADO 103

//Codigos para as opcoes selecionadas no menu
#define NOVO_JOGO 104
#define CARREGAR_JOGO 105
#define SALVAR_JOGO 106
#define VOLTAR_JOGO 107
#define SAIR_JOGO 108

//Booleanos para teste
#define TRUE 1
#define FALSE 0

void letreiro(){
    char buffer[100];
    FILE *arq;
    int j=0;

    if(!(arq = fopen("letreiro2.txt","r"))) { // abre para leitura
//        flag=0; Nao temos return
    } else {
        gotoxy(8,j+4);

        while(fgets(buffer, 100, arq) != NULL) {
                printf("%s",buffer);
                j++;
            gotoxy(8,j+4);
            }

             fclose(arq);
        printf("\n\n");
        }
}

//Iremos usar isso pra setar os status iniciais do heroi
void inicializa_heroi(PERSONAGEM *heroi, int nivel)
{
    //Se for o primeiro nivel, preciso zerar pontuacao e colocar as vidas em 3
    if(nivel == 0)
    {
        heroi->vidas = 3;
        heroi->pontos=0;
        heroi->nivel=1;
    }
    heroi->moedas = 0;
    heroi->moedas_old = 0;
    heroi->moedas = 0;
    heroi->invencivel = 0;
    heroi->segundos_invencivel=0;
    heroi->chaves=0;
}
//Função para gerenciar o relogio
int tempo(RELOGIO* relogio)
{
    int morreu=FALSE;

    //Se os segundos atingirem zero, então o programa decrementa um do campo dos minutos e inicializa os segundos em 59
    if(relogio->segundos==0)
    {
        relogio->minutos=relogio->minutos-1;
        relogio->segundos=59;
    }
    else //Senão decrementa um segundo do ponteiro dos segundos
        relogio->segundos=relogio->segundos-1;
    if(relogio->segundos==0 && relogio->minutos==0) //Se os dois campos do relógio zerarem, o programa altera a flag morreu para que possa ser retirada uma vida do bomberman
        morreu=TRUE;
    return morreu;
}

//FUNCOES DE MENU
//Essa funcao realiza a abertura do menu no rodape para toda vez que o jogador apertar TAB
void status_cabecalho(PERSONAGEM *heroi, RELOGIO relogio)
{
    //Começaremos a printar acima do desenho do mapa
    int i=1;
    int j=45;      //A primeira coluna abaixo do print do mapa

    //Print das opções do menu rodape
    gotoxy(j,i);
    cprintf("Vidas: %d", heroi->vidas);

    gotoxy(j+16,i);
    printf("Moedas: %d",heroi->moedas);

    gotoxy(j+30,i);
    printf("Score: %d", heroi->pontos);

    gotoxy(j,i+3);
    printf("Chaves: %d", heroi->chaves);

    gotoxy(j+16,i+3);
    printf("%d : %d", relogio.minutos, relogio.segundos);
}

//Funcao feita com o unico proposito de tratar as opcoes do menu
int menu_opcao()
{
    int t, codigo_acao;
    codigo_acao = ACAO_INVALIDA; //Inicializando com valor invalido
    char tecla;
    //Enquanto nao pega uma tecla
    while(!kbhit);

    if(t!=0){
        tecla=getch();
        tecla = toupper(tecla);
        switch(tecla){
            //Trata a movimentação
            case 'N':
                codigo_acao = NOVO_JOGO;
                break;
            case 'C':
                codigo_acao = CARREGAR_JOGO;
                break;
            case 'S':
                codigo_acao = SALVAR_JOGO;
                break;
            case 'V':
                codigo_acao = VOLTAR_JOGO;
                break;
            case 'Q':
                codigo_acao = SAIR_JOGO;
                break;
            default:
                codigo_acao = ACAO_INVALIDA;
        }
    }
    return codigo_acao;
}

//Essa funcao realiza a abertura do menu no rodape para toda vez que o jogador apertar TAB
int menu_rodape()
{
    int opcao;
    //Começaremos a printar a umas duas linhas abaixo do desenho do mapa
    int i=N_LINHA+14;
    int j=10;      //A primeira coluna abaixo do print do mapa

    //Print das opções do menu rodape
    gotoxy(j,i);
    printf("(N)Novo Jogo");

    gotoxy(j+16,i);
    printf("(C)Carrega Jogo");

    gotoxy(j+33,i);
    printf("(S)Salva Jogo");

    gotoxy(j,i+3);
    printf("(V)Voltar");

    gotoxy(j+16,i+3);
    printf("(Q)Sair");

    //Aqui iremos pegar a opcao digitada pelo usuario
    opcao = menu_opcao();

    //Aqui iremos retornar a opcao escolhida pelo usuario
    return opcao;
}

//AQUI FICAM AS FUNCOES RELACIONADAS A LOAD/SAVE do mapa

int salva_jogo(char mapa[][N_COLUNA], int nivel, PERSONAGEM *heroi)
{
    FILE *arq;
    int flag=1;
    int i,j;
    if(!(arq = fopen("jogo_salvo.txt","a+"))) { // abre para escrita
        flag=0;
    }
    else
    {
        //Primeiro vamos salvar o nivel no qual o jogador esta bem como os dados do heroi
        fprintf(arq, "%d,",nivel);
        //Agora vamos gravar os dados do jogador
        fprintf(arq, "%d,", heroi->vidas);
        fprintf(arq, "%d,", heroi->moedas);
        fprintf(arq, "%d,", heroi->invencivel);
        fprintf(arq, "%d,", heroi->segundos_invencivel);
        fprintf(arq, "%d,", heroi->chaves);
        fprintf(arq, "%d\n", heroi->pontos);


        for(i=0;i<N_LINHA;i++)
        {
            for(j=0; j<N_COLUNA;j++)
            {
                fputc(mapa[i][j],arq);
            }
            fputc("\r\n",arq);
        }
    }
    return flag;
}

//Desenha o mapa carregando o arquivo
int carrega_mapa(char nome_mapa[], PERSONAGEM *pos_jogador, OGRO ogros[] , char mapa[][N_COLUNA], int *conta_moedas, POSICAO *pos_ini, int *conta_chaves, int load_mapa){
    int flag=1;
    FILE *arq;
    int pega_info;
    char linha[100];
    char nome_arq[100];
    strcpy(nome_arq,nome_mapa);  //Manipula para nomes de arquivos
    char *token;
    char elemento_mapa;
    int insere_inimigos = 0; //Indice que ira inserir as posicoes dos inimigos
    int i=8;
    int j=45;

    //Se vamos carregar um arquivo salvo, devemos usar a mesma estrutura usada na funcao de salvar
    if(load_mapa == -1)
    {
        fgets(linha,100,arq);
        *token = strtok(linha, ","); //Particionamos a string com os dados do heroi para separar os numeros
        pos_jogador->nivel = atoi(token);
        token = strtok(NULL, ",");  //Pega o proximo dado
        pos_jogador->vidas = atoi(token);
        token = strtok(NULL, ",");  //Pega o proximo dado
        pos_jogador->moedas = atoi(token);
        token = strtok(NULL, ",");  //Pega o proximo dado
        pos_jogador->invencivel = atoi(token);
        token = strtok(NULL, ",");  //Pega o proximo dado
        pos_jogador->segundos_invencivel = atoi(token);
        token = strtok(NULL, ",");  //Pega o proximo dado
        pos_jogador->chaves = atoi(token);
        token = strtok(NULL, ",");  //Pega o proximo dado
        pos_jogador->pontos = atoi(token);
        token = strtok(NULL, ",");  //Pega o proximo dado
        //FIM DO CARREGAMENTO DOS DADOS DO HEROI
        strcpy(nome_arq,"jogo_salvo.txt");
    }



    if(!(arq = fopen(nome_arq,"r"))){ // abre para leitura
        flag=0;
    }
    else{
        elemento_mapa=getc(arq);
        //Para printar o mapa corretamente a gente passa j,i para gotoxy
        while(elemento_mapa != EOF){

            switch(elemento_mapa){

            case PAREDE :
            gotoxy(j,i);
            textbackground(DARKGRAY);
            cprintf("   ");
            //Ajustando os indices para serem armazenados nas posicoes contiguas dos vetores
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case INIMIGO :
            //Lembrar de salvar as coordenadas do ogro, talvez um vetor de pontos?
            ogros[insere_inimigos].pos.x = i-8;
            ogros[insere_inimigos].pos.y = j-45;
            ogros[insere_inimigos].vivo = TRUE;
            insere_inimigos++;      //Atualiza o indice para escrever na proxima posicao
            gotoxy(j,i);
            textbackground(GREEN);
            cprintf("   ");
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case JOGADOR :
            //Lembrar de gravar as coordenadas XY do jogador aqui
            pos_jogador->pos.x = i-8;
            pos_jogador->pos.y = j-45;
            //Guardamos as coordenadas iniciais do jogo para fazer o respawn caso perca uma vida
            pos_ini->x = i-8;
            pos_ini->y = j-45;
            pos_jogador->moedas= 0;     //Garantir que no inicio de cada fase o jogador nao tenha nenhuma moeda
            gotoxy(j,i);
            textbackground(MAGENTA);
            cprintf("   ");
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case MOEDA :
            *conta_moedas++;
            gotoxy(j,i);
            textbackground(YELLOW);
            cprintf("   ");
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case ALAVANCA :
            gotoxy(j,i);
            textbackground(BROWN);
            cprintf("   ");
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case CHAVE :
            *conta_chaves++;
            gotoxy(j,i);
            textbackground(WHITE);
            cprintf("   ");
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case PM_ATIVADA :
            gotoxy(j,i);
            textbackground(LIGHTBLUE);
            cprintf("   ");
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case PM_DESATIVADA :
            gotoxy(j,i);
            textbackground(LIGHTRED);
            cprintf("   ");
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case VAZIO :
            gotoxy(j,i);
            textbackground(LIGHTGRAY);
            cprintf("   ");
            mapa[i-8][j-45] = elemento_mapa;
            break;

            case '\n':
               j=45;
               i++;
            break; //ignora \n
            }

            if(elemento_mapa!= '\n'){
                j++;
            }
        elemento_mapa=getc(arq);
        }
        fclose(arq);
    }
    return flag;
}

int desenha_mapa_matriz(char mapa[][N_COLUNA])
{
    int i, j;
    int conta_moeda=0;
    char pega_elemento;
    for(i=0;i < N_LINHA; i++)
    {
        for(j=0; j < N_COLUNA; j++)
        {
            pega_elemento = mapa[i][j];

            switch(pega_elemento)
            {
                case PAREDE :
                gotoxy(j+45,i+8);
                textbackground(DARKGRAY);
                cprintf("   ");
                break;

                case INIMIGO:
                gotoxy(j+45,i+8);
                textbackground(GREEN);
                cprintf("   ");
                break;

                case JOGADOR:
                gotoxy(j+45,i+8);
                textbackground(MAGENTA);
                cprintf("   ");
                break;

                case MOEDA:
                conta_moeda++;
                gotoxy(j+45,i+8);
                textbackground(YELLOW);
                cprintf("   ");
                break;

                case ALAVANCA :
                gotoxy(j+45,i+8);
                textbackground(BROWN);
                cprintf("   ");
                break;

                case CHAVE :
                gotoxy(j+45,i+8);
                textbackground(WHITE);
                cprintf("   ");
                break;

                case PM_ATIVADA :
                gotoxy(j+45,i+8);
                textbackground(LIGHTBLUE);
                cprintf("   ");
                break;

                case PM_DESATIVADA :
                gotoxy(j+45,i+8);
                textbackground(LIGHTRED);
                cprintf("   ");
                break;

                case VAZIO :
                gotoxy(j+45,i+8);
                textbackground(LIGHTGRAY);
                cprintf("   ");
                break;
            }
        }
    }
    return conta_moeda;
}

//Esta funcao carrega o nivel que iremos jogar no momento bem como controla a jogab
int joga_nivel(PERSONAGEM *heroi, int nivel)
{
    int flag_ogro=1;
    int i,j;
    int morreu=0;
    int teclado_input;
    int chama_funcao;
    int grava_jogo;
    int opcao=0;    //Retorno para saber como acabou a fase - se o jogador morreu, saiu ou ganhou a fase
    //Nome e matriz mapa jogo
    char nome_mapa[] = "mapa0x.txt";
    char mapa_jogo[N_LINHA][N_COLUNA];
    //Estruturas e variaveis auxiliares
    OGRO lista_ogros[INIMIGOS];
    POSICAO inicio_jogador;
    RELOGIO relogio;
    int contar_rel=TRUE;
    int total_moedas=0;
    int total_chaves=0;
    clock_t inicio_rel, fim_rel; //Variaveis de relogio para fazer a passada de tempo

    //Por garantia vamos executar a limpeza de tela aqui tbm pra evitar transtornos
    system("cls");

    //Inicializar o relogio
    relogio.minutos=2;
    relogio.segundos=0; //Inicializa o relógio

    //Isso faz com que o mapa correto seja selecionado para carregar no jogo
    nome_mapa[5]=nivel+48;

    //Se o jogador decidiu carregar um nivel, tentamos executar essa acao
    if(nivel == -1)
    {
        //Aqui iremos tentar fazer o load de um jogo previamente salvp
        chama_funcao = carrega_mapa(nome_mapa,heroi,lista_ogros,mapa_jogo,&total_moedas,&inicio_jogador,&total_chaves,nivel);

        if(chama_funcao == 0)
            return ACAO_INVALIDA; //Informamos que nao é possivel carregar um arquivo pois provavelmente ele nao existe
    }
    else
    {
        //Apos isso devemos carregar o mapa para a partida
        chama_funcao = carrega_mapa(nome_mapa,heroi,lista_ogros,mapa_jogo,&total_moedas,&inicio_jogador,&total_chaves,nivel);
    }


    //Caso tenha dado erro na funcao, retornamos um codigo de erro
    if(chama_funcao == 0)
        return -1;

    //Apos desenharmos o mapa, iremos preparar o teclado
    hidecursor();

    //Looping de execucao do jogo
    do
    {
        //MODO DEBBUGER DO JOGO
        total_moedas = desenha_mapa_matriz(mapa_jogo);

        //Desenhamos o cabeçalho inicial com os dados do jogador
        status_cabecalho(heroi,relogio);


        if(contar_rel) //Pega o clock inicial da execução do código
        {
                inicio_rel=clock();
                contar_rel=FALSE;
        }
        //Desenhamos o mapa para ir atualizando o desenho
        //desenha_mapa_matriz(mapa_jogo);

        if(kbhit())
        {
            teclado_input = getch();
        }

        //Pegamos a acao momentanea do jogador
        teclado_input = chama_teclado(teclado_input);
        //Se o usuario clicou TAB, iremos chamar uma funcao de menu
        if(teclado_input == TAB)
        {
            //Fica pedindo input enquanto nao for dado uma opcao valida
            do
            {
                teclado_input = menu_rodape();
            }while(teclado_input == ACAO_INVALIDA);
            //Apos isso decidimos o que faremos com a acao
            switch(teclado_input)
            {
                //Retornamos o codigo da op  para sabermos do que se trata no controle de execucao das fases
                case NOVO_JOGO:
                case CARREGAR_JOGO:
                    return teclado_input;
                case SALVAR_JOGO:
                    grava_jogo = salva_jogo(mapa_jogo,nivel,heroi);
                    if(grava_jogo == 1)
                    {
                        printf("\nJogo Salvo");
                    }
                    break;
                case VOLTAR_JOGO:
                    break;
                case SAIR_JOGO:
                    return teclado_input;
            }
        }

        //Realizamos a acao determinada pela tecla pressionada
        chama_funcao = executa_acao(heroi,lista_ogros,mapa_jogo,teclado_input,&total_moedas,inicio_jogador);
        //Verificamos se o heroi nao perdeu todas as vidas
        if(chama_funcao == 1)
        {
            morreu = 1;     //Significa que devemos para o laço de execução do jogo
        }
        fim_rel=clock(); //Pega um clock "final" para que assim possamos pegar a variação de tempo em função do clock

        //Forma de evitar o bug do 1s
        if(heroi->moedas != heroi->moedas_old && heroi->moedas % 10 == 0 && heroi->invencivel < 2)
        {
                gotoxy(75,4);
                printf("Invencivel");
                heroi->invencivel = 1; //Codigo 1 - invencibilidade temporaria
                heroi->segundos_invencivel+=5;  //Ganha cinco segundos de invencibilidade
                heroi->moedas_old = heroi->moedas; //Salvamos a quantidade de moedas no momento em que o heroi ficar invencivel
        }


        if((fim_rel-inicio_rel)/CLOCKS_PER_SEC>=1) //Calculo da variação de clocks por segundo
        {
            if(total_moedas == 0)
            {
                //Codigo 2 - Invencivel pra sempre
                gotoxy(75,4);
                printf("Invencivel");
                heroi->invencivel = 2;
            }
            //Teste pra garantir que o personagem nao esta invencivel ainda
            if(heroi->moedas != heroi->moedas_old && heroi->moedas % 10 == 0 && heroi->invencivel < 2)
            {
                gotoxy(75,4);
                printf("Invencivel");
                heroi->invencivel = 1; //Codigo 1 - invencibilidade temporaria
                heroi->segundos_invencivel+=5;  //Ganha cinco segundos de invencibilidade
                heroi->moedas_old = heroi->moedas; //Salvamos a quantidade de moedas no momento em que o heroi ficar invencivel
            }
            //Teste para decrementar os segundos de invencibilidade
            if(heroi->segundos_invencivel > 0)
            {
                heroi->segundos_invencivel--;   //Decrementa um segundo de invencibilidade
                if(heroi->segundos_invencivel == 0 && heroi->invencivel == 1)
                {
                    gotoxy(75,4);
                    printf("           ");
                    heroi->invencivel = 0; //Desabilita a invencibilidade do heroi
                    heroi->moedas_old = heroi->moedas;
                }
            }
            //FIM TESTES RELACIONADOS A INVENCIBILIDADE DO HEROI

            //Se nao morreu pra nenhum inimigo, temos que ver se o tempo acabou
            if(!morreu) //Verifica se o usuário não morreu em função do valor retornado pelo relógio
            {
                morreu=tempo(&relogio); //chama a função tempo para ir decrementando um segundo do relógio
                if(!morreu)
                {
                    //Fazemos os ogros se mover
                    if(flag_ogro == 1)
                    {
                        morreu = moveOgros(lista_ogros, mapa_jogo, heroi, inicio_jogador);
                        if(existem_ogros(lista_ogros) == FALSE)
                        {
                            heroi->vidas++;
                            flag_ogro=0;
                        }
                    }
                    if(!morreu) //Ativa um sleep para impedir que o movimento dos monstros seja rápido demais
                    {
                        Sleep(1);
                    }
                }
            }
            else
            {
                //Perde vidas em virtude do relogio ter fechado o tempo
                heroi->vidas--;
                //Ajusta o relogio para a proxima partida
                relogio.minutos=2;
                relogio.segundos=0;

            }
            if(heroi->chaves == 5)
            {
                return GANHOU;
            }

            contar_rel = TRUE;
        }

    }while(heroi->vidas > 0 || heroi->chaves < total_chaves || teclado_input != SAIR_JOGO); //Enquanto o jogador nao morrer, nao ganhar ou nao sair do jogo

    //Se heroi morreu todas as vezes vamos retornar um numerp equivalente
    if(heroi->vidas == 0)
    {
        opcao = PERDEU;
    }
    else if(heroi->chaves == 5)
    {
        opcao = GANHOU;
    }
    else
    {
        opcao = OPCAO_SAIR;
    }

    return opcao;
}

//AQUI CHEGAMOS NO FIM DA FUNCOES DE CARREGAR MAPA

//Funcao para esconder o cursor do usuario durante o jogo
void hidecursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}


//A funcao trata o apertar de uma tecla e retorna um codigo referente a acao que o personagem vai executar
int chama_teclado(char tecla)
{
    int codigo_acao;
    codigo_acao = ACAO_INVALIDA; //Inicializando com valor invalido
    tecla = toupper(tecla);
    switch(tecla)
    {
    //Trata a movimentação
    case 'W':
    case SETA_CIMA:
        codigo_acao = CIMA;
        break;
    case 'A':
    case SETA_ESQ:
        codigo_acao = ESQUERDA;
        break;
    case 'S':
    case SETA_BAIXO:
        codigo_acao = BAIXO;
        break;
    case 'D':
    case SETA_DIR:
        codigo_acao = DIREITA;
        break;
    case 'B':
        codigo_acao = APERTA_ALAVANCA;
        break;
    case TAB:
        codigo_acao = TAB;
        break;
    default:
        codigo_acao = ACAO_INVALIDA;
    }
    return codigo_acao;
}

int executa_acao(PERSONAGEM *heroi, OGRO ogros[], char mapa[][N_COLUNA], int acao, int *total_moedas, POSICAO inicio_jogador)
{
    //Variavel indicando o status do jogo
    int status_personagem = 0;
    //Variaveis para fazer a predição do proximo movimento
    int prox_pos_x = heroi->pos.x;
    int prox_pos_y = heroi->pos.y;
    //Variavel que iremos usar para varrer o vetor de ogros
    int i,j,busca_ogro;
    //Salvo aqui tbm para manter os valores antigos
    int jogador_x = heroi->pos.x;
    int jogador_y = heroi->pos.y;
    char elemento_matriz;
    //Se funcao for uma acao do heroi
    if(acao>=CIMA && acao<=APERTA_ALAVANCA)
    {
        switch(acao)
        {
            case CIMA:
                prox_pos_x--;
                break;
            case BAIXO:
                prox_pos_x++;
                break;
            case ESQUERDA:
                prox_pos_y--;
                break;
            case DIREITA:
                prox_pos_y++;
                break;
            case APERTA_ALAVANCA:
                //Vamos checar primeiro se a alavanca esta no entorno de nosso personagem
                if( (mapa[jogador_x][jogador_y+1] == ALAVANCA) || (mapa[jogador_x][jogador_y-1] == ALAVANCA) || (mapa[jogador_x+1][jogador_y] == ALAVANCA) || (mapa[jogador_x-1][jogador_y] == ALAVANCA))
                {
                    //Aqui devemos fazer o chaveamento das portas magicas
                    for(i=0; i < N_LINHA; i++)
                    {
                        for(j=0; j < N_COLUNA; j++)
                        {
                            elemento_matriz=mapa[i][j];
                            switch(elemento_matriz)
                            {
                                case PM_ATIVADA:
                                    mapa[i][j] = PM_DESATIVADA;
                                    break;
                                case PM_DESATIVADA:
                                    mapa[i][j] = PM_ATIVADA;
                                    break;
                                default:
                                    break;
                            }
                        }
                    }

                }
            default:
                break;
        }

        //Aqui vamos reescrever os testes
        //Aqui iremos verificar a movimentacao
        if(prox_pos_y != jogador_y)
        {
            elemento_matriz = mapa[jogador_x][prox_pos_y];
            switch(elemento_matriz)
            {
                case VAZIO:
                mapa[jogador_x][jogador_y] = VAZIO;
                //Setando a nova posicao do jogador
                    heroi->pos.y = prox_pos_y;
                    mapa[jogador_x][prox_pos_y] = JOGADOR;
                    break;
                //SE FOR UM ELEMENTO BLOQUEADO FAZ NADA
                case PAREDE:
                case PM_DESATIVADA:
                    break;
                case PM_ATIVADA:
                    if(prox_pos_y > jogador_y) //Se positivo, estamos andando para direita, entao devemos aumentar o Y
                    {
                        prox_pos_y++;
                    }
                    else
                    {
                        prox_pos_y--;
                    }
                    //Atualizo minhas novas coordenadas
                    mapa[jogador_x][jogador_y] = VAZIO;
                    heroi->pos.y = prox_pos_y;
                    mapa[jogador_x][prox_pos_y] = JOGADOR;
                    break;
                case MOEDA:
                    //Vamos colocar branco na antiga posicao
                    mapa[jogador_x][jogador_y] = VAZIO;
                    //Agora temos que atualizar a posicao do heroi, contar as moedas e aumentar os pontos
                    heroi->moedas++;
                    *total_moedas--;
                    heroi->pontos+=10;
                    //Setando a nova posicao do jogador
                    heroi->pos.y = prox_pos_y;
                    mapa[jogador_x][prox_pos_y] = JOGADOR;
                    break;
                case CHAVE:
                    //Vamos colocar branco na antiga posicao
                    mapa[jogador_x][jogador_y] = VAZIO;
                    //Agora temos que atualizar a qtd de chaves
                    heroi->chaves++;
                    //Setando a nova posicao do jogador
                    mapa[jogador_x][prox_pos_y] = JOGADOR;
                    heroi->pos.y = prox_pos_y;
                    break;
                case INIMIGO:
                    if(heroi->invencivel == TRUE)
                    {
                        //Aqui vamos ter que remover o ogro morto, atualizar os pontos e mover por cima
                        for(i=0; i < INIMIGOS; i++)
                        {
                            //Verifica se as coordenadas do ogro sao as procuradas
                            if((ogros[i].pos.x == jogador_x) && (ogros[i].pos.y == prox_pos_y))
                                busca_ogro = i;
                        }
                        //Desativando o ogro
                        ogros[busca_ogro].vivo = FALSE;
                        heroi->pontos+=100;
                        //Colocando branco na posicao anterior do jogador
                        mapa[jogador_x][jogador_y] = VAZIO;
                        //Setando a nova posicao do jogador
                        mapa[jogador_x][prox_pos_y] = JOGADOR;
                        heroi->pos.y = prox_pos_y;
                    }
                    else
                    {
                        //O heroi tem que perder uma vida e voltar pro inicio da fase
                        heroi->vidas--;
                        mapa[jogador_x][jogador_y] = VAZIO;
                        heroi->pos.x = inicio_jogador.x;
                        heroi->pos.y = inicio_jogador.y;
                        mapa[inicio_jogador.x][inicio_jogador.y] = JOGADOR;
                        if(heroi->vidas == 0)
                        {
                            status_personagem = 1; //Personagem perdeu todas as vidas
                        }
                    }
                    break;

                default:
                    break;

            }
        }

        //Aqui devemos testar o eixo x

        //Aqui verificamos se o movimento foi 'de lado'
        if(prox_pos_x != jogador_x)
        {
            elemento_matriz = mapa[prox_pos_x][jogador_y];
            switch(elemento_matriz)
            {
                case VAZIO:
                mapa[jogador_x][jogador_y] = VAZIO;
                //Setando a nova posicao do jogador
                    heroi->pos.x = prox_pos_x;
                    mapa[prox_pos_x][jogador_y] = JOGADOR;
                    break;
                //SE FOR UM ELEMENTO BLOQUEADO FAZ NADA
                case PAREDE:
                case PM_DESATIVADA:
                    break;
                case PM_ATIVADA:
                    if(prox_pos_x > jogador_x) //Se positivo, estamos andando para baixo, entao devemos aumentar o X
                    {
                        prox_pos_x++;
                    }
                    else
                    {
                        prox_pos_x--;
                    }
                    //Atualizo minhas novas coordenadas
                    mapa[jogador_x][jogador_y] = VAZIO;
                    heroi->pos.x = prox_pos_x;
                    mapa[prox_pos_x][jogador_y] = JOGADOR;
                    break;
                case MOEDA:
                    //Vamos colocar branco na antiga posicao
                    mapa[jogador_x][jogador_y] = VAZIO;
                    //Agora temos que atualizar a posicao do heroi, contar as moedas e aumentar os pontos
                    heroi->moedas++;
                    *total_moedas--;
                    heroi->pontos+=10;
                    //Setando a nova posicao do jogador
                    heroi->pos.x = prox_pos_x;
                    mapa[prox_pos_x][jogador_y] = JOGADOR;
                    break;
                case CHAVE:
                    //Vamos colocar branco na antiga posicao
                    mapa[jogador_x][jogador_y] = VAZIO;
                    //Agora temos que atualizar a qtd de chaves
                    heroi->chaves++;
                    //Setando a nova posicao do jogador
                    mapa[prox_pos_x][jogador_y] = JOGADOR;
                    heroi->pos.x = prox_pos_x;
                    break;
                case INIMIGO:
                    if(heroi->invencivel == TRUE)
                    {
                        //Aqui vamos ter que remover o ogro morto, atualizar os pontos e mover por cima
                        for(i=0; i < INIMIGOS; i++)
                        {
                            //Verifica se as coordenadas do ogro sao as procuradas
                            if((ogros[i].pos.x == jogador_x) && (ogros[i].pos.y == prox_pos_y))
                                busca_ogro = i;
                        }
                        //Desativando o ogro
                        ogros[busca_ogro].vivo = FALSE;
                        heroi->pontos+=100;
                        //Colocando branco na posicao anterior do jogador
                        mapa[jogador_x][jogador_y] = VAZIO;
                        //Setando a nova posicao do jogador
                        mapa[prox_pos_x][jogador_y] = JOGADOR;
                        heroi->pos.x = prox_pos_x;
                    }
                    else
                    {
                        //O heroi tem que perder uma vida e voltar pro inicio da fase
                        heroi->vidas--;
                        mapa[jogador_x][jogador_y] = VAZIO;
                        heroi->pos.x = inicio_jogador.x;
                        heroi->pos.y = inicio_jogador.y;
                        mapa[inicio_jogador.x][inicio_jogador.y] = JOGADOR;
                        if(heroi->vidas == 0)
                        {
                            status_personagem = 1; //Personagem perdeu todas as vidas
                        }
                    }
                    break;

                default:
                    break;

            }
        }
    }
    return status_personagem;
}



//Funcao para movimentar cada inimigo do jogo. Iremos retornar um flag caso algum ogro mate o personagem durante a movimentacao
int moveOgros(OGRO m_ogro[], char mapa[][N_COLUNA], PERSONAGEM *heroi, POSICAO inicio_jogador)
{
    int i=0;
    int min = -1;
    int max = 1;
    int new_pos_x=0, new_pos_y=0;
    char elemento_mapa;
    int morreu=0;


    for(i=0; i<INIMIGOS; i++)
    {
        //Verificar se o ogro esta vivo e deve ser movimentado
        if(m_ogro[i].vivo != FALSE)
        {
            m_ogro[i].dir.x = min + rand() % ( max - min + 1 );
            m_ogro[i].dir.y = min + rand() % ( max - min + 1 );
            //Uso das direcoes para atualização das coordenadas de cada inimigo
            new_pos_x = m_ogro[i].pos.x + m_ogro[i].dir.x;
            new_pos_y = m_ogro[i].pos.y + m_ogro[i].dir.y;
            //Temos que testar primeiro por quais itens a nova coordenada esta passando para garantir a integridade do mapa
            elemento_mapa = mapa[new_pos_x][new_pos_y];

            switch(elemento_mapa)
            {
            case MOEDA:
                mapa[m_ogro[i].pos.x][m_ogro[i].pos.y]=MOEDA;
                m_ogro[i].pos.x = new_pos_x;
                m_ogro[i].pos.y = new_pos_y;
                mapa[new_pos_x][new_pos_y]=INIMIGO;
            case VAZIO:
                mapa[m_ogro[i].pos.x][m_ogro[i].pos.y]=VAZIO;
                m_ogro[i].pos.x = new_pos_x;
                m_ogro[i].pos.y = new_pos_y;
                mapa[new_pos_x][new_pos_y]=INIMIGO;
                break;
            case JOGADOR:
                //Heroi perde uma vida e é colocado no ponto inicial de spawn
                heroi->vidas--;
                morreu == 1;
                mapa[inicio_jogador.x][inicio_jogador.y] == JOGADOR;
                //Ogro é posicionado na nova coordenada
                m_ogro[i].pos.x = new_pos_x;
                m_ogro[i].pos.y = new_pos_y;
                mapa[new_pos_x][new_pos_y]=INIMIGO;

            //Se for qualquer outro elemento do mapa o jogo deve preservar estes
            default:
                break;
            }

        }

    }
    return morreu;
}

//Esta funcao ira fazer todo o controle do jogo - menu inicial, jogar fase, tratamento das opcoes, etc;
void controlador_jogo()
{
    //Inicializamos nossos parametros principais
    PERSONAGEM heroi;
    int opcao, nivel=0;     //P.S se nivel tiver em -1 o jogo deve ser carregado de um save previo
    int gravar_jogo;
    inicializa_heroi(&heroi,nivel);
    //Printamos o titulo
    letreiro();
    //Esperamos o usuario apertar uma tecla pra carregar o menu iniciar
    system("pause");
    //Limpamos a tela para dar uma sensação de transição
    system("cls");
    //Aqui iremos carregar o menu principal
    opcao = menu_rodape();
    //Primeiro laco para tratar situacoes do menu inicial
    do
    {
        switch(opcao)
        {
            case NOVO_JOGO:
                nivel = 1;
                inicializa_heroi(&heroi,nivel);
                opcao = joga_nivel(&heroi,nivel);
                break;
            case CARREGAR_JOGO:
                nivel=-1;   //Flag pra indicar que quero carregar uma partida
                opcao=joga_nivel(&heroi,nivel);
                break;
            //Essa opcao nao estara habilitada nesse menu especificamente pois nao quero gerar um bug
            case SALVAR_JOGO:
            case VOLTAR_JOGO:
                break;
            case SAIR_JOGO:
                break;

            default:
                break;
        }
    }while(opcao == ACAO_INVALIDA || opcao == SALVAR_JOGO || opcao == VOLTAR_JOGO);
    //Aqui vai ser o que iremos fazer apos a execucao de uma partida, seja ela carregada ou nova
    while(opcao != SAIR_JOGO || opcao != OPCAO_SAIR)
    {

        //Limpamos a execucao do fim da partida pra evitar problemas de poluicao visual na tela
        system("cls");

        //Depois que sairmos do loop de execucao/selecao vamos decidir qual acao deve ser tomada
        if(opcao == SAIR_JOGO || opcao == OPCAO_SAIR)
        {
            printf("BYE BYE");
        }
        else     //Aqui iremos descobrir o que aconteceu no jogo
        {
            switch(opcao)
            {
                //Se morrermos é game over e encerramos o jogo
                case PERDEU:
                    opcao = SAIR_JOGO;
                    printf("VOCE PERDEU");
                    break;
                case GANHOU:    //Devemos ir para o proximo nivel e zerar moedas e afins que vieram com o nivel anterior
                    nivel++;
                    inicializa_heroi(&heroi,nivel);
                    printf("VAMOS PARA O PROXIMO NIVEL");
                    opcao = joga_nivel(&heroi,nivel);
                    break;
                case CARREGAR_JOGO:
                    nivel=-1;   //Flag pra indicar que quero carregar uma partida
                    opcao=joga_nivel(&heroi,nivel);
                    break;
                case SALVAR_JOGO:
                    break;


                default:
                    printf("Houston we have a problem");
                    opcao = SAIR_JOGO; //Acabamos com o jogo para evitar algum problema maior
                    break;

            }
        }
    }
}

int existem_ogros(OGRO lista_ogros[])
{
    int i=0;
    int existe=FALSE;
    for(i=0;i<INIMIGOS;i++)
    {
        if(lista_ogros[i].vivo == TRUE)
        {
            existe=TRUE;
        }
    }
    return existe;
}
