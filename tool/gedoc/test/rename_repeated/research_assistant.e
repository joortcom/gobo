class RESEARCH_ASSISTANT
inherit
	PERSON rename addr as student_addr end  -- field student_addr inherit the dorm semantics
	PERSON rename addr as faculty_addr end  -- field faculty_addr inherit the lab  semantics
	-- then select, NOTE: not needed by SmartEiffel, but needed by GOBO and ISE compiler
	PERSON  select addr end

create {ANY}
   make

feature {ANY}
   get_student_addr():STRING  is do Result := student_addr end  -- assign dorm semantics to addr
   set_student_addr(a:STRING) is do student_addr := a end

   get_faculty_addr():STRING  is do Result := faculty_addr end  -- assign lab semantics to addr
   set_faculty_addr(a:STRING) is do faculty_addr := a end


   print_ra() is  -- print out all 3 addresses
     do
       io.put_string(name +" has 3 addresses: <"+ addr +", "+ student_addr +", "+ faculty_addr + ">%N")
     end

   make is  -- the constructor
     do
       name := "ResAssis"
       addr := "home"          -- the home semantics
       student_addr := "dorm"  -- the dorm semantics
       faculty_addr := "lab"   -- the lab  semantics
     end

   take_rest() is
     do
       io.put_string(name + " take_rest in the: " + student_addr + "%N");
     end

   do_benchwork() is
     do
       io.put_string(name + " do_benchwork in the: " + faculty_addr + "%N");
     end
end
