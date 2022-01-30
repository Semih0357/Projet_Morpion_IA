//*****************************************************************************************
// PROJET DE STRUCTURE DE DONNEES
// BASBUNAR Semih
//*****************************************************************************************
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#define CONTINUE 0
#define FINISHED 1

typedef enum transformation{ID, ROT_90, ROT_180,ROT_270, MIROIR_VERT} transformation;

typedef struct _grille
 {
     uint32_t score;
     uint32_t cases[9];
     int32_t nb_bille;				
     struct _grille *suivant;
     struct _grille *coup_suivant;
     uint8_t bille_joue;
 } grille;

typedef struct
 {
     uint8_t taille;				
     grille *tete;				
 } liste;

typedef struct
 {
     liste **tab;				
     uint32_t taille;
 } tab_hachage;

typedef struct
{
    uint8_t nbr_transformation;			
    uint32_t score_min_similaire;
} cle;



grille *new_grille(uint32_t score,uint32_t nb_bille);           //Initialisation de la grille
liste *new_liste();						// Initialisation de la liste
tab_hachage* new_tab_hachage(uint32_t taille,uint8_t g[3][3], tab_hachage *th);	//Initialisation du tableau de hachage
void add_grille_tete(liste *l, uint8_t grille[3][3]);		//Fonction d'ajout en tête de liste

_Bool est_vide( liste *l );
void rem_tete(liste *l,FILE *out);		//Fonction de suppression en tête de liste
void vider(liste *l,FILE *out);			//Fonction permettant la suppresion d'une liste
void vider_th(tab_hachage *th,FILE *out);	//Fonction permettant du tableau d'hachage

char print_value(uint8_t value);
void appliquer_transformation_base(uint8_t grille[3][3], transformation t);
void print_data_grille(grille *g, FILE *f);		//Fonction permettant d'afficher les données 'une grille dans un fichier
void print_grille_2d(uint8_t grille[3][3], FILE *f);
void print_grille(uint8_t grille[3][3]);
uint8_t next_configuration(uint8_t grille[3][3]);

uint8_t test_gagnant_ou_pas(uint8_t g[3][3]);		//Fonction testant s'il y a un gagnant
uint8_t test_fin_de_partie(uint8_t g[3][3]);		//Fonction testant si c'est une fin de partie
uint8_t compteur_o(uint8_t grille[3][3]);		//Fonction comptant le nombre de 'O'
uint8_t compteur_x(uint8_t grille[3][3]);		//Fonction comptant le nombre de 'X'
uint32_t score_grille(uint8_t grille[3][3]);		//Fonction qui calcul la valeur d'une grille

cle hachage(uint8_t g[3][3]);									//Fonction de hachage
void Manip_config_similaire(uint8_t cases_libre[3][3], cle cle);				//Fonction retrouvant une grille en appliquant des transformation
void Machineplay(uint8_t g[3][3], uint8_t cases_libre[3][3], tab_hachage* th,int Whoplay);      //Fonction permettant à la machine de jouer son	tour						
uint8_t tirage_au_sort_bille(grille *g);							//Fonction permettant de tirer au sort une bille parmi les billes d'une grille
void execution_game(tab_hachage* th, uint8_t g[3][3], uint8_t cases_libre[3][3]);		//Fonction permettant d'afficher le menu du jeu


//*****************************************************************************************

grille *new_grille(uint32_t score, uint32_t nb_bille)
 {
     grille *g = malloc(sizeof(grille));
     if(g==NULL)
        {
            assert(0);
        }
     g->score = score;
     g->coup_suivant=NULL;
     g->suivant = NULL;
     g->nb_bille=nb_bille*10; 
     int i=8,x=0;
     for(i=8; i>=0; i--)
     {
         x=score%3;
         score=score/3;
         if(x==0){			//On fait des divisions successive et met 10 billes pour chaque case vide de la grille
             g->cases[i]=10;
         }
         else{
             g->cases[i]=0;
         }
     }
     return g;
 }

liste *new_liste()
 {
     liste *r = malloc(sizeof(liste));
     if(r==NULL)
     {
         assert(0);
     }
     r->taille=0;
     r->tete = NULL;
     return r;
}

//Fonction d'ajout en tête de liste
void add_grille_tete(liste *l, uint8_t gr[3][3])
{
    uint32_t nb_bille= 9-compteur_o(gr)-compteur_x(gr);
    grille *g = new_grille( score_grille(gr), nb_bille );
    g->suivant = l->tete;
    l->tete = g;
    l->taille += 1;
    
}


//Fonction permettant l'initialisation de la structure de table de hachage
tab_hachage* new_tab_hachage(uint32_t taille,uint8_t g[3][3], tab_hachage *th)
 {
     th = malloc( sizeof(tab_hachage) );	//Allocation mémoire du tableau de hachage
     assert(th != NULL);
     th->tab = malloc(taille*sizeof(liste*));	//Allocation mémoire pour les listes de notre tableau
     assert(th->tab != NULL);
     uint32_t i;     
     for( i=0; i<taille; i=i+1 )
     {
         th->tab[i] = new_liste();	// Allocation d'une liste pour chaque case tu tableau d'hachage
     }
     th->taille = taille;
     cle c;
     uint32_t score_min;
     add_grille_tete( th->tab[0] ,g);
      while(next_configuration(g)==CONTINUE)
      {
          if ( (test_gagnant_ou_pas(g)==0) && (test_fin_de_partie(g)==0) )
          {
              if ((compteur_o(g)==compteur_x(g)||compteur_o(g)+1==compteur_x(g)) && compteur_o(g)+compteur_x(g)<8 && abs(compteur_o(g)-compteur_x(g))<2)
                  {
                      c = hachage(g);
                      score_min = c.score_min_similaire;
                     if ( score_grille(g)==score_min )
                     {
                           add_grille_tete( th->tab[score_min%100] ,g);    //Seul les grilles respectant les conditions sont ajoutés dans notre structure
                          
                     }
                 }
          }
          
      }
     return th;
}

//Fonction ajoutant une grille en tête de liste 
void add_grille_joue(liste *l, grille *g, uint8_t coupjouer)
{
    g->coup_suivant = l->tete;
    l->tete = g;
    g->bille_joue = coupjouer;
    l->taille += 1;  
}

//Fonction remplissant les billes d'une grille à l'aide de son score
void remplissage_grille( grille* g)
 {	
 	int i=8,x;
 	uint32_t score=g->score;
 	for(i=8; i>=0; i--)
     	{
         	x=score%3;
         	score=score/3;				// On utilise la méthode de division successive
         	if(x==0){
             	g->cases[i]=10;
             	g->nb_bille= g->nb_bille +10;
         	}
         	else{
             	g->cases[i]=0;
         	}
     	}
  }
 
 //*****************************************************************************************
 // DESTRUCTION COMPLETE DE LA STRUCTURE
 //*****************************************************************************************
 
 //Test si la liste est vide
_Bool est_vide( liste *l )
{
    return l->taille == 0;
}

//Fonction retirant la tête d'une liste en récupérant les données en sortie.txt et libère la mémoire
void rem_tete(liste *l, FILE *out )
 {
     grille *t = l->tete;
     print_data_grille(t,out);
     l->tete = l->tete->suivant;
     free(t);
     l->taille -= 1;
 }
 
//Fonction permettant de vider une liste tout en libérant la mémoire
void vider(liste *l,FILE *out)
 {
     while( ! est_vide(l) )
     {
     rem_tete(l,out);  //On retire la tête jusqu'à vider
     }
     free(l);
 }
   
 //Fonction permettant de vider le tableau de pointeurs de liste tout en libérant la mémoire 
 void vider_th(tab_hachage *th,FILE *out)
 {
     uint32_t i;
     for( i=0; i<th->taille; i=i+1 )
     {
         vider(th->tab[i],out);
     }
     free(th->tab);
     free(th);
 }


//*****************************************************************************************
// FONCTION D'AIDE PROJET
//*****************************************************************************************

char print_value(uint8_t value)
{
    switch(value)
    {
        case (0):
            return ' ';
        case (1):
            return 'x';
        case (2):
            return 'o';
        default:
            assert(0);
    }
}

void appliquer_transformation_base(uint8_t grille[3][3], transformation tr)
{
    uint8_t t;
    switch(tr)
    {
        case(ID):
            break;

        case(ROT_90):
            t = grille[0][0];
            grille[0][0] = grille[2][0];
            grille[2][0]=grille[2][2];
            grille[2][2]=grille[0][2];
            grille[0][2]=t;

            t = grille[0][1];
            grille[0][1] = grille[1][0];
            grille[1][0]=grille[2][1];
            grille[2][1]=grille[1][2];
            grille[1][2]=t;
            break;
            
        case(ROT_180):
            appliquer_transformation_base(grille, ROT_90);
            appliquer_transformation_base(grille, ROT_90);
            break;
            
        case(ROT_270):
            appliquer_transformation_base(grille, ROT_90);
            appliquer_transformation_base(grille, ROT_90);
            appliquer_transformation_base(grille, ROT_90);
            break;

        case(MIROIR_VERT):
            t = grille[0][0];
            grille[0][0] = grille[0][2];
            grille[0][2] = t;

            t = grille[1][0];
            grille[1][0] = grille[1][2];
            grille[1][2] = t;

            t = grille[2][0];
            grille[2][0] = grille[2][2];
            grille[2][2] = t;
            break;
    }
}

void print_data_grille(grille *g, FILE *f)
{
    fprintf(f,"%d %d %d %d %d %d %d %d %d %d\n",g->score,g->cases[0],g->cases[1],g->cases[2],g->cases[3],g->cases[4],g->cases[5],g->cases[6],g->cases[7],g->cases[8]);
}

void print_grille_2d(uint8_t grille[3][3], FILE *f)
{
    fprintf(f, "|%c|%c|%c|\n", print_value(grille[0][0]), print_value(grille[0][1]), print_value(grille[0][2]));
    fprintf(f, "|%c|%c|%c|\n", print_value(grille[1][0]), print_value(grille[1][1]), print_value(grille[1][2]));
    fprintf(f, "|%c|%c|%c|\n", print_value(grille[2][0]), print_value(grille[2][1]), print_value(grille[2][2]));
    fprintf(f, "\n");
}

void print_grille(uint8_t grille[3][3])
{
    printf("  %c ┃ %c ┃ %c \n",print_value(grille[0][0]), print_value(grille[0][1]), print_value(grille[0][2]));
    printf("━━━━━━━━━━━━━ \n");
    printf("  %c ┃ %c ┃ %c \n",print_value(grille[1][0]), print_value(grille[1][1]), print_value(grille[1][2]));
    printf("━━━━━━━━━━━━━ \n");
    printf("  %c ┃ %c ┃ %c \n",print_value(grille[2][0]), print_value(grille[2][1]), print_value(grille[2][2]));
    printf( "\n");
}

uint8_t next_configuration(uint8_t grille[3][3])
{
    uint8_t i=0, j=0;

    grille[0][0]++;
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            if(grille[i][j]==3)
            {
                grille[i][j]=0;
                if(j<2) grille[i][j+1]++;
                else if(i<2) grille[i+1][0]++;
                else return FINISHED;
            }
        }
    }

    return CONTINUE;
}

//*****************************************************************************************
// FONCTIONS DE TESTS sur les grilles
//*****************************************************************************************

// Fonction renvoyant 1 si le joueur 1 a réussi un alignement
//		      2              2
//                    0 sinon
uint8_t test_gagnant_ou_pas(uint8_t g[3][3]){
    int8_t winner;

    if( (g[0][0]==1 && g[1][0]==1 && g[2][0]==1) || (g[0][1]==1 && g[1][1]==1 && g[2][1]==1)  ||
        (g[0][2]==1 && g[1][2]==1 && g[2][2]==1) || (g[0][0]==1 && g[0][1]==1 && g[0][2]==1)  ||   // On test tout les alignement possible dans un tableau 3x3
        (g[1][0]==1 && g[1][1]==1 && g[1][2]==1) || (g[2][0]==1 && g[2][1]==1 && g[2][2]==1)  ||
        (g[0][0]==1 && g[1][1]==1 && g[2][2]==1) || (g[2][0]==1 && g[1][1]==1 && g[0][2]==1)  )
    {
        winner =1;
        return winner;
    }
    if( (g[0][0]==2 && g[1][0]==2 && g[2][0]==2) || (g[0][1]==2 && g[1][1]==2 && g[2][1]==2)  ||
        (g[0][2]==2 && g[1][2]==2 && g[2][2]==2) || (g[0][0]==2 && g[0][1]==2 && g[0][2]==2)  ||   // On test tout les alignement possible dans un tableau 3x3
        (g[1][0]==2 && g[1][1]==2 && g[1][2]==2) || (g[2][0]==2 && g[2][1]==2 && g[2][2]==2)  ||
        (g[0][0]==2 && g[1][1]==2 && g[2][2]==2) || (g[2][0]==2 && g[1][1]==2 && g[0][2]==2)  )
        {
            winner =2;
            return winner;
    }
    return 0;
}
    
// Fonction testant si toutes les cases ont été prises
uint8_t test_fin_de_partie(uint8_t g[3][3])
    {
        if( g[0][0]!=0 && g[1][0]!=0 && g[2][0]!=0 &&  g[0][1]!=0 && g[1][1]!=0 && g[2][1]!=0 &&
         g[0][2]!=0 && g[1][2]!=0 && g[2][2]!=0 )
        {
            return 1;
        }
    return 0;
    }

//Fonction renvoyant le nombre de 'X' d'une grille
uint8_t compteur_x(uint8_t grille[3][3])
{   uint8_t t=0;
    int i=0, j=0;
    
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            if(grille[i][j]==1)
                t++;
        }
    }
    return t;
}

//Fonction renvoyant le nombre de '0' d'une grille
uint8_t compteur_o(uint8_t grille[3][3])
{   uint8_t t=0;
    int i=0, j=0;
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            if(grille[i][j]==2)
                t++;
        }
    }
    return t;
}

//Fonction renvoyant la valeur d'une grille 
uint32_t score_grille(uint8_t grille[3][3]){
    uint32_t somme=0;
    uint8_t i, j;

    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            somme = somme + pow(3,(8-j-3*i))* grille[i][j];
        }
    }
    return somme;
}

//*****************************************************************************************
// Fonctions de hachage
//*****************************************************************************************


// Fonction renvoyant une cle de hachage 
cle hachage(uint8_t g[3][3])
 {
    cle cle;
    uint32_t tab[8];
     if(score_grille(g)==0)
     {
         cle.score_min_similaire=0;
         cle.nbr_transformation=0;
         return cle;
     }
    tab[0]=score_grille(g);
    appliquer_transformation_base(g, ROT_90);
    tab[1]=score_grille(g);
    appliquer_transformation_base(g, ROT_90);
    tab[2]=score_grille(g);
    appliquer_transformation_base(g, ROT_90);
    tab[3]=score_grille(g);
    appliquer_transformation_base(g, ROT_90);
    appliquer_transformation_base(g, MIROIR_VERT);	// Calcul des valeurs de grilles pour tout les configurations différentes
    tab[4]=score_grille(g);
    appliquer_transformation_base(g, ROT_90);
    tab[5]=score_grille(g);
    appliquer_transformation_base(g, ROT_90);
    tab[6]=score_grille(g);
    appliquer_transformation_base(g, ROT_90);
    tab[7]=score_grille(g);
    appliquer_transformation_base(g, ROT_90);
    appliquer_transformation_base(g, MIROIR_VERT);
    
    cle.score_min_similaire = tab[0];
    cle.nbr_transformation=0;
     int i=0;
     for(i=0; i<8; i++)
     {
         if (tab[i] <= cle.score_min_similaire )
         {
             cle.score_min_similaire = tab[i];  //On récupère la valeur de grille minimum parmi les 8 configuration possible
             cle.nbr_transformation = i;	//On récupère le nombre de transformation faite pour trouve le minimum des valeurs
         }
     }
     return cle;  // Retourne la clé
 }


// Fonction appliquant des transformation de rotation et de symétrie pour retrouver la grille de valeur minimum
void Manip_config_similaire(uint8_t cases_libre[3][3], cle cle)
{
    switch(cle.nbr_transformation)
    {
        case(1):
            appliquer_transformation_base(cases_libre, ROT_270);
            break;

        case(2):
            appliquer_transformation_base(cases_libre, ROT_270);
            appliquer_transformation_base(cases_libre, ROT_270);
            break;
        case(3):
            appliquer_transformation_base(cases_libre, ROT_270);
            appliquer_transformation_base(cases_libre, ROT_270);
            appliquer_transformation_base(cases_libre, ROT_270);
            break;
        case(4):
            appliquer_transformation_base(cases_libre, ROT_90);
            appliquer_transformation_base(cases_libre, ROT_270);
            appliquer_transformation_base(cases_libre, MIROIR_VERT);
            break;

        case(5):
            appliquer_transformation_base(cases_libre, ROT_270);
            appliquer_transformation_base(cases_libre, MIROIR_VERT);
            break;

        case(6):
            appliquer_transformation_base(cases_libre, ROT_180);
            appliquer_transformation_base(cases_libre, MIROIR_VERT);
            break;
        case(7):
            appliquer_transformation_base(cases_libre, ROT_90);
            appliquer_transformation_base(cases_libre, MIROIR_VERT);
            break;
    }
}


//Fonction permettant à la machine de jouer son tour
void Machineplay(uint8_t g[3][3], uint8_t cases_libre[3][3], tab_hachage* th,int Whoplay )
{
    unsigned int i=0, j=0;
    if(compteur_x(g)+compteur_o(g)>=8)
    {
        for(i=0; i<3; i++)
        {
            for(j=0; j<3; j++)
            {
                if(g[i][j]==0)
                g[i][j]=2;
            }
        }
    }else{
        cle cle = hachage(g);					// execution de la fonction d'hachage
        uint32_t lignetab = cle.score_min_similaire%100;	// recupère la ligne du tableau d'hachage
        
        grille *e;
        e = th->tab[lignetab]->tete;				//On se positionne en tête de liste
        
        for(i=0; i< th->tab[lignetab]->taille ; i++)
        {
            while (e != NULL)						
            {
                if ( e->score == cle.score_min_similaire )	// On Parcours la liste jusqu'a retrouver la grille qu'on recherche
                {
                    uint8_t coupjouer;
                    coupjouer = tirage_au_sort_bille(e);		     // On tire au hasard une bille
                    add_grille_joue(th->tab[99+Whoplay],e,coupjouer);    	//Ajoute la grille ainsi que la bille joué dans notre liste de grilles rencontrés
                    cases_libre[(coupjouer-1)/3][(coupjouer-1)%3]=Whoplay;	//On place la bille dans un tableau 3x3 vide
                    Manip_config_similaire(cases_libre, cle);			//On applique des transformation de rotation et de symétrie
                    for(i=0; i<3; i++)
                    {
                        for(j=0; j<3; j++)
                        {
                            if(cases_libre[i][j] == Whoplay)
                                g[i][j] = Whoplay;				//Place la bille se trovant dans cases_libre dans g en même position
                            ;
                        }
                    }
                    memset(cases_libre,0, 9*sizeof(uint8_t ));			//On met tous les cases de cases_libre à 0 
                }
            e = e->suivant;
            }
        }
    }
    
}

//Fonction permettant de tirer au sort une bille            
uint8_t tirage_au_sort_bille(grille *g)
     {
         uint32_t nb_alea=0;
         uint32_t i;
         if(g->nb_bille<=0){  // Si la grille ne possède plus aucun bille on le remplit
             int i,x;
             uint32_t score= g->score;
             for(i=8; i>=0; i--)
     		{
         		x=score%3;
         		score=score/3;		//Méthode de division successive par 3
         		if(x==0){
             			g->cases[i]=10;
             			g->nb_bille= g->nb_bille +10;
         		}
         		else{
             			g->cases[i]=0;
         		}
     		}
    
         }      
       nb_alea= rand()%g->nb_bille +1 ;  // on tire au hasard un nombre 
       i=0;
         while( nb_alea>0 )
         {
             if(i>9){
                 i=0;
             }

             if(g->cases[i]!=0 )
              {
                  uint32_t x=g->cases[i];
                  if(x>=nb_alea){
                      return i+1;	// On renvoie le numéro de bille sortie aléatoirement
                      }
                  else
                      nb_alea=nb_alea-x;
              }
            i++;
         }
         return i;
     }

//Fonction permettant de faire la mise à jour des billes
void mise_a_jour_nb_bille(tab_hachage* th, uint8_t g[3][3]){
    grille *e1;
    grille *e2;
    int i;
    
    e1 = th->tab[100]->tete;
    e2 = th->tab[101]->tete;		//On se positionne en tête des 2 listes servant à mémoriser les grilles joués
    i=th->tab[101]->taille;	
    
    while (e2!= NULL && i>0 )		//On parcours les 2 listes
    {

        if(test_gagnant_ou_pas(g)==1){
        	
        	if (th->tab[101]->taille!=0 && e1!=NULL){
        		e1->cases[e1->bille_joue-1] = e1->cases[e1->bille_joue-1]+3;
            		e1->nb_bille = e1->nb_bille+3;
            	}
           	 e2->cases[e2->bille_joue-1] = e2->cases[e2->bille_joue-1]-1;
           	 e2->nb_bille = e2->nb_bille-1;
        }
            else if(test_gagnant_ou_pas(g)==2){
            		if (th->tab[101]->taille!=0 && e1!=NULL){
                		e1->cases[e1->bille_joue-1] = e1->cases[e1->bille_joue-1]-1;
                		e1->nb_bille = e1->nb_bille-1;
                	}
               		e2->cases[e2->bille_joue-1] = e2->cases[e2->bille_joue-1]+3;
               		e2->nb_bille = e2->nb_bille+3;
            	}
                else if(test_fin_de_partie(g)==1){
                	if (th->tab[101]->taille!=0 && e1!=NULL){
                    		e1->cases[e1->bille_joue-1] = e1->cases[e1->bille_joue-1]+1;
                    		e1->nb_bille = e1->nb_bille+1;
                    	}
                    	e2->cases[e2->bille_joue-1] = e2->cases[e2->bille_joue-1]+1;
                   	e2->nb_bille = e2->nb_bille+1;
                }
        if (th->tab[101]->taille!=0 && e1!=NULL){
        	e1 = e1->coup_suivant;
        	}
        e2 = e2->coup_suivant;
        i--;
    }
    th->tab[100]->tete=NULL;		// On libère la grille en tête de liste
    th->tab[100]->taille=0;		// Initialise la taille à 0
    th->tab[101]->tete=NULL;
    th->tab[101]->taille=0;
}



//Fonction permettant d'afficher le menu du jeu et de récupérer les entrées
void execution_game(tab_hachage* th, uint8_t g[3][3], uint8_t cases_libre[3][3])
{
    char ModeJeu=0;
    int coupjouer=0;
    int Whoplay=0;
    char Again='0';
    
    while (Again=='0' || Again=='1'){
        if (Again=='0') {
                printf("Choisissez un mode de jeu:  1- Player  vs Machine\n");
                printf("                            2- Machine vs Machine\n" );
                printf("                            3- Quitter le jeu\n" );
                do{
                    fflush(stdin);
                    printf("Taper 1 ou 2 ou 3:");
                    scanf("%s", &ModeJeu);
                }
                while( ModeJeu!='1' && ModeJeu!='2'&& ModeJeu!='3');
               
                if( ModeJeu=='1'){
                     printf("Vous avez choisi de jouer contre la machine : ModeJeu=%c\n",ModeJeu);
                }else if( ModeJeu=='2'){
                     printf("Vous avez choisi de faire jouer la machine contre elle même : ModeJeu=%c\n",ModeJeu);
                }else{
                    Again='2';
                }
        }
	memset(g,0, 9*sizeof(uint8_t ));
        
        if(ModeJeu=='1'){
            Whoplay = rand() % 2 + 1;
            if( Whoplay==1){
                printf("Vous commencez la partie :\n");
            }else{
                printf("La machine commence la partie\n");
            }
            while (test_gagnant_ou_pas(g)==0 && test_fin_de_partie(g)==0 )
            {
                if(Whoplay==2)
                {
                    Machineplay(g, cases_libre ,th,Whoplay);
                    printf("La machine a joué son tour \n");
                    print_grille(g);
                    Whoplay=1;
                }
                if(test_gagnant_ou_pas(g)==0 && test_fin_de_partie(g)==0 && Whoplay==1){
                    do {
                    fflush(stdin);
                    printf("C'est à votre tour : Veuillez entrer un numéro de case (1-9):\n");
                    print_grille(g);
                    scanf("%d", &coupjouer);
                    }
                    while( coupjouer<1 || coupjouer>9  );
                    
                    if(g[(coupjouer-1)/3][(coupjouer-1)%3] != 0)
                    {
                        printf("La case est déjà prise, rejouez !\n");
                        Whoplay=1;
                        
                    }else if(g[(coupjouer-1)/3][(coupjouer-1)%3] == 0){
                        g[(coupjouer-1)/3][(coupjouer-1)%3] = 1;
                        print_grille(g);
                        coupjouer=0;
                        Whoplay=2;
                    }
                }
            }
            if(test_gagnant_ou_pas(g)==1){
            		printf("Bravo! Vous avez remporté la partie\n");
            }
            if(test_gagnant_ou_pas(g)==2){
            		printf("Perdu ! La machine a remporté la partie\n");
            }
            	
            mise_a_jour_nb_bille(th,g);
            printf("La machine apprend à force de jouer contre vous :\n");
            printf("Voulez-vous rejouer ? 0->non 1->oui:\n");
            Again=0;
            do{
		fflush(stdin);
		printf("Taper 0 ou 1 :");
		scanf("%s", &Again);
       		}while( Again!='0' && Again!='1');
         }
                
        if(ModeJeu=='2'){
            uint32_t N=0;
            uint32_t i=0;
            do{
                fflush(stdin);
                printf("Combien de partie voulez-vous que la machine joue contre elle même? :");
                scanf("%d", &N);
            }
            while(N==0);
            
            while(i<N){
                memset(g,0, 9*sizeof(uint8_t ));
                Whoplay = 1;
                while (test_gagnant_ou_pas(g)==0 && test_fin_de_partie(g)==0 ){
                    if(Whoplay==2){
                        Machineplay(g, cases_libre ,th,Whoplay);
                        Whoplay=1;
                       // print_grille(g);
                    }
                    if(test_gagnant_ou_pas(g)==0 && test_fin_de_partie(g)==0 && Whoplay==1){
                        Machineplay(g, cases_libre ,th,Whoplay);
                        Whoplay=2;
                       // print_grille(g);
                    }
                }
                mise_a_jour_nb_bille(th,g);
                i++;
            }
            Again='0';
            printf("La machine à bien jouée %d parties contre elle-même\n",N);
       }
    }
}


int main(){
    
        FILE *out = fopen("sortie.txt", "w");
        uint8_t g[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
        uint8_t cases_libre[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
               	
       	tab_hachage *th=NULL;
       	th= new_tab_hachage(102,g,th);
        
        srand(time(NULL));
        execution_game(th,g,cases_libre);
     
        vider_th(th,out);
        fclose(out);

       return EXIT_SUCCESS;
    }
       

                
