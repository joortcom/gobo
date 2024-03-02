class FACULTY
inherit PERSON rename addr as faculty_addr end  -- field faculty_addr inherit the lab  semantics

create {ANY}
   make

feature {ANY}
   get_faculty_addr():STRING  is do Result := faculty_addr end  -- assign lab semantics to addr
   set_faculty_addr(a:STRING) is do faculty_addr := a end

   print_faculty() is  -- print out all 2 addresses
     do
       io.put_string(name +" has 2 addresses: <"+ get_addr() +", "+ faculty_addr + ">%N")
     end

   do_benchwork() is
     do
       io.put_string(name + " do_benchwork in the: " + get_faculty_addr() + "%N");
     end

   make is  -- the constructor
     do
       name := "Faculty"
       set_addr("home")        -- the home semantics
       faculty_addr := "lab"   -- the lab  semantics
     end
end
