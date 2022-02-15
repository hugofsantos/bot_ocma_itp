#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 50

// ---------------------------------------- Definição de registros -----------------------------------
typedef struct
{
  char id[11]; // Cada identificador só pode ter no máximo 10 caracteres
  int x;
  int y;
  int currentLoad; //Carga atual
}Player;

typedef struct
{
  int data;
  int hasBoat;
}Cell;

// Função de Módulo
int module(int x){
  return (x>=0)?x:-1*x;
}
// -----------------------------------------------------------------------------------------------------

// Leitura dos dados do turno
void readData(int h, int w, Cell** map, Player* player) { 
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

// ------------------------------------- Funções relacionadas ao porto ---------------------------------

// Calcular distância
int getDistance(int x, int y, Player* Player){
  return (module(Player->x - x) + module(Player->y - y));
}

// Verificar porto mais próximo
void checkNearestHarbor(int h, int w, Cell** map, Player* player, int* xHarbor, int* yHarbor){
  int distance = h+w;
  int currentDistance = distance;
  int *xHarbor = -1;
  int *yHarbor = -1;

  for(int y=0; y<h;y++){
    for(int x=0;x<w;x++){
      if(map[y][x].data==1){// Se tem um porto naquela posição
        currentDistance = getDistance(x, y, player);
        if(currentDistance < distance){
          distance = currentDistance;
          *xHarbor = x;
          *yHarbor = y;
        }
      }
    }
  }

}

// Se locomover para alguma direção (L: left, R: Right, U: Up, D: Down)
void move(char direction){

  switch (direction){
    case 'U':case 'u': printf("UP\n");break; // Se move pra cima
    case 'D': case 'd': printf('DOWN\n');break; // Se move pra baixo
    case 'L': case 'l': printf('LEFT\n');break; // Se move para a esquerda
    case 'R': case 'r': printf('RIGHT\n');break; // Se move para a direita
    default: printf("PASS\n"); // Passar a vez

  }

}

// Se mover para o porto mais próximo
void moveToHarbor(int h, int w, Cell** map, int xHarbor, int yHarbor, Player* player){
  char response[MAX_STR] = {0}; // Linha para receber a resposta da ação executada

  char horizontalMove = (player->x < xHarbor)?'R':'L'; // R (Right) e L (Left)
  char verticalMove = (player->y < yHarbor)?'D':'U'; // U (Up) e D (Down)
  
  int moveIncrement = (player->x < xHarbor)?1:-1; // Variável para verificar se tem bots na célula em que se planeja ir

  int isFirstMove = 1;

  int horizontalMoves = module(player->x - xHarbor); // Quantidade de movimentos horizontais restantes para chegar ao porto
  int verticalMoves = module(player->y - yHarbor); // Quantidade de movimentos verticais restantes para chegar ao porto

  // Vai realizando as movimentações horizontais para chegar ao portos
  while(horizontalMoves>0){
    if(!isFirstMove){
      readData(h, w, map, player);
    }

    if(map[player->y][player->x+moveIncrement].hasBoat && verticalMoves>0){ // Se a área está ocupada e ainda restam movimentos verticais
      move(verticalMove);
      scanf("%s", response); // Obtém resposta

      if(strcmp(response,"DONE")==0){ // Se a resposta foi DONE (Sucesso)
        verticalMoves-=1;

        isFirstMove = 0;
        continue;
      }
    }

    move(horizontalMove);
    scanf("%s", response); // Obtém resposta

    if(strcmp(response, "DONE")==0){
      horizontalMoves-=1;
    }

    isFirstMove = 0;
  }

  // Vai realizando as movimentações verticais restantes
  while(verticalMoves>0){
    if(!isFirstMove){
      readData(h, w, map, player);
    }

    move(verticalMove);
    scanf("%s", response); // Obtém resposta

    if(strcmp(response, "DONE")==0){
      verticalMoves-=1;
    }    

    isFirstMove = 0;
  }

}
// --------------------------------------- Funções para lógica de locomoção geral do barco -------------------------------

// Ver lucratividade de algum movimento em específico
int getMoveProfitability(char direction, int h, int w, Cell** map, Player* player){
  int xIntent = player->x;
  int yIntent =player->y;

  switch(direction){
    case 'U':case 'u': // Up
      yIntent-=1;
      break;
    case 'D':case 'd': // Down
      yIntent+=1;
      break;
    case 'L':case 'l': // Left
      xIntent-=1;
      break;
    case 'R':case 'r': // Right
      xIntent+=1;
  }

  return getCellProfitability(xIntent, yIntent, h, w, map);
}
// Ver lucratividade de uma célula em específico
int getCellProfitability(int xIntent, int yIntent, int h, int w, Cell** map){
  Cell cell = {0};

  int numberOfFish = 0;
  int kindOfFish = 0;

  int fishValues[3] = {100, 150, 200}; // Preço da Tainha, Cioba e Robalo respectivamente (No contexto do jogo)
  int isOutMap = (xIntent < 0 || xIntent > w) || (yIntent < 0 || yIntent > h); // Se a cordenada está fora dos limites do mapa

  if(isOutMap){
    return -500; // Valor da multa por exceder o mapa
  }

  cell = map[yIntent][xIntent];

  if(cell.hasBoat){ // Se tem um outro bot no local
    return -1;
  }
  if(cell.data==1 && cell.data==0){// Se tem um porto ou não tem nada
    return 0;
  }

  numberOfFish = cell.data%10;
  kindOfFish = (int) (cell.data/10);

  return fishValues[kindOfFish-1] * (numberOfFish-1); // Não podemos pescar o último peixe de uma determinada área
}

int main() {
  char response[MAX_STR];   // dados temporários
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
    scanf("%s", response);
  }

  return 0;
}