make:
	clear

.PHONY: printf
printf:
	./macas scan.as
	./macas	printf.as
	./macas	tprintf.as
	./maclk printf.mac tprintf.maco scan.maco printf.maco
	./macsim printf.mac

.PHONY: scan
scan:
	./macas scan.as
	./macas	print.as
	./macas	tscan.as
	./maclk scan.mac tscan.maco scan.maco print.maco
	./macsim scan.mac

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
	clear
