OPT=-finalize
DIR=F_code
DIR=W_code
ise:
	ec -freeze -c_compile -config src/system.ecf

gobo:  # does not work: https://github.com/gobo-eiffel/gobo/issues/71
	gec --finalize --no-benchmark src/system.ecf

fmt=implicit_converts
fmt=field_rename
demo:
	gedoc --format=$(fmt) ./test/tool/system.ecf

clean:
	$(RM) *.c *.o *.make *.h *.sh
