# calabouco-dos-ogros
Trabalho Final de Algoritmos e Programação 2020/1

Informações sobre o Código

Estruturas
A estrutura utilizada para a movimentação do Jogador foi a POSICAO chamada em na estrutura PERSONAGEM, onde lá ficam as características que o envolvem. POSICAO também foi utilizada para a movimentação do Ogro, com a diferenciação é de que o Ogro é composto também pela estrutura DIRECAO, já que sua movimentação é randômica. 
A estrutura RELOGIO foi definida para que fosse implementa a contagem regressiva dos 2 minutos.



Funções:
void letreiro ();

Função destinada a fazer uma abertura “bonitinha” com o nome do jogo.


int carrega_mapa (char nome_mapa[], PERSONAGEM *pos_jogador, OGRO pos_ogros[], char mapa[][N_COLUNA], int *conta_moedas, POSICAO *pos_ini, int *conta_chaves);

A função tem como características abrir e ler o arquivo do nível em que o jogador se encontra, lendo um caractere por vez, o escrevendo na tela de maneira visual utilizando as propriedades da biblioteca coinio2.h e salvando em uma matriz para que seja realizada sua alteração sem que haja mudanças no mapa base. 


int tempo (RELOGIO* relogio);

A função decrementa o tempo máximo para que seja jogado o nível, após a passagem e não finalizado a coleta das chaves o jogador perde uma vida e reinicia no ponto de partida.


int chama_teclado (char tecla);

A função é destinada para a comunicação do teclado em relação à movimentação e interação do personagem no jogo e também seleção do menu.


void hidecursor (void);

Função definida na biblioteca windows.h para que o cursor seja escondido, deixando o jogo com um melhor aspecto.


int moveOgros (OGRO m_ogro[], char mapa[][N_COLUNA], PERSONAGEM *heroi, POSICAO inicio_jogador);

Função utilizada para realizar o movimento dos ogros, adaptada do laboratório da semana 13.


int desenha_mapa_matriz (char mapa[][N_COLUNA]);

Função destinada para que a cada movimento o mapa carregado pela função carrega_mapa seja atualizado com novas posições retornando quantidade de moedas toais do mapa.


int executa_acao (PERSONAGEM *heroi, OGRO ogros[], char mapa[][N_COLUNA], int acao, int *total_moedas, POSICAO inicio_jogador);

Função responsável por tratar cada ação também todas as colisões no jogo.


void inicializa_heroi(PERSONAGEM *heroi, int nivel);

Função destinada a inicializar as estatísticas do jogador.


void controlador_jogo (void);

Função que gerencia a execução completa do jogo, é a única função chamada na main.


int joga_nivel (PERSONAGEM *heroi, int nivel);

Função responsável por executar uma fase completa. 


int menu_rodape (void);

Exibe as opções de menu no rodapé da página.


int menu_opcao (void);

Seleciona a opção digitada pelo usuário antes do início do jogo e após digitar tab.


int salva_jogo (char mapa[][N_COLUNA], int nivel, PERSONAGEM *heroi);

Essa função salva o jogo conforme ele foi alterado, para que quando o usuário deseje carregar um jogo salvo ele inicie de onde parou.


int status_cabecalho(RELOGIO relogio);

Função para a exibição das informações de vida, moedas, pontos e tempo.


int existem_ogros (OGRO lista_ogros[]);

Essa função testa se todos os ogros foram mortos, para que seja incrementada uma vida a mais como bônus ao jogador.
