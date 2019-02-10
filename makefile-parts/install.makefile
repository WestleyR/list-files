.PHONY:
install: $(SCRIPT_NAME)
	@echo "installing to: $(PREFIX)"
	chmod +x $(SCRIPT_NAME)
	cp -f $(SCRIPT_NAME) $(PREFIX)

