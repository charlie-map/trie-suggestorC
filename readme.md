# Trie Suggestor
### Charlie Hall

This project works through using web sockets in C to create a typing test for fun! This project was mainly designed to learn more about C and the system that underlies servers in C for future projects.

# Run the project
After you install the project you can run the following `gcc` command to create a binary that will run indefinitely (until you close it) and listens on a specific port (which is defined at the top of `server.c`):

```
gcc server.c trie.c -pthread
```
Adding `trie.c` connects the necessary components of the trie I created (_possible generalized package version coming soon_) and the `-pthread` connects components for listening to multiple users simultaneously.

# Future upgrades
- Currently the project does not _suggest_. Rather, it will send a continous stream of words (weighted accross most used in the english lexicon thanks to [Peter Norvig](http://norvig.com/google-books-common-words.txt) to someone taking the typing test. Next steps in this regard are trying to add this suggest functionality. I currently had a _minimum working_ (see `suggest.c`). However, this is relatively slow and often has issues on larger sets within a trie.
- Feel free to add commments about issues / aspects or enhancements I should consider
