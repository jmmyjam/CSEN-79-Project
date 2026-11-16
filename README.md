# Project 2: Subscription alert service

Users subscribe to things they care about: a horoscope sign, a news category, or an item they want to buy. The service receives a stream of published events and delivers each one to the right users, either immediately or at a scheduled time such as "my horoscope at 8:00" or "a news digest every 6 hours". The clock is simulated, so the demo can fast-forward a week in seconds.

Topics are hierarchical paths such as `news/sports/soccer`, `horoscope/leo`, or `items/phones/pixel-10`. A subscription to `news/sports` also receives `news/sports/soccer`. An item subscription can carry a condition such as "notify me when the price drops below $600".

Required commands:

- `SUB <user> <topic> [below <price>] [at <hh:mm> | every <hours>]`
- `UNSUB <user> <topic>`
- `PUB <topic> <message>` and `PRICE <item> <new price>`
- `TICK <minutes>`: advance the simulated clock and fire due deliveries.
- `INBOX <user> [n]`: show a user's last n alerts.
- `HISTORY <user> [n]`: show everything that user has done, oldest first: subscriptions, unsubscriptions, and every alert delivered to them, with the simulated time. With n, show only the last n entries.
- `STATS`: print operation counters.

How input works. There is no real news feed. Every event is one line in a command script, and the message is simply the rest of the PUB line; its content never matters, only who receives it and when. A short script:

```text
SUB alice news/sports at 08:00
SUB bob items/phones/pixel-10 below 600
SUB carol horoscope/leo every 24
PUB news/sports/soccer Arsenal win 2-1
PUB horoscope/leo A good day for big decisions
PRICE items/phones/pixel-10 649
PRICE items/phones/pixel-10 579
TICK 480
INBOX alice
INBOX bob
INBOX carol
STATS
```

At 08:00 Alice gets the soccer result through her `news/sports` subscription. Bob is alerted immediately at $579, but not at $649. Carol's inbox is still empty, because her first delivery is due 24 hours after she subscribed.

Testing. Write a small script for each delivery rule below, with the output you expect, and check the app with diff against it. The starter kit's sample script is the first of these. For benchmarks, the kit's workload generator writes large seeded scripts of the same commands (W1 to W3), run with `--quiet`.

Performance requirements. The design must meet these with 100,000 subscribers and 100,000 pending deliveries:

- Publishing to a topic reaches every subscriber of that topic and of each parent topic, and nobody else.
- Unsubscribing must not require scanning every subscriber of the topic.
- A price change notifies exactly the subscribers whose threshold is now met, without checking every threshold on that item.
- Scheduled deliveries fire in time order.
- Each user's inbox keeps only the last K alerts, with K set at startup.
- `HISTORY` keeps a user's full record even after their inbox drops old alerts, and takes time proportional to that user's own activity, not to the total number of events.

Delivery rules. Follow these exactly; the hidden test scripts check them:

- Clock: the simulated clock starts at Day 1, 00:00. An immediate alert is delivered at the current simulated time.
- One subscription per topic: a user has at most one subscription to each exact topic. A second `SUB` to the same topic replaces the first.
- No duplicates: a user receives each event at most once, even when several of their subscriptions match it (for example `news/sports` and `news/sports/soccer`). The most specific matching subscription decides when it is delivered.
- Unsubscribing: UNSUB removes only that exact topic, not its child topics, and cancels any of its deliveries still waiting.
- Price thresholds: an alert fires when the price drops strictly below the threshold. It fires once, and then that subscription is removed; the user must SUB again to be alerted again.
- Scheduled delivery: `at <hh:mm>` delivers once a day at that time; `every <hours>` delivers at that interval, counted from the moment of subscribing. Each delivery hands over every event collected since the previous one, one alert per event, in publish order. A delivery with nothing collected sends nothing.
- TICK and ties: TICK fires every delivery that falls due up to and including the new time, in time order, so TICK 1440 on an every 6 subscription fires 4 times. Deliveries due at the same minute fire in the order they were scheduled.
- Nothing to do: PUB to a topic with no subscribers, or a PRICE that meets no threshold, changes nothing and prints nothing.

## Technical requirements

Teams choose their own data structures. The design must use at least one linear structure and at least one tree. You may use standard library containers (such as `std::vector`, `std::map` or `std::list`) anywhere in the app, and you may use them for your core structures while you get the app working. By the final submission, your required linear structure and tree must be your own implementations, with your own nodes, links and operations, sitting behind the same interface the STL version used. Your report must benchmark your implementations against the STL containers they replaced and explain any differences you see.

- Language and build: C++17 or later, built with one command (make or CMake) with `-O2` `-Wall` `-Wextra` and no warnings.
- Templates: each hand-built structure is a class template with its own header, usable outside the app.
- Memory: no leaks. Run once under AddressSanitizer or Valgrind and include the clean output.
- Tests: unit tests for every hand-built structure, including edge cases (empty, one element, duplicates, removing the first, last, and only element). Any framework is fine; doctest or Catch2 single-header is easiest.
- Instrumentation: every hand-built structure keeps counters that can be printed and reset: comparisons, elements visited, allocations, and any structure-specific work such as elements moved. Counters must be cheap enough to leave on during benchmarks, or switchable with a compile flag.
- Swappable implementations: the app accepts a flag that swaps at least one core structure for an alternative the team chose, for example `--impl=main|alt`. This is what the metrics compare, and you will flip it live in the demo.
- Input: commands come from a file or standard input, so benchmarks and the demo replay the same script. Output follows the starter kit's `OUTPUT_FORMAT.md` exactly, so test scripts can be checked with diff, and a `--quiet` flag turns off everything except STATS for benchmarks.

Scope is the same for teams of 3 and 4:

| Team size | Hand-built core structures                                       | Alternatives to compare |
| --------- | ---------------------------------------------------------------- | ----------------------- |
| 3 or 4    | At least 3, including at least one linear structure and one tree | 2 decisions             |

## Metrics and benchmarks

Every data structure choice must be backed by a prediction written before measuring, then numbers that confirm or contradict it. A wrong prediction that the team explains is worth full credit; a right answer with no prediction is not.

### Design Decision Records

For each core structure, the team writes a half-page Design Decision Record (DDR) with these six parts:

1. Requirement: what the app must do, in user terms.
2. Operations and frequency: which operations it needs and roughly how often (for example, 60% insert, 30% cancel, 10% sweep).
3. Candidates: at least two structures that could do the job.
4. Prediction: the Big-O of each candidate for those operations, and the expected ratio between them at N = 100,000.
5. Measurement: the numbers from M1 to M5 below.
6. Verdict: the choice, and anything that surprised the team, such as constant factors, cache effects, or worst-case input.

### Required measurements

| ID  | Metric           | What to report                                                                                               |
| --- | ---------------- | ------------------------------------------------------------------------------------------------------------ |
| M1  | Operation counts | Average and maximum comparisons or nodes visited per command type, from the built-in counters                |
| M2  | Scaling          | Time per operation and op counts at N = 1,000, 10,000, 100,000, and 1,000,000, plotted on log-log axes       |
| M3  | Alternatives     | The same workload on the chosen structure and on each alternative, one comparison per required decision      |
| M4  | Worst case       | At least one workload built to hurt a structure, and how the chosen one holds up                             |
| M5  | Memory           | Bytes per stored element for the chosen structure and the alternative (node size times count, plus overhead) |

Timing rules: build with `-O2`, parse the input before starting the clock, use `std::chrono::steady_clock`, report the median of 5 runs, and list the machine used. A benchmark that includes printing to the screen does not count.

### Workloads and suggested alternatives

Each workload is generated from a fixed random seed so results can be reproduced.

| Project          | Workloads                                                                                                                                                                              |
| ---------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1: Order book    | W1 realistic: prices clustered near the mid price, 30% cancels. W2 trending: prices only rise. W3 cancel-heavy: 90% cancels.                                                           |
| 2: Alert service | W1 realistic: topic popularity follows a Zipf curve. W2 hot topic: half of all subscribers follow one topic. W3 price storm: 100k thresholds on one item, prices falling step by step. |

For M4, teams pick whichever of these workloads is hardest for their own design, or add a workload of their own and explain why it is the worst case.

## Tentative schedule and team roles (fill the dates and show with your presentation)

| Date    | Goal                                                               | Done when                                                     |
| ------- | ------------------------------------------------------------------ | ------------------------------------------------------------- |
|         | Pick the project, split ownership, write draft DDRs (parts 1 to 4) | Every structure has an owner and a written prediction         |
|         | Hand-built structures with unit tests                              | Checkpoint: headers and passing tests pushed to the team repo |
|         | Wire structures into the app's command loop                        | The sample command script runs end to end                     |
|         | Counters, workload generator, the swap flag                        | STATS prints real counts; W1 runs at N = 100,000              |
|         | Full benchmark runs: M1 to M5 on all workloads                     | CSV results and plots committed                               |
|         | Finish DDRs, fix bugs, sanitizer run, rehearse the demo            | Demo fits in 8 minutes                                        |
| Week 11 | Submit and demo in class                                           | Tagged release in the repo                                    |

Every student also owns one graded piece: a hand-built structure and its DDR, or, in a team of 4, the benchmark harness or the test suite. Every student must be able to explain any structure in the project.

- Structures lead: sets the template interface and the counter conventions, reviews every structure.
- App lead: command parser, matching or delivery logic, the swap flag.
- Benchmark lead: workload generator, timing harness, plots.
- Quality lead: test suite, sanitizer runs, README, demo script.

You should assign a person to each role. Make sure to add it to your presentation.

Teams of 3 should merge the roles.

## Deliverables and demo

Teams submit one repository and give one 15-minute demo.

### What to submit

- [ ] Source code that builds with one command, with a README covering build, run, and the swap flag
- [ ] Unit tests for every hand-built structure, plus a clean sanitizer or Valgrind log
- [ ] Workload generator and the exact seeds used
- [ ] Raw benchmark results as CSV, and plots for M2 and M3
- [ ] One DDR per core structure (2 to 4 pages in total)
- [ ] A demo script: the command file the team will replay in class
- [ ] A one-paragraph contribution statement from each student

### In-class demo (15 minutes max)

1. The app (3-5 min): replay the demo script. Project 1 shows a large order sweeping several price levels, then a cancel. Project 2 shows a price drop alerting exactly the right users, then a TICK that fires scheduled deliveries.
2. The evidence (3-5 min): one prediction, the M3 plot that tested it, and the worst-case result from M4.
3. The live swap (1 min): rerun one workload with the alternative structure, for example `--impl=alt` on W2, and show STATS change.
4. Questions (1-5 min or more): the instructor asks any team member a "what if" question, such as "what changes if cancels drop to 1%?" or "why not a hash map here?".

## Grading rubric

The evidence for data structure choices is worth 40 of 100 points, more than any other part.

| Category                          | Points | Full credit looks like                                                                                             |
| --------------------------------- | ------ | ------------------------------------------------------------------------------------------------------------------ |
| Metrics and benchmarks (M1 to M5) | 25     | All five metrics on all three workloads, reproducible from the seeds, plots readable without explanation           |
| Design Decision Records           | 15     | Predictions written before measuring, every gap between prediction and result explained                            |
| Data structure implementation     | 20     | Hand-built templates, correct under edge cases, counters accurate, structure invariants hold after every operation |
| Application correctness and tests | 15     | Matching or delivery rules are right on the demo script and hidden test scripts; tests cover edge cases            |
| Demo and answers                  | 15     | Runs without a hitch in 8 minutes; any member answers a "what if" question                                         |
| Code quality and build            | 10     | One-command build, no warnings, no leaks, readable code                                                            |

Individual grades are the team grade adjusted by up to 20% using contribution statements, peer evaluations, git history, and each student's answer in the demo.

## Starter code: a small kit, not an app skeleton

There is a thin starter kit. Please read `README.md` to better understand what is in it.
