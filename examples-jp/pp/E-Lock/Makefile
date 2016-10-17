PDFLATEX=dotexshop
#PDFLATEX=altjpdflatex
#PDFLATEX=pdfelatex
#PDFLATEX=pdflatex
BIBTEX=jbibtex

%.pdf: %.tex sahara.bib
	rm -f $*.aux $*.out $*.toc
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

report.pdf: report.tex

sahara.bib: sahara.bib
	ln -s $< $@

clean: 
	rm report.pdf
