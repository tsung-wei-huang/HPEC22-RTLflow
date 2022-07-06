FILE_NAME=hpec

read: pdf
	evince $(FILE_NAME).pdf &

pdf: ps
	ps2pdf $(FILE_NAME).ps

ps: dvi
	dvips -t letter -Pdownload35 -o $(FILE_NAME).ps $(FILE_NAME).dvi

dvi: 
	latex $(FILE_NAME).tex
	bibtex $(FILE_NAME)
	latex $(FILE_NAME).tex
	latex $(FILE_NAME).tex

clean: 
	rm -f $(FILE_NAME).ps
	rm -f $(FILE_NAME).pdf
	rm -f $(FILE_NAME).log
	rm -f $(FILE_NAME).aux
	rm -f $(FILE_NAME).out
	rm -f $(FILE_NAME).blg
	rm -f $(FILE_NAME).dvi
	rm -f $(FILE_NAME).bbl

backup: 
	tar -cvzf backup.tar.gz *

