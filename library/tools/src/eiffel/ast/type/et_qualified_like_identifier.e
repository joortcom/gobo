﻿note

	description:

		"Eiffel qualified anchored types"

	library: "Gobo Eiffel Tools Library"
	copyright: "Copyright (c) 2003-2023, Eric Bezault and others"
	license: "MIT License"

deferred class ET_QUALIFIED_LIKE_IDENTIFIER

inherit

	ET_LIKE_IDENTIFIER
		redefine
			reset,
			reset_qualified_anchored_types,
			named_type_with_type_mark,
			shallow_named_type_with_type_mark,
			named_type_has_class,
			named_type_is_formal_type,
			has_unqualified_anchored_type,
			depends_on_qualified_anchored_type,
			has_formal_types,
			add_adapted_base_classes_to_list,
			adapted_base_class_with_named_feature,
			adapted_base_class_with_seeded_feature,
			same_syntactical_qualified_like_identifier_with_type_marks,
			same_named_class_type_with_type_marks,
			same_named_formal_parameter_type_with_type_marks,
			same_named_tuple_type_with_type_marks,
			same_base_class_type_with_type_marks,
			same_base_formal_parameter_type_with_type_marks,
			same_base_tuple_type_with_type_marks,
			conforms_from_class_type_with_type_marks,
			conforms_from_formal_parameter_type_with_type_marks,
			conforms_from_tuple_type_with_type_marks,
			is_type_reference_with_type_mark,
			is_type_detachable_with_type_mark
		end

feature -- Initialization

	reset
			-- Reset type as it was just after it was last parsed.
		do
			name.reset
			target_type.reset
		end

	reset_qualified_anchored_types
			-- Reset qualified anchored types contained in current type
			-- as they were just after they were last parsed.
		do
			name.reset
			target_type.reset_qualified_anchored_types
		end

feature -- Access

	target_type: ET_TYPE
			-- Target anchored type
		deferred
		ensure
			target_type_not_void: Result /= Void
		end

	name: ET_FEATURE_NAME
			-- Name of the feature in `target_type'
			-- associated with current type
		do
			Result := qualified_name.feature_name
		ensure then
			definition: Result = qualified_name.feature_name
		end

	qualified_name: ET_QUALIFIED_FEATURE_NAME
			-- Qualified feature name

	seed: INTEGER
			-- Feature ID of one of the seeds of the
			-- feature associated with current type;
			-- 0 if not resolved yet
		do
			Result := name.seed
		end

	named_base_class (a_context: ET_TYPE_CONTEXT): ET_NAMED_CLASS
			-- Same as `base_class' except that it returns information about this
			-- class (e.g. its name) as known from the universe it is used from
			-- (instead of from the universe it is written in).
			-- Return "*UNKNOWN*" class if unresolved identifier type,
			-- or unmatched formal generic parameter.
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.named_base_class (l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	adapted_base_class_with_named_feature (a_name: ET_CALL_NAME; a_context: ET_TYPE_CONTEXT): ET_ADAPTED_CLASS
			-- Base class of current type when it appears in `a_context', or in case of
			-- a formal parameter one of its constraint adapted base classes containing
			-- a feature named `a_name' (or any of the constraints if none contains such
			-- feature)
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.adapted_base_class_with_named_feature (a_name, l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	adapted_base_class_with_seeded_feature (a_seed: INTEGER; a_context: ET_TYPE_CONTEXT): ET_ADAPTED_CLASS
			-- Base class of current type when it appears in `a_context', or in case of
			-- a formal parameter one of its constraint adapted base classes containing
			-- a feature with seed `a_seed' (or any of the constraints if none contains
			-- such feature)
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.adapted_base_class_with_seeded_feature (a_seed, l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	base_type_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): ET_BASE_TYPE
			-- Same as `base_type' except that its type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.base_type_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	shallow_base_type_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): ET_BASE_TYPE
			-- Same as `shallow_base_type' except that its type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
						-- Here we have to use `base_type' and not `shallow_base_type'
						-- because otherwise the actual generic parameters will not
						-- be viewed from `a_context' but from `l_target_context'.
					Result := l_query.type.base_type_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	base_type_actual (i: INTEGER; a_context: ET_TYPE_CONTEXT): ET_NAMED_TYPE
			-- `i'-th actual generic parameter's type of the base type of current
			-- type when it appears in `a_context'
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.base_type_actual (i, l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	base_type_actual_parameter (i: INTEGER; a_context: ET_TYPE_CONTEXT): ET_ACTUAL_PARAMETER
			-- `i'-th actual generic parameter of the base type of current
			-- type when it appears in `a_context'
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.base_type_actual_parameter (i, l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	base_type_index_of_label (a_label: ET_IDENTIFIER; a_context: ET_TYPE_CONTEXT): INTEGER
			-- Index of actual generic parameter with label `a_label' in
			-- the base type of current type when it appears in `a_context';
			-- 0 if it does not exist
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := 0
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := 0
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.base_type_index_of_label (a_label, l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	named_type_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): ET_NAMED_TYPE
			-- Same as `named_type' except that its type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.named_type_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	shallow_named_type_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): ET_NAMED_TYPE
			-- Same as `shallow_named_type' except that its type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := tokens.unknown_class
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := tokens.unknown_class
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
						-- Here we have to use `named_type' and not `shallow_named_type'
						-- because otherwise the actual generic parameters will not
						-- be viewed from `a_context' but from `l_target_context'.
					Result := l_query.type.named_type_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	hash_code: INTEGER
			-- Hash code
		do
			Result := seed
		end

	position: ET_POSITION
			-- Position of first character of
			-- current node in source code
		do
			if attached type_mark as l_type_mark and then not l_type_mark.is_implicit_mark and then not l_type_mark.position.is_null then
				Result := l_type_mark.position
			else
				Result := like_keyword.position
			end
			if Result.is_null then
				Result := target_type.position
			end
		end

	last_leaf: ET_AST_LEAF
			-- Last leaf node in current node
		do
			Result := qualified_name.last_leaf
		end

	implementation_class: ET_CLASS
			-- Class where the current type appears

feature -- Measurement

	base_type_actual_count (a_context: ET_TYPE_CONTEXT): INTEGER
			-- Number of actual generic parameters of the base type of current type
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := 0
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := 0
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.base_type_actual_count (l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

feature -- Status report

	is_type_separate_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Same as `is_type_separate' except that the type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.is_type_separate_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	is_type_expanded_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Same as `is_type_expanded' except that the type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.is_type_expanded_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	is_type_reference_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Same as `is_type_reference' except that the type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.is_type_reference_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	is_type_attached_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Same as `is_type_attached' except that the type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.is_type_attached_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	is_type_detachable_with_type_mark (a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Same as `is_type_detachable' except that the type mark status is
			-- overridden by `a_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.is_type_detachable_with_type_mark (overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	has_non_separate_reference_attributes (a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Does current type contain attributes whose types are declared
			-- of non-separate reference types when viewed from `a_context'?
			-- True in case of a formal generic parameter because the actual
			-- generic parameter may contain non-separate reference attributes.
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.has_non_separate_reference_attributes (l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	has_nested_non_separate_reference_attributes (a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Does current type contain non-separate reference attributes when
			-- viewed from `a_context', or recursively does it contain expanded
			-- attributes whose types contain non-separate reference attributes?
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.has_nested_non_separate_reference_attributes (l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	has_unqualified_anchored_type: BOOLEAN
			-- Does current type contain an unqualified anchored type
			-- (i.e. 'like Current' or 'like feature_name')?
		do
			Result := target_type.has_unqualified_anchored_type
		end

	depends_on_qualified_anchored_type (a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Does current type depend on a qualified anchored type when
			-- viewed from `a_context' when trying to determine its base type?
		do
			Result := True
		end

	has_formal_types (a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Does current type contain a formal generic parameter
			-- when viewed from `a_context'?
		do
			Result := target_type.has_formal_types (a_context)
		end

	named_type_is_formal_type (a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Is named type of current type a formal parameter when viewed from `a_context'?
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.named_type_is_formal_type (l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	base_type_has_class (a_class: ET_CLASS; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Does the base type of current type contain `a_class'
			-- when it appears in `a_context'?
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := a_class.is_unknown
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := a_class.is_unknown
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.base_type_has_class (a_class, l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	named_type_has_class (a_class: ET_CLASS; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Does the named type of current type contain `a_class'
			-- when it appears in `a_context'?
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := a_class.is_unknown
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := a_class.is_unknown
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.named_type_has_class (a_class, l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	named_type_has_class_with_ancestors_not_built_successfully (a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Does the named type of current type contain a class
			-- whose ancestors have not been built successfully
			-- when it appears in `a_context'?
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.named_type_has_class_with_ancestors_not_built_successfully (l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

feature -- Basic operations

	add_adapted_base_classes_to_list (a_list: DS_ARRAYED_LIST [ET_ADAPTED_CLASS]; a_context: ET_TYPE_CONTEXT)
			-- Add to `a_list' the base class of current type when it appears in `a_context' or
			-- the adapted base classes of the constraints (in the same order they appear in
			-- 'constraint_base_types') in case of a formal parameter.
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				a_list.force_last (tokens.unknown_class)
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					a_list.force_last (tokens.unknown_class)
				else
					l_target_context := a_context.as_nested_type_context
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					l_query.type.add_adapted_base_classes_to_list (a_list, l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

feature -- Comparison

	same_syntactical_type_with_type_marks (other: ET_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Same as `same_syntactical_type' except that the type mark status of `Current'
			-- and `other' is overridden by `a_type_mark' and `other_type_mark', if not Void
		do
			if other = Current and then other_type_mark = a_type_mark and then other_context = a_context then
				Result := True
			else
				Result := other.same_syntactical_qualified_like_identifier_with_type_marks (Current, a_type_mark, a_context, other_type_mark, other_context)
			end
		end

	same_named_type_with_type_marks (other: ET_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Same as `same_named_type' except that the type mark status of `Current'
			-- and `other' is overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if other = Current and then other_type_mark = a_type_mark and then other_context = a_context then
				Result := True
			elseif seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.same_named_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	same_base_type_with_type_marks (other: ET_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Same as `same_base_type' except that the type mark status of `Current'
			-- and `other' is overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if other = Current and then other_type_mark = a_type_mark and then other_context = a_context then
				Result := True
			elseif seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.same_base_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

feature {ET_TYPE, ET_TYPE_CONTEXT} -- Comparison

	same_syntactical_qualified_like_identifier_with_type_marks (other: ET_QUALIFIED_LIKE_IDENTIFIER; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Are current type appearing in `a_context' and `other'
			-- type appearing in `other_context' the same type?
			-- (Note: We are NOT comparing the base types here!
			-- Therefore anchored types are considered the same
			-- only if they have the same anchor. An anchor type
			-- is not considered the same as any other type even
			-- if they have the same base type.)
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_query: detachable ET_QUERY
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_other_target_type: ET_TYPE
		do
			if other = Current and then other_type_mark = a_type_mark and then other_context = a_context then
				Result := True
			elseif seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			elseif a_context.attachment_type_conformance_mode and then not same_attachment_marks_with_default (overridden_type_mark (a_type_mark), other.overridden_type_mark (other_type_mark), Void) then
				Result := False
			else
					-- They should have the same target type.
				l_target_type := target_type
				l_other_target_type := other.target_type
				if l_target_type.same_syntactical_type (l_other_target_type, other_context, a_context) then
						-- They should refer to the same feature.
					if other.seed = seed then
						Result := True
					else
						if other.implementation_class = other_context.root_context.base_class then
							l_adapted_base_class := l_other_target_type.adapted_base_class_with_named_feature (other.name, other_context)
							l_query := l_adapted_base_class.named_query (other.name)
						else
							l_adapted_base_class := l_other_target_type.adapted_base_class_with_seeded_feature (other.seed, other_context)
							l_query := l_adapted_base_class.base_class.seeded_query (seed)
						end
						Result := l_query /= Void and then l_query.has_seed (seed)
						if not Result then
							if implementation_class = a_context.root_context.base_class then
								l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
								l_query := l_adapted_base_class.named_query (name)
							else
								l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
								l_query := l_adapted_base_class.base_class.seeded_query (seed)
							end
							Result := l_query /= Void and then l_query.has_seed (other.seed)
						end
					end
				end
			end
		end

	same_named_class_type_with_type_marks (other: ET_CLASS_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Do current type appearing in `a_context' and `other' type
			-- appearing in `other_context' have the same named type?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.same_named_class_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	same_named_formal_parameter_type_with_type_marks (other: ET_FORMAL_PARAMETER_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Do current type appearing in `a_context' and `other' type
			-- appearing in `other_context' have the same named type?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.same_named_formal_parameter_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	same_named_tuple_type_with_type_marks (other: ET_TUPLE_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Do current type appearing in `a_context' and `other' type
			-- appearing in `other_context' have the same named type?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.same_named_tuple_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	same_base_class_type_with_type_marks (other: ET_CLASS_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Do current type appearing in `a_context' and `other' type
			-- appearing in `other_context' have the same base type?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.same_base_class_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	same_base_formal_parameter_type_with_type_marks (other: ET_FORMAL_PARAMETER_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Do current type appearing in `a_context' and `other' type
			-- appearing in `other_context' have the same base type?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.same_base_formal_parameter_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	same_base_tuple_type_with_type_marks (other: ET_TUPLE_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT): BOOLEAN
			-- Do current type appearing in `a_context' and `other' type
			-- appearing in `other_context' have the same base type?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.same_base_tuple_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

feature -- Conformance

	conforms_to_type_with_type_marks (other: ET_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT; a_system_processor: ET_SYSTEM_PROCESSOR): BOOLEAN
			-- Same as `conforms_to_type' except that the type mark status of `Current'
			-- and `other' is overridden by `a_type_mark' and `other_type_mark', if not Void
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if other = Current and then other_type_mark = a_type_mark and then other_context = a_context then
				Result := True
			elseif seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.conforms_to_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context, a_system_processor)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

feature {ET_TYPE, ET_TYPE_CONTEXT} -- Conformance

	conforms_from_class_type_with_type_marks (other: ET_CLASS_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT; a_system_processor: ET_SYSTEM_PROCESSOR): BOOLEAN
			-- Does `other' type appearing in `other_context' conform
			-- to current type appearing in `a_context'?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
			-- (Note: 'a_system_processor.ancestor_builder' is used on the classes
			-- whose ancestors need to be built in order to check for conformance.)
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.conforms_from_class_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context, a_system_processor)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	conforms_from_formal_parameter_type_with_type_marks (other: ET_FORMAL_PARAMETER_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT; a_system_processor: ET_SYSTEM_PROCESSOR): BOOLEAN
			-- Does `other' type appearing in `other_context' conform
			-- to current type appearing in `a_context'?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
			-- (Note: 'a_system_processor.ancestor_builder' is used on the classes
			-- whose ancestors need to be built in order to check for conformance.)
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.conforms_from_formal_parameter_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context, a_system_processor)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

	conforms_from_tuple_type_with_type_marks (other: ET_TUPLE_TYPE; other_type_mark: detachable ET_TYPE_MARK; other_context: ET_TYPE_CONTEXT; a_type_mark: detachable ET_TYPE_MARK; a_context: ET_TYPE_CONTEXT; a_system_processor: ET_SYSTEM_PROCESSOR): BOOLEAN
			-- Does `other' type appearing in `other_context' conform
			-- to current type appearing in `a_context'?
			-- Note that the type mark status of `Current' and `other' is
			-- overridden by `a_type_mark' and `other_type_mark', if not Void
			-- (Note: 'a_system_processor.ancestor_builder' is used on the classes
			-- whose ancestors need to be built in order to check for conformance.)
		local
			l_adapted_base_class: ET_ADAPTED_CLASS
			l_target_type: ET_TYPE
			l_target_context: ET_NESTED_TYPE_CONTEXT
			l_query: detachable ET_QUERY
			l_old_count: INTEGER
		do
			if seed = 0 then
					-- Qualified anchored type not resolved yet.
				Result := False
			else
				l_target_type := target_type
				if implementation_class = a_context.root_context.base_class then
					l_adapted_base_class := l_target_type.adapted_base_class_with_named_feature (name, a_context)
					l_query := l_adapted_base_class.named_query (name)
				else
					l_adapted_base_class := l_target_type.adapted_base_class_with_seeded_feature (seed, a_context)
					l_query := l_adapted_base_class.base_class.seeded_query (seed)
				end
				if l_query = Void then
						-- Internal error: an inconsistency has been
						-- introduced in the AST since we resolved
						-- current qualified anchored type.
						-- In the implementation class, we know that there is
						-- extactly one constraint with a query of this name.
						-- In descendant classes, we know that there is
						-- at least one constraint with a query with this seed,
						-- and if there are more than one, then they all have
						-- the same type.
					Result := False
				else
					if a_context /= other_context then
						l_target_context := a_context.as_nested_type_context
					else
						l_target_context := a_context.to_nested_type_context
					end
					l_old_count := l_target_context.count
					l_target_context.force_last (l_target_type)
					{ET_ADAPTED_BASE_CLASS_CHECKER}.reset_context_if_multiple_constraints (not attached {ET_CLASS} l_adapted_base_class, l_adapted_base_class, l_target_context)
					Result := l_query.type.conforms_from_tuple_type_with_type_marks (other, other_type_mark, other_context, overridden_type_mark (a_type_mark), l_target_context, a_system_processor)
					l_target_context.keep_first (l_old_count)
				end
			end
		end

feature -- Resolving

	resolve_identifier_type (a_seed: INTEGER)
			-- Resolve current type with `a_seed'.
		require
			a_seed_positive: a_seed > 0
		do
			name.set_seed (a_seed)
		ensure
			seed_set: seed = a_seed
		end

invariant

	qualified_name_not_void: qualified_name /= Void
	implementation_class_not_void: implementation_class /= Void

end
