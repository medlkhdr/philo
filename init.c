#include "philos.h"

void print_status(t_ph *ph, char *status)
{
  unsigned long time = time_now_ms() - ph->data->start_time;
  printf("%lu %d %s\n", time, ph->id, status);
}
void eat(t_ph *ph, int *meal_counter)
{
  if(ph->data->notme != -1)
  {
    if(*meal_counter == ph->data->notme)
      return;
    (*meal_counter)++;
  }
  if(ph->id % 2 == 0)
  {
    pthread_mutex_lock(ph->lfork);
    pthread_mutex_lock(ph->rfork);
  }
  else
  {
    pthread_mutex_lock(ph->rfork);
    pthread_mutex_lock(ph->lfork);
  }
  print_status(ph, FORK);
  print_status(ph, FORK);
  print_status(ph, EAT);
  ph->last_meal = time_now_ms();
  usleep(ph->data->tte * 1000);
  pthread_mutex_unlock(ph->rfork);
  pthread_mutex_unlock(ph->lfork);
}
unsigned long time_now_ms()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
void issleep(t_ph *ph)
{
  print_status(ph, SLEEP);
  usleep(ph->data->tts * 1000);
}
void think(t_ph *ph)
{
  print_status(ph, THINK);
}
void *routine(void *arg)
{
  t_ph *ph = (t_ph *)arg;
  int meal_counter = 0;

  while (1)
  {
    if (ph->data->notme != -1 && meal_counter == ph->data->notme)
      break;
    if(ph->stop)
      return NULL;
    eat(ph, &meal_counter);
    think(ph);
    issleep(ph);
  }
  return NULL;
}
void *monitor_routine(void *arg)
{
  t_ph *ph = (t_ph *)arg;
  int size = ph->data->nop;
  while (1)
  {
    for (int i = 0; i < size; i++)
    {
      if(ph[i].stop)
        return NULL;
      if (ph[i].last_meal != 0 && time_now_ms() - ph[i].last_meal >= ph[i].data->ttd)
      {
        print_status(&ph[i], DEATH);
        for (int j = 0; j < size; j++)
          ph[j].stop = true;
        break;
      }
    }
  }
  return NULL;
}
void init(t_data *data)
{
  int size = data->nop ;
  pthread_t monitor;
  t_ph *ph = malloc(sizeof(t_ph) * size);
  pthread_t *thread = malloc(sizeof(pthread_t) * size);
  pthread_mutex_t *forks;
  forks = malloc(sizeof(pthread_mutex_t) * size);
  for (int i = 0; i < size; i++)
  {
    pthread_mutex_init(&forks[i], NULL);
    ph[i].rfork = &forks[i];
    ph[i].lfork = &forks[(i + 1) % size];
  }
  for (int i = 0; i < size; i++)
  {
    ph[i].data = data;
    ph[i].id = i + 1;
    ph[i].last_meal = 0;
    ph[i].stop = false;
  }
  for (int i = 0; i < size;i++)
    pthread_create(&thread[i], NULL, &routine, (void *)&ph[i]);
  pthread_create(&monitor, NULL, &monitor_routine, (void *)ph);
  for (int i = 0 ; i < size ;  i++)
      pthread_mutex_destroy(&forks[i]);
    for (int i = 0; i < size; i++)
      pthread_join(thread[i], NULL);
    pthread_join(monitor, NULL);
    free(ph);
    free(thread);
    free(forks);
}