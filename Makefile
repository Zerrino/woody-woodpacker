# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/06 09:46:53 by zerrino           #+#    #+#              #
#    Updated: 2025/04/06 18:22:50 by Zerrino          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = woody_woodpacker

CC = gcc
CFLAGS = -Wall -Wextra -Werror

NASM = nasm
NASMFLAGS = -f elf64

SRC_DIR = srcs
SRC_DIR_ASM = srcs_asm
OBJ_DIR = .objs
OBJ_DIR_ASM = .objs_asm
INC_DIR = includes
LIBFT_DIR = libft

MAIN_FILES = main.c\

SRC_FILES = utilitary.c\
			elf_header_parsing.c\
			ft_elf64.c\
			ft_elf32.c\
			utilitary_elf.c\
			elf32_symbol.c\
			elf64_symbol.c\
			utilitary1.c\
			utilitary2.c\
			elf64_change.c\

SRC_FILES_ASM = rotate.s


SRCS_MAIN = $(addprefix $(SRC_DIR)/, $(MAIN_FILES))


SRCS_ASM = $(addprefix $(SRC_DIR_ASM)/, $(SRC_FILES_ASM))


SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJS_ASM = $(SRCS_ASM:$(SRC_DIR_ASM)/%.s=$(OBJ_DIR_ASM)/%.o)

ROM = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

OBJS := $(ROM) $(OBJS_ASM)  $(SRCS_MAIN:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# export LC_COLLATE=C

all: $(NAME)

$(NAME): $(OBJS)
	@$(MAKE) -C $(LIBFT_DIR)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -L$(LIBFT_DIR) -lft

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR_ASM)/%.o: $(SRC_DIR_ASM)/%.s
	@mkdir -p $(OBJ_DIR_ASM)
	@$(NASM) $(NASMFLAGS) $< -o $@

clean:
	rm -f $(OBJS)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re bonus
