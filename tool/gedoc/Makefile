OPT=-finalize
DIR=F_code
DIR=W_code
ise:
	ec  -config src/system.ecf
	cd EIFGENs/gedoc/$(DIR)/ && finish_freezing

gobo:  # does not work: https://github.com/gobo-eiffel/gobo/issues/71
	gec --finalize --no-benchmark src/system.ecf

fmt=implicit_converts
fmt=field_rename
demo:
	gedoc --format=$(fmt) ./test/tool/system.ecf

