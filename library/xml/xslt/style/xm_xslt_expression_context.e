indexing

	description:

		"XSLT expression contexts"

	library: "Gobo Eiffel XSLT Library"
	copyright: "Copyright (c) 2004, Colin Adams and others"
	license: "Eiffel Forum License v2 (see forum.txt)"
	date: "$Date$"
	revision: "$Revision$"

class XM_XSLT_EXPRESSION_CONTEXT

inherit

	XM_XPATH_STATIC_CONTEXT

	XM_XPATH_DEBUGGING_ROUTINES

feature -- Access

	style_element: XM_XSLT_STYLE_ELEMENT is
			-- TODO
		do
				todo ("style-element", False)
		end

	host_language: STRING is
			-- Name of host language
		do
			Result := "XSLT"
		end
	
	default_element_namespace: INTEGER is
			-- Default XPath namespace, as a namespace code that can be looked up in `name_pool'
		do
			todo ("default-element-namespace", False)
		end

	default_collation_name: STRING
			-- URI naming the default collation

	collator (a_collation_name: STRING): ST_COLLATOR is
			-- Collation named `a_collation_name'
		do
			todo ("collator", False)
		end

	uri_for_prefix (an_xml_prefix: STRING): STRING is
			-- URI for a namespace prefix
		do
			todo ("uri-for-prefix", False)
		end

		is_backwards_compatible_mode: BOOLEAN
			-- Is Backwards Compatible Mode used?

feature -- Status report

	is_prefix_declared (an_xml_prefix: STRING): BOOLEAN is
			-- Is `an_xml_prefix' allocated to a namespace?
		do
			todo ("is-prefix-declared", False)
		end

	is_variable_declared (a_fingerprint: INTEGER): BOOLEAN is
			-- Does `a_fingerprint' represent a variable declared in the static context?
		do
			todo ("is-variable-declared", False)
		end
	
	is_data_type_valid (a_fingerprint: INTEGER): BOOLEAN is
			-- Does `a_fingerprint' represent a data-type in `Current'?
		do
			Result := False

			-- Customized host languages must redefine this routine.
			-- It is not called for host languages supported directly
			--  by this library.
			
		end

feature -- Element change
	
	bind_variable (a_fingerprint: INTEGER) is
			-- Bind variable to it's declaration.
		do
			todo ("bind-variable", False)
		end

	bind_function (a_qname: STRING; arguments: DS_ARRAYED_LIST [XM_XPATH_EXPRESSION]) is
			-- Identify a function appearing in an expression.
		do
			todo ("bind-function", False)
		end

feature -- Output

	issue_warning (a_warning: STRING) is
			-- Issue a warning message
		do
			todo ("issue-warning", False)
		end

end

