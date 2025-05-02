#include "philos.h"

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
void *func(void *philo1)
{
  t_ph *philo = philo1;
  int count = 0;

  while (1)
  {
    pthread_mutex_lock(philo->mutex_last_meal);
    if((philo->data->notme != -1) && (count < philo->data->notme))
    {
      count++;
      pthread_mutex_unlock(philo->mutex_last_meal);
    }
    pthread_mutex_lock(&philo->data->mutex_stop);
    if (philo->data->stop)
    {
      pthread_mutex_unlock(&philo->data->mutex_stop);
      return NULL;
    }
    pthread_mutex_unlock(&philo->data->mutex_stop);
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
    pthread_mutex_lock(&philo->data->mutex_stop);
    if (philo->data->stop)
    {
      pthread_mutex_unlock(&philo->data->mutex_stop);
      pthread_mutex_unlock(philo->lfork);
      pthread_mutex_unlock(philo->rfork);
      return NULL;
    }
    pthread_mutex_unlock(&philo->data->mutex_stop);
    print_status(philo, EAT);
    pthread_mutex_lock(philo->mutex_last_meal);
    philo->last_meal = time_now_ms();
    pthread_mutex_unlock(philo->mutex_last_meal);
    usleep(philo->data->tte * 1000);
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

        pthread_mutex_lock(&philo[i].data->mutex_stop);
        if (!philo[i].data->stop)
        {
          philo[i].data->stop = true;
          print_status(&philo[i], DEATH);
        }
        pthread_mutex_unlock(&philo[i].data->mutex_stop);
        return NULL;
      }
      pthread_mutex_unlock(philo[i].mutex_last_meal);
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
    philo[i].lfork = &mutex[(i + 1) % data->nop];
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
}