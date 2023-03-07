import ForSyDe.Shallow

system s_in = (s_out_1, s_out_2) where
  (s_out_1, s_out_2) = k_1 c_1 s_in
  c_1                = d_1 s_in

-- Kernel 'k_1'
k_1 = kernel12SADF  

-- Detector 'd_1'
--   - starts in state 0
--   - alterantes between state 0 and 1
--   - ignores input data value
--   - executes scenario 0 in state 0
--   - executes scenario 1 in state 1
d_1 = detector11SADF consume_rate next_state select_scenario initial_state where
  consume_rate = 1
  -- Next State Function 'next_state' ignores input value
  next_state 0 _ = 1
  next_state 1 _ = 0
  -- Definition of scenarios
  -- Kernel 1
  --   - Scenario 0: arc_0: identity function, arc_1: empty token
  --   - Scenario 1: arc_0: empty token, arc_1: negation function
  k_1_scenario_0 = (1, (1, 0), \[x] -> ([x], []))  
  k_1_scenario_1 = (1, (0, 1), \[x] -> ([], [-x]))
  -- Function for Selection of scenarios
  select_scenario 0 = (1, [k_1_scenario_0])
  select_scenario 1 = (1, [k_1_scenario_1])
  -- Initial State
  initial_state = 0

s_test2 = takeS 4 $ infiniteS (+1) 1
  -- Create some examples but take specific amount of values from an infinite list