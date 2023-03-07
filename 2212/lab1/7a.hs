module Netlist where

import ForSyDe.Shallow

-- Netlist
system s_in = (s_out_x,s_out_y) where
  s_1= a_1 s_in s_out_x
  s_2 = a_2 s_1
  s_3 = a_3 s_2 s_out_y
  s_out_y = a_5 s_in  
  s_out_x = d_1 s_3


a_1 = actor21SDF (1,1) 1 f_1 where
  f_1 [x] [y] = [x+y]

a_2 = actor11SDF 1 1 f_2 where 
  f_2 [x] = [2*x]

a_3 = actor21SDF (1,1) 1 f_3 where
  f_3 [x] [y] = [x+y] 

a_5 = actor11SDF 1 1 f_5 where 
  f_5 [x] = [x+1]

d_1 = delaySDF [0]

test = signal [1,2,3]