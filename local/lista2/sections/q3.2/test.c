Get a, b, n;
h = (b − a)/n;
local_n = n/comm_sz; // Devemos garantir que a divisão seja inteira

local_a = a + my_rank∗local_n∗h;
local_b = local_a + local_n∗h;
local_integral = Trap(local_a, local_b, local_n, h);

Atom package to highlight bad characters such as
No-break space ( ) and the Greek question mark (;) 
