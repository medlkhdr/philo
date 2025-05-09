/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   activities.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlakhdar <mlakhdar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:29:09 by mlakhdar          #+#    #+#             */
/*   Updated: 2025/05/09 15:29:33 by mlakhdar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philos.h"



void	eat(t_ph *ph, int *meal_counter)
{
	if (ph->data->notme != -1)
	{
		if (*meal_counter == ph->data->notme)
			return ;
		(*meal_counter)++;
	}
	if (ph->id % 2 == 0)
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
	pthread_mutex_lock(ph->mutex.mutex_last_meal);
	ph->last_meal = time_now_ms();
	pthread_mutex_unlock(ph->mutex.mutex_last_meal);
	print_status(ph, EAT);
	msleep(ph->data->tte);
	if (ph->id % 2 == 0)
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

void	issleep(t_ph *ph)
{
	msleep(ph->data->tts);
	print_status(ph, SLEEP);
}

void	think(t_ph *ph)
{
	print_status(ph, THINK);
}
void	die(t_ph *ph)
{
	msleep(ph->data->ttd);
	print_status(ph, DEATH);
}
