# Extra Problems — Processes, Pipes, Exec

## Problem A — Exec Pipeline (processes + pipe + exec)

EN: Write a C program that receives two command-line arguments: a filename and a word.
The program creates two child processes connected by a pipe. The first child uses `execl`/`execlp`
to run `grep` on the file, searching for the given word, and writes the output to the pipe.
The second child uses `exec` to run `wc -l` on the pipe input to count the matching lines.
The parent waits for both children and prints the final count.

RO: Scrieți un program C care primește doi argumenți din linia de comandă: un nume de fișier și
un cuvânt. Programul creează două procese copil conectate printr-un pipe. Primul copil folosește
`execl`/`execlp` pentru a rula `grep` pe fișier, căutând cuvântul dat, și scrie rezultatul în
pipe. Al doilea copil folosește `exec` pentru a rula `wc -l` pe intrarea din pipe, numărând
liniile găsite. Procesul părinte așteaptă ambii copii și afișează numărul final.

---

## Problem B — Parallel File Processing (processes + pipe + exec)

EN: Write a C program that receives N filenames as command-line arguments. For each file, the
program creates a child process that uses `execlp` to run `wc -w` (word count) on that file and
sends the result back to the parent via a dedicated pipe. The parent collects all results and
prints the filename alongside the word count, then prints the total word count across all files.

RO: Scrieți un program C care primește N nume de fișiere ca argumente din linia de comandă.
Pentru fiecare fișier, programul creează un proces copil care folosește `execlp` pentru a rula
`wc -w` (numărul de cuvinte) pe acel fișier și trimite rezultatul înapoi părintelui printr-un
pipe dedicat. Părintele colectează toate rezultatele și afișează numele fișierului împreună cu
numărul de cuvinte, apoi afișează numărul total de cuvinte din toate fișierele.

---

## Problem C — Fan-out Aggregation (processes + pipes)

EN: Write a C program that reads N integers from the keyboard. It creates K child processes (K
given as a command-line argument). The parent splits the N integers into K roughly equal chunks
and sends each chunk to a different child via a dedicated pipe. Each child computes the **maximum**
value in its chunk and sends it back to the parent. The parent collects all K maximums and prints
the global maximum. All processes must print a message when sending or receiving data.

RO: Scrieți un program C care citește N numere întregi de la tastatură. Se creează K procese copil
(K dat ca argument din linia de comandă). Părintele împarte cele N numere în K fragmente aproximativ
egale și trimite fiecare fragment unui copil diferit printr-un pipe dedicat. Fiecare copil
calculează valoarea **maximă** din fragmentul său și o trimite înapoi părintelui. Părintele
colectează toate cele K maxime și afișează maximul global. Toate procesele trebuie să afișeze un
mesaj la trimiterea sau primirea datelor.

---

## Problem D — Pipeline of Three (processes + pipes)

EN: Write a C program that receives a filename as a command-line argument. The parent creates two
child processes forming a pipeline: the parent reads the file line by line and sends each line to
**child 1** via a pipe. Child 1 converts all uppercase letters to lowercase and forwards each line
to **child 2** via a second pipe. Child 2 counts the total number of vowels across all received
lines and sends the final count back to the parent via a third pipe. The parent prints the total
vowel count. All processes must print a message when sending or receiving data.

RO: Scrieți un program C care primește un nume de fișier ca argument din linia de comandă.
Părintele creează două procese copil formând un pipeline: părintele citește fișierul linie cu
linie și trimite fiecare linie către **copilul 1** printr-un pipe. Copilul 1 convertește toate
literele mari în litere mici și transmite fiecare linie către **copilul 2** printr-un al doilea
pipe. Copilul 2 numără totalul vocalelor din toate liniile primite și trimite numărul final înapoi
părintelui printr-un al treilea pipe. Părintele afișează numărul total de vocale. Toate procesele
trebuie să afișeze un mesaj la trimiterea sau primirea datelor.

---

## Problem E — Fan-in Collection (processes + pipes)

EN: Write a C program that receives two numbers N and M as command-line arguments. The main
process creates N child processes. Each child generates M random integers between 1 and 1000 and
sends them to the parent via a dedicated pipe (one pipe per child). The parent reads all values
from all N pipes and prints the **sum** and **average** of all `N * M` numbers received. Each
child prints a message for every number it sends, and the parent prints a message for every number
it receives.

RO: Scrieți un program C care primește două numere N și M ca argumente din linia de comandă.
Procesul principal creează N procese copil. Fiecare copil generează M numere întregi aleatoare
între 1 și 1000 și le trimite părintelui printr-un pipe dedicat (câte un pipe per copil). Părintele
citește toate valorile din cele N pipe-uri și afișează **suma** și **media** tuturor celor `N * M`
numere primite. Fiecare copil afișează un mesaj pentru fiecare număr trimis, iar părintele
afișează un mesaj pentru fiecare număr primit.
