indexing

	description:

		"Contents of geant project files"

	library:    "Gobo Eiffel Ant"
	author:     "Sven Ehrke <sven.ehrke@sven-ehrke.de>"
	copyright:  "Copyright (c) 2001, Sven Ehrke and others"
	license:    "Eiffel Forum Freeware License v1 (see forum.txt)"
	date:       "$Date$"
	revision:   "$Revision$"


class GEANT_PROJECT

inherit

	GEANT_ELEMENT_NAMES
		export {NONE} all end

	KL_SHARED_ARGUMENTS

creation

	make, make_with_filename

feature {NONE} -- Initialization

	make is
			-- Create a new project using file `build.eant'.
		do
			make_with_filename (Default_build_filename)
		end

	make_with_filename (a_filename: UC_STRING) is
			-- Create a new project using file `a_filename'.
		require
			a_filename_not_void: a_filename /= Void
			a_filename_not_empty: not a_filename.empty
		do
			build_filename := a_filename
		ensure
			build_filename_set: build_filename = a_filename
		end

feature -- Access

	build_filename: UC_STRING
			-- Name of the file containing the configuration
			-- information to build current project

	targets: DS_ARRAYED_LIST [GEANT_ELEMENT]
			-- Target elements found in current project

	build_successful: BOOLEAN
			-- Was last build successful?

	target_with_name (a_name: UC_STRING): GEANT_ELEMENT is
			-- Target with `name' `a_name'
		require
			loaded: targets /= Void
			a_name_not_void: a_name /= Void
			a_name_not_empty: a_name.count > 0
		local
			i, nb: INTEGER
			a_target: GEANT_ELEMENT
		do
			nb := targets.count
			from i := 1 until i > nb or Result /= Void loop
				a_target := targets.item (i)
				if
					a_target.has_attribute (Name_attribute_name) and then
					a_target.attribute_value_by_name (Name_attribute_name).is_equal (a_name)
				then
					Result := a_target
				end

				i := i + 1
			end
		end

feature -- Setting

feature -- Processing

	load (a_start_target_name : STRING) is
			-- Read current project's configuration from `build_filename'
			-- and convert it into a 'GEANT_DOM'.
			-- When set use `a_start_target_name' for first target to execute
		local
			xml_parser: GEANT_PROJECT_PARSER
			ucs: UC_STRING
			start_target_name: UC_STRING
	    do
				-- Reset current project's state:
			reset
				-- Create xml parser:
			!! xml_parser.make_from_imp (Parser_factory.new_eiffel_event_parser_imp)
			xml_parser.parse_from_file_name (build_filename)
				-- Setup project's root element:
			root_element := xml_parser.root_element
			if root_element /= Void then
					-- Find all the targets of current project:
				targets := root_element.children_by_name (Target_element_name)

					-- Use passed start target if provided and exists
				if a_start_target_name /= Void and then a_start_target_name.count > 0 then
					!! ucs.make_from_string (a_start_target_name)
					if target_with_name (ucs) /= Void then
						start_target_name := ucs
					end
				end

					-- Find start target:
				if start_target_name = Void or else start_target_name.count = 0 then
					if root_element.has_attribute (Default_attribute_name) then
						ucs := root_element.attribute_value_by_name (Default_attribute_name)
						if target_with_name (ucs) /= Void then
							start_target_name := ucs
						end
					else
							-- Use first target in project file for start_target_name:
						if targets /= void and then targets.count > 0 then
							ucs := targets.item(1).attribute_value_by_name (Name_attribute_name)
							if target_with_name (ucs) /= Void then
								start_target_name := ucs
							end
						end
					end
				end
				if start_target_name = Void then
					reset
					print ("geant error: unknown target%N")
				elseif start_target_name.empty then
					reset
					print ("geant error: unknown target%N")
				else
							-- put start target on the stack:
					Build_targets.force (target_with_name (start_target_name))
				end
			else
				reset
				print ("Parsing error in file %"" + build_filename.out + "%"%N")
			end
	    end

	build is
			-- Build project: execute project's tasks.
		require
			loaded: targets /= Void
		local
			a_target: GEANT_ELEMENT
		do
			a_target := Build_targets.item
			execute_tasks_of_target (a_target)
			Build_targets.remove
		end

	execute_tasks_of_target(a_target: GEANT_ELEMENT) is
			-- Execute all tasks of `a_target' in sequential order
		local
			children: DS_ARRAYED_LIST [GEANT_ELEMENT]
			i, nb: INTEGER
			an_element: GEANT_ELEMENT
			a_task: GEANT_TASK
		do
			children := a_target.children
			nb := children.count
			from i := 1 until i > nb loop
				an_element := children.item (i)
					-- Dispatch tasks:
				if an_element.name.is_equal (Compile_se_task_name) then
						-- compile_se: SmallEiffel compilation
					!GEANT_COMPILE_SE_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Exec_task_name) then
						-- exec
					!GEANT_EXEC_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Lcc_task_name) then
						-- lcc
					!GEANT_LCC_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Var_task_name) then
						-- var
					!GEANT_VAR_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Gexace_task_name) then
						-- gexace
					!GEANT_GEXACE_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Gelex_task_name) then
						-- gelex
					!GEANT_GELEX_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Geyacc_task_name) then
						-- geyacc
					!GEANT_GEYACC_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Gepp_task_name) then
						-- gepp
					!GEANT_GEPP_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Getest_task_name) then
						-- getest
					!GEANT_GETEST_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Geant_task_name) then
						-- geant
					!GEANT_GEANT_TASK! a_task.make_from_element (an_element)
				elseif an_element.name.is_equal (Echo_task_name) then
						-- echo
					!GEANT_ECHO_TASK! a_task.make_from_element (an_element)
				else
						-- Default:
					a_task := Void
				end
					-- Execute task:
				if a_task = Void then
					print ("WARNING: unknown task : " + an_element.name.out + "%N")
				elseif not a_task.is_executable then
					print ("WARNING: cannot execute task : " + an_element.name.out + "%N")
				else
					a_task.execute
				end
				i := i + 1
			end
		end


	reset is
			-- Reset current state of project.
		do
			root_element := Void
			targets := Void
			build_successful := True
		end

feature {NONE} -- Implementation

	root_element: GEANT_ELEMENT
			-- Root element of project

	Parser_factory: XM_PARSER_FACTORY is
			-- Factory to create xml parsers
		once
			!! Result.make
		ensure
			parser_factory_not_void: Result /= Void
		end

	Build_targets: DS_ARRAYED_STACK [GEANT_ELEMENT] is
			-- Targets to be executed
		once
			!! Result.make (10)
		ensure
			build_targets_not_void: Result /= Void
		end

invariant

	build_filename_not_void: build_filename /= Void
	build_filename_not_empty: not build_filename.empty
	no_void_target: targets /= Void implies not targets.has (Void)
	current_target_name_not_void: targets /= Void implies current_target_name /= Void
	current_target_name_not_empty: targets /= Void implies not current_target_name.empty

end -- class GEANT_PROJECT
