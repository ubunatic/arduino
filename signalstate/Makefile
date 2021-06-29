.PHONY: all clean install clean-install uninstall compile

ZIP = SignalState.zip
SRC = $(shell echo src library.* keywords.txt README.* LICENSE)

all: install compile

$(ZIP): $(SRC) Makefile
	# SRC=$(SRC)
	rm -f $@
	rm -rf SignalState
	mkdir -p SignalState
	cp -r $(SRC) SignalState/
	zip $@ --move -r SignalState

clean:
	rm -rf build *.zip

clean-install: clean uninstall install

# enable unsafe install and install the lib from the local zip file
install: $(ZIP)
	./install.sh $(ZIP)

uninstall:
	arduino-cli lib uninstall SignalState

compile:
	arduino-cli compile -b arduino:avr:uno examples/TwoSignals
	arduino-cli compile -b arduino:avr:uno examples/IRSignal
