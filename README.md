# biti

The name literally means nothing. It was the first thing that came to my head and I did not want to invest alot of time on it!

# Short description
A log file watcher that sends alerts whenever a particular pattern is observed

# Goal
Efficiently watch (text) log files for changes and trigger an action whenever a particular chang is observed eg maybe send an alert whenever the keyword error appears in the log output.

# Concepts
__pattern:__ Simply a keyword/s that you want to track and be notified whenever it appears in a logfile.

__backend:__ This is the notification mechanism that is triggered whenever a pattern is observed. An example of a simple backend is the email backend that sends an email to the configured email address.

# Building

At the moment am keeping things pretty straight forward and compiling can be easily done by compiling with the good old g++ directly

# Dependecies

- inotify for notifications on file changes
- https://github.com/nlohmann/json for parsing the json config

# TODO
1. Attach email backend
2. Add configuration example to readme
3. Add Cmake config for building
4. Explore ways to add backends in a scaleable manner
5. Add performance numbers 
6. Add tests and coverage stuff
7. Add support for 'space' delimeters eg TABs