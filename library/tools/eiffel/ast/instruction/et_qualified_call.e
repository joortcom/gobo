note

	description:

		"Eiffel qualified calls"

	library: "Gobo Eiffel Tools Library"
	copyright: "Copyright (c) 2002-2014, Eric Bezault and others"
	license: "MIT License"
	date: "$Date$"
	revision: "$Revision$"

class ET_QUALIFIED_CALL

inherit

	ET_AST_NODE

create

	make

feature {NONE} -- Initialization

	make (a_name: like qualified_name; args: like arguments)
			-- Create a new qualified call.
		require
			a_name_not_void: a_name /= Void
		do
			qualified_name := a_name
			arguments := args
		ensure
			name_set: qualified_name = a_name
			arguments_set: arguments = args
		end

feature -- Initialization

	reset
			-- Reset call as it was when it was last parsed.
		do
			name.reset
			if attached arguments as l_arguments then
				l_arguments.reset
			end
		end

feature -- Access

	qualified_name: ET_QUALIFIED_FEATURE_NAME
			-- Qualified feature name

	name: ET_FEATURE_NAME
			-- Feature name
		do
			Result := qualified_name.feature_name
		ensure
			definition: Result = qualified_name.feature_name
		end

	arguments: detachable ET_ACTUAL_ARGUMENT_LIST
			-- Arguments

	position: ET_POSITION
			-- Position of first character of
			-- current node in source code
		do
			Result := qualified_name.position
		end

	first_leaf: ET_AST_LEAF
			-- First leaf node in current node
		do
			Result := qualified_name.first_leaf
		end

	last_leaf: ET_AST_LEAF
			-- Last leaf node in current node
		do
			if attached arguments as l_arguments then
				Result := l_arguments.last_leaf
			else
				Result := qualified_name.last_leaf
			end
		end

feature -- Setting

	set_name (a_name: like qualified_name)
			-- Set `qualified_name' to `a_name'.
		require
			a_name_not_void: a_name /= Void
		do
			qualified_name := a_name
		ensure
			name_set: qualified_name = a_name
		end

	set_arguments (args: like arguments)
			-- Set `arguments' to `args'.
		do
			arguments := args
		ensure
			arguments_set: arguments = args
		end

feature -- Measurement

	arguments_count: INTEGER
			-- Number of arguments
		do
			if attached arguments as l_arguments then
				Result := l_arguments.count
			end
		ensure
			arguments_count_not_negative: Result >= 0
			no_argument: arguments = Void implies Result = 0
			with_arguments: attached arguments as l_arguments implies Result = l_arguments.count
		end

feature -- Processing

	process (a_processor: ET_AST_PROCESSOR)
			-- Process current node.
		do
			a_processor.process_qualified_call (Current)
		end

invariant

	qualified_name_not_void: qualified_name /= Void

end
