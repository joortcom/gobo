
indexing

	description: 
	
		"Scanner adapter to external DTD"
	
	library: "Gobo Eiffel XML Library"
	copyright: "Copyright (c) 2002, Eric Bezault and others"
	license: "Eiffel Forum License v1 (see forum.txt)"
	date: "$Date$"
	revision: "$Revision$"
		
class XF_SCANNER_DTD

inherit

	XF_FULL_SCANNER
		redefine
			make_scanner,
			read_token
		end
		
creation

	make_scanner
	
feature {NONE} -- Creation

	make_scanner is
			-- Make dtd scanner.
		do
			Precursor
			set_start_condition (doctype)
		end

feature {NONE} -- State

	decl_start_sent: BOOLEAN
	decl_end_sent: BOOLEAN
	
feature -- Input

	read_token is
			-- Horrible hack that places the content of an external 
			-- DTD within a (DOCTYPE_DECL_START, DOCTYPE_DECL_END)
			-- token pair.
		do
			debug ("xml_parser")
				io.put_string ("--dtd-scanner--")
				if has_error then io.put_string (last_error) end
			end
			if not decl_start_sent then
				set_last_token (DOCTYPE_DECLARATION_START)
				decl_start_sent := True
			elseif decl_end_sent then
				terminate
			else
				Precursor
				if end_of_file then
					set_last_token (DOCTYPE_DECLARATION_END)
					decl_end_sent := True
				end
			end
		end
		
end
