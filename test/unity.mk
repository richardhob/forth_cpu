
UNITY_SRC := unity.c unity_memory.c unity_fixture.c
UNITY_OBJ := $(UNITY_SRC:.c=.o)

vpath %.c unity

libunity.a: $(UNITY_OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: unity_clean
unity_clean:
	rm $(UNITY_OBJ)
	rm libunity.a
