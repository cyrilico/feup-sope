# feup-sope
Second project for Operating Systems course. A unisex sauna system simulation.
There are two programs, the request generator (gerador) and the access controller (sauna).

To run gerador, use:
gerador 'nr_requests' 'max_usage_time', where:
- nr_requests is the total number of requests to be generated throughout the process
- max_usage_time is the max amount of time a request will want to stay in the sauna (in ms)

To run sauna, use:
sauna 'capacity', where:
- capacity is the maximum number of people from the same gender that can be at the sauna simultaneously

When there's a request from a different gender than the one that's currently occupying the sauna, it is sent back to the generator to be later re-requested. If the number of rejections surpasses 3, it is discarded

Both gerador and sauna register every event request-related in ger.'pid' and bal.'pid', respectively, where pid is the process ID from the current execution
