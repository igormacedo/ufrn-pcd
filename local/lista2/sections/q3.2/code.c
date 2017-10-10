get a, b, n;
h = (b - a)/n;
local_n = n/comm_sz; //Devemos garantir que a divisao sera inteira

n_mod_comm = n % comm_sz;
local_a = a + (my_rank*local_n* +
               my_rank*((int)(my_rank < n_mod_comm))+
               n_mod_comm*((int)(my_rank >= n_mod_comm && n_mod_comm > 0)))*h;


local_b = local_a + (local_n + (int)(my_rank < n_mod_comm))*h;
local_integral = Trap(local_a, local_b, local_n, h);
