ftp: ftp.o

.PHONY: run
run: ftp
	@./ftp

.PHONY: format
format:
	clang-format -i *.c *.h

.PHONY: clean
clean:
	$(RM) ftp *.o
