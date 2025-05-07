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
  bool stop;
  unsigned long notme;
  unsigned long start_time;
} t_data;

typedef struct s_mutex 
{
  pthread_mutex_t *stop_mutex;
  pthread_mutex_t *mutex_last_meal;
  pthread_mutex_t *rfork;
  pthread_mutex_t *lfork;
  pthread_mutex_t *print_mutex;
  pthread_mutex_t *forks;
} t_mutex;

typedef struct s_ph
{
  // pthread_t  *thread;
  t_mutex mutex;
  t_data *data;
  int id;
  unsigned long last_meal;
} t_ph;
typedef struct s_housekeeped
{
  t_ph *ph;
  pthread_t *monitor;
  pthread_mutex_t *mutex_last_meal;
  pthread_mutex_t *stop;
  pthread_mutex_t *print_mutex;
  pthread_mutex_t *forks;
  pthread_t *thread;
} t_housekeeped;

unsigned long time_now_ms(void);
void init(t_data *data );
#endif 