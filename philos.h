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
  bool stop;
  int id;
  unsigned long last_meal;
  pthread_mutex_t *mutex_last_meal;
  pthread_mutex_t *rfork;
  pthread_mutex_t *lfork;
} t_ph;
unsigned long time_now_ms(void);
void init(t_data *data );
#endif 