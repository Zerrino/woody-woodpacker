# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/23 16:07:48 by alexafer          #+#    #+#              #
#    Updated: 2025/02/01 07:40:32 by Zerrino          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror
SRC_DIR = .
INC_DIR = .
SRCS =  ft_striteri.c ft_strmapi.c ft_itoa.c ft_split.c \
		ft_strjoin.c ft_strtrim.c ft_substr.c ft_files.c  \
		ft_calloc.c ft_mem.c ft_mem2.c ft_strdup.c ft_atoi.c \
		ft_isfind.c  ft_strlcpy.c ft_strncmp.c ft_uplow.c \
		ft_chr.c     ft_strlcat.c ft_strlen.c  ft_strnstr.c

BONUS = ft_list_bonus.c ft_list2_bonus.c
OBJS = $(addprefix $(SRC_DIR)/, $(SRCS:.c=.o))
BONUS_OBJ = $(addprefix $(SRC_DIR)/, $(BONUS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS) $(BONUS_OBJ)
	ar -rcs $@ $^

bonus: $(BONUS_OBJ)
	ar -rcs $(NAME) $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I./$(INC_DIR) -c $< -o $@

clean:
	rm -f $(OBJS) $(BONUS_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus

