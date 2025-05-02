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
    think(ph);
    ph->last_meal = time_now_ms();
    eat(ph, &meal_counter);
    issleep(ph);
  }
  print_status(ph, DEATH);
  return NULL;
}
void init(t_data *data)
{
  int size = data->nop ;
  t_ph *ph = malloc(sizeof(t_ph) * size );
  pthread_t *thread = malloc(sizeof(pthread_t) * size);
  for (int i = 0; i < size; i++)
  {
    pthread_create(&thread[i], NULL, &routine, (void *)&ph[i]);
  }
}