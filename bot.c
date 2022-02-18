#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
// ------------------------------------------------ Funções Gerais ----------------------------------------------

// Leitura dos dados do turno
void readData(int h, int w, Cell** map, Player* player) { 
  char id[MAX_STR];
  int numberOfBots;
  int l, c;

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
    scanf("%s %i %i", id, &l, &c);
    
    if(strcmp(id,player->id)==0){ // Se o id lido for igual ao id do player
      player->x = c;
      player->y = l;
      continue;
    }
    (map[l][c]).hasBoat = 1; // Indica que a posição xy da matriz tem um bote
  }
}

// Se locomover para alguma direção (L: left, R: Right, U: Up, D: Down)
void move(char direction){

  switch (direction){
    case 'U':case 'u': printf("UP\n");break; // Se move pra cima
    case 'D': case 'd': printf("DOWN\n");break; // Se move pra baixo
    case 'L': case 'l': printf("LEFT\n");break; // Se move para a esquerda
    case 'R': case 'r': printf("RIGHT\n");break; // Se move para a direita
    default: printf("PASS\n"); // Passar a vez

  }

}

void toFish(Player* player){
  char response[MAX_STR] = {0}; // Linha para receber a resposta da ação executada

  printf("FISH\n"); // Envia a ação para pescar
  scanf("%s", response);

  // Se a pesca ocorreu com sucesso
  if((strcmp(response,"SEABASS")==0) || (strcmp(response, "SNAPPER")==0) || (strcmp(response, "MULLET")==0)){
    player->currentLoad+=1;
  }
}

void toSell(Player* player){
  char response[MAX_STR] = {0};

  printf("SELL\n"); // Dá o comando para vender o peixe
  scanf("%s", response);

  player->currentLoad = 0;
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
  
  *xHarbor = -1;
  *yHarbor = -1;

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

// Se mover para o porto mais próximo
int moveToHarbor(int h, int w, Cell** map, int xHarbor, int yHarbor, Player* player){
  char response[MAX_STR] = {0}; // Linha para receber a resposta da ação executada

  char horizontalMove = (player->x < xHarbor)?'R':'L'; // R (Right) e L (Left)
  char verticalMove = (player->y < yHarbor)?'D':'U'; // U (Up) e D (Down)
  
  int moveIncrement = (player->x < xHarbor)?1:-1; // Variável para verificar se tem bots na célula em que se planeja ir

  int isFirstMove = 1;

  int horizontalMoves = module(player->x - xHarbor); // Quantidade de movimentos horizontais restantes para chegar ao porto
  int verticalMoves = module(player->y - yHarbor); // Quantidade de movimentos verticais restantes para chegar ao porto

  if(horizontalMoves==0 && verticalMoves==0)
    return 0;

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

  return 1;
}
// --------------------------------------- Funções para verificação da lucratividade das células -------------------------------

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
  if(cell.data==1 || cell.data==0){// Se tem um porto ou não tem nada
    return 0;
  }

  numberOfFish = cell.data%10;
  kindOfFish = (int) (cell.data/10);

  return fishValues[kindOfFish-1] * (numberOfFish-1); // Não podemos pescar o último peixe de uma determinada área
}

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
// -------------------------------------- Funções para locomoção geral do barco --------------------------------
int getMaxProfitability(int cellProfitability[5], int* index){
  int max = cellProfitability[0];
  *index = 0;

  for(int i=1;i<5;i++){
    if(cellProfitability[i]>max){
      max = cellProfitability[i];
      *index = i;
    }
  }

  return max;
}

char getDirectionByIndex(int index){
  switch(index){
    case 0: return 'C'; // Current
    case 1: return 'U'; // Up
    case 2: return 'D'; // Down
    case 3: return 'L'; // Left
    case 4: return 'R'; // Right
  }
}

// ---------------------------------------------------- Main -------------------------------------------------
int main() {
  char response[MAX_STR];   // dados temporários
  char myId[MAX_STR] = {0};   // identificador do bot em questão (Inicializando vazio para eliminar eventuais lixos de memória)
  
  Player player = {0}; // Inicializa o registro do player principal com os dados vazios
  char* possibleMoves = NULL; // Movimentos disponíveis para quando não houver algum lucrativo para fazer
  int numberOfMoves = 0;

  int xHarbor = -1; // Cordenada x do porto mais próximo
  int yHarbor = -1; // Cordenada y do porto mais próximo

  // Array de lucratividade das células
  int cellProfitability[5] = {-1,-1,-1,-1,-1}; // 0: atual, 1: cima, 2: baixo, 3:esquerda, 4:direita
  int maxProfitability = 0;
  int indexMaxProfitability = 0;

  setbuf(stdin, NULL);   // stdin, stdout e stderr não terão buffers
  setbuf(stdout, NULL);  // assim, nada é "guardado temporariamente"
  setbuf(stderr, NULL);

  // === INÍCIO DA PARTIDA ===
  int h, w; // Height e width
  scanf("AREA %i %i", &h, &w);  // lê a dimensão da área de pesca: altura (h) x largura (w)
  
  // Inicializando o mapa
  Cell** map = (Cell**) calloc(h, sizeof(Cell*));
  for(int i=0;i<h;i++){
    map[i] = (Cell*) calloc(w, sizeof(Cell));
  }

  // Limpa o valor das células
  for(int i=0;i<h;i++){
    for(int j=0;j<w;j++){
      map[i][j].data = 0;
      map[i][j].hasBoat = 0;
    }
  }
  
  scanf(" ID %s", myId);        // ...e o id do bot
  // obs: o " " antes de ID é necessário para ler o '\n' da linha anterior

  // Atribui o valor do id lido para o campo id do player
  for(int i=0;i<11 && myId[i]!='\000';i++){
    player.id[i] = myId[i];
  }

  // === PARTIDA === 
  // O bot entra num laço infinito, mas não se preocupe porque o simulador irá matar
  // o processo quando o jogo terminar.
  while (1) {
    // LÊ OS DADOS DO JOGO E ATUALIZA OS DADOS DO BOT
    readData(h, w, map, &player);

    // LÓGICA PARA A MOVIMENTAÇÃO DO BOT

    if(player.currentLoad==10){ // Se já estiver com a carga máxima de peixes
      checkNearestHarbor(h, w, map, &player, &xHarbor, &yHarbor);

      if(moveToHarbor(h, w, map, xHarbor, yHarbor, &player)) // Se houve algum movimento para o porto
        readData(h, w, map, &player);
      
      toSell(&player);

      continue;
    }

    // Calcular a lucratividade de cada direção e verificar os movimentos não lucrativos possíveis
    for(int i=0;i<5;i++){
      cellProfitability[i] = getMoveProfitability(getDirectionByIndex(i), h, w, map, &player);

      if(cellProfitability[i]==0 && i>0){ // O elemento 0 (posição atual) não será adicionada aos movimentos possíveis
        numberOfMoves+=1;

        if(numberOfMoves==1){ // Se for o primeiro elemento do array
          possibleMoves = (char*) calloc(numberOfMoves, sizeof(char)); // Aloca espaço na memória
        }else{
          possibleMoves = (char*) realloc(possibleMoves,sizeof(char) * numberOfMoves); // Aumenta o espaço do array na memória
        }

        possibleMoves[numberOfMoves-1] = getDirectionByIndex(i);
      }
    }

    // Verifica qual ação tomar
    maxProfitability = getMaxProfitability(cellProfitability,&indexMaxProfitability);

    if(maxProfitability>0){ // Se tiver alguma jogada lucrativa
      
      if(indexMaxProfitability==0){ // Se a posição de maior lucratividade for a atual
        toFish(&player);
      }else{
        move(getDirectionByIndex(indexMaxProfitability)); // Se move para a célula de maior valor
        scanf("%s", response);
      }
    }else{ // Faz um movimento aleatório para alguma posição possível
      srand(time(NULL)); // Gera uma semente aleatória baseada no tempo (Para dar maior aleatoriedade)

      if(numberOfMoves>0){ // Se houver movimentos possíveis
        move(possibleMoves[random()%numberOfMoves]);
        scanf("%s", response);
      }else{
        move('P'); // Passar a vez
        scanf("%s", response);
      }
    }

    // Reiniciar algumas variáveis
    maxProfitability = 0;
    indexMaxProfitability = 0;

    numberOfMoves = 0;
    free(possibleMoves);
    possibleMoves = NULL;
  }

  return 0;
}