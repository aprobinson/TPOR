PDFLATEX := pdflatex

all: overview_presentation.pdf

# Overview presentation
overview_presentation.pdf: 
	$(PDFLATEX) overview_presentation

clean:
	-rm -f *~
	-rm -f *.log
	-rm -f *.aux
	-rm -f *.nav
	-rm -f *.snm
	-rm -f *.toc
	-rm -f *.out
	-rm -f test.prof
	-rm -f overview_presentation.pdf