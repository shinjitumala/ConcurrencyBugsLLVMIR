# Notes
- [Original bug report.](https://sqlite.org/forum/info/373fe506fa332617)
- Successfully reproduced by FPR in version 3.31.1.
- The original test code is `test.cpp`. FPR translated it into C (`test.c`) because his analysis tool can only handle C programs.