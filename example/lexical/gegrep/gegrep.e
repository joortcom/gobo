indexing

	description:

		"Gobo Eiffel Grep"

	author:     "Eric Bezault <ericb@gobo.demon.co.uk>"
	copyright:  "Copyright (c) 1997, Eric Bezault"
	date:       "$Date$"
	revision:   "$Revision$"

class GEGREP

inherit

	ARGUMENTS
		export
			{NONE} all
		end

	KL_SHARED_EXCEPTIONS

	KL_SHARED_INPUT_STREAM_ROUTINES

	KL_SHARED_STANDARD_FILES

creation

	execute

feature -- Execution

	execute is
			-- Start 'gegrep' execution.
		local
			i, nb: INTEGER
			a_file: like INPUT_STREAM_TYPE
			a_filename: STRING
			case_insensitive: BOOLEAN
		do
			nb := argument_count
			if nb = 0 then
				std.error.put_string (Usage_message)
				std.error.put_character ('%N')
				exceptions_.die (1)
			else
				if argument (1).is_equal ("-i") then
					case_insensitive := True
					if nb = 1 then
						std.error.put_string (Usage_message)
						std.error.put_character ('%N')
						exceptions_.die (1)
					end	
					i := 2
				else
					i := 1
				end
				!! regexp.compile (argument (i), case_insensitive)
				if not regexp.compiled then
					std.error.put_string ("gegrep: invalid regular expression%N")
					exceptions_.die (1)
				else
					i := i + 1
					inspect nb - i + 1
					when 0 then
						parse_file (std.input, Void)
					when 1 then
						a_filename := argument (i)
						a_file := input_stream_.make_file_open_read (a_filename)
						if input_stream_.is_open_read (a_file) then
							parse_file (a_file, Void)
							input_stream_.close (a_file)
						else
							std.error.put_string ("gegrep: cannot read %'")
							std.error.put_string (a_filename)
							std.error.put_string ("%'%N")
							exceptions_.die (1)
						end
					else
						from until i > nb loop
							a_filename := argument (i)
							a_file := input_stream_.make_file_open_read (a_filename)
							if input_stream_.is_open_read (a_file) then
								parse_file (a_file, a_filename)
								input_stream_.close (a_file)
							else
								std.error.put_string ("gegrep: cannot read %'")
								std.error.put_string (a_filename)
								std.error.put_string ("%'%N")
								exceptions_.die (1)
							end
							i := i + 1
						end
					end
				end
			end
		rescue
			std.error.put_string ("gegrep: internal error%N")
			exceptions_.die (1)
		end

feature -- Parsing

	parse_file (a_file: like INPUT_STREAM_TYPE; a_filename: STRING) is
			-- Parse `a_file'.
		require
			a_file_not_void: a_file /= Void
			a_file_open_read: input_stream_.is_open_read (a_file)
			regexp_not_void: regexp /= Void
			regexp_compiled: regexp.compiled
		local
			a_line: STRING
		do
			from
				a_file.read_line
			until
				input_stream_.end_of_input (a_file)
			loop
				a_line := a_file.last_string
				if regexp.matches (a_line) then
					if a_filename /= Void then
						std.output.put_string (a_filename)
						std.output.put_string (": ")
					end
					std.output.put_string (a_line)
					std.output.put_character ('%N')
				end
				a_file.read_line
			end
		end

feature -- Access

	Usage_message: STRING is "usage: gegrep [-i] regexp [filename...]"

	regexp: LX_DFA_REGULAR_EXPRESSION
			-- Regular expression

end -- class GEGREP
