OPT=-finalize
DIR=F_code
DIR=W_code
ise:
	ec -freeze -c_compile -config src/system.ecf

clean_ise:
	ec -clean -config src/system.ecf


gobo:  # does not work: https://github.com/gobo-eiffel/gobo/issues/71
	gec --finalize --no-benchmark src/system.ecf

fmt=implicit_converts
fmt=field_rename

demo_ecf = ./test/tool/system.ecf
demo_ecf = src/system.ecf
demo:
	gedoc --format=$(fmt) $(demo_ecf) > inherited_fields.path

clean:
	$(RM) *.c *.o *.make *.h *.sh
