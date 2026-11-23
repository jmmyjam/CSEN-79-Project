//
// A separate program that loads a workload file, replays it on your app several times, 
// and writes one CSV row per measurement 
// using the kit's kit::median_ns and kit::CsvWriter (see the kit README for an example).
//

#include "kit/bench.h"

auto lines = kit::read_lines("w1_100k.txt");      // load BEFORE timing
kit::Counters total;
const int runs = 5;

double ns = kit::median_ns(runs,
    [&] { /* untimed: build a fresh app in --quiet mode */ },
    [&] { /* timed: feed every line to the app */ },
    [&] { /* untimed: add the app's counters into total */ });
    
// total holds counters summed over the runs; store one run's worth.
kit::Counters per_run;
per_run.comparisons = total.comparisons / runs;

// ... same for the other fields you use
kit::CsvWriter csv("results.csv");

csv.row({"team7", "orderbook", "W1", "main", lines.size(), "replay", runs, ns, per_run});