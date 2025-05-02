#ifndef PHILO_H
#define PHILO_H
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>
#include<limits.h>
#include <stdbool.h>
#define DEATH "died"
#define EAT "is eating"
#define SLEEP "is sleeping"
#define THINK "is thinking"
#define FORK "has taken a fork"
#define LIMIT 4294967295


typedef struct s_data
{
  int nop;
  unsigned long ttd;
  unsigned long tts;
  unsigned long tte;
  unsigned long notme;
  unsigned long start_time;
} t_data;

typedef struct s_ph
{
  t_data *data ;
  int id;
  unsigned long last_meal;
  pthread_mutex_t *mutex_last_meal;
  pthread_mutex_t *rfork;
  pthread_mutex_t *lfork;
} t_ph;
unsigned long time_now_ms(void);
void init(t_ph *philo , t_data *data );
#endif #include "philos.h"

unsigned long time_now_ms()
{
  unsigned long time;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
  return time;
}
void print_status(t_ph *philo, char *str)
{
  unsigned long time;

  time = time_now_ms() - philo->data->start_time;
  printf("%lu %d %s\n", time, philo->id, str);
}

// int smart_usleep(unsigned long ms)
// {

// }


void *func(void *philo1)
{
  t_ph *philo = philo1;
  int count = 0;

  while (1)
  {
    if((philo->data->notme != -1) && (count < philo->data->notme))
    {
      count++;
      if (count >= philo->data->notme)
        return NULL;
    }
    print_status(philo, THINK);
    if (philo->id % 2 == 0)
    {
      pthread_mutex_lock(philo->rfork);
      pthread_mutex_lock(philo->lfork);
    }
    else
    {
      pthread_mutex_lock(philo->lfork);
      pthread_mutex_lock(philo->rfork);
    }

    print_status(philo, FORK);
    print_status(philo, FORK);
    print_status(philo, EAT);
    usleep(philo->data->tte * 1000);
    pthread_mutex_lock(philo->mutex_last_meal);
    philo->last_meal = time_now_ms();
    pthread_mutex_unlock(philo->mutex_last_meal);
    pthread_mutex_unlock(philo->lfork);
    pthread_mutex_unlock(philo->rfork);
    print_status(philo, SLEEP);
    usleep(philo->data->tts * 1000);
  }

  return NULL;
}

void *untilonedie(void *a)
{
  t_ph *philo = a;
  int size = philo[0].data->nop;

  while (1)
  {
    for (int i = 0; i < size; i++)
    {
      pthread_mutex_lock(philo[i].mutex_last_meal);
      if (time_now_ms() - philo[i].last_meal >= philo[i].data->ttd)
      {
        pthread_mutex_unlock(philo[i].mutex_last_meal);
      pthread_mutex_unlock(philo[i].mutex_last_meal);
      }
    }
    usleep(1000);
  }
  return NULL;
}

void init(t_ph *philo , t_data *data )
{
  pthread_t *thread;
  pthread_mutex_t *mutex;
  pthread_t monitor;
  pthread_mutex_t *meals;

  int size = data->nop;
  meals = malloc(sizeof(pthread_mutex_t) * size);
  for (int j = 0; j < size; j++)
    pthread_mutex_init(&meals[j], NULL);

  mutex = malloc(sizeof(pthread_mutex_t) * size);
  if(!mutex)
    return ;
  for (int j = 0; j < size; j++)
    pthread_mutex_init(&mutex[j], NULL);
  thread = malloc(sizeof(pthread_t) * data->nop);
  if (!thread)
    return ;
  for (int i = 0; i < size; i++)
  {
    philo[i].data = data;
    philo[i].id = i + 1;
    philo[i].mutex_last_meal = &meals[i];
    philo[i].rfork = &mutex[i];
    philo[i].lfork = &mutex[(i + 1) % size];
  }
  for (int j = 0; j < data->nop; j++)
  {
    if (pthread_create(&thread[j], NULL, &func, &philo[j]) != 0)
    {
      printf("Error creating thread\n");
      return;
    }
  }
   pthread_create(&monitor, NULL, &untilonedie, philo);
   for (int j = 0; j < size; j++)
  {
    pthread_join(thread[j], NULL);
    }
   for (int j = 0; j < size; j++)
     pthread_mutex_destroy(&mutex[j]);
   pthread_join(monitor, NULL);
   free(thread);
   free(meals);
   free(mutex);
}#include "philos.h"
unsigned long tol(char *string)
{
	int		i;
	unsigned long	res;

	res = 0;
	i = 0;
	if (string[i] == '-')
	{
		printf("NO NEGATIVE NUMBERS PLEASE!\n");
		printf("THE NUMBER YOU ENTERED IS INVALID IN MY PROJECT : %s", string);
    return -1;
  }
  if (string[i] == '+')
		i++;
	while (string[i] >= '0' && string[i] <= '9')
	{
		if (res > LIMIT)
		{
			printf("overflow !!! \n");
      return -1;
    }
    res = res * 10 + (string[i] - '0');
		i++;
	}
	if (string[i] != '\0')
	{
		printf("INVALID PARAM: %s\n", string);
    return -1;
  }
  return (res);
}



int main(int ac , char **av)
{
  t_ph	*philo;
  t_data	data;
  if(ac != 5 && ac != 6)
  {
    printf("TRY TO ENTER THE RIGHT ARGUMENTS HONEY :| ");
    return 1;
  }

  data.nop = tol(av[1]);
  data.ttd = tol(av[2]);
  data.tte = tol(av[3]);
  data.tts = tol(av[4]);
  if(ac == 6)
    data.notme = tol(av[5]);
  else
    data.notme = -1;  
  data.start_time = time_now_ms();
  philo = malloc(sizeof(t_ph) * data.nop);
  if (!philo)
    return (1);
  init(philo, &data);
  free(philo);
  return (0);
}