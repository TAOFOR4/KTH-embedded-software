module BDF where

import ForSyDe.Shallow

switch_bdf (s_a,s_b) = (s_out_1, s_out_2) where
  (s_out_1, s_out_2) = k_1 c_1 s_a
  c_1 = d_1 s_b

  
select_bdf (s_b,s_c,s_d) = s_out where
  s_out = k_2 c_2 s_c s_d
  c_2 = d_2 s_b
  
k_1 = kernel12SADF
k_2 = kernel21SADF

system (s_in,s_b)=s_out where
  (s_c1, s_d1)=switch_bdf (s_in,s_b)
  s_c2 = a_1 s_c1
  s_d2 = a_2 s_d1
  s_out = select_bdf (s_b,s_c2,s_d2)
  
  
a_1 = actor11SDF 1 1 (\[x] -> [x])
a_2 = actor11SDF 1 1 (\[x] -> [-x])
d_1 = detector11SADF consume_rate next_state select_scenario initial_state where
  consume_rate = 1
  -- Next State Function 'next_state' ignores input value
  next_state _[s_control]=s_control
  -- Definition of scenarios  
  k_1_scenario_1 = (1,(1, 0), \[x] -> ([x], [])) -- Scenario 0: Send token
  k_1_scenario_0 = (1,(0, 1), \[x] -> ([], [x])) -- Scenario 1: Ignore token
  -- Function for Selection of scenarios
  select_scenario 0 = (1, [k_1_scenario_0])
  select_scenario 1 = (1, [k_1_scenario_1])
  -- Initial State
  initial_state = 0

  
d_2 = detector11SADF consume_rate next_state select_scenario initial_state where
  consume_rate = 1
  -- Next State Function 'next_state' ignores input value
  next_state _[s_control]=s_control
  -- Definition of scenarios  
  k_2_scenario_1 = ((1,0),1, \[x][] -> [x])
  k_2_scenario_0 = ((0,1),1, \[][x] -> [x])
  -- Function for Selection of scenarios
  select_scenario 0 = (1, [k_2_scenario_0])
  select_scenario 1 = (1, [k_2_scenario_1])
  initial_state = 0

  
s_test = signal[1,2,3,4,5]
s_test2 = signal[-6,-7,-8,-9,-10]
s_control=signal[0,0,1,0,1]