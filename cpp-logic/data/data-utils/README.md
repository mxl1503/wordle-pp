# Wordlist Utilities

This folder contains the wordlist maintenance CLI:

- `wordlist_tool.py`

## Usage

Show help:

```sh
python3 cpp-logic/data/data-utils/wordlist_tool.py --help
```

Sort unsorted answers into the canonical answer list:

```sh
python3 cpp-logic/data/data-utils/wordlist_tool.py sort --in cpp-logic/data/wordlists/unsortedWordleAnswers.txt --out cpp-logic/data/wordlists/answerList.txt
```

Generate feedback permutations:

```sh
python3 cpp-logic/data/data-utils/wordlist_tool.py perms --out cpp-logic/data/wordlists/stringPermutations.txt --length 5 --alphabet BGY
```
