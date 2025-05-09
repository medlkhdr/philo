/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlakhdar <mlakhdar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:18:40 by feedback          #+#    #+#             */
/*   Updated: 2025/05/09 15:44:29 by mlakhdar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philos.h"

void	creater_joiner(t_ph *ph, pthread_t *thread)
{
	int			size;
	t_data		*data;
	pthread_t	monitor;
	int			i;

	i = 0;
	data = ph[0].data;
	size = ph[0].data->nop;
	while (i < size)
	{
		pthread_create(&thread[i], NULL, &routine, (void *)&ph[i]);
		i++;
	}
	if (data->nop != 1)
		pthread_create(&monitor, NULL, &monitor_routine, (void *)ph);
	i = 0;
	while (i < size)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
	if (size != 1)
		pthread_join(monitor, NULL);
}
void	init_used_data(t_ph *ph, t_data *data, pthread_t *thread,
		pthread_mutex_t *forks)
{
	int				size;
	int				i;
	pthread_mutex_t	*mutex_last_meal;
	pthread_mutex_t	*stop;
	pthread_mutex_t	*print_mutex;

	print_mutex = malloc(sizeof(pthread_mutex_t));
	mutex_last_meal = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(print_mutex, NULL);
	stop = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(stop, NULL);
	pthread_mutex_init(mutex_last_meal, NULL);
	data->stop = false;
	size = data->nop;
	i = 0;
	while (i < size)
	{
		pthread_mutex_init(&forks[i], NULL);
		ph[i].mutex.rfork = &forks[i];
		ph[i].mutex.lfork = &forks[(i + 1) % size];
		ph[i].mutex.stop_mutex = stop;
		ph[i].mutex.print_mutex = print_mutex;
		ph[i].mutex.mutex_last_meal = mutex_last_meal;
		ph[i].data = data;
		ph[i].id = i + 1;
		ph[i].count = 0;
		ph[i].last_meal = time_now_ms();
		i++;
	}
}

t_housekeeped	cleaner_init(t_ph *ph, pthread_t *thread,
		pthread_mutex_t *forks)
{
	static t_housekeeped	clean;

	clean.forks = forks;
	clean.mutex_last_meal = ph[0].mutex.mutex_last_meal;
	clean.ph = ph;
	clean.print_mutex = ph[0].mutex.print_mutex;
	clean.stop = ph[0].mutex.stop_mutex;
	clean.thread = thread;
	return (clean);
}
void	ph_dining_solution(t_data *data)
{
	int				size;
	t_housekeeped	clean;
	pthread_mutex_t	*forks;
	pthread_t		*thread;
	t_ph			*ph;

	size = data->nop;
	forks = malloc(sizeof(pthread_mutex_t) * size);
	thread = malloc(sizeof(pthread_t) * size);
	ph = malloc(sizeof(t_ph) * size);
	init_used_data(ph, data, thread, forks);
	clean = cleaner_init(ph, thread, forks);
	data->start_time = time_now_ms();
	creater_joiner(ph, thread);
	Housekeeping(clean);
}
