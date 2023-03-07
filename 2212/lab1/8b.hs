module VendingMachine where

import ForSyDe.Shallow

data Coin = C5 | C10 deriving (Show, Eq, Ord)
data Bottle = B deriving (Show, Eq, Ord)
data Return = R deriving (Show, Eq, Ord)

type Coin_Event = AbstExt Coin
type Bottle_Event = AbstExt Bottle
type Return_Event = AbstExt Return

vendingMachine :: Signal Coin_Event -- Signal of Coins
               -> Signal (Bottle_Event, Return_Event) -- Signal of (Bottle, Return)


vendingMachine = mealySY input output 0

input 0 (Prst C5) = 1
input 0 (Prst C10) = 0
input 0 (Abst) = 0
input 1 (Prst C5) = 0
input 1 (Prst C10) = 0
input 1 (Abst) = 1

output 0 (Prst C5) = (Abst, Abst)
output 0 (Prst C10) = (Prst B, Abst)
output 0 (Abst) = (Abst, Abst)
output 1 (Prst C5) = (Prst B, Abst)
output 1 (Prst C10) = (Prst B, Prst R)
output 1 (Abst) = (Abst, Abst)


--test from the lab PDF
s_coin = signal [Prst C10, Prst C5, Prst C5, Prst C5, Prst C10, Abst]


