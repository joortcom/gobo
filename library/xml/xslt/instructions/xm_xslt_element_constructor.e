indexing

	description:

		"Compiled instructions that produce element output"

	library: "Gobo Eiffel XSLT Library"
	copyright: "Copyright (c) 2004, Colin Adams and others"
	license: "Eiffel Forum License v2 (see forum.txt)"
	date: "$Date$"
	revision: "$Revision$"

deferred class XM_XSLT_ELEMENT_CONSTRUCTOR

inherit

	XM_XSLT_ATTRIBUTE_SET_ROUTINES

	XM_XSLT_INSTRUCTION
		redefine
			sub_expressions, evaluate_item, item_type, compute_cardinality, compute_special_properties,
			creates_new_nodes, native_implementations, promote_instruction
		end

	XM_XPATH_RECEIVER_OPTIONS

	XM_XPATH_SHARED_NODE_KIND_TESTS

	XM_XSLT_VALIDATION

feature -- Access
	
	item_type: XM_XPATH_ITEM_TYPE is
			-- Data type of the expression, when known
		do
			Result := element_node_kind_test
		end

	sub_expressions: DS_ARRAYED_LIST [XM_XPATH_EXPRESSION] is
			-- Immediate sub-expressions
		do
			create Result.make (1)
			Result.set_equality_tester (expression_tester)
			Result.put (content, 1)
		end

	validation_action: INTEGER
			-- Validation action

	name_code (a_context: XM_XSLT_EVALUATION_CONTEXT): INTEGER is
			-- Name code
		require
			context_not_void: a_context /= Void
		deferred
		end

feature -- Status report

	creates_new_nodes: BOOLEAN is
			-- Can `Current' create new nodes?
		do
			Result := True
		end

feature -- Optimization

	simplify is
			-- Perform context-free optimizations.
		do
			content.simplify
			if content.was_expression_replaced then
				content := content.replacement_expression
				adopt_child_expression (content)
			end
		end

	analyze (a_context: XM_XPATH_STATIC_CONTEXT) is
			-- Perform static analysis of `Current' and its subexpressions.
		do
			content.analyze (a_context)
			if content.was_expression_replaced then
				content := content.replacement_expression
				adopt_child_expression (content)
			end
		end

	promote_instruction (an_offer: XM_XPATH_PROMOTION_OFFER) is
			-- Promote this instruction.
		do
			content.promote (an_offer)
			if content.was_expression_replaced then
				content := content.replacement_expression
				adopt_child_expression (content)
			end
		end

feature -- Evaluation

	evaluate_item (a_context: XM_XPATH_CONTEXT) is
			-- Evaluate as a single item.
		local
			a_transformer: XM_XSLT_TRANSFORMER
			a_receiver: XM_XPATH_SEQUENCE_RECEIVER
			a_validator: XM_XPATH_RECEIVER
			a_new_context: XM_XSLT_EVALUATION_CONTEXT
			an_outputter: XM_XSLT_SEQUENCE_OUTPUTTER
			a_name_code, some_properties: INTEGER
		do
			a_new_context ?= a_context.new_minor_context
			check
				evaluation_context: a_new_context /= Void
				-- This is XSLT
			end
			a_transformer := a_new_context.transformer
			create an_outputter.make_with_size (1, a_transformer)
			a_receiver := an_outputter
			a_name_code := name_code (a_new_context)
			a_validator := a_transformer.configuration.element_validator (a_receiver, a_name_code, Void, validation_action)
			if a_validator = a_receiver then
				a_new_context.change_to_sequence_output_destination (a_receiver)
			else
				check
					schema_aware: False
					-- Only Basic XSLT processor is supported now
				end
			end
			a_receiver.start_document
			if not is_inherit_namespaces then some_properties := Disinherit_namespaces  end
			a_receiver.start_element (a_name_code, -1, some_properties)
			output_namespace_nodes (a_new_context, a_receiver)
			content.process (a_new_context)
			a_receiver.end_element
			a_receiver.end_document
			last_evaluated_item := an_outputter.first_item
		end

	process_leaving_tail (a_context: XM_XSLT_EVALUATION_CONTEXT) is
			-- Execute `Current', writing results to the current `XM_XPATH_RECEIVER'.
		local
			a_name_code, some_properties: INTEGER
			a_transformer: XM_XSLT_TRANSFORMER
			a_receiver: XM_XPATH_SEQUENCE_RECEIVER
			a_validator: XM_XPATH_RECEIVER
			an_error: XM_XPATH_ERROR_VALUE
		do
			a_name_code := name_code (a_context)
			if a_name_code = -1 then
				-- TODO: this might be an XTDE0820
				create an_error.make_from_string ("Name is not a valid QName", "", "XTDE0830", Dynamic_error)
				a_context.transformer.report_fatal_error (an_error, Current)
			else
				a_transformer := a_context.transformer
				a_receiver := a_context.current_receiver
				a_validator := a_transformer.configuration.element_validator (a_receiver, a_name_code, Void, validation_action)
				if a_validator /= a_receiver then
					check
						schema_aware: False
						-- Only Basic XSLT processor is supported now
					end
				end
				if not is_inherit_namespaces then some_properties := Disinherit_namespaces  end
				a_receiver.start_element (a_name_code, -1, some_properties)

				-- Output the required namespace nodes via a call-back

				output_namespace_nodes (a_context, a_receiver)
				if not a_transformer.is_error then

					-- Apply the content of any attribute sets mentioned in use-attribute-sets.

					if attribute_sets /= Void then expand_attribute_sets (attribute_sets, a_context) end
					content.process (a_context)

					if not a_transformer.is_error then
						
						-- Output the element end tag (which will fail if validation fails)

						a_receiver.end_element
					end
				end
			end
			last_tail_call := Void
		end

	skip_element (a_context: XM_XSLT_EVALUATION_CONTEXT) is
			-- Take recovery action when the element name is invalid.
			-- We need to tell the receiver about this,
			--  so that it can ignore attributes in the content
		require
			context_not_void: a_context /= Void
		do

			-- Sending a namecode of -10 to the receiver is a special signal to ignore
			--  this element and the attributes that follow it

			a_context.current_receiver.start_element (-10, 0, 0)
			content.process (a_context)

			-- Note, we don't bother with an end_element call.
			
		end

feature {XM_XSLT_ELEMENT_CREATOR} -- Local

	output_namespace_nodes (a_context: XM_XSLT_EVALUATION_CONTEXT; a_receiver: XM_XPATH_RECEIVER) is
			-- Output namespace nodes for the new element.
		require
			context_not_void: a_context /= Void
			receiver_not_void: a_receiver /= Void
		deferred
		end

	
feature {XM_XPATH_EXPRESSION} -- Restricted

	compute_cardinality is
			-- Compute cardinality.
		do
			set_cardinality_exactly_one
		end

	compute_special_properties is
			-- Compute special properties.
		do
			Precursor
			set_single_document_nodeset
		end

	native_implementations: INTEGER is
			-- Natively-supported evaluation routines
		do
			Result := INTEGER_.bit_or (Supports_process, Supports_evaluate_item)
		end

feature {NONE} -- Implementation

	type: XM_XPATH_SCHEMA_TYPE

	attribute_sets: DS_ARRAYED_LIST [XM_XSLT_COMPILED_ATTRIBUTE_SET]
			-- Used attribute sets

	is_inherit_namespaces: BOOLEAN
			-- Are namespaces inherited

	content: XM_XPATH_EXPRESSION
			-- Element content

invariant

	validation_action: initialized implies validation_action >= Validation_strict  and then Validation_strip >= validation_action
	content_not_void: initialized implies content /= Void

end
	
