/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitanig <keitanig@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/09 11:51:12 by keitanig          #+#    #+#             */
/*   Updated: 2022/04/09 12:25:30 by keitanig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void	error(char *str)
{
	if (str)
		free(str);
	printf("client: unexpected error.\n");
	exit(EXIT_FAILURE);
}

int	send_null(int pid, char *str)
{
	static int	i = 0;
	if (i++ != 8)
	{
		if (kill(pid, SIGUSR1) == -1)
			error(str);
		return (0);
	}
	return (1);
}

int	send_bit(int pid, char *str)
{
	static char	*message = 0;
	static int	s_pid = 0;
	static int	bits = -1;

	if (str)
		message = strdup(str);
	if (!message)
		error(0);
	if (pid)
		s_pid = pid;
	if (message[++bits / 8])
	{
		if (message[bits / 8] & (0x80 >> (bits % 8)))
		{
			if (kill(s_pid, SIGUSR2) == -1)
				error(message);
		}
		else if (kill(s_pid, SIGUSR1) == -1)
			error(message);
		return (0);
	}
	if (!send_null(s_pid, message))
		return (0);
	free(message);
	return (1);
}

void	handler_sigusr(int signum)
{
	int	is_success;

	is_success = 0;
	if (signum == SIGUSR1)
		is_success = send_bit(0, 0);
	else if (signum == SIGUSR2)
	{
		printf("client: server ended unexpectedly.\n");
		exit(EXIT_FAILURE);
	}
	if (is_success)
	{
		printf("client: operation succesful.\n");
		exit(EXIT_SUCCESS);
	}
}

int	main(int argc, char **argv)
{
	int	pid;

	if (argc != 3)
	{
		printf("client: invalid arguments\n");
		printf("\tcorrct format [./%s SERVER_PID MESSAGE]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	signal(SIGUSR1, handler_sigusr);
	signal(SIGUSR2, handler_sigusr);
	pid = atoi(argv[1]);
	send_bit(pid, argv[2]);
	while (1)
		pause();
	return (0);
}