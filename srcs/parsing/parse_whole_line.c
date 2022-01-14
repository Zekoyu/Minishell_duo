/* ************************************************************************** */
/*                       .-.                       .                          */
/*                      / -'                      /                           */
/*       .  .-. .-.   -/--).--..-.  .  .-. .-.   /-.  .-._.)  (               */
/*        )/   )   )  /  /    (  |   )/   )   ) /   )(   )(    )         .    */
/*   By: '/   /   (`.'  /      `-'-.-/   /.- (.''--'`-`-'  `--':        /     */
/*                  -'            (   \  / .-._.).--..-._..  .-.  .-../ .-.   */
/*   Created: 13-01-2022  by       `-' \/ (   )/    (   )  )/   )(   / (  |   */
/*   Updated: 14-01-2022 15:05 by      /\  `-'/      `-'  '/   (  `-'-..`-'-' */
/*                                 `._;  `._;                   `-            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_cmd	**get_cmd_lst(void);

// #define PIPE_CHAR 1
// #define OR_CHAR 2;
// #define SINGLE_AMPERSTAND 4; // &&
// #define DOUBLE_AMPERSTAND 4; // &&
// #define AND_CHAR 5;
// #define SINGLE_LEFT_REDIRECT 6;
// #define DOUBLE_LEFT_REDIRECT 7;
// #define SINGLE_RIGHT_REDIRECT 8;
// #define DOUBLE_RIGHT_REDIRECT 9;

typedef struct s_cmd_layout
{
	int operator_chars[4096];
	int operators_nb;
	int	non_redirect_operators_nb;
} t_cmd_layout;

typedef enum e_splitable_char
{
	 PIPE_CHAR = 1, 			// |
	 SINGLE_RIGHT_REDIRECT,		// >
	 SINGLE_LEFT_REDIRECT, 		// <
	 OR_CHAR, 					// ||
	 AND_CHAR, 					// &&
	 DOUBLE_LEFT_REDIRECT, 		// <<
	 DOUBLE_RIGHT_REDIRECT 		// >>
}	t_splitable_char;

int is_splitable_char(char c)
{
	if ( c == '|' || c == '>' || c == '<' || c == '&')
		return (1);
	return (0);
}

static void	layout_pipe(t_cmd_layout *layout, char *line, int *index)
{
	if(line[(*index) + 1] == '|' && line[(*index) + 1])
	{
		layout->operator_chars[(*index)] = OR_CHAR;
		layout->operators_nb++;
		layout->non_redirect_operators_nb++;
		((*index))++;
	}
	else
	{
		layout->operator_chars[(*index)] = PIPE_CHAR;
		layout->operators_nb++;
		layout->non_redirect_operators_nb++;
	}
}

static void layout_right_redirect(t_cmd_layout *layout, char *line, int *index)
{
	if (line[(*index) + 1] == '>' && line[(*index) + 1])
	{
		layout->operator_chars[(*index)] = DOUBLE_RIGHT_REDIRECT;
		layout->operators_nb++;
		(*index)++;
	}
	else
	{
		layout->operator_chars[(*index)] = SINGLE_RIGHT_REDIRECT;
		layout->operators_nb++;
	}
}
static void layout_left_redirect(t_cmd_layout *layout, char *line, int *index)
{
	if(line[(*index) + 1] == '<' && line[(*index) + 1])
	{
		layout->operator_chars[(*index)] = DOUBLE_LEFT_REDIRECT;
		layout->operators_nb++;
		(*index)++;
	}
	else
	{
		layout->operator_chars[(*index)] = SINGLE_LEFT_REDIRECT;
		layout->operators_nb++;
	}
}
static void layout_and(t_cmd_layout *layout, char *line, int *index)
{
	if(line[(*index) + 1] == '&' && line[(*index) + 1])
	{
		layout->operator_chars[(*index)] = AND_CHAR;
		(*index)++;
		layout->operators_nb++;
		layout->non_redirect_operators_nb++;
	}
}

static void set_layout_char(t_cmd_layout *layout, char* line, int *index)
{
	if (line[(*index)] == '|')
		layout_pipe(layout,line,index);	
	else if (line[(*index)] == '>')
		layout_right_redirect(layout,line,index);	
	else if (line[(*index)] == '<')
		layout_left_redirect(layout,line,index);	
	else if (line[(*index)] == '&')
		layout_and(layout,line,index);
}

void create_cmd_layout(t_cmd_layout *layout, char *line)
{
	int i;

	layout->non_redirect_operators_nb = 0;
	layout->operators_nb = 0;
	ft_bzero(layout->operator_chars, 4096);
	i = 0;
	while(line[i])
	{
		if ((line[i] == '\'' || line[i] == '"') && is_closed_quote(&line[i]))
			i += is_closed_quote(&line[i]) + 1; // Goes after the second quote
		if (!line[i])
			break;
		if (is_splitable_char(line[i]))
		{
			if(is_splitable_char(line[i]) && line[i])
			{
				set_layout_char(layout, line, &i);
			}
		}
		i++;
	}
}

int	get_redirect_type_associated_with_redirect_char(int redirect_char)
{
	if (redirect_char == PIPE_CHAR)
		return (REDIRECT_PIPE);
	else if (redirect_char == SINGLE_RIGHT_REDIRECT)
		return (REDIRECT_OUTPUT_FILE);
	else if (redirect_char == DOUBLE_RIGHT_REDIRECT)
		return (REDIRECT_OUTPUT_FILE_APPEND);
	else if (redirect_char == SINGLE_LEFT_REDIRECT)
		return (REDIRECT_INPUT_FILE);
	else if (redirect_char == DOUBLE_LEFT_REDIRECT)
		return (REDIRECT_INPUT_FILE_DELIMITER);
	else
		return (REDIRECT_UNKNOWN);
}

int	is_valid_in_filename(char c)
{
	return (!(c == '>' || c == '<' || c == '|' || c == ':' || c == '&'));
}


//"echo >test.txt pouet pouet"

static int	is_quoted_arg(char *str)
{
	return ((str[0] == '\'' || str[0] == '\"') && is_closed_quote(str));
}
/*
	Args need to NOT be unquoted (">" should stay ">" and not become only >)
	So that we can make difference between literal string (">") 
		and redirect char (>)
	This takes args as input (eg. ["echo", "salut bg", ">", "test.txt"]) and :

	- Remove the redirection from args (new args = ["echo", "salut bg"])
	- Returns the file ("test.txt")
	args[0] = program_name, so skip it

	filename = pointer to first char of filename (string)
	filename_length is the length for the substr'

	i = args index (current arg)
	j = string index (current char)
	k = filename index (current char in filename)

*/
char	*get_last_redirection_filename(t_cmd *cmd, char **args)
{
	int		i;
	int		j;
	int		k;
	int		filename_length;
	int		has_another_redirect;

	char	*filename;
	char	*final_filename;
	char	*last_filename;

	t_cmd_layout	layout;

	i = 1;
	has_another_redirect = 0;
	j = 0;
	last_filename = NULL;
	final_filename = NULL;
	while (args[i])
	{
		filename = NULL;
		if (!is_quoted_arg(args[i])) // If we have a closed quote, we don't interpret the redirection
		{
			create_cmd_layout(&layout, args[i]);
			while (args[i][j] && layout.operator_chars[j] == 0 /*args[i][j] && !(args[i][j] == '<' || args[i][j] == '>')*/) // While we do not encounter redirect chars, continue
				j++;
			if (args[i][j]) // If a redirection is found
			{
				/*if (args[i][j + 1] == '>' || args[i][j + 1] == '<') // If we have a double redirection (<< or >>), length is 2 not 
					j += 2;*/
				int type = args[i][j];
				if (type == DOUBLE_LEFT_REDIRECT || type == DOUBLE_RIGHT_REDIRECT)
					j += 2;
				else
					j += 1; // Skip the '>' char
				while (args[i][j] && ft_isspace(args[i][j]))
					j++;
				if (args[i][j]) // We reached a character in the arg (It's either a ">file.txt or >  file.txt" case)
					filename = &(args[i][j]);
				else // Our arg was only composed of a ">", so file is in next arg;
				{
					if (!args[i + 1]) // If there is no next argument, we have an error
					{
						printf("Missing filename. TODO\n");
						return (NULL);
					}
					else
						filename = args[i + 1];
				}
				k = 0; // Now that we have our string filename, start from first char
				while (filename[k] && is_valid_in_filename(filename[k]))
					k++;
				final_filename = ft_substr(filename, 0, k);

				cmd->redirect_filename = final_filename;
				cmd->redirect_type = type;

				// SHOULD MOVE THIS TO ANOTHER FUNCTION
				/*
				Appended output redirection shall cause the file whose name results from the expansion of word to be opened for output on the designated file descriptor.
				 The file is opened as if the open() function as defined in the System Interfaces volume of POSIX.1-2008 was called with the O_APPEND flag. 
				 If the file does not exist, it shall be created.


				I ran this: strace -o spork.out bash -c "echo 1234 >> some-file" to figure out your question. This is what I found:

				open("some-file", O_WRONLY|O_CREAT|O_APPEND, 0666) = 3
				No file named "some-file" existed in the directory in which I ran the echo command.
				*/
			
				int fd = open (final_filename,  O_WRONLY | O_CREAT, 0666);
				close(fd);
				
				if (final_filename)
				{
					free(last_filename);
					last_filename = ft_strdup(final_filename);
				}
				final_filename = NULL;
				
				while (filename[k] && !(filename[k] == '>' || filename[k] == '<')) // Check if we have another redirection next
					k++;
				if (filename[k]) // Here we have another redirection next
				{
					// Stay at the same index in the same string and operate once more
					// Preserve the index j for the next iteration
					continue; // Since we continue there is no i++ so we run the loop again but with j != 0 this time (not starting from the beginning of the string)
				}
				else
					j = 0; // Go back at the beginning of the new string
				// If we reached here, we have our filename no matter what case we are in
			}
			else
				j = 0;
		}
		i++;
	}
	return (last_filename);
}

/*
	Takes our line (eg. echo "test" > test.txt | cat)
                                    ^          ^
				cmd layout =>  RIGHT_REDIR    PIPE

	Returns a list of commands (just separated on |, || and &&)
	(in this case "echo "test" > test.txt", "cat")
*/
static int	is_operator_but_not_redirection(char c)
{
	return (c == PIPE_CHAR || c == OR_CHAR || c == AND_CHAR);
}

static int	get_operator_str_len(char c)
{
	if (c == PIPE_CHAR || c == SINGLE_RIGHT_REDIRECT \
	|| c == SINGLE_LEFT_REDIRECT)
		return (1);
	else if (c == OR_CHAR || c == AND_CHAR || c == DOUBLE_LEFT_REDIRECT \
	|| c == DOUBLE_RIGHT_REDIRECT)
		return (2);
	return (0);
}

static int	get_next_non_redirect_operator(char *line, int index, t_cmd_layout layout)
{
	while (line[index] \
	&& !is_operator_but_not_redirection(layout.operator_chars[index]))
		index++;
	return (index);
}

/*
	Input = "echo test | cat > test.txt"

	Output = ["echo test", "cat > test.txt"]
*/
char	**split_command_operands(char *line)
{
	t_cmd_layout	layout;
	int				i;
	int				cmd_start;
	int				cmd_end;
	char			**programs_list;

	create_cmd_layout(&layout, line);
	i = 0;
	cmd_start = 0;
	cmd_end = get_next_non_redirect_operator(line, 0, layout);
	programs_list = malloc(sizeof(char *) * (layout.non_redirect_operators_nb + 2)); // If 1 program, our redirect nb = 0, so malloc 2 (1 prog + NULL at the end)
	while (layout.non_redirect_operators_nb-- >= 0)
	{
		programs_list[i++] = ft_substr(line, cmd_start, cmd_end - cmd_start);
		cmd_start = cmd_end + get_operator_str_len(layout.operator_chars[cmd_end]);
		if (line[cmd_end])
			cmd_end++; // Skip the current operator to go to the next cmd
		cmd_end = get_next_non_redirect_operator(line, cmd_end, layout);
	}
	programs_list[i] = NULL;
	return (programs_list);
}

/*
	Takes our line return a list of fully parsed programs
*/
t_cmd	*parse_cmds(char *line)
{
	t_cmd	*commands;
	char	**args;

	args = split_command_operands(line);
	interpret_all_args(&args);

	for (int i = 0; args[i]; i++)
	{
		printf("arg=\"%s\"\n", args[i]);
	}
	printf("\n");
	return (commands);
}

/*
	"echo "$USER" | cat > pouet.txt"

	
	["echo "$USER" (output fd = input of "cat"), "cat > pouet.txt"]
	["echo "mframbou"" | cat > pouet.txt"]
	["echo "mframbou"" (output fd = input of "cat"), "cat" (output fd = "pouet.txt")]
	[["echo", "mframbou" (output fd = input of "cat")], ["cat"]]

	Execute

	- Split programs
	- interpret environment
	- Remove redirections
	- Fully parse program
	- Execute
*/
