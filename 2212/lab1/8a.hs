module VendingMachine where

import ForSyDe.Shallow

vendingMachine :: Signal Bool -- Signal of 5 SEK coins
               -> Signal Bool -- Signal of 10 SEK coins
               -> Signal (Bool, Bool) -- Signal of (Bottle, Return)

vendingMachine = mealy2SY input output 0
--list of input
input 0 True False = 1
input 0 False True = 0
input 0 False False = 0
input 1 True False = 0
input 1 False True = 0
input 1 False False = 1

--list of output
output 0 True False = (False, False)
output 0 False True = (True, False)
output 0 False False = (False, False)
output 1 True False = (True, False)
output 1 False True = (True, True)
output 1 False False = (False, False)


--test from the lab PDF
s_coin5 =  signal [False, True, True, True, False, False]
s_coin10 = signal [True, False, False, False, True, False]
