#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 50

// Definição de registros
typedef struct
{
  char id[11]; // Cada identificador só pode ter no máximo 10 caracteres
  int x;
  int y;
}Player;

typedef struct
{
  int data;
  int hasBoat;
}Cell;

// Leitura dos dados do turno
void readData(int h, int w,Cell** map,Player* player) { 
  char id[MAX_STR];
  int numberOfBots;
  int x, y;

  // lê os dados da área de pesca
  for (int i = 0; i < h; i++) {   
    for (int j = 0; j < w; j++) {
      scanf("%i", &map[i][j].data);
    }
  }
  // lê os dados dos bots
  scanf(" BOTS %i", &numberOfBots);
  // o " " antes de BOTS é necessário para ler o '\n' da linha anterior
  for (int i = 0; i < numberOfBots; i++) {
    scanf("%s %i %i", id, &x, &y);
    
    if(strcmp(id,player->id)==0){ // Se o id lido for igual ao id do player
      player->x = x;
      player->y = y;
      continue;
    }
    (map[x][y]).hasBoat = 1; // Indica que a posição xy da matriz tem um bote
  }
}

int main() {
  char line[MAX_STR];   // dados temporários
  char myId[MAX_STR] = {0};   // identificador do bot em questão (Inicializando vazio para eliminar eventuais lixos de memória)
  Player player = {0}; // Inicializa o registro do player principal com os dados vazios


  setbuf(stdin, NULL);   // stdin, stdout e stderr não terão buffers
  setbuf(stdout, NULL);  // assim, nada é "guardado temporariamente"
  setbuf(stderr, NULL);

  // === INÍCIO DA PARTIDA ===
  int h, w; // Height e width
  scanf("AREA %i %i", &h, &w);  // lê a dimensão da área de pesca: altura (h) x largura (w)
  
  // Inicializando o mapa
  Cell** map = (Cell**) calloc(sizeof(Cell*),h);
  for(int i=0;i<w;i++){
    map[i] = (Cell*) calloc(sizeof(Cell), w);
  }

  scanf(" ID %s", myId);        // ...e o id do bot
  // obs: o " " antes de ID é necessário para ler o '\n' da linha anterior

  // Atribui o valor do id lido para o campo id do player
  for(int i=0;i<11 && myId[i]!='\000';i++){
    player.id[i] = myId[i];
  }

  // Para "debugar", é possível enviar dados para a saída de erro padrão (stderr).
  // Esse dado não será enviado para o simulador, apenas para o terminal.
  // A linha seguinte é um exemplo. Pode removê-la se desejar.
  // fprintf(stderr, "Meu id = %s\n", myId);

  // === PARTIDA === 
  // O bot entra num laço infinito, mas não se preocupe porque o simulador irá matar
  // o processo quando o jogo terminar.
  while (1) {

    // LÊ OS DADOS DO JOGO E ATUALIZA OS DADOS DO BOT
    readData(h, w, map, &player);

    // INSIRA UMA LÓGICA PARA ESCOLHER UMA AÇÃO A SER EXECUTADA

    // envia a ação escolhida (nesse exemplo, ir para esquerda)
    printf("LEFT\n");

    // lê qual foi o resultado da ação (e eventualmente atualiza os dados do bot).
    scanf("%s", line);
  }

  return 0;
}