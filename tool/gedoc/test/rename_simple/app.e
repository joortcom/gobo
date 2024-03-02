-- to build with SmartEiffel: compile  app.e -o app
class APP inherit INTERNAL

create {ANY}
   make

feature {ANY}
   p:  PERSON
   f:  FACULTY

   -- problematic implementation: direct field access
   print_faculty_addr_direct_field(u: FACULTY) is
      do io.put_string(u.name + " as FACULTY.addr: " + u.get_addr() + "%N") end

   -- correct implementation: use semantic assigning accessor
   print_faculty_addr_via_accessor(u: FACULTY) is
      do io.put_string(u.name + " as FACULTY.addr: " + u.get_faculty_addr() + "%N") end

   make is
      do
	 create  p.default_create
	 create  f.make

	 f.print_faculty()
	 io.put_string("PERSON  size: " +physical_size(p ).out+ "%N")
	 io.put_string("FACULTY size: " +physical_size(f ).out+ "%N")

	 f.do_benchwork()  -- which addr field will this calls access?

	 io.put_string("-- print_faculty_addr_direct_field%N")
	 print_faculty_addr_direct_field(f)

	 io.put_string("-- print_faculty_addr_via_accessor%N")
	 print_faculty_addr_via_accessor(f)

	 io.put_string("-- check reference identity%N")
  	 if                  f.get_addr()  = f.faculty_addr
	 then io.put_string("f.addr  = f.faculty_addr%N")
         else io.put_string("f.addr != f.faculty_addr%N") end

	 io.put_string("-- test some assignment: suppose f moved lab2%N")
	 f.set_faculty_addr("lab2")
	 f.print_faculty()
      end
end
