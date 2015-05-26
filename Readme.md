# A greedy algorithm for taxi sharing optimization

Car pooling is the concept of sharing car journeys, and it has gained massive public attention in recent years. Both economical and environmental benefits (at individual and collective levels) are obtained by sharing car trips, as it minimizes travel expenses as well as the amount of vehicles on the streets, thus reducing pollution. This contributes to minimize the impact of transportation in the environment, which is a major concern nowadays, especially for big cities. Additionally, less traffic leads to fewer traffic jams, resulting in a more fluent, thus more efficient, trip. From an economical perspective, sharing trips between multiple passengers significantly reduces the transportation costs. 

The aforementioned benefits have led to initiatives to attend the public concern on this topic. Exclusive car-pool lanes, campaigns to promote car sharing, and a plethora of mobile applications to find car pooling mates, are some of the many examples that illustrate the importance of this subject. 

Taxis are a fast and reliable mean of transportation. However, they rarely run at full capacity and could therefore benefit from the car pooling idea. There are some web platforms that provide solutions for ride-sharing scheduling, and some research works on different variants of the taxi-pooling problem. The taxi sharing problem is NP-hard. Thus, heuristics and metaheuristics are needed to find high-quality solutions in reasonable execution times for realistic problem instances.

## Greedy

A greedy algorithm is a method that constructs solutions by making locally optimal decisions in each step. In order to compare the results achieved by the NSGA-II, two greedy algorithms were developed for each of the objective functions: cost and delay. Similar strategies can be expected from a group of human users trying to solve the problem.

The greedy method that minimizes cost sequentially adds the passenger whose destination is closer to the origin in a taxi, until that taxi is full. In the case where the cost of adding one passenger to the current taxi is greater than the one obtained by assigning a new taxi to serve that passenger request, the current taxi is closed, and a new one is formed. The algorithm ends when every passenger is assigned to a taxi.

The greedy strategy for optimizing the delay takes the most hurried passengers (those with smaller tolerance for delay) and assigns each of them to a new taxi. It then takes the rest of the passengers in order of hurriedness and assigns them to the last location of the taxi that minimizes their delay. When a taxi has as many passengers as the maximum capacity at the moment, it is consider as closed. If one taxi has more passengers assigned than the maximum capacity available it is deleted, and the passengers of that taxi are assigned to two new taxis.
