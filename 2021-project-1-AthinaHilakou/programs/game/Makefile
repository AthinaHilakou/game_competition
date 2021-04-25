# paths
LIB = ../../lib
INCLUDE = ../../include


# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -Werror -g  -I$(INCLUDE)
LDFLAGS = -lm



# Αρχεία .o
#υλοποίηση state.c 
OBJS1 = game.o  state.o  interface.o  $(LIB)/k08.a $(LIB)/libraylib.a 
#υλοποίηση state_alt.c 
OBJS2 = game.o state_alt.o  interface.o set_utils.o $(LIB)/k08.a $(LIB)/libraylib.a

# Το εκτελέσιμο πρόγραμμα
EXEC1 = game
EXEC2 = game_alt

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS =

# Παράμετροι της βιβλιοθήκης raylib
include $(LIB)/libraylib.mk


$(EXEC1): $(OBJS1)
	$(CC) $(OBJS1) -o $(EXEC1) $(LDFLAGS)
	@if [ -f $(EXEC1).exe ]; then ln -sf $(EXEC1).exe $(EXEC1); fi

$(EXEC2): $(OBJS2)
	$(CC) $(OBJS2) -o $(EXEC2) $(LDFLAGS)
	@if [ -f $(EXEC2).exe ]; then ln -sf $(EXEC2).exe $(EXEC2); fi


# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a

clean:
	rm -f $(OBJS1) $(EXEC1)
	rm -f $(OBJS2) $(EXEC2)

run: $(EXEC1)
	./$(EXEC1) $(ARGS)

run_alt: $(EXEC2)
	./$(EXEC2) $(ARGS)


valgrind: $(EXEC1)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC1) $(ARGS)

valgrind_alt: $(EXEC2)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC2) $(ARGS)