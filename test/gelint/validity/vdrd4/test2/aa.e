class AA

creation

	make

feature

	make is
		local
			b: BB
		do
			!! b
			b.f
			print ("AA%N")
		end

end -- class AA
