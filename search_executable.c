/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_executable.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mframbou <mframbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 13:58:30 by mframbou          #+#    #+#             */
/*   Updated: 2022/01/07 18:01:14 by mframbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <sys/wait.h>

/*
	Max path length is PATH_MAX bytes
	https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap03.html#tag_03_271
*/

// /bin:/oronda::/bin
// [/bin, /oronda, /bin]

int	is_builtin(char *program)
{
	static char	*envs[] = {"export", "unset", "env", "cd", "pwd", "exit", "echo", NULL};
	int			i;

	i = 0;
	while (envs[i])
		if (strcmp(envs[i++], program) == 0)
			return (1);
	return (0);
}

/*
	If successful return the path, else return NULL
*/
char	*test_all_paths(char *path_variable, char *program)
{
	char	**paths;
	char	full_path[PATH_MAX];
	int		i;

	paths = ft_split(path_variable, ':');
	i = 0;
	free_ft_split(paths);
	while (paths[i])
	{
		ft_bzero(full_path, PATH_MAX);
		if (!is_line_empty(paths[i]))
		{
			ft_strlcpy(full_path, paths[i], PATH_MAX);
			ft_strlcat(full_path, "/", PATH_MAX);
		}
		ft_strlcat(full_path, program, PATH_MAX);
		res = access(full_path, F_OK | X_OK);
		if (res == 0)
		{
			free_ft_split(paths);
			return (ft_strdup(full_path));
		}
		i++;
	}
	return (NULL);
}

/*
	If successful return the path, else return NULL
*/
char	*is_program_in_path(char *program)
{
	char	*path;
	int		res;

	path = get_env_variable("PATH");
	if (!path)
	{
		add_env_variable("PATH", ft_strdup(PATH_STR));
		path = get_env_variable("PATH");
	}
	return (test_all_paths(path, program));
}
