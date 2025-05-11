/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feedback <feedback@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:18:40 by feedback          #+#    #+#             */
/*   Updated: 2025/05/11 23:27:57 by feedback         ###   ########.fr       */
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

void	init_mutexes(pthread_mutex_t **print_mutex,
		pthread_mutex_t **mutex_last_meal, pthread_mutex_t **stop)
{
	*print_mutex = malloc(sizeof(pthread_mutex_t));
	*mutex_last_meal = malloc(sizeof(pthread_mutex_t));
	*stop = malloc(sizeof(pthread_mutex_t));
	if (!*print_mutex || !*mutex_last_meal || !*stop)
		return ;
	pthread_mutex_init(*print_mutex, NULL);
	pthread_mutex_init(*mutex_last_meal, NULL);
	pthread_mutex_init(*stop, NULL);
}

void	init_philosophers(t_help *help)
{
	int	size;
	int	i;

	size = help->data->nop;
	i = 0;
	while (i < size)
	{
		pthread_mutex_init(&help->forks[i], NULL);
		help->ph[i].mutex.rfork = &help->forks[i];
		help->ph[i].mutex.lfork = &help->forks[(i + 1) % size];
		help->ph[i].mutex.stop_mutex = help->stop;
		help->ph[i].mutex.print_mutex = help->print_mutex;
		help->ph[i].mutex.mutex_last_meal = help->mutex_last_meal;
		help->ph[i].data = help->data;
		help->ph[i].id = i + 1;
		help->ph[i].count = 0;
		help->ph[i++].last_meal = time_now_ms();
	}
}

void	init_used_data(t_ph *ph, t_data *data, pthread_t *thread,
		pthread_mutex_t *forks)
{
	pthread_mutex_t	*mutex_last_meal;
	pthread_mutex_t	*stop;
	pthread_mutex_t	*print_mutex;
	t_help			*help;

	init_mutexes(&print_mutex, &mutex_last_meal, &stop);
	if (!print_mutex || !mutex_last_meal || !stop)
		return ;
	data->stop = false;
	help = malloc(sizeof(t_help));
	if (!help)
		return ;
	help->ph = ph;
	help->data = data;
	help->forks = forks;
	help->print_mutex = print_mutex;
	help->mutex_last_meal = mutex_last_meal;
	help->stop = stop;
	init_philosophers(help);
	free(help);
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
	housekeeping(clean);
}
