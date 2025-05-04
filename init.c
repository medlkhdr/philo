#include "philos.h"
void ft_usleep(unsigned long usec)
{
  unsigned long loops = usec / 100;
  for (unsigned long i = 0; i < loops; i++)
    usleep(100);
}


void print_status(t_ph *ph, char *status)
{
  unsigned long time = time_now_ms() - ph->data->start_time;
  // mujtex lock
  if (!ph->data->stop)
    printf("%lu %d %s\n", time, ph->id, status);
  // mujtex unlock
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
  pthread_mutex_lock(ph->mutex_last_meal);
  ph->last_meal = time_now_ms();
  pthread_mutex_unlock(ph->mutex_last_meal);
  ft_usleep(ph->data->tte * 1000);
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
  ft_usleep(ph->data->tts * 1000);
}

void think(t_ph *ph)
{
  print_status(ph, THINK);
}
void die(t_ph *ph)
{
  ft_usleep(ph->data->ttd * 1000);
  print_status(ph, DEATH);
}
void *routine(void *arg)
{
  t_ph *ph = (t_ph *)arg;
  int meal_counter = 0;

  while (1)
  {
    if(ph->data->nop == 1)
    {
      print_status(ph, FORK);
      die(ph);
      return NULL;
    }
    if(ph->data->stop)
        return NULL;
    if (ph->data->notme != -1 && meal_counter == ph->data->notme)
    {
      ph->data->stop = true;
      return NULL;
    }
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
      // if(ph[i].stop)
      //   return NULL;
      pthread_mutex_lock(ph[i].mutex_last_meal);
      if (ph[i].last_meal != 0 && time_now_ms() - ph[i].last_meal > ph[i].data->ttd)
      {
        pthread_mutex_unlock(ph[i].mutex_last_meal);
        die(&ph[i]);
        pthread_mutex_lock(ph[i].stop_mutex);
        ph[i].data->stop = true;
        pthread_mutex_unlock(ph[i].stop_mutex);
        break;
      }
      pthread_mutex_unlock(ph[i].mutex_last_meal);
    }
  }
  return NULL;
}


void init(t_data *data)
{
  int size = data->nop ;
  pthread_t monitor;
  pthread_mutex_t *mutex_last_meal;
  // pthread_mutex_t *stop
  mutex_last_meal = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_t *stop;

  t_ph *ph = malloc(sizeof(t_ph) * size);
  pthread_t *thread = malloc(sizeof(pthread_t) * size);
  pthread_mutex_t *forks;
  forks = malloc(sizeof(pthread_mutex_t) * size);
  pthread_mutex_init(mutex_last_meal, NULL);
  stop = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(stop, NULL);
  data->stop = false;
  data->start_time = time_now_ms();
  for (int i = 0; i < size; i++)
  {
    pthread_mutex_init(&forks[i], NULL);
    ph[i].rfork = &forks[i];
    ph[i].stop_mutex = stop;
    ph[i].lfork = &forks[(i + 1) % size];
    ph[i].mutex_last_meal = mutex_last_meal;
  }
  for (int i = 0; i < size; i++)
  {
    ph[i].data = data;
    ph[i].id = i + 1;
    ph[i].last_meal = 0;
    // ph[i].stop = false;
  }
  for (int i = 0; i < size;i++)
    pthread_create(&thread[i], NULL, &routine, (void *)&ph[i]);
  if(data->nop != 1)
    pthread_create(&monitor, NULL, &monitor_routine, (void *)ph);
  for (int i = 0 ; i < size ;  i++)
      pthread_mutex_destroy(&forks[i]);
  for (int i = 0; i < size; i++)
      pthread_join(thread[i], NULL);
  if(data->nop != 1)
    pthread_join(monitor, NULL);
  pthread_mutex_destroy(mutex_last_meal);
  free(ph);
  free(thread);
  free(mutex_last_meal);
  free(forks);
}