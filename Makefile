NAME				= minishell

# Directorios de includes y fuentes.
LIBFT				= ./libft/libft.a
INC					= -Iinc/
SRC_DIR				= srcs/
OBJ_DIR				= obj/

# Compiler and CFlags
CC					= cc
CFLAGS				= -Wall -Werror -Wextra -g -I$(INC)
LDFLAGS				= -lreadline
RM					= rm -f

# Source Files
COMMANDS_DIR		=	srcs/minishell.c \
									srcs/lexer.c \
									srcs/parser.c \
									srcs/redirection.c \
									srcs/expander.c \
									srcs/executor.c \
									srcs/main_utils.c \
									srcs/env_utils.c \
									srcs/builtin_unset.c \
									srcs/builtin_pwd.c \
									srcs/builtin_export.c \
									srcs/builtin_exit.c \
									srcs/builtin_env.c \
									srcs/builtin_echo.c \
									srcs/builtin_cd.c 
									

# Concatenate all source files
SRCS 				= $(COMMANDS_DIR) 

# Apply the pattern substitution to each source file in SRC and produce a corresponding list of object files in the OBJ_DIR
OBJ 				= $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)%.o,$(SRCS))

# Build rules
start:				
					@make -s all

$(LIBFT):
					@make -sC ./libft

all: 				$(NAME)

$(NAME): 			$(OBJ) $(LIBFT)
					@$(CC) $(CFLAGS) $(INC) $(OBJ) $(LIBFT) $(LDFLAGS) -o $(NAME) 2>&1 

# Compile object files from source files
$(OBJ_DIR)%.o:		$(SRC_DIR)%.c 
					@mkdir -p $(@D)
					@echo "compiling $<" 
					@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 2>&1 

clean:
					@$(RM) -r $(OBJ_DIR)
					@make clean -sC ./libft
					@echo "Delete files objets" 

fclean: 			clean
					@$(RM) $(NAME)
					@$(RM) $(LIBFT)
					@echo "Delete all files" 

re: 				fclean all
					@echo "Finsh Recompiling" 
# Phony targets represent actions not files
.PHONY: 			start all clean fclean re
