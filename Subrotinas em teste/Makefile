make:
	clear

.PHONY: main
main:
	./macas tmain.as
	./macas print.as
	./maclk main.mac tmain.maco print.maco
	./macsim main.mac 3

.PHONY: scanf
scanf:
	./macas scan.as
	./macas scanf.as
	./macas	printf.as
	./macas	tscanf.as
	./maclk scanf.mac tscanf.maco scan.maco scanf.maco printf.maco
	./macsim scanf.mac

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
