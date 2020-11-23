#define N_LINHA 11
#define N_COLUNA 27
//Aqui vao structs e cabeçalhos das funcoes que iremos criar
//Structs

//Estrutura para contarmos o tempo do jogo
typedef struct
{
    int minutos;
    int segundos;
} RELOGIO; //Estrutura utilizada para representar o relógio no jogo

typedef struct{
    int x;
    int y;
} POSICAO;

typedef struct{
    int x;
    int y;
} DIRECAO;

//Estrutura dos inimigos
typedef struct{
    POSICAO  pos;
    DIRECAO dir;
    int vivo;
} OGRO;

//Estrutura do jogador
typedef struct{
    int vidas;
    int moedas;
    int moedas_old;
    int invencivel;
    int segundos_invencivel;
    int chaves;
    int pontos;
    int nivel;
    POSICAO pos;
} PERSONAGEM;

//Cabecalhos
void letreiro();
void inicializa_heroi(PERSONAGEM *heroi, int nivel);
int tempo(RELOGIO* relogio);
void status_cabecalho(PERSONAGEM *heroi, RELOGIO relogio);
int menu_opcao(void);
int menu_rodape(void);
int salva_jogo(char mapa[][N_COLUNA], int nivel, PERSONAGEM *heroi);
int carrega_mapa(char nome_mapa[], PERSONAGEM *pos_jogador, OGRO pos_ogros[] , char mapa[][N_COLUNA], int *conta_moedas, POSICAO *pos_ini,int *conta_chaves, int load_mapa);
int desenha_mapa_matriz(char mapa[][N_COLUNA]);
int joga_nivel(PERSONAGEM *heroi, int nivel);
void hidecursor(void);
int chama_teclado(char tecla);
int executa_acao(PERSONAGEM *heroi, OGRO ogros[], char mapa[][N_COLUNA], int acao, int *total_moedas, POSICAO inicio_jogador);
int moveOgros(OGRO m_ogro[], char mapa[][N_COLUNA], PERSONAGEM *heroi, POSICAO inicio_jogador);
int existem_ogros(OGRO lista_ogros[]);
void controlador_jogo(void);
