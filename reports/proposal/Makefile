MAC = Darwin
OS = $(shell uname)
APP = proposal.pdf

all: $(APP)

$(APP): $(APP:.pdf=.tex)
	# Ignore errors because rubber outputs a lot of warnings.
ifneq ($(MAC),$(findstring $(MAC),$(OS)))
	-rubber --module xelatex $<
else
	xelatex $(basename $^)
	biber $(basename $^)
	xelatex $(basename $^)
	xelatex $(basename $^)
endif

PHONY: clean

clean:
ifneq ($(MAC),$(findstring $(MAC),$(OS)))
	rubber --clean --pdf $(basename $(APP))
else
	rm -f *.aux *.log *.pdf *.out
endif
