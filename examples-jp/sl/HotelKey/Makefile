# TeX形式の清書用PDFファイルを生成するMakefile
# 「注記」
# 　　bibliographyに関するエラーメッセージが出るが、returnキーを押して続行してよい。

#PDFLATEX=altjpdflatex
#PDFLATEX=pdfelatex
#PDFLATEX=pdflatex
PDFLATEX=dotexshop
BIBTEX=jbibtex

%.pdf: %.tex
	rm -f $*.aux $*.out $*.toc
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

HotelKey.pdf: HotelKey.tex

clean:
	rm -rf *.aux *.log *.bbl *.blg *.dvi *~ *.toc *.idx
	rm -rf *.bak vdmprofile *.vdm.tex *.ps

realclean: clean
	rm -rf HotelKey.pdf