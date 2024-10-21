// ldd: -lncurses
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// type pour représenter les cases du jeu
typedef struct {
  int cells[4][4]; // tableau représentant le plateau avec les cases
} Plateau;

Plateau board;

void init_board() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      board.cells[i][j] = 0; // on importe le tableau des cases par board et on lui associe chaque case par sa valeur, et on l'initialise à 0 pour qu'il start par des cases vides
    }
  }
}

int score = 0;

void display_board() {
  move(0,0);
  // affichage de la bannière
  printw("== 2048 ============================== \n");
  printw("======================= Score :      %5i \n", score);
 
  // affichage du plateau
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printw("%5i", board.cells[i][j]); // ça affiche la valeur de la case
    }
    printw("\n"); // pour aller à la ligne après chaque ligne du plateau
  }
  refresh();
}

void INIT_2048 () {
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  srand(time(NULL));
}

void DONE_2048() {
  endwin();
  exit(0);
}

int count_empty() {
    int count = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board.cells[i][j] == 0) {
                count++;
            }
        }
    }

    return count;
}

void add_two(int empty) {

    if (empty == 0) {
        return; // sile plateau est plein, ne rien faire pr l'instant
    }

    // itialiser le générateur de nombres aléatoires avec une graine différente à chaque exécution
    srand(time(NULL));

    // gnérer un nombre aléatoire entre 0 et empty-1 pour sélectionner une case vide
    int random_index = rand() % empty;

    int count = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board.cells[i][j] == 0) { // vérifier si la case est vide
                if (count == random_index) { // si c'est la case sélectionnée aléatoirement
                    board.cells[i][j] = 2; // insérer le nombre 2 dans cette case
                    return; // sortir de la fonction après avoir inséré le nombre 2
                }
                count++;
            }
        }
    }
}

int game_over(int add) {
    int empty_count = count_empty();

    if (empty_count == 0) {
        clear();
        printw("=============(Game Over)=============\n");
      	printw("=============(press a key)=============\n");
        refresh();
        getch();
        return 1;
    }
    // S'il y a au moins une case libre et que add est vrai
    else if (add && empty_count > 0) {
        add_two(empty_count);
        display_board();
        return 0;
    }

    else {
        return 0;
    }
}

int shift_board() {
    int moved = 0;

    for (int i = 0; i < 4; i++) {
        int index = 0; // index pour suivre la position du prochain entier non nul

        for (int j = 0; j < 4; j++) {

            if (board.cells[i][j] != 0) {
                // si l'index est différent de la colonne actuelle, il y a un espace vide
                if (index != j) {
                    // déplacer la valeur vers la gauche
                    board.cells[i][index] = board.cells[i][j];
                    board.cells[i][j] = 0;
                    moved++;
                }
                index++; // pour suivre la position du prochain entier non nul
            }
        }
    }

    return moved;
}

int update_board() {
    bool moved_or_added = false; // indicateur pour savoir si au moins un entier a été déplacé ou ajouté

    // décaler les valeurs vers la gauche
    if (shift_board()) {
        moved_or_added = true; // indiquer que des valeurs ont été déplacées
    }

    // faire les additions des valeurs consécutives égales
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - 1; j++) {
            if (board.cells[i][j] != 0 && board.cells[i][j] == board.cells[i][j + 1]) {
                board.cells[i][j] *= 2; // additionner les valeurs consécutives
                board.cells[i][j + 1] = 0; // vider la case de droite
                // mettre à jour le score en ajoutant la valeur de la case à gauche
                score += board.cells[i][j];
                moved_or_added = true; // indiquer que des valeurs ont été ajoutées
            }
        }
    }

    // décaler à nouveau les valeurs vers la gauche pour compresser les cases vidées par une addition
    if (shift_board()) {
        moved_or_added = true; // indiquer que des valeurs ont été déplacées
    }

    return moved_or_added; // renvoyer vrai si au moins un entier a été déplacé ou ajouté, faux sinon
}

typedef enum {
    DIRECTION_LEFT,
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_GAME_OVER
} key;

key get_key() {
    int ch;
    key pressed_key = DIRECTION_GAME_OVER;

    // boucle pour lire les touches jusqu'à ce qu'une correspondance soit trouvée
    while (true) {
        ch = getch();

        // vérifie si c'est une des 4 directions ou backspace
        switch (ch) {
            case KEY_UP:
                pressed_key = DIRECTION_UP;
                break;
            case KEY_DOWN:
                pressed_key = DIRECTION_DOWN;
                break;
            case KEY_LEFT:
                pressed_key = DIRECTION_LEFT;
                break;
            case KEY_RIGHT:
                pressed_key = DIRECTION_RIGHT;
                break;
            case KEY_BACKSPACE:
                pressed_key = DIRECTION_GAME_OVER;
                break;
            default:
                continue; // on ignore les autres touches et continuer la boucle
        }

        // et on sort de la boucle si une correspondance est trouvée
        if (pressed_key != DIRECTION_GAME_OVER) {
            break;
        }
    }

    return pressed_key; // enfin on renvoie la touche correspondante
}


void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void mirror_board() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 / 2; j++) {
            // c'est pour échanger les éléments qui sont symétriques de chaque ligne
            swap(&board.cells[i][j], &board.cells[i][4 - 1 - j]);
        }
    }
}

void pivot_board() {
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            // échange les éléments (i, j) avec (j, i) simplement
            swap(&board.cells[i][j], &board.cells[j][i]);
        }
    }
}

int play(key dir) {

    switch (dir) {
        case KEY_UP:
            mirror_board(); // Inverser les lignes pour simuler un mouvement vers le haut
            pivot_board();  // Transposer le plateau pour se préparer à la compression vers le haut
            break;
        case KEY_DOWN:
            mirror_board(); // Inverser les lignes pour simuler un mouvement vers le bas
            pivot_board();  // Transposer le plateau pour se préparer à la compression vers le bas
            break;
        case KEY_LEFT:
            // Aucun besoin de transformation pour un mouvement vers la gauche
            break;
        case KEY_RIGHT:
            mirror_board(); // Inverser les lignes pour simuler un mouvement vers la droite
            break;
        default:
            return 0; // Retourner 0 en cas de direction non reconnue
    }

    // on appliquer la compression vers la gauche (ou vers le haut/droite selon la préparation)
    int movement_occurred = update_board();

    // et on restaure l'orientation initiale du plateau après la compression
    switch (dir) {
        case KEY_UP:
            pivot_board();  // Transposer de nouveau pour restaurer l'orientation originale
            mirror_board(); // Inverser les lignes de nouveau pour restaurer l'orientation originale
            break;
        case KEY_DOWN:
            pivot_board();  // Transposer de nouveau pour restaurer l'orientation originale
            mirror_board(); // Inverser les lignes de nouveau pour restaurer l'orientation originale
            break;
        case KEY_LEFT:
            // Aucun besoin de restauration pour un mouvement vers la gauche
            break;
        case KEY_RIGHT:
            mirror_board(); // Inverser les lignes pour restaurer l'orientation originale
            break;
    }

    return movement_occurred; // Renvoyer vrai si un mouvement a eu lieu, faux sinon
}


int main() {
  INIT_2048();
 
  init_board();
  
  int game_over_flag = 0;
  int add = 1;

  while (!game_over(add)) {
    getch();
    if (update_board()) {
      display_board(); // afficher le plateau mis à jour si au moins une valeur a été déplacée ou ajoutée pr compresser
        }
  }
  
  while (!game_over_flag) {
        key dir = get_key(); // Lire la touche du clavier
        int movement_occurred = play(dir); // Effectuer le mouvement correspondant

        if (movement_occurred) {
            add = 1;
        } else {
            add = 0;
        }

        game_over_flag = game_over(add); // Vérifier et gérer la fin de partie
    }
 
  display_board();
 
  DONE_2048();
}
