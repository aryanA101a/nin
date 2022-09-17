nin: src/main.c
	$(CC) src/main.c src/api/terminal.c src/api/row_operations.c src/api/editor_operations.c src/api/file_io.c src/api/find.c src/utils/append_buffer.c src/api/input.c src/api/output.c -o nin -Wall -Wextra -pedantic -std=c99
