#include "philos.h"

unsigned long time_now_ms()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
void ft_msleep(unsigned long msec)
{
  size_t start;
  size_t target;

  start = time_now_ms();
  target = start + msec;
  while (time_now_ms() < target)
    {
      usleep(100);
    }
}

void print_status(t_ph *ph, char *status)
{
  unsigned long time = time_now_ms() - ph->data->start_time;
  pthread_mutex_lock(ph->mutex.stop_mutex);
  if (!ph->data->stop)
  {
    pthread_mutex_lock(ph->mutex.print_mutex);
    printf("%lu %d %s\n", time, ph->id, status);
    pthread_mutex_unlock(ph->mutex.print_mutex);
  }
  pthread_mutex_unlock(ph->mutex.stop_mutex);
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
    pthread_mutex_lock(ph->mutex.lfork);
    pthread_mutex_lock(ph->mutex.rfork);
  }
  else
  {
    pthread_mutex_lock(ph->mutex.rfork);
    pthread_mutex_lock(ph->mutex.lfork);
  }
  print_status(ph, FORK);
  print_status(ph, FORK);
  print_status(ph, EAT);
  pthread_mutex_lock(ph->mutex.mutex_last_meal);
  ph->last_meal = time_now_ms();
  pthread_mutex_unlock(ph->mutex.mutex_last_meal);
  ft_msleep(ph->data->tte);
  if(ph->id % 2 == 0)
  {
    pthread_mutex_unlock(ph->mutex.rfork);
    pthread_mutex_unlock(ph->mutex.lfork);
  }
  else
  {
    pthread_mutex_unlock(ph->mutex.lfork);
    pthread_mutex_unlock(ph->mutex.rfork);
  }
}


void issleep(t_ph *ph)
{
  print_status(ph, SLEEP);
  ft_msleep(ph->data->tts);
}

void think(t_ph *ph)
{
  print_status(ph, THINK);
}
void die(t_ph *ph)
{
  ft_msleep(ph->data->ttd);
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
    pthread_mutex_lock(ph->mutex.stop_mutex);
    if (ph->data->stop)
    {
      pthread_mutex_unlock(ph->mutex.stop_mutex);
      return NULL;
    }
    pthread_mutex_unlock(ph->mutex.stop_mutex);
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
      if(ph[i].data->stop)
        return NULL;
      pthread_mutex_lock(ph[i].mutex.mutex_last_meal);
      if (ph[i].last_meal != 0 && time_now_ms() - ph[i].last_meal > ph[i].data->ttd)
      {
        pthread_mutex_unlock(ph[i].mutex.mutex_last_meal);
        die(&ph[i]);
        pthread_mutex_lock(ph[i].mutex.stop_mutex);
        ph[i].data->stop = true;
        pthread_mutex_unlock(ph[i].mutex.stop_mutex);
        break;
      }
      pthread_mutex_unlock(ph[i].mutex.mutex_last_meal);
    }
  }
  return NULL;
}

void Housekeeping(t_housekeeped clean)
{
  int size;

  size = clean.ph[0].data->nop;

  for (int i = 0; i < size; i++)
  {
    pthread_mutex_destroy(&clean.forks[i]);
  }
  pthread_mutex_destroy(clean.mutex_last_meal);
  pthread_mutex_destroy(clean.stop);
  free(clean.ph);
  free(clean.thread);
  free(clean.mutex_last_meal);
  free(clean.forks);
}

void creater_joiner(pthread_t *thread,  t_ph *ph)
{
  int size;
  t_data *data;
  pthread_t  monitor; 
  data = ph[0].data;

  size = ph[0].data->nop;
  for (int i = 0; i < size; i++)
  {
    pthread_create(&thread[i], NULL, &routine, (void *)&ph[i]);
  }
  if(data->nop != 1)
    pthread_create(&monitor, NULL, &monitor_routine, (void *)ph);
for (int i = 0; i < size; i++)
{  pthread_join(thread[i], NULL);}
 if(size != 1)
  pthread_join(monitor, NULL);
}
void init(t_data *data)
{
  t_housekeeped clean;
  int size = data->nop;
  // pthread_t monitor;
  pthread_mutex_t *mutex_last_meal;
  mutex_last_meal = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_t *stop;
  pthread_mutex_t *print_mutex;
  print_mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(print_mutex , NULL);
  t_ph *ph = malloc(sizeof(t_ph) * size);
  pthread_t *thread = malloc(sizeof(pthread_t) * size);
  pthread_mutex_t *forks;
  forks = malloc(sizeof(pthread_mutex_t) * size);
  pthread_mutex_init(mutex_last_meal, NULL);
  stop = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(stop, NULL);
  data->stop = false;
  data->start_time = time_now_ms();
  clean.forks = forks;
  clean.mutex_last_meal = mutex_last_meal;
  clean.ph = ph;
  clean.print_mutex = print_mutex;
  clean.stop = stop;
  clean.thread = thread; 

  for (int i = 0; i < size; i++)
  {
    pthread_mutex_init(&forks[i], NULL);
    ph[i].mutex.rfork = &forks[i];
    ph[i].mutex.stop_mutex = stop;
    ph[i].mutex.lfork = &forks[(i + 1) % size];
    ph[i].id_thread = &thread[i];
    ph[i].mutex.print_mutex = print_mutex;
    ph[i].mutex.mutex_last_meal = mutex_last_meal;
    ph[i].data = data;
    ph[i].id = i + 1;
    ph[i].last_meal = 0;
  }
  creater_joiner(thread, ph);
  Housekeeping(clean);
}

