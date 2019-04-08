#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <semaphore.h>

#include "cube.h"
#include "wizard.h"

#define DEFAULT_CUBE_SIZE 4
#define DEFAULT_TEAM_SIZE 5
#define DEFAULT_SEED 1
#define HOWBUSY 100000000
#define TRUE 1
#define FALSE 0




void command_line_usage()
{
  fprintf(stderr, "-size <size of cube> -teamA <size of team> -teamB <size of team> -seed <seed value>\n");
}
//TODO implement kill wizards after thread functionality has been added
// Kills all of the wizards given either teamA or teamB
void kill_wizards(struct wizard *w)
{
  /* Fill in */

  int threadCount = w->cube->teamA_size + w->cube->teamB_size;
  int i;

  // Kills all the the threads
  for(i = 0; i < threadCount; i++)
  {
    pthread_cancel(w->cube->threads[i]);
  }

  return;
}

// retunns winner of the game
// -1 game still active , 0 -team A ,1 - team B
int check_winner(struct cube *cube)
{
  /* Fill in */
  // Check if all the wizards of a givn team are frozen
  // TODO
  int isTeamAActive = FALSE;
  int isTeamBActive = FALSE;
  int i;

  for(i = 0 ; i < cube->teamA_size; i++)
  {
    // wizard is active , status == 0
    if (!cube->teamA_wizards[i]->status )
    {
      isTeamAActive = TRUE;
    }
  }

  for(i =0 ; i < cube->teamB_size; i++)
  {
    // wizard is active , status == 0
    if (!cube->teamB_wizards[i]->status )
    {
      isTeamBActive = TRUE;
    }
  }

  if (isTeamAActive && isTeamBActive )
  {
    return -1;
  } 
  else if(isTeamAActive) 
  {
    return 0;
  } else
  {
    return 1;
  }


}

void print_cube(struct cube *cube)
{
  int i;
  int j;
  int k;

  assert(cube);

  for (i = 0; i < cube->size; i++)
  {
    printf("+");
    for (j = 0; j < cube->size; j++)
    {
      printf("--+");
    }
    printf("\n|");

    for (j = 0; j < cube->size; j++)
    {
      /* Print the status of wizards in this room here */
      for (k = 0; k < 2; k++)
      {
        if (cube->rooms[j][i]->wizards[k] != NULL)
        {
          if (cube->rooms[j][i]->wizards[k]->status)
          {
            printf("%c", tolower(cube->rooms[j][i]->wizards[k]->team));
          }
          else
          {
            printf("%c", toupper(cube->rooms[j][i]->wizards[k]->team));
          }
        }
        else
        {
          printf(" ");
        }
      }
      printf("|");
    }
    printf("\n");
  }
  printf("+");
  for (j = 0; j < cube->size; j++)
  {
    printf("--+");
  }
  printf("\n");
  return;
}

struct wizard *init_wizard(struct cube *cube, char team, int id)
{
  int x, newx;
  int y, newy;
  int initflag;
  struct wizard *w;

  w = (struct wizard *)malloc(sizeof(struct wizard));
  assert(w);

  initflag = FALSE;

  w->team = team;
  w->id = id;
  w->status = 0;
  w->cube = cube;

  x = rand() % cube->size;
  y = rand() % cube->size;

  if (cube->rooms[x][y]->wizards[0] == NULL)
  {
    cube->rooms[x][y]->wizards[0] = w;
    w->x = x;
    w->y = y;
  }
  else if (cube->rooms[x][y]->wizards[1] == NULL)
  {
    cube->rooms[x][y]->wizards[1] = w;
    w->x = x;
    w->y = y;
  }
  else
  {
    newx = (x + 1) % cube->size;
    if (newx == 0)
      newy = (y + 1) % cube->size;
    else
      newy = y;

    while ((newx != x || newy != y) && !initflag)
    {

      if (cube->rooms[newx][newy]->wizards[0] == NULL)
      {
        cube->rooms[newx][newy]->wizards[0] = w;
        w->x = newx;
        w->y = newy;
        initflag = TRUE;
      }
      else if (cube->rooms[newx][newy]->wizards[1] == NULL)
      {
        cube->rooms[newx][newy]->wizards[1] = w;
        w->x = newx;
        w->y = newy;
        initflag = TRUE;
      }
      else
      {
        newx = (newx + 1) % cube->size;

        if (newx == 0)
        {
          newy = (newy + 1) % cube->size;
        }
      }
    }
    if (!initflag)
    {
      free(w);
      return NULL;
    }
  }

  /* Fill in */
  // Sets initializes semaphore that is declared within the wizard
  int pshared = 0;
  int value = 1;
  int ret = sem_init(&w->sleep, pshared, value);

  return w;
}

int interface(void *cube_ref)
{
  struct cube *cube;
  char *line;
  char *command;
  int i;

  cube = (struct cube *)cube_ref;
  assert(cube);

  using_history();
  while (1)
  {
  int a;
  int tCount = cube->teamA_size + cube->teamB_size;    

  // if(cube->game_status >= 0 )
  // {
  //      void * q;
  //      for( a = 0; a < tCount; a++)
  //     {
  //         printf("\n THE JOIN FUNCTION IS CALLED! \n");
  //         if(pthread_join(cube->threads[a], &q))
  //         {
  //           printf("\n THE JOIN FUNCTION FAILED! \n");
  //         }
  //     } 

  // } 


    if(cube->game_status == 0)
    {
      // Spin lock until the wizard is done with its move
      while(!printPrompt)
      {

      }
      // Flips the flag
      printPrompt = FALSE;
    }   

    line = readline("cube> ");
    if (line == NULL)
      continue;
    if (strlen(line) == 0)
      continue;

    add_history(line);

    i = 0;
    while (isspace(line[i]))
      i++;

    command = &line[i];
    if (!strcmp(command, "exit"))
    {
      return 0;
    }
    else if (!strcmp(command, "show"))
    {
      print_cube(cube);
      printPrompt = 1;
    }
    else if (!strcmp(command, "start"))
    {
      
      // hasStarted = TRUE;
      if (cube->game_status == 1)
      {
        fprintf(stderr, "Game is over. Cannot be started again\n");
      }
      else if (cube->game_status == 0)
      {
        fprintf(stderr, "Game is in progress. Cannot be started again\n");
      }
      else
      {
        cube->game_status = 0;

        /* Start the game */

        /* Fill in */
        // create the threads array
        // pass the threads using pthread create
        // pass the wizard_func
        int threadCount = cube->teamA_size + cube->teamB_size;
        cube->threads = (pthread_t *)malloc(sizeof(pthread_t *) * threadCount);

        int i;
        int teamBStart = cube->teamA_size - 1 ; // may be off by 1
        printf("before the thread is created!\n");
        // Start threads for team A
        for(i = 0; i < cube->teamA_size; i++)
        {
          if(pthread_create(&cube->threads[i], NULL, wizard_func, cube->teamA_wizards[i]))
          {
            printf("\n THE CREATE FUNCTION FAILED! \n");
          }
        }
        // Start threajds for team B
        for(i = 0; i < cube->teamB_size; i++, teamBStart++)
        {
          if( pthread_create(&cube->threads[teamBStart], NULL, wizard_func, cube->teamB_wizards[i]))
          {
            printf("\n THE CREATE FUNCTION FAILED! \n");
          }
        }
        
        // Spin lock until
        printf("after the thread is created!\n");
        // while(check_winner(cube) == 0)
        // {

        // }  
      }
    }
    else if (!strcmp(command, "stop"))
    {
      /* Stop the game */
      // Kills all wizards regardless of which team is passed
      kill_wizards(cube->teamA_wizards[0]);

      return 1;
    }
    else if (!strcmp(command, "s"))
    {
      // TODO add step functionality 
      sem_post(&stepSemaphore);
        
      // return 1;
    }
    else if (!strcmp(command, "c"))
    {
       while(1)
       {
         if(!cube->game_status)
         {
           sem_wait(&cImplementation);
           sem_post(&stepSemaphore);
         }
       }
      // TODO add continue functionality 
      return 1;
    }
    else
    {
      fprintf(stderr, "unknown command %s\n", command);
    }

    free(line);
  }

  return 0;
}

int main(int argc, char **argv)
{
  int cube_size = DEFAULT_CUBE_SIZE;
  int teamA_size = DEFAULT_TEAM_SIZE;
  int teamB_size = DEFAULT_TEAM_SIZE;
  unsigned seed = DEFAULT_SEED;
  struct cube *cube;
  struct room *room;
  struct room **room_col;
  int res;
  struct wizard *wizard_descr;
  int i, j;

  /* Parse command line and fill:
     teamA_size, timeBsize, cube_size, and seed */
     
  // Sets initializes semaphore that is declared within the room
  int pshared = 0;
  // Value set to to 1 to represent that only one thread can check the status of a room at a time
  int value = 1;
  sem_init(&stepSemaphore, pshared, value);
  sem_init(&cImplementation, pshared, value);
  printPrompt = FALSE;


  i = 1;
  while (i < argc)
  {
    if (!strcmp(argv[i], "-size"))
    {
      i++;
      if (argv[i] == NULL)
      {
        fprintf(stderr, "Missing cube size\n");
        command_line_usage();
        exit(-1);
      }
      cube_size = atoi(argv[i]);
      if (cube_size == 0)
      {
        fprintf(stderr, "Illegal cube size\n");
        exit(-1);
      }
    }
    else if (!strcmp(argv[i], "-teamA"))
    {
      i++;
      if (argv[i] == NULL)
      {
        fprintf(stderr, "Missing team size\n");
        command_line_usage();
        exit(-1);
      }
      teamA_size = atoi(argv[i]);
      if (teamA_size == 0)
      {
        fprintf(stderr, "Illegal team size\n");
        exit(-1);
      }
    }
    else if (!strcmp(argv[i], "-teamB"))
    {
      i++;
      if (argv[i] == NULL)
      {
        fprintf(stderr, "Missing team size\n");
        command_line_usage();
        exit(-1);
      }
      teamB_size = atoi(argv[i]);
      if (teamB_size == 0)
      {
        fprintf(stderr, "Illegal team size\n");
        exit(-1);
      }
    }
    else if (!strcmp(argv[i], "-seed"))
    {
      i++;
      if (argv[i] == NULL)
      {
        fprintf(stderr, "Missing seed value\n");
        command_line_usage();
        exit(-1);
      }
      seed = atoi(argv[i]);
      if (seed == 0)
      {
        fprintf(stderr, "Illegal seed value\n");
        exit(-1);
      }
    }
    else
    {
      fprintf(stderr, "Unknown command line parameter %s\n", argv[i]);
      command_line_usage();
      exit(-1);
    }
    i++;
  }

  /* Sets the random seed */
  srand(seed);

  /* Checks that the number of wizards does not violate
     the "max occupancy" constraint */
  if ((teamA_size + teamB_size) > ((cube_size * cube_size) * 2))
  {
    fprintf(stderr, "Sorry but there are too many wizards!\n");
    exit(1);
  }

  /* Creates the cube */
  cube = (struct cube *)malloc(sizeof(struct cube));
  assert(cube);
  cube->size = cube_size;
  cube->game_status = -1;

  /* Creates the rooms */
  cube->rooms = malloc(sizeof(struct room **) * cube_size);
  assert(cube->rooms);

  for (i = 0; i < cube_size; i++)
  {
    /* Creates a room column */
    room_col = malloc(sizeof(struct room *) * cube_size);
    assert(room_col);

    for (j = 0; j < cube_size; j++)
    {
      /* Creates a room */
      room = (struct room *)malloc(sizeof(struct room));
      assert(room);
      room->x = i;
      room->y = j;
      room->wizards[0] = NULL;
      room->wizards[1] = NULL;
      room_col[j] = room;

      /* Fill in */

      // Sets initializes semaphore that is declared within the room
      // int pshared = 0;
      // Value set to to 1 to represent that only one thread can check the status of a room at a time
      // int value = 1;
      // int ret = sem_init(&room->roomFull, pshared, value);
      
      // if(ret < 0)
      // {
      //   printf("sem init failed on the room creation!!!");
      //   // TODO add exit function later
      // } 
    }

    cube->rooms[i] = room_col;
  }

  /* Creates the wizards and positions them in the cube */
  cube->teamA_size = teamA_size;
  cube->teamA_wizards = (struct wizard **)malloc(sizeof(struct wizard *) *
                                                 teamA_size);
  assert(cube->teamA_wizards);

  cube->teamB_size = teamB_size;
  cube->teamB_wizards = (struct wizard **)malloc(sizeof(struct wizard *) * teamB_size);

  assert(cube->teamB_wizards);

  /* Team A */
  for (i = 0; i < teamA_size; i++)
  {
    if ((wizard_descr = init_wizard(cube, 'A', i)) == NULL)
    {
      fprintf(stderr, "Wizard initialization failed (Team A number %d)\n", i);
      exit(1);
    }
    cube->teamA_wizards[i] = wizard_descr;
  }

  /* Team B */

  for (i = 0; i < teamB_size; i++)
  {
    if ((wizard_descr = init_wizard(cube, 'B', i)) == NULL)
    {
      fprintf(stderr, "Wizard initialization failed (Team B number %d)\n", i);
      exit(1);
    }
    cube->teamB_wizards[i] = wizard_descr;
  }

  /* Fill in */
  //TODO possbily add isSPressed here later?

  /* Goes in the interface loop */
  res = interface(cube);

  exit(res);
}

void dostuff()
{
  int i;
  int wait;

  wait = rand() % HOWBUSY;

  for (i = 0; i < wait; i++)
  {
  }

  return;
}

struct room * choose_room(struct wizard *w)
{
  int newx = 0;
  int newy = 0;

  /* The two values cannot be both 0 - no move - or 1 - diagonal move */
  while (newx == newy)
  {
    newx = rand() % 2;
    newy = rand() % 2;
  }
  if ((rand() % 2) == 1)
  {
    newx = 0 - newx;
    newy = 0 - newy;
  }

  return w->cube->rooms[(w->x + w->cube->size + newx) % w->cube->size][(w->y + w->cube->size + newy) % w->cube->size];
}

// Returns if found a spot in a room successfully
int try_room(struct wizard *w, struct room *oldroom, struct room *newroom)
{

  /* Fill in */
  // Checks the semaphore, if it is 1 then then does a sem_wait to enter the room
  // int isAvailable;

  // sem_getvalue(&newroom->roomFull, &isAvailable);
  // if(isAvailable)
  // {
    // int failed = sem_wait(&newroom->roomFull);
    // if (failed)
    // {
    //   printf("sem_wait function failed! in try_room() \n");
    // }
    // if room is not full return 0
    if(newroom->wizards[0] == NULL || newroom->wizards[1] == NULL ) 
    {
      return 0;
    }
    else
    {
      // sem_post(&newroom->roomFull);
      // return 1;
    }
    
  // }
  // else
  // {
  //   // try room failed
  //   return 1;
  // }
  
}

struct wizard * find_opponent(struct wizard *self, struct room *room)
{
  struct wizard *other = NULL;

  /* Updates room wizards and determines opponent */
  if ((room->wizards[0] == self))
  {
    other = room->wizards[1];
  }
  else if (room->wizards[1] == self)
  {
    other = room->wizards[0];
  }

  return other;
}

void switch_rooms(struct wizard *w, struct room *oldroom, struct room *newroom)
{
  struct wizard *other;

  /* Removes self from old room */
  if (oldroom->wizards[0] == w)
  {
    oldroom->wizards[0] = NULL;
  }
  else if (oldroom->wizards[1] == w)
  {
    oldroom->wizards[1] = NULL;
  }
  else /* This should never happen */
  {
    printf("Wizard %c%d in room (%d,%d) can't find self!\n",
           w->team, w->id, oldroom->x, oldroom->y);
    print_cube(w->cube);
    exit(1);
  }

  /* Fill in */
  // oldroom->occupancy -= 1;
  // newroom->occupancy += 1;
  // Release control over the old room
  //TODO remove this line, may be cause of the bug where 3 wizards enter a room
  // int failed = sem_post(&oldroom->roomFull);
  // if(failed)
  // {
  //   printf("sem_post function failed! in switch_rooms() \n");
  // }

  /* Updates room wizards and determines opponent */
  if (newroom->wizards[0] == NULL)
  {
    newroom->wizards[0] = w;
    other = newroom->wizards[1];
  }
  else if (newroom->wizards[1] == NULL)
  {
    newroom->wizards[1] = w;
    other = newroom->wizards[0];
  }
  else /* This should never happen */
  {
    printf("Wizard %c%d in room (%d,%d) gets in a room already filled with people!\n",
           w->team, w->id, newroom->x, newroom->y);
    print_cube(w->cube);
    exit(1);
  }

  /* Sets self's location to current room */
  w->x = newroom->x;
  w->y = newroom->y;
}

int fight_wizard(struct wizard *self, struct wizard *other, struct room *room)
{
  int res;

  /* Computes the result of the fight */
  res = rand() % 2;

  /* The opponent becomes frozen */
  if (res == 0)
  {
    printf("Wizard %c%d in room (%d,%d) freezes enemy %c%d\n",
           self->team, self->id, room->x, room->y,
           other->team, other->id);

    /* Fill in */
    other->status = 1;
  }

  /* Self freezes and release the lock */
  else
  {

    printf("Wizard %c%d in room (%d,%d) gets frozen by enemy %c%d\n",
           self->team, self->id, room->x, room->y,
           other->team, other->id);

    /* Fill in */
    self->status = 1;
    int failed = sem_wait(&self->sleep);
    if (failed)
    {
      printf("sem_wait function failed! in fight_wizard() \n");
    }

    return 1;
  }
  return 0;
}

int free_wizard(struct wizard *self, struct wizard *other, struct room *room)
{
  int res;

  /* Computes the results of the unfreeze spell */
  res = rand() % 2;

  /* The friend is unfrozen */
  if (res == 0)
  {
    printf("Wizard %c%d in room (%d,%d) unfreezes friend %c%d\n",
           self->team, self->id, room->x, room->y,
           other->team, other->id);

    /* Fill in */

    other->status = 0;
    int failed = sem_post(&other->sleep);
    if(failed)
    {
      printf("sem_post function failed! in free_wizard function\n");
    }

  }

  /* The spell failed */
  printf("Wizard %c%d in room (%d,%d) fails to unfreeze friend %c%d\n",
         self->team, self->id, room->x, room->y,
         other->team, other->id);

  return 0;
}
