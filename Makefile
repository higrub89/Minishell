NAME				= minishell

LIBFT				= ./libft/libft.a
INC					= -Iinc/
SRC_DIR				= srcs/
OBJ_DIR				= obj/

CC					= cc
CFLAGS				= -Wall -Werror -Wextra -g -I$(INC)
LDFLAGS				= -lreadline
RM					= rm -f

COMMANDS_DIR		= 	srcs/main/minishell.c \
							srcs/main/minishell_utils.c \
							srcs/main/main_utils.c \
							srcs/main/singleton.c \
							srcs/lexer/lexer.c \
							srcs/lexer/token_utils.c \
							srcs/lexer/words_handler.c \
							srcs/lexer/redirection_handler.c \
							srcs/parser/parser.c \
							srcs/parser/parser_utils.c \
							srcs/parser/parser_cleanup.c \
							srcs/redirections/redirection.c \
							srcs/redirections/heredoc_utils.c \
							srcs/expander/expander.c \
							srcs/expander/expander_dollar.c \
							srcs/expander/expander_utils.c \
							srcs/expander/string_builder.c \
							srcs/executor/executor.c \
							srcs/executor/process_utils.c \
							srcs/executor/builtin_handler.c \
							srcs/executor/cmd_utils_one.c \
							srcs/executor/cmd_utils_two.c \
							srcs/executor/process_heredoc.c \
							srcs/executor/pipeline_utils.c \
							srcs/executor/redirection_executor.c \
							srcs/executor/redirection_executor_utils.c \
							srcs/executor/executor_utils.c \
							srcs/executor/redirection_executor_handler.c \
							srcs/builtins/cd/builtin_cd.c \
							srcs/builtins/cd/cd_special.c \
							srcs/builtins/cd/cd_utils.c \
							srcs/builtins/echo/builtin_echo.c \
							srcs/builtins/env/builtin_env.c \
							srcs/builtins/env/env_utils.c \
							srcs/builtins/env/array_copy.c \
							srcs/builtins/exit/builtin_exit.c \
							srcs/builtins/exit/exit_utils.c \
							srcs/builtins/export/builtin_export.c \
							srcs/builtins/export/export_display.c \
							srcs/builtins/export/export_envp.c \
							srcs/builtins/export/export_list.c \
							srcs/builtins/export/export_utils.c \
							srcs/builtins/pwd/builtin_pwd.c \
							srcs/builtins/unset/builtin_unset.c \
							srcs/signals/signals.c 

SRCS 				= $(COMMANDS_DIR) 

OBJ 				= $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)%.o,$(SRCS))

start:				
					@make -s all

$(LIBFT):
					@make -sC ./libft

all:				$(NAME)

$(NAME):			$(OBJ) $(LIBFT)
					@$(CC) $(CFLAGS) $(INC) $(OBJ) $(LIBFT) $(LDFLAGS) -o $(NAME) 2>&1 

$(OBJ_DIR)%.o:		$(SRC_DIR)%.c 
					@mkdir -p $(@D)
					@echo "compiling $<"
					@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 2>&1 

clean:
					@$(RM) -r $(OBJ_DIR)
					@make clean -sC ./libft
					@echo "Delete files objets" 

fclean:			clean
					@$(RM) $(NAME)
					@$(RM) $(LIBFT)
					@echo "Delete all files" 

re:				fclean all
					@echo "Finsh Recompiling" 

.PHONY:			start all clean fclean re