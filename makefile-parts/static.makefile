.PHONY:
static: $(CODE_NAME)
	@echo "Compileing code..."
	$(COMPILER) $(FLAG) $(STATIC_BUILD_FLAG) -o $(SCRIPT_NAME) $(CODE_NAME) $(INFO_NAME)
	@echo "Done."
	@echo ">> Do:"
	@echo "  $$ sudo make install"

