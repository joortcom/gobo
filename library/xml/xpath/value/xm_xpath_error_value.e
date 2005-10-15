indexing

	description:

		"XPath error values"

	library: "Gobo Eiffel XPath Library"
	copyright: "Copyright (c) 2004, Colin Adams and others"
	license: "Eiffel Forum License v2 (see forum.txt)"
	date: "$Date$"
	revision: "$Revision$"

class XM_XPATH_ERROR_VALUE

inherit

	XM_XPATH_LOCATOR
	
	XM_XPATH_STANDARD_NAMESPACES
		export {NONE} all end

	XM_XPATH_ERROR_TYPES
		export {NONE} all end

	KL_IMPORTED_STRING_ROUTINES
		export {NONE} all end

create

	make, make_empty, make_from_string, make_unknown

feature {NONE} -- Initialization

	make (a_description, a_namespace_uri, an_error_code: STRING; a_value: like value; an_error_type: like type) is
			-- Create a general error value.
		require
			error_value_not_void: a_value /= Void
			namespace_uri_not_void: a_namespace_uri /= Void
			local_part_not_void: an_error_code /= Void
			description_not_void: a_description /= Void
			valid_error_type: an_error_type = Static_error or an_error_type = Type_error or an_error_type = Dynamic_error
		do
			value := a_value
			namespace_uri := a_namespace_uri
			code := an_error_code
			type := an_error_type
			description := a_description
			initialize_location
		ensure
			description_set: description = a_description
			value_set: value = a_value
			namespace_set: namespace_uri = a_namespace_uri
			code_set: code = an_error_code
			type_set: type = an_error_type
		end

	make_empty (a_namespace_uri, an_error_code: STRING an_error_type: INTEGER) is
			-- Create an empty-sequence error value
		require
			valid_error_type: an_error_type = Static_error or an_error_type = Type_error or an_error_type = Dynamic_error
			namespace_uri_not_void: a_namespace_uri /= Void
			local_part_not_void: an_error_code /= Void
		local
			an_empty_sequence: XM_XPATH_EMPTY_SEQUENCE
		do
			create an_empty_sequence.make
			make ("()", a_namespace_uri, an_error_code, an_empty_sequence, an_error_type)
		ensure
			namespace_set: namespace_uri = a_namespace_uri
			code_set: code = an_error_code
			type_set: type = an_error_type
		end

	make_from_string (a_description, a_namespace_uri, an_error_code: STRING; an_error_type: like type) is
			-- Create from `a_string'.
		require
			namespace_uri_not_void: a_namespace_uri /= Void
			local_part_not_void: an_error_code /= Void
			description_not_void: a_description /= Void
			valid_error_type: an_error_type = Static_error or an_error_type = Type_error or an_error_type = Dynamic_error
		local
			a_string_value: XM_XPATH_STRING_VALUE
		do
			create a_string_value.make (a_description)
			make (a_description, a_namespace_uri, an_error_code, a_string_value, an_error_type)
		ensure
			description_set: description = a_description
			namespace_set: namespace_uri = a_namespace_uri
			code_set: code = an_error_code
			type_set: type = an_error_type
		end

	make_unknown is
			-- Create an unknown error
		local
			an_empty_sequence: XM_XPATH_EMPTY_SEQUENCE
		do
			create an_empty_sequence.make
			make ("Unknown error", Xpath_errors_uri, "FOER0000", an_empty_sequence, Dynamic_error)
		end

	initialize_location is
			-- Initialize location to unknown.
		do
			system_id := ""
			line_number := 0
		end

feature -- Access

	namespace_uri, code: STRING
			-- Error code (as an expanded URI)

	description: STRING
			-- Description

	type: INTEGER
			-- Type of error

	value: XM_XPATH_VALUE
			-- Error value

	system_id: STRING
			-- SYSTEM ID of physical entity where error occurred

	line_number: INTEGER
			-- Approximate line number of current event, or 0 if unknown
		
	error_message: STRING is
			-- Textual error message
		local
			a_message: STRING
		do
			if namespace_uri.count > 0 then
				a_message := STRING_.concat (namespace_uri, "#")
				a_message := STRING_.appended_string (a_message, code)
			else
				a_message := code
			end
			a_message := STRING_.appended_string (a_message, ": ")
			Result := STRING_.appended_string (a_message, description)
		ensure
			error_message_not_void: Result /= Void
		end

feature -- Status report

	is_location_known: BOOLEAN is
			-- is the location of the error known?
		do
			Result := not system_id.is_empty
		end
	
feature -- Comparison

	same_error (other: XM_XPATH_ERROR_VALUE): BOOLEAN is
			-- Are `Current' and `other' the same error?
		require
			other_not_void: other /= Void
		do

			-- Same error iff QName error code and description and type are all the same

			Result := STRING_.same_string (code, other.code) and then type=other.type
				and then STRING_.same_string (error_message, other.error_message)
				and then STRING_.same_string (namespace_uri, other.namespace_uri)
		end

feature -- Element change

	set_location (a_system_id: like system_id; a_line_number: like line_number) is
			-- Set location information.
		require
			location_unknown: not is_location_known
			system_id_known: not a_system_id.is_empty
		do
			system_id := a_system_id
			if a_line_number > 0 then line_number := a_line_number end
		ensure
			location_known: is_location_known
			system_id_set: system_id = a_system_id
			line_number_set: a_line_number > 0 implies line_number = a_line_number
		end

invariant

	error_value_not_void: value /= Void
	namespace_uri_not_void: namespace_uri /= Void
	local_part_not_void: code /= Void
	description_not_void: description /= Void
	error_type: type = Static_error or type = Type_error or type = Dynamic_error

end
	