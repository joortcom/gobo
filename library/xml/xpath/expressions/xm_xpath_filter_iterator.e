indexing

	description:

		"Objects that filter a sequence using a filter expression."

	library: "Gobo Eiffel XPath Library"
	copyright: "Copyright (c) 2004, Colin Adams and others"
	license: "Eiffel Forum License v2 (see forum.txt)"
	date: "$Date$"
	revision: "$Revision$"

class XM_XPATH_FILTER_ITERATOR

inherit

	XM_XPATH_SEQUENCE_ITERATOR [XM_XPATH_ITEM]

	KL_IMPORTED_STRING_ROUTINES

		-- This class is not used where the filter is a constant number.
		-- Instead, use XM_XPATH_POSITION_FILTER, so this class does not
		--  need to do optimization for numeric predicates.

creation

	make, make_non_numeric

feature {NONE} -- Initialization

	make (a_base_iterator: XM_XPATH_SEQUENCE_ITERATOR [XM_XPATH_ITEM]; a_filter: XM_XPATH_EXPRESSION; a_context: XM_XPATH_CONTEXT) is
			-- Establish invariant.
		require
			base_iterator_not_void: a_base_iterator /= Void
			filter_not_void: a_filter /= Void
			context_not_void: a_context /= Void
		do
			base_iterator := a_base_iterator
			filter := a_filter
			filter_context := a_context.new_context
			filter_context.set_current_iterator (base_iterator)
		ensure
			base_iterator_set: base_iterator = a_base_iterator
			filter_set: filter = a_filter
		end

	make_non_numeric (a_base_iterator: XM_XPATH_SEQUENCE_ITERATOR [XM_XPATH_ITEM]; a_filter: XM_XPATH_EXPRESSION; a_context: XM_XPATH_CONTEXT) is
			-- Establish invariant for non-numeric results.
		require
			base_iterator_not_void: a_base_iterator /= Void
			filter_not_void: a_filter /= Void
			context_not_void: a_context /= Void
		do
			non_numeric := True
			make (a_base_iterator, a_filter, a_context)
		end
		
feature -- Access
	
	item: XM_XPATH_ITEM is
			-- Value or node at the current position
		do
			Result := current_item
		end

feature -- Status report

	after: BOOLEAN is
			-- Are there any more items in the sequence?
		do
			Result := not before and then current_item = Void
		end

feature -- Cursor movement

	forth is
			-- Move to next position
		do
			index := index + 1
			advance
		end

feature -- Duplication

	another: like Current is
			-- Another iterator that iterates over the same items as the original
		do
			if non_numeric then
				create Result.make (base_iterator.another, filter, filter_context)
			else
				create Result.make_non_numeric (base_iterator.another, filter, filter_context)
			end
		end

feature {NONE} -- Implementation

	non_numeric: BOOLEAN
			-- Is statically known mueric result not possible?

	current_item: like item
			-- Current item

	base_iterator: XM_XPATH_SEQUENCE_ITERATOR [XM_XPATH_ITEM]
			-- The underlying iterator

	filter: XM_XPATH_EXPRESSION
			-- Filter to apply to `base_iterator'

	filter_context: XM_XPATH_CONTEXT
			-- Evaluation context for the filter

	advance is
			-- Move to next matching node.
		local
			next_item: like item
			matched: BOOLEAN
		do
			from
				if base_iterator.before then base_iterator.start end
			until
				is_error or matched or else base_iterator.after
			loop
				next_item := base_iterator.item
				matched := matches
				
				base_iterator.forth
			end

			if is_error then
				create {XM_XPATH_BOOLEAN_VALUE} current_item.make (False) -- we need SOMETHING to set an error upon!
				current_item.set_evaluation_error (last_error)
			elseif matched then
				current_item := next_item
			else
				current_item := Void
			end
		end

	matches: BOOLEAN is
			-- Does the context item match the filter predicate?
		require
			filter_not_in_error: not filter.is_error
		local
			an_iterator: XM_XPATH_SEQUENCE_ITERATOR [XM_XPATH_ITEM]
			an_item: like item
			a_node: XM_XPATH_NODE
			a_boolean_value: XM_XPATH_BOOLEAN_VALUE
			an_integer_value: XM_XPATH_INTEGER_VALUE
			a_numeric_value: XM_XPATH_NUMERIC_VALUE
			a_string_value: XM_XPATH_STRING_VALUE
		do
			if non_numeric then
				a_boolean_value := filter.effective_boolean_value (filter_context)
				if a_boolean_value.is_error then
					set_last_error (a_boolean_value.last_error)
				else
					Result := a_boolean_value.value
				end
			else

				-- This code is carefully designed to avoid reading more items from the
				-- iteration of the filter expression than are absolutely essential.
			
				an_iterator := filter.iterator (filter_context)
				if not an_iterator.is_error then
					an_iterator.start
					if not an_iterator.after then
						an_item := an_iterator.item
						a_node ?= an_item
						if a_node /= Void then
							Result := True
						else
							a_boolean_value ?= an_item
							if a_boolean_value /= Void then
								if a_boolean_value.value then	Result := True	else an_iterator.forth; Result := not an_iterator.after end
							else
								an_integer_value ?= an_item
								if an_integer_value /= Void then
									if an_integer_value.value = base_iterator.index then	Result := True	else an_iterator.forth; Result := not an_iterator.after end
								else
									a_numeric_value ?= an_item
									if a_numeric_value /= Void then
										create an_integer_value.make (base_iterator.index)
										Result := a_numeric_value.same_expression (an_integer_value)
										if not Result then an_iterator.forth; Result := not an_iterator.after end
									else
										a_string_value ?= an_item
										if a_string_value /= Void then
											Result := STRING_.same_string (a_string_value.string_value, "")
											if not Result then an_iterator.forth; Result := not an_iterator.after end
										else
											Result := True
										end
									end
								end
							end
						end
					end
				else

					-- We are in error

					Result := False
					set_last_error (an_iterator.last_error)
				end
			end
		end

invariant

	base_iterator_not_void: base_iterator /= Void
	filter_not_void: filter /= Void
	filter_context_not_void: filter_context /= Void

end
