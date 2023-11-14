﻿note
	description:
		"[
		Strategies for calculating several features for subsets containing
		items of any type.
		]"
	library: "Free implementation of ELKS library"
	legal: "See notice at end of class."
	status: "See notice at end of class."
	date: "$Date: 2023-09-26 10:12:52 +0000 (Tue, 26 Sep 2023) $"
	revision: "$Revision: 107311 $"

class SUBSET_STRATEGY_GENERIC [G] inherit

	SUBSET_STRATEGY [G]

feature -- Comparison

	disjoint (set1, set2: TRAVERSABLE_SUBSET [G]): BOOLEAN
			-- Are `set1' and `set2' disjoint?
		local
			tmp_set: LINKED_SET [G]
		do
			if set1 = set2 then
				Result := False
			else
				create tmp_set.make
				if set1.object_comparison then
					tmp_set.compare_objects
				end
				from
					Result := True
					set1.start
					set2.start
				until
					not Result or else (set1.after and set2.after)
				loop
					if not set1.after then
						Result := not (tmp_set ∋ set1.item)
						if Result then tmp_set.put (set1.item) end
					end
					if Result and then not set2.after then
						Result := not (tmp_set ∋ set2.item)
						if Result then tmp_set.put (set2.item) end
					end
					set1.forth
					set2.forth
				end
			end
		end

feature -- Basic operations

	symdif (set1, set2: TRAVERSABLE_SUBSET [G])
			-- Remove all items of `set1' that are also in `set2', and add all
			-- items of `set2' not already present in `set1'.
		local
			tmp_set: LINKED_LIST [G]
		do
			if set1 = set2 then
				set1.wipe_out
			else
				create tmp_set.make
				if set1.object_comparison then
					tmp_set.compare_objects
				end
				from set1.start until set1.after loop
					tmp_set.extend (set1.item)
					set1.forth
				end
				from set2.start until set2.after loop
					if tmp_set.has (set2.item) then
						tmp_set.prune (set2.item)
					else
						tmp_set.extend (set2.item)
					end
					set2.forth
				end
				set1.wipe_out
				from tmp_set.start until tmp_set.after loop
					set1.extend (tmp_set.item)
					tmp_set.forth
				end
			end
		end

note
	copyright: "Copyright (c) 1984-2019, Eiffel Software and others"
	license: "Eiffel Forum License v2 (see http://www.eiffel.com/licensing/forum.txt)"
	source: "[
			Eiffel Software
			5949 Hollister Ave., Goleta, CA 93117 USA
			Telephone 805-685-1006, Fax 805-685-6869
			Website http://www.eiffel.com
			Customer support http://support.eiffel.com
		]"

end
