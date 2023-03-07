module SADF_Adapter where

import ForSyDe.Shallow

system s_in = s_out where
  (s_out,s_1) = k_1 c_1 s_in
  s_2 = delay s_1
  c_1 = d_1 s_2
  
--delay
delay s = delaySDF [0] s
--kernel
k_1 = kernel12SADF

-- Detector
d_1 = detector11SADF consume_rate next_state select_scenario initial_state where
  consume_rate = 1

--change mode
  next_state s [x] | x >= 21 = 1 | x <= 9 = 0 | otherwise = s

--models
  k_1_scenario_0 = (3, (1,1), \[x1,x2,x3] -> ([x1 + x2 + x3],[x1 + x2 + x3]))
  k_1_scenario_1 = (2, (1,1), \[x1,x2] -> ([x1 + x2],[x1 + x2]))
  select_scenario 0 = (1, [k_1_scenario_0])
  select_scenario 1 = (1, [k_1_scenario_1])

  initial_state = 0

s_test = signal [4,5,6,8,8,9,9,8,2,4,8,5,2]