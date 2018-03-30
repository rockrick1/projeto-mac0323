make:
	clear

.PHONY: print
print:
	./macas print.as
	./macas	tprint.as
	./maclk print.mac tprint.maco print.maco
	./macsim print.mac

.PHONY: clean
clean:
	rm -rf *.mac
	rm -rf *.maco
