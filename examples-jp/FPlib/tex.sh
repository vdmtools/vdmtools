#!/bin/sh
platex --interaction batchmode $1.tex
dvipdfm $1.dvi