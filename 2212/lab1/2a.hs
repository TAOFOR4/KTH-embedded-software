module CSDF where

import ForSyDe.Shallow

-- Netlist
system s_in = (s_out1, s_out2) where
  s_out1     = a_2 s_1
  s_out2     = a_3 s_2
  (s_1, s_2) = a_1 s_in

-- CSDF actor 'a_1' that in each
--   - odd cycle:  outputs value on signal s_1 (scenario 1)
--   - even cycle: outputs value on signal s_2 (scenario 2)
a_1 = actor12SDF 1 (1,1) f_1 where
  f_1 [x] = ([x],[x])

-- SDF actor 'a_2' that implements identity function   
a_2 = actor11SDF 2 1 f_2 where
  f_2 [x1,x2] = [x1]

-- SDF actor 'a_3' that implements negation function     
a_3 = actor11SDF 2 1 f_3 where
  f_3 [x1,x2] = [-x2]
-- Definition of a signal with 10 items of a possible infinite signal
s_test = takeS 10 $ infiniteS (+1) 0

-- Simulation gives:
-- *CSDF_Tutorial_Example> system s_test
-- ({0,2,4,6,8},{-1,-3,-5,-7,-9})
