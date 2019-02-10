.PHONY:
all: $(CODE_NAME)
	@echo "Compileing code..."
	$(COMPILER) $(FLAG) -o $(SCRIPT_NAME) $(CODE_NAME) $(INFO_NAME)
	@echo "Done."
	@echo ">> Do:"
	@echo "  $$ sudo make install"

