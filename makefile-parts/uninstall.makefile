.PHONY:
uninstall: $(PREFIX)/$(SCRIPT_NAME)
	@echo "Uninstalling: $(SCRIPT_NAME), from: $(PREFIX)..."
	rm -f $(PREFIX)/$(SCRIPT_NAME)
